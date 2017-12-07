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

#ifndef OPTFRAME_ESContinous_HPP_
#define OPTFRAME_ESContinous_HPP_

#include <math.h>
#include <vector>

#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../SingleObjSearch.hpp"
#include "../../Evaluator.hpp"
#include "../../Constructive.hpp"

#include "../../Timer.hpp"
namespace optframe
{

//ESTRUTURA DA ESTRATEGIA EVOLUTIVA
//CONSTITUIDA DE UMA MATRIX DE DESVIOS PADROES
template<class ESStruct>
struct ESContinuousStructure
{
	ESStruct desvs;

	ESContinuousStructure(ESStruct _desvs) :
			desvs(_desvs)
	{
	}
};

//CADA INDIVIDUO EH UM PAR DE SOLUCAO E UMA TUPLE COM O PARAMETROS DA ESTRATEGIA
template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class ESStruct = double>
class ESContinous: public SingleObjSearch<R, ADS>
{
private:

	Solution<R, ADS>* sStar;
	Evaluation* eStar;
	Evaluator<R, ADS>& eval;
	Constructive<R, ADS>& constructive;
	vector<NSSeq<R, ADS>*> vNS;
	LocalSearch<R, ADS>& ls;

	const int mi;
	const int lambda;
	const int gMax;
	int iterSemMelhora;
	int gAtual;

	typedef pair<Solution<R, ADS>*, ESContinuousStructure<ESStruct>*> Individuo;

	typedef vector<pair<Individuo, double> > Populacao;

	static bool compara(pair<Individuo, double> p1, pair<Individuo, double> p2)
	{
		return p1.second < p2.second;
	}

	//FUNCAO UTILZIADA NO QuickSort

	virtual void changeParameters(ESContinuousStructure<ESStruct>* p) = 0;

	virtual void applyParameters(Solution<R, ADS>* s, ESContinuousStructure<ESStruct>* p) = 0;

	virtual ESStruct generateInitialESStructure(Solution<R, ADS>* s) = 0;

public:

	ESContinous(Evaluator<R, ADS>& _eval, Constructive<R, ADS>& _constructive, vector<NSSeq<R, ADS>*> _vNS, LocalSearch<R, ADS>& _ls, int _mi, int _lambda, int _gMax) :
			eval(_eval), constructive(_constructive), vNS(_vNS), ls(_ls), mi(_mi), lambda(_lambda), gMax(_gMax)
	{
		sStar = nullptr;
		eStar = nullptr;

		iterSemMelhora = 0;
		gAtual = 0;
	}

	virtual ~ESContinous()
	{
	}

	void applyLocalSearchBest(Populacao& p, int nBuscas)
	{

		bool aux[nBuscas];
		for (int i = 0; i < nBuscas; i++)
			aux[i] = false;

		//ORDECAO QuickSort
		vector<pair<Individuo, double> > v;

		for (int i = 0; i < p.size(); i++)
		{
			Evaluation& e = eval.evaluate(*p[i].first);
			v.push_back(make_pair(p[i], e.evaluation()));
			delete &e;
		}

		sort(v.begin(), v.end(), compara); // ordena com QuickSort

		int n = 0;

		while (n < nBuscas)
		{
			int ind;
			ind = rand() % nBuscas;

			if (aux[ind] == false)
			{
				Solution<R, ADS>* filhoo = &ls.search(*p[ind].first);
				delete p[ind].first;
				p[ind].first = filhoo;

				aux[ind] = true;
				n++;
			}
		}
	}

