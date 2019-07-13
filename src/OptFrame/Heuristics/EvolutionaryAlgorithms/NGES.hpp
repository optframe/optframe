// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_NGES_HPP_
#define OPTFRAME_NGES_HPP_

#include <math.h>
#include <vector>

#include "../../LocalSearch.hpp"
#include "../../NS.hpp"
#include "../../Evaluator.hpp"
#include "../../Evaluation.hpp"
#include "../../Constructive.hpp"
#include "../../RandGen.hpp"
#include "../../SingleObjSearch.hpp"
#include "../../Timer.hpp"
//#include <gsl/gsl_rng.h>
//#include <gsl/gsl_randist.h>
//#include "../../NSSeq.hpp"



// NGES - Neighborhood Guided Evolution Strategies

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
struct NGESIndStructure
{
	double pr; // probability
	int nap; // number of applications
	double sigmaN;
	double sigmaB;

	NGESIndStructure(double _pr, int _nap, double _sigmaN, double _sigmaB) :
			pr(_pr), nap(_nap), sigmaN(_sigmaN), sigmaB(_sigmaB)
	{
	}
};

//vNSeqMaxApplication -- vector with the max number of moves of each neigh
//optional local search, selectionMethod, mutationRate, others...
struct NGESParams
{
	vector<int> vNSeqMaxApplication;
	int selectionMethod;
	double mutationRate;
	int mi;
	int lambda;
	int gMaxWithoutImprovement;
	string outputFile;
	int batch;

