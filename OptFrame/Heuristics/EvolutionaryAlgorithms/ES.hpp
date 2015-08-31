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

#ifndef OPTFRAME_ES_HPP_
#define OPTFRAME_ES_HPP_

#include <math.h>
#include <vector>

#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Evaluator.hpp"
#include "../../Constructive.h"
#include "../../RandGen.hpp"
#include "../../SingleObjSearch.hpp"
//#include <gsl/gsl_rng.h>
//#include <gsl/gsl_randist.h>

#include "../../Timer.hpp"

//ESTRUTURA DA ESTRATEGIA EVOLUTIVA
//CONSTITUIDA POR PROBABILIDADE DE APLICACAO, N APLICACOES, MOVIMENTO]

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
struct EsStructure
{
	double pr; // probability
	int nap; // number of applications
	double sigmaN;
	double sigmaB;

	EsStructure(double _pr, int _nap, double _sigmaN, double _sigmaB) :
			pr(_pr), nap(_nap), sigmaN(_sigmaN), sigmaB(_sigmaB)
	{
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
struct IndividuoES
{
	Solution<R, ADS>* sInd; // probability
	Evaluation* e;
	vector<EsStructure<R, ADS> >* vEsStructureInd; // number of applications
	vector<int> vNSInd;

	IndividuoES(Solution<R, ADS>* _sInd, Evaluation* _e, vector<EsStructure<R, ADS> >* _vEsStructureInd, int nNS) :
			sInd(_sInd), e(_e), vEsStructureInd(_vEsStructureInd)
	{
		for (int i = 0; i < nNS; i++)
			vNSInd.push_back(i);

		random_shuffle(vNSInd.begin(), vNSInd.end());
	}

	IndividuoES(Solution<R, ADS>* _sInd, Evaluation* _e, vector<EsStructure<R, ADS> >* _vEsStructureInd, vector<int> _vNSInd) :
			sInd(_sInd), e(_e), vEsStructureInd(_vEsStructureInd), vNSInd(_vNSInd)
	{

	}
};

//CADA INDIVIDUO EH UM PAR DE SOLUCAO E UMA TUPLE COM O PARAMETROS DA ESTRATEGIA
template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class ES: public SingleObjSearch<R, ADS>
{
private:
	string path, outputFile;
	int batch;
	Solution<R, ADS>* sStar;
	Evaluation* eStar;
	Evaluator<R, ADS>& eval;
	Constructive<R, ADS>& constructive;
	vector<NSSeq<R, ADS>*> vNS;
	vector<int> vNSeqMaxApplication;
	LocalSearch<R, ADS>& ls;
	RandGen& rg;

	int nNS;
	const int mi;
	const int lambda;
	const int gMaxWithoutImprovement;
	int iterWithoutImprovement;
	int gAtual;
	int selectionMethod;
	double mutationRate;

	typedef vector<IndividuoES<R, ADS> > Populacao;

	static bool compareESIndividuo(IndividuoES<R, ADS> p1, IndividuoES<R, ADS> p2)
	{
		double eP1 = p1.e->evaluation();
		double eP2 = p2.e->evaluation();
		return eP1 < eP2;
	}

public:

	//Evaluator, constructive, vNS -- vector with neighboorhods strucutures able to move solution,
	//vNSeqMaxApplication -- vector with the max number of moves of each neigh
	//optional local search, selectionMethod, mutationRate, others...
	ES(Evaluator<R, ADS>& _eval, Constructive<R, ADS>& _constructive, vector<NSSeq<R, ADS>*> _vNS, vector<int> _vNSeqMaxApplication, LocalSearch<R, ADS>& _ls, int _selectionMethod, double _mutationRate, RandGen& _rg, int _mi, int _lambda, int _gMaxWithoutImprovement, string _outputFile, int _batch) :
			eval(_eval), constructive(_constructive), vNS(_vNS), vNSeqMaxApplication(_vNSeqMaxApplication), ls(_ls), selectionMethod(_selectionMethod), mutationRate(_mutationRate), rg(_rg), mi(_mi), lambda(_lambda), gMaxWithoutImprovement(_gMaxWithoutImprovement), outputFile(_outputFile), batch(_batch)
	{
		nNS = vNS.size();
		sStar = NULL;
		eStar = NULL;

		iterWithoutImprovement = 0;
		gAtual = 0;

		// selectionMethod == 0 low selection pressure (mi,lambda)
		// selectionMethod == 1 selection pressure (mi+lambda)
	}

	virtual ~ES()
	{
	}

	void mutateESParams(vector<EsStructure<R, ADS> >* p, vector<int>& vNSInd, int nNS)
	{
		double z = rg.rand01();
		if (z <= mutationRate)
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

			iter_swap(vNSInd.begin() + posX, vNSInd.begin() + posY);
		}

		for (int param = 0; param < nNS; param++) // 8 vizinhancas
		{

			p->at(param).sigmaN += rg.randG(0, 0.1) / 100.0;
			if (p->at(param).sigmaN < 0)
				p->at(param).sigmaN = 0;
			if (p->at(param).sigmaN > 3)
				p->at(param).sigmaN = 3;
			p->at(param).pr += rg.randG(0, p->at(param).sigmaN);
			if (p->at(param).pr < 0)
				p->at(param).pr = 0;
			if (p->at(param).pr > 1)
				p->at(param).pr = 1;

			p->at(param).sigmaB += rg.randG(0, 0.1) / 100.0;
			if (p->at(param).sigmaB < 0)
				p->at(param).sigmaB = 0;
			if (p->at(param).sigmaB > 1)
				p->at(param).sigmaB = 1;
			p->at(param).nap += rg.randBinomialWithNegative(p->at(param).sigmaB, 10);
			if (p->at(param).nap < 1)
				p->at(param).nap = 1;
			if (p->at(param).nap > vNSeqMaxApplication[param])
				p->at(param).nap = vNSeqMaxApplication[param];
		}

	}

	void applyMutationOperators(Solution<R, ADS>* s, vector<EsStructure<R, ADS> >* p, vector<int> vNSInd, int nNS)
	{
		for (int i = 0; i < nNS; i++)
		{
			int param = vNSInd[i];
			double rx = rg.rand01();
			if (rx < p->at(param).pr)
				for (int a = 1; a <= p->at(param).nap; a++)
				{
					Move<R, ADS>* mov_tmp = &vNS[param]->move(*s);

//					int tries = 0;
//					int maxTries = 1;
//
//					while ((!mov_tmp->canBeApplied(*s)) && (tries < maxTries))
//					{
//						delete mov_tmp;
//						mov_tmp = &vNS[param]->move(*s);
//						tries++;
//					}

					if (mov_tmp->canBeApplied(*s))
					{
						Move<R, ADS>* mov_rev = mov_tmp->apply(*s);
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

	/*
	 void applyLocalSearch(Populacao& p, int nBuscas, int lambda)
	 {

	 bool aux[lambda];
	 for (int i = 0; i++; i < lambda)
	 aux[i] = false;
	 int n = 0;

	 while (n < nBuscas)
	 {
	 int ind;
	 ind = rand() % lambda;

	 if (aux[ind] == false)
	 {

	 Solution<R, ADS>* filhoo = vnd.search(p[ind].first);
	 delete p[ind].first;
	 p[ind].first = filhoo;

	 aux[ind] = true;
	 n++;
	 }
	 }
	 }*/

	void aplicaBuscaLocalBests(Populacao& p, int nBuscas)
	{

		bool aux[nBuscas];
		for (int i = 0; i < nBuscas; i++)
			aux[i] = false;

		//ORDECAO QuickSort
		vector<IndividuoES<R, ADS> > v;

		for (int i = 0; i < p.size(); i++)
		{
			v.push_back(p[i]);
		}

		sort(v.begin(), v.end(), compareESIndividuo); // ordena com QuickSort

		int n = 0;

		while (n < nBuscas)
		{
			int ind;
			ind = rand() % nBuscas;

			if (aux[ind] == false)
			{

				//EvaluationOPM* e = eval.evaluate(p[ind].first);

				//cout << "VALOR DA FO ANTES DA BL= " << e->evaluation() << endl;

				Solution<R, ADS>* filhoo = &ls.search(*p[ind].first);
				delete p[ind].first;
				p[ind].first = filhoo;

				//EvaluationOPM* ee = eval.evaluate(p[ind].first);
				//cout << "VALOR DA FO DPS DA BL= " << ee->evaluation() << endl;

				//delete e;
				//delete ee;

				aux[ind] = true;
				n++;
			}
		}
	}

	Populacao& competicao(Populacao& pais, Populacao& filhos)
	{
		vector<IndividuoES<R, ADS> > v;

		for (int i = 0; i < pais.size(); i++)
			v.push_back(pais[i]);

		for (int i = 0; i < filhos.size(); i++)
			v.push_back(filhos[i]);

		sort(v.begin(), v.end(), compareESIndividuo); // ordena com QuickSort

		Populacao* p = new Populacao;

		double fo_pop = 0;
		int nIndComp = v.size();

		for (int i = 0; i < nIndComp; i++)
		{
			if (i < mi)
			{
				p->push_back(v[i]);
				fo_pop += v[i].e->evaluation();
			}
			else
			{
				delete v[i].sInd; // Solution
				delete v[i].vEsStructureInd; // vectors de mutacao e prob
				delete v[i].e; // vectors de mutacao e prob
			}
		}

//		fo_pop = fo_pop / mi;
//		cout << "Average fo after competition:" << fo_pop << endl;
//		getchar();

		//AVALIA MELHOR INDIVIDUO
		double fo = v[0].e->evaluation();

		if (eval.betterThan(fo, eStar->evaluation()))
		{
			delete eStar;
			delete sStar;
			eStar = &v[0].e->clone();
			sStar = &v[0].sInd->clone();

			cout << "Gen:" << gAtual << " | noImp: " << iterWithoutImprovement;
			cout << " | Best: " << eStar->evaluation() << "\t [";
			for (int param = 0; param < nNS; param++)
			{
				cout << "(" << p->at(0).vEsStructureInd->at(param).pr;
				cout << ",";
				cout << p->at(0).vEsStructureInd->at(param).nap << ") ";
			}
			cout << "]\t";
			cout << p->at(0).vNSInd << endl;

			if (Component::debug)
			{
				stringstream ss;
				ss << outputFile << "_" << "best";
				string outputBest = ss.str();
				FILE* arquivo = fopen(outputBest.c_str(), "a");
				if (!arquivo)
				{
					cout << "ERRO: falha ao criar arquivo \"outputBest.txt\"" << endl;
				}
				else
				{
					fprintf(arquivo, "%d\t%d\t%d\t", batch, gAtual, iterWithoutImprovement);
					for (int param = 0; param < nNS; param++)
					{
						double pr = p->at(0).vEsStructureInd->at(param).pr;
						int nap = p->at(0).vEsStructureInd->at(param).nap;
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

		return *p;
	}

	Populacao& lowSelectivePression(Populacao& pop, Populacao& pop_filhos)
	{
		//onlyOffsprings
		Populacao pop_nula;
		Populacao& pNova = competicao(pop_nula, pop_filhos);
		pop_nula.clear();

		for (int i = 0; i < pop.size(); i++)
		{
			delete pop[i].sInd;
			delete pop[i].vEsStructureInd;
			delete pop[i].e;
		}

		return pNova;
	}

	Populacao&
	highSelectivePression(Populacao& pop, Populacao& pop_filhos)
	{
		Populacao& pNova = competicao(pop, pop_filhos);
		return pNova;
	}

	virtual void localSearch(Solution<R, ADS>& s, Evaluation& e, double timelimit, double target_f)
	{
		ls.exec(s, e, timelimit, target_f);
	}

	pair<Solution<R, ADS>&, Evaluation&>*
	search(double timelimit = 100000000, double target_f = 0, const Solution<R, ADS>* _s = NULL, const Evaluation* _e = NULL)
	{
		cout << "ES search(" << target_f << "," << timelimit << ")" << endl;

		Timer tnow;

		Populacao pop;
		double fo_inicial = 0;

		//GERANDO VETOR DE POPULACAO INICIAL
		for (int i = 0; i < mi; i++)
		{
			//PartialGreedyInitialSolutionOPM is(opm, 0.4, 0.4, 0.4); // waste, ore, shovel
			Solution<R, ADS>* s = &constructive.generateSolution();
			vector<EsStructure<R, ADS> >* m = new vector<EsStructure<R, ADS> >;

			for (int aux = 0; aux < nNS; aux++)
			{
				//probability, application, sigmaNomal, sigmaBinomial
				m->push_back(EsStructure<R, ADS>(rg.rand01(), rg.randBinomial(0.5, 10) + 1, rg.rand01(), rg.rand01()));
			}

			Evaluation* e = &eval.evaluate(*s);
			IndividuoES<R, ADS> ind(s, e, m, nNS);
			pop.push_back(ind);

			fo_inicial += e->evaluation();

			if (i == 0)
			{
				eStar = &pop[i].e->clone();
				sStar = &pop[i].sInd->clone();
			}
			else
			{
				if (eval.betterThan(pop[i].e->evaluation(), eStar->evaluation()))
				{
					delete sStar;
					delete eStar;
					eStar = &pop[i].e->clone();
					sStar = &pop[i].sInd->clone();
				}
			}

		}

		cout << "Valor Medio das FO's da POP inicial: " << fo_inicial / mi << endl;
		cout << " eStar = " << (double) eStar->evaluation() << endl;
		// ===============================
//

		//INICIA PARAMETROS DE MUTACAO
		vector<vector<pair<double, double> > > meanParamsGenerations;

		iterWithoutImprovement = 0;
		while ((iterWithoutImprovement < gMaxWithoutImprovement) && ((tnow.now()) < timelimit) && eval.betterThan(target_f, eStar->evaluation()))
		{
			Populacao pop_filhos;
			double fo_filhos = 0;

			//GERA OS OFFSPRINGS
			for (int l = 1; l <= lambda; l++)
			{
				int x = rg.rand(mi);

				// Cria Filho e Tuple de Parametros (pi,nap,vizinhança)
				Solution<R, ADS>* filho = &pop[x].sInd->clone();
				vector<EsStructure<R, ADS> >* vt = new vector<EsStructure<R, ADS> >(*pop[x].vEsStructureInd);
				vector<int> vNSInd = pop[x].vNSInd;

				// Mutacao dos parametros l
				//mutaParametros(vt);

				mutateESParams(vt, vNSInd, nNS);

				// application dos parametros para gerar filho completo
				applyMutationOperators(filho, vt, vNSInd, nNS);

				// Optional -- Local Search in each Offspring.

				// Sem Busca Local
				Solution<R, ADS>* filho_bl = filho;

				Evaluation* e = &eval.evaluate(*filho_bl);
				fo_filhos += e->evaluation();

				IndividuoES<R, ADS> ind(filho_bl, e, vt, vNSInd);
				pop_filhos.push_back(ind);
			}

			//cout << "Offspring mean FO, iter " << gAtual << ":\t" << fo_filhos / mi << endl;

			//APLICA B.L VND EM 'nb' INDIVIDUOS DA POP_FILHOS
			//aplicaBuscaLocalBests(pop_filhos, 2);
			//cout<<"Applying local Search ..."<<endl;

			//applyLocalSearch(pop_filhos, nb, lambda);

			//cout<<" local search finished!"<<endl;
			//getchar();
			// ETAPA DE SELECAO - MI,LAMBDA ou MI + LAMBDA // ATUALIZA BEST

			// =====================Selection ==================

			Populacao* pNew;

			switch (selectionMethod)
			{
			case 0:
				pNew = &lowSelectivePression(pop, pop_filhos); //Estrategia (Mi,Lamda)
//				cout << "Selection low selective pression!" << endl;
//				getchar();
				break;
			case 1:
				pNew = &highSelectivePression(pop, pop_filhos); //Estrategia (Mi+Lamda)
				break;
//			case 2:
//
//				if (iterWithoutImprovement < 500)
//					pNew = highSelectivePression(pop, pop_filhos); //Estrategia (Mi,Lamda)
//				else
//					pNew = lowSelectivePression(pop, pop_filhos); //Estrategia (Mi,Lamda)
//
//				break;

			default:
				cout << "Error! Selection not performed!" << endl;
				getchar();
			}

			pop.clear();
			pop_filhos.clear();
			pop = *pNew;
			pNew->clear(); //TODO check if something should be deleted
			// =====================End Selection ==================

			// ====================================================
			//Statitics about evolution of the params

			vector<pair<double, double> > meanParams;

			for (int param = 0; param < nNS; param++)
			{
				double meanPR = 0;
				double meanNAP = 0;
				for (int i = 0; i < mi; i++)
				{
					meanPR += pop[i].vEsStructureInd->at(param).pr;
					meanNAP += pop[i].vEsStructureInd->at(param).nap;
					//					cout << "(" << pop[i].second->at(param).pr;
					//					cout << ",";
					//					cout << pop[i].second->at(param).nap << ") ";
				}
				meanPR /= mi;
				meanNAP /= mi;
				meanParams.push_back(make_pair(meanPR, meanNAP));
				//				cout << "(" << meanPR;
				//				cout << ",";
				//				cout << meanNAP << ") ";

			}
			//cout << endl;
			meanParamsGenerations.push_back(meanParams);

			if (Component::debug)
			{
				FILE* arquivo = fopen(outputFile.c_str(), "a");
				if (!arquivo)
				{
					cout << "ERRO: falha ao criar arquivo \"outputFileES.txt\"" << endl;
				}
				else
				{
					fprintf(arquivo, "%d\t%d\t", batch, gAtual);
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

			gAtual++;
		}

		//BUSCA LOCAL NO MELHOR INDIVIDUO

		/*Solution<R, ADS>* sStarBL = &ls.search(*sStar);
		 Evaluation& eStarBL = eval.evaluate(*sStarBL);
		 cout << "eStarBL = " << (double) eStarBL.evaluation() << endl;
		 cout << "eStar = " << (double) eStar->evaluation() << endl;*/

		cout << "tnow.now() = " << tnow.now() << " timelimit = " << timelimit << endl;
		cout << "Acabou ES = iterWithoutImprovement = " << iterWithoutImprovement << " gMaxWithoutImprovement = " << gMaxWithoutImprovement << endl;
		cout << "target_f = " << target_f << " eStar->evaluation() = " << eStar->evaluation() << endl;
		//getchar();

		return new pair<Solution<R, ADS>&, Evaluation&>(*sStar, *eStar);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS>::idComponent() << "ES";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

}

#endif /* ES_HPP_ */