	Populacao& competition(Populacao& pais, Populacao& filhos)
	{
		vector<pair<Individuo, double> > v;

		for (int i = 0; i < (int) pais.size(); i++)
		{
			v.push_back(make_pair(pais[i].first, pais[i].second));
		}

		for (int i = 0; i < (int) filhos.size(); i++)
		{
			v.push_back(make_pair(filhos[i].first, filhos[i].second));
		}

		sort(v.begin(), v.end(), compara); // ordena com QuickSort

		Populacao* p = new Populacao;

		double fo_pop = 0;

		for (int i = 0; i < (int) v.size(); i++)
			if (i < mi)
			{
				p->push_back(make_pair(v[i].first, v[i].second));
				fo_pop += v[i].second;
			}
			else
			{
				delete v[i].first.first; // Solution
				delete v[i].first.second; // vectors de mutacao e prob
			}

		fo_pop = fo_pop / mi;
		//cout << "Media Competicao, media: " << fo_pop << endl;

		//AVALIA MELHOR INDIVIDUO
		double fo = v[0].second;

		//for(int i = 0;i<v.size();i++)
		//	cout<<v[i].second<<endl;
		//getchar();

		if (eval.betterThan(fo, eStar->evaluation()))
		{
			delete eStar;
			delete sStar;

			sStar = &v[0].first.first->clone();
			(*eStar) = eval.evaluateSolution(*sStar);

			/*
			 cout << "vRealClone = " << (&eval.evaluate(*v[0].first.first))->evaluation() << endl;
			 cout << "vRealClone = " << (&eval.evaluate(*v[0].first.first))->evaluation() << endl;
			 cout << "vRealClone = " << (&eval.evaluate(*v[0].first.first))->evaluation() << endl;
			 cout << "vRealClone = " << (&eval.evaluate(*v[0].first.first))->evaluation() << endl;
			 cout << "vRealClone = " << (&eval.evaluate(*v[0].first.first))->evaluation() << endl;
			 cout << "vRealClone = " << (&eval.evaluate(*v[0].first.first))->evaluation() << endl;
			 cout << "eStarClone = " << (&eval.evaluate(v[0].first.first->clone()))->evaluation() << endl;
			 cout << "eStarClone = " << (&eval.evaluate(v[0].first.first->clone()))->evaluation() << endl;
			 cout << "eStarClone = " << (&eval.evaluate(v[0].first.first->clone()))->evaluation() << endl;
			 cout << "eStarClone = " << (&eval.evaluate(v[0].first.first->clone()))->evaluation() << endl;
			 cout << "eStarClone = " << eStar->evaluation() << endl;

			 cout << v[0].first.first->getR() << endl;
			 cout << sStar->getR() << endl;
			 */
			cout << "Iter:" << gAtual << "\tWithoutImprovement: " << iterSemMelhora;
			cout << "\t Best: " << v[0].second;
			cout << "\t [";
			cout << "imprimirMatriz";
			cout << "]" << endl;

			/*
			 FILE* arquivo = fopen("logParam.txt", "a");
			 if (!arquivo)
			 {
			 cout << "ERRO: falha ao criar arquivo \"logParam.txt\"" << endl;
			 }
			 else
			 {
			 fprintf(arquivo, "%d\t", gAtual);
			 for (int param = 0; param < nParam; param++)
			 {
			 //fprintf(arquivo, "%.4f\t%d\t", p->at(0).second->at(param).pr,p->at(0).second->at(param).nap);
			 }
			 fprintf(arquivo, "\n");
			 fclose(arquivo);
			 }*/

			iterSemMelhora = 0;

		}
		else
			iterSemMelhora++;

		return *p;
	}

	Populacao& lowSelectivePression(Populacao& pop, Populacao& pop_filhos)
	{
		//onlyOffsprings
		Populacao pop_nula;
		Populacao& pNova = competition(pop_nula, pop_filhos);

		for (int i = 0; i < pop.size(); i++)
		{
			delete pop[i].first.first;
			delete pop[i].first.second;
		}

		return pNova;
	}

	Populacao& highSelectivePression(Populacao& pop, Populacao& pop_filhos)
	{
		Populacao& pNova = competition(pop, pop_filhos);

		return pNova;
	}

	virtual void localSearch(Solution<R, ADS>& s, Evaluation& e, SOSC& stopCriteria)
	{
		ls.exec(s, e, stopCriteria);
	}