	NGESParams(vector<int> _vNSeqMaxApplication, int _selectionMethod, double _mutationRate, int _mi, int _lambda, int _gMaxWithoutImprovement, string _outputFile, int _batch) :
			vNSeqMaxApplication(_vNSeqMaxApplication), selectionMethod(_selectionMethod), mutationRate(_mutationRate), mi(_mi), lambda(_lambda), gMaxWithoutImprovement(_gMaxWithoutImprovement), outputFile(_outputFile), batch(_batch)
	{
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
struct NGESInd
{
	Solution<R, ADS> sInd; // probability
	Evaluation e;
	vector<NGESIndStructure<R, ADS> > vEsStructureInd; // number of applications
	vector<int> vNSInd;

	NGESInd(Solution<R, ADS>& _sInd, Evaluation& _e, vector<NGESIndStructure<R, ADS> >& _vEsStructureInd, int nNS) :
			sInd(std::move(_sInd)), e(std::move(_e)), vEsStructureInd(std::move(_vEsStructureInd))
	{
		for (int i = 0; i < nNS; i++)
			vNSInd.push_back(i);

		random_shuffle(vNSInd.begin(), vNSInd.end());
		//change to rg shuffle, because seed may influence result
	}

	NGESInd(Solution<R, ADS>& _sInd, Evaluation& _e, vector<NGESIndStructure<R, ADS> >& _vEsStructureInd, vector<int>& _vNSInd) :
			sInd(std::move(_sInd)), e(std::move(_e)), vEsStructureInd(std::move(_vEsStructureInd)), vNSInd(std::move(_vNSInd))
	{

	}

	NGESInd(NGESInd<R, ADS> && nges) :
			sInd(std::move(nges.sInd)), e(std::move(nges.e)), vEsStructureInd(std::move(nges.vEsStructureInd)), vNSInd(std::move(nges.vNSInd))

	{

	}

//	NGESInd(const NGESInd<R, ADS>& nges) :
//			sInd(nges.sInd), e(nges.e), vEsStructureInd(nges.vEsStructureInd), vNSInd(nges.vNSInd)
//
//	{
//
//	}
//
//	NGESInd& operator=(const NGESInd<R, ADS>& nges)
//	{
//		sInd = nges.sInd;
//		e = nges.e;
//		vEsStructureInd = nges.vEsStructureInd;
//		vNSInd = nges.vNSInd;
//		return (*this);
//
//	}

	~NGESInd()
	{
	}

};

//CADA INDIVIDUO EH UM PAR DE SOLUCAO E UMA TUPLE COM O PARAMETROS DA ESTRATEGIA
template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class NGES: public SingleObjSearch<R, ADS>
{
private:
	typedef vector<NGESInd<R, ADS>*> NGESPopulation;

	Evaluator<R, ADS>& eval;
	Constructive<R, ADS>& constructive;
	vector<NS<R, ADS>*> vNS;
	LocalSearch<R, ADS>& ls;
	RandGen& rg;
	NGESParams& ngesParams;

	//Local variables
	int nNS;

public:

	//Evaluator, constructive, vNS -- vector with neighboorhods strucutures able to move solution,
	// selectionMethod: 0-low selection pressure (mi,lambda);1 selection pressure (mi+lambda)
	//TODO - Check why vector<NSSeq*> can not be passed as parameter - Tried but failled
	NGES(Evaluator<R, ADS>& _eval, Constructive<R, ADS>& _constructive, vector<NS<R, ADS>*> _vNS, LocalSearch<R, ADS>& _ls, RandGen& _rg, NGESParams& _ngesParams) :
			eval(_eval), constructive(_constructive), vNS(_vNS), ls(_ls), rg(_rg), ngesParams(_ngesParams)
	{
		nNS = vNS.size();
	}

	virtual ~NGES()
	{
	}

	void mutateESParams(vector<NGESIndStructure<R, ADS> >& p, vector<int>& vNSInd, const int nNS)
	{
		double z = rg.rand01();
		if (z <= ngesParams.mutationRate)
		{
			int posX = rg.rand(nNS);
			int posY = rg.rand(nNS);
			if (nNS > 1)
			{
				while (posY == posX)
				{
					posY = rg.rand(nNS);
				}
			}

			//Swaps only NS order, since the index are preserved for function applyMutationOperators
			iter_swap(vNSInd.begin() + posX, vNSInd.begin() + posY);
		}

		for (int param = 0; param < nNS; param++) // 8 vizinhancas
		{

			p[param].sigmaN += rg.randG(0, 0.1) / 100.0;
			if (p[param].sigmaN < 0)
				p[param].sigmaN = 0;
			if (p[param].sigmaN > 3)
				p[param].sigmaN = 3;
			p[param].pr += rg.randG(0, p[param].sigmaN);
			if (p[param].pr < 0)
				p[param].pr = 0;
			if (p[param].pr > 1)
				p[param].pr = 1;

			p[param].sigmaB += rg.randG(0, 0.1) / 100.0;
			if (p[param].sigmaB < 0)
				p[param].sigmaB = 0;
			if (p[param].sigmaB > 1)
				p[param].sigmaB = 1;
			p[param].nap += rg.randBinomialWithNegative(p[param].sigmaB, 10);
			if (p[param].nap < 1)
				p[param].nap = 1;
			if (p[param].nap > ngesParams.vNSeqMaxApplication[param])
				p[param].nap = ngesParams.vNSeqMaxApplication[param];
		}

	}

	void applyMutationOperators(Solution<R, ADS>& s, const vector<NGESIndStructure<R, ADS> >& p, const vector<int> vNSInd, const int nNS)
	{
		for (int i = 0; i < nNS; i++)
		{
			int param = vNSInd[i]; //Extract index
			double rx = rg.rand01();
			if (rx < p[param].pr)
				for (int a = 0; a < p[param].nap; a++)
				{
					Move<R, ADS>* mov_tmp = vNS[param]->randomMoveSolution(s);
//					int tries = 0;
//					int maxTries = 1;
//
//					while ((!mov_tmp->canBeApplied(*s)) && (tries < maxTries))
//					{
//						delete mov_tmp;
//						mov_tmp = &vNS[param]->move(*s);
//						tries++;
//					}

					if (mov_tmp->canBeAppliedToSolution(s))
					{
						Move<R, ADS>* mov_rev = mov_tmp->applySolution(s);
						delete mov_rev;
					}
					else
					{
//						cout << "cannot be applied NS:" << param;
//						cout << "\tnumber of tries:" << tries << endl;
//						getchar();
					}


					delete mov_tmp;
				}
		}

	}

	void competition(NGESPopulation& pop, NGESPopulation& offsprings, Solution<R, ADS>& sStar, Evaluation& eStar, int& iterWithoutImprovement, const int& gCurrent, int selectionType)
	{

		if (selectionType == 1)
		{
			offsprings.reserve(offsprings.size() + ngesParams.mi);
			for (int i = 0; i < ngesParams.mi; i++)
				offsprings.push_back(pop[i]);
		}

		auto compInd = [&](NGESInd<R,ADS>* indA, NGESInd<R,ADS>* indB)-> bool
		{
			return eval.betterThan(indA->e,indB->e);
		};

		sort(offsprings.begin(), offsprings.end(), compInd); // ordena com QuickSort

		pop.clear();
		pop.resize(ngesParams.mi);
		double fo_pop = 0;

		for (int i = 0; i < ngesParams.mi; i++)
		{
			pop[i] = offsprings[i];
			fo_pop += pop[i]->e.evaluation();
		}


		for (int i = 0; i < ngesParams.lambda; i++)
			delete offsprings[i + ngesParams.mi];

		Evaluation evBest = pop[0]->e.evaluation();

		if (eval.betterThan(evBest, eStar))
		{
//			delete &eStar;
//			delete &sStar;
			eStar = pop[0]->e;
			sStar = pop[0]->sInd;
//			cout << "inside comp:" << &eStar << "/" << eStar.evaluation() << endl;
			if (Component::information)
			{
				cout << "Gen:" << gCurrent << " | noImp: " << iterWithoutImprovement;
				cout << " | Best: " << eStar.evaluation() << "\t [";
				for (int param = 0; param < nNS; param++)
				{
					cout << "(" << pop[0]->vEsStructureInd[param].pr;
					cout << ",";
					cout << pop[0]->vEsStructureInd[param].nap << ") ";
				}
				cout << "]\t";
				cout << pop[0]->vNSInd << endl;
//				cout<<sStar.getR()<<endl;
//				getchar();
			}

			if (Component::debug)
			{
				stringstream ss;
				ss << ngesParams.outputFile << "_" << "best";
				string outputBest = ss.str();
				FILE* arquivo = fopen(outputBest.c_str(), "a");
				if (!arquivo)
				{
					cout << "ERRO: falha ao criar arquivo \"outputBest.txt\"" << endl;
				}
				else
				{
					fprintf(arquivo, "%d\t%d\t%d\t", ngesParams.batch, gCurrent, iterWithoutImprovement);
					for (int param = 0; param < nNS; param++)
					{
						double pr = pop[0]->vEsStructureInd[param].pr;
						int nap = pop[0]->vEsStructureInd[param].nap;
						double prNap = pr * nap;
						fprintf(arquivo, "%f\t%d\t%f\t", pr, nap, prNap);
					}
					fprintf(arquivo, "\n");
					fclose(arquivo);
				}
			}
			iterWithoutImprovement = 0;

		}
		else
			iterWithoutImprovement++;
	}

	pair<Solution<R, ADS>, Evaluation>* search(SOSC& stopCriteria, const Solution<R, ADS>* _s = nullptr, const Evaluation* _e = nullptr)
	{
		Timer tnow;
		NGESPopulation pop;
		Solution<R, ADS>* sStar = nullptr;
		Evaluation* eStar = nullptr;

		int iterWithoutImprovement = 0, gCurrent = 0;

		cout << "NGES search(" << stopCriteria.target_f << "," << stopCriteria.timelimit << ")" << endl;

		double inititPopFitness = 0;
		//Generating initial population
		pop.reserve(ngesParams.mi);
		for (int i = 0; i < ngesParams.mi; i++)
		{
			//PartialGreedyInitialSolutionOPM is(opm, 0.4, 0.4, 0.4); // waste, ore, shovel
			Solution<R, ADS>* s = constructive.generateSolution(stopCriteria.timelimit); //MAKE MOVE TODO
			vector<NGESIndStructure<R, ADS> > m;

			//probability, application, sigmaNomal, sigmaBinomial
			for (int aux = 0; aux < nNS; aux++)
				m.push_back(NGESIndStructure<R, ADS>(rg.rand01(), rg.randBinomial(0.5, 10) + 1, rg.rand01(), rg.rand01()));

			Evaluation e = eval.evaluateSolution(*s);
			NGESInd<R, ADS>* ind = new NGESInd<R, ADS>(*s, e, m, nNS); //TODO MAKE MOVE

			pop.push_back(ind);

			if (i == 0)
			{
				eStar = new Evaluation(pop[i]->e);
				sStar = new Solution<R, ADS>(pop[i]->sInd);
			}
			else
			{
				if (eval.betterThan(pop[i]->e, *eStar))
				{
					(*eStar) = pop[i]->e;
					(*sStar) = pop[i]->sInd;
				}
			}

			inititPopFitness += pop[i]->e.evaluation();

			delete s;
		}

		if (Component::information)
		{
			cout << "InitialPop avg: " << inititPopFitness / ngesParams.mi << "\t eStar:" << (double) eStar->evaluation() << endl;
		}
		// ===============================
//

		//INICIA PARAMETROS DE MUTACAO
		vector<vector<pair<double, double> > > meanParamsGenerations;

		iterWithoutImprovement = 0;
		while ((iterWithoutImprovement < ngesParams.gMaxWithoutImprovement) && ((tnow.now()) < stopCriteria.timelimit) && eval.betterThan(stopCriteria.target_f, (double) eStar->evaluation()))
		{
			NGESPopulation popOffsprings(ngesParams.lambda,nullptr);
			double fo_filhos = 0;

			//GERA OS OFFSPRINGS
			for (int l = 0; l < ngesParams.lambda; l++)
			{
				int x = rg.rand(ngesParams.mi);

				// Cria Filho e Tuple de Parametros (pi,nap,vizinhança)
				Solution<R, ADS> filho = pop[x]->sInd;

				vector<NGESIndStructure<R, ADS> > vt = pop[x]->vEsStructureInd;
				vector<int> vNSOffspring = pop[x]->vNSInd;

				// Mutacao dos parametros l
				mutateESParams(vt, vNSOffspring, nNS);

				// applying Neighborhood Operators for generating Mutated Offspring
				applyMutationOperators(filho, vt, vNSOffspring, nNS);

				// TODO: Optional -- Local Search in each Offspring.
				//Do anything you want with filho and return filho_ls
				//Solution<R, ADS> filho_ls = filho;

				Evaluation e = eval.evaluateSolution(filho);
				fo_filhos += e.evaluation();

				NGESInd<R, ADS>* ind = new NGESInd<R, ADS>(filho, e, vt, vNSOffspring); //TODO MAKE MOVE

				popOffsprings[l] = ind;
			}

			//cout << "Offspring mean FO, iter " << gAtual << ":\t" << fo_filhos / mi << endl;

			//TODO: optionally apply LS in the best individuals
			//aplicaBuscaLocalBests(pop_filhos, 2);
			//cout<<"Applying local Search ..."<<endl;


			// =====================Selection ==================
			// MI,LAMBDA or MI + LAMBDA
			switch (ngesParams.selectionMethod)
			{
			case 0:
				competition(pop, popOffsprings, *sStar, *eStar, iterWithoutImprovement, gCurrent, 0);
				break;
			case 1:
				competition(pop, popOffsprings, *sStar, *eStar, iterWithoutImprovement, gCurrent, 1);
				break;
		    //TODO create other selection strategies

			default:
				cout << "Error! Selection not performed!" << endl;
				getchar();
			}
			// =====================End Selection ==================

			// ====================================================
			//Statistics regarding the evolution of the params
			if (Component::debug)
			{
				vector<pair<double, double> > meanParams;
				for (int param = 0; param < nNS; param++)
				{
					double meanPR = 0;
					double meanNAP = 0;
					for (int i = 0; i < ngesParams.mi; i++)
					{
						meanPR += pop[i]->vEsStructureInd[param].pr;
						meanNAP += pop[i]->vEsStructureInd[param].nap;
						//					cout << "(" << pop[i].second->at(param).pr;
						//					cout << ",";
						//					cout << pop[i].second->at(param).nap << ") ";
					}
					meanPR /= ngesParams.mi;
					meanNAP /= ngesParams.mi;
					meanParams.push_back(make_pair(meanPR, meanNAP));
					//				cout << "(" << meanPR;
					//				cout << ",";
					//				cout << meanNAP << ") ";

				}
				//cout << endl;
				meanParamsGenerations.push_back(meanParams);

				FILE* arquivo = fopen(ngesParams.outputFile.c_str(), "a");
				if (!arquivo)
				{
					cout << "ERRO: falha ao criar arquivo \"outputFileES.txt\"" << endl;
				}
				else
				{
					fprintf(arquivo, "%d\t%d\t", ngesParams.batch, gCurrent);
					for (int param = 0; param < nNS; param++)
					{
						double pr = meanParams[param].first;
						double nap = meanParams[param].second;
						double prNap = meanParams[param].second * meanParams[param].first;
						fprintf(arquivo, "%f\t%f\t%f\t", pr, nap, prNap);
					}
					fprintf(arquivo, "\n");
					fclose(arquivo);
				}
			}

			// ====================================================

			gCurrent++;
		}

		for (int i = 0; i < ngesParams.mi; i++)
			delete pop[i];

		cout << "Fishing NGES search:\nIterWithoutImp: " << iterWithoutImprovement << "/" << ngesParams.gMaxWithoutImprovement << endl;
		cout << "BestSol::" << eStar->evaluation() << "/" << stopCriteria.target_f << endl;
		cout << "tnow.now():" << tnow.now() << "/" << stopCriteria.timelimit << endl;
		cout << "g:" << gCurrent << endl;
		cout << "NGES Finished" << endl;
		cout << "--------------------------------------------------------------------------" << endl;
		//getchar();

		pair<Solution<R, ADS>, Evaluation>* pairToReturn = new pair<Solution<R, ADS>, Evaluation>(make_pair(*sStar, *eStar));
		delete sStar;
		delete eStar;
		return pairToReturn;
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS>::idComponent() << "NGES";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

}

#endif /* NGES_HPP_ */