	pair<Solution<R, ADS>, Evaluation>* search(SOSC& stopCriteria, const Solution<R, ADS>* _s = nullptr, const Evaluation* _e = nullptr)
	{
		cout << "ES search(" << stopCriteria.target_f << "," << stopCriteria.timelimit << ")" << endl;

		Timer tnow;

		Populacao pop(mi);

		double fo_Constructive = 0;

		//GERANDO VETOR DE POPULACAO INICIAL
		for (int i = 0; i < mi; i++)
		{
			Solution<R, ADS>* s = constructive.generateSolution(stopCriteria.timelimit);

			ESStruct a = generateInitialESStructure(s);
			//cout << a;
			//getchar();

			ESContinuousStructure<ESStruct>* m = new ESContinuousStructure<ESStruct>(a);

			Evaluation e = eval.evaluateSolution(*s);

			pop[i] = make_pair(make_pair(s, m), e.evaluation());

			fo_Constructive += e.evaluation();

			if (i == 0)
			{
				//First solution of the method
				eStar = &e.clone();
				sStar = &pop[i].first.first->clone();
				//cout<<"e.evaluation() = "<<(double)e.evaluation()<<endl;
				//cout<< " eStar = "<<(double)eStar->evaluation()<<endl;
				//getchar();

			}
			else
			{
				if (eval.betterThan(e.evaluation(), eStar->evaluation()))
				{
					delete eStar;
					delete sStar;
					eStar = &e.clone();
					sStar = &pop[i].first.first->clone();

				}
			}

			delete &e;
		}

		cout << "Mean initial population FO: " << fo_Constructive / mi << "\t";
		cout << "best:" << (double) eStar->evaluation() << endl;
		// ===============================

		iterSemMelhora = 0;

		//Timer tnowClone;
		//double sumClone = 0;
		//double sumEval = 0;
		//int counter = 1;

		while ((iterSemMelhora < gMax) && ((tnow.now()) < stopCriteria.timelimit) && eval.betterThan(stopCriteria.target_f, eStar->evaluation()))
		{
			//cout << "gAtual = " << gAtual << endl;
			//getchar();

			Populacao pop_filhos;
			double fo_filhos = 0;

			//GERA OS OFFSPRINGS
			for (int l = 1; l <= lambda; l++)
			{
				int x = rand() % mi;

				// Cria Filho e Tuple de Parametros (pi,nap,vizinhança)
				//double tClone = tnowClone.now();
				Solution<R, ADS>* filho = &pop[x].first.first->clone();
				//sumClone += tnowClone.now() - tClone;

				ESContinuousStructure<ESStruct>* vt = new ESContinuousStructure<ESStruct>(*pop[x].first.second);

				// Mutacao dos parametros l
				changeParameters(vt);

				// application dos parametros para gerar filho completo
				applyParameters(filho, vt);

				// ApplyLocal Search in each individual of the Population
				//TODO

				// Sem Busca Local
				Solution<R, ADS>* filho_bl = filho;

				//double tEval = tnowClone.now();
				Evaluation e = eval.evaluateSolution(*filho_bl);
				//sumEval += tnowClone.now() - tEval;
				//counter++;

				pop_filhos.push_back(make_pair(make_pair(filho_bl, vt), e.evaluation()));

				fo_filhos += e.evaluation();

			}

			//cout << "Valor Medio das FO's dos filhos: " << fo_filhos / mi << endl;

			//APLICA B.L VND EM 'nb' INDIVIDUOS DA POP_FILHOS
			//applyLocalSearchBest(pop_filhos, 2);
			//cout<<"Applying local Search ..."<<endl;

			//applyLocalSearch(pop_filhos, nb, lambda);

			//cout<<" local search finished!"<<endl;
			//getchar();
			// ETAPA DE SELECAO - MI,LAMBDA ou MI + LAMBDA // ATUALIZA BEST
			//cout<<"Applying selection ..."<<endl;

			//Populacao& pNova = lowSelectivePression(pop, pop_filhos); //Estrategia (Mi,Lamda)
			Populacao& pNova = highSelectivePression(pop, pop_filhos); //Estrategia (Mi+Lamda)

			//cout<<"selection finished !"<<endl;
			pop.clear();
			pop_filhos.clear();

			pop = pNova;

			//INCREMENTA GERACAO
			gAtual++;

			delete &pNova;
		}

		/*cout << sumEval << endl;
		cout << sumClone << endl;
		cout << counter << endl;
		cout << sumEval / counter << endl;
		cout << sumClone / counter << endl;
		cout << tnow.now() << endl;
		getchar();*/

		cout << "tnow.now() = " << tnow.now() << " timelimit = " << stopCriteria.timelimit << endl;
		cout << "Acabou ES = iterSemMelhor = " << iterSemMelhora << " gMax = " << gMax << endl;
		cout << "target_f = " << stopCriteria.target_f << " eStar->evaluation() = " << (double) eStar->evaluation() << endl;
		//getchar();

		for (int i = 0; i < (int) pop.size(); i++)
		{
			delete pop[i].first.first;
			delete pop[i].first.second;
		}

		Solution<R, ADS>& s = *sStar;
		Evaluation& e = *eStar;

		//cout<<s.getR();
		//getchar();
		//delete eStar;
		//delete sStar;

		return new pair<Solution<R, ADS>, Evaluation>(s, e);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS>::idComponent() << "ESContinous";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

}
#endif /* OPTFRAME_ESContinous_HPP_ */
