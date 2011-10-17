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

#ifndef NONSORTINGGENETICALGORITHMII_HPP_
#define NONSORTINGGENETICALGORITHMII_HPP_

#include <algorithm>

#include "../Heuristic.hpp"
#include "../Evaluator.hpp"
#include "../Evaluation.hpp"
#include "../Population.hpp"
#include "../NSSeq.hpp"
#include "../ParetoDominance.hpp"
#define INFINITO 1000000000
template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class NSGAII: public Heuristic<R, M>
{
	typedef vector<Evaluation<M>*> FitnessValues;

private:
	vector<Evaluator<R, M>*> v_e;
	ParetoDominance<R, M> pDominance;
	int gMax;

	static bool compara(pair<double, int> p1, pair<double, int> p2)
	{
		return p1.first > p2.first;
	}

protected:
	RandGen& rg;
public:

	using Heuristic<R, M>::exec; // prevents name hiding

	NSGAII(vector<Evaluator<R, M>*> _v_e, int _gMax, RandGen& _rg) :
		v_e(_v_e), rg(_rg)
	{
		pDominance.insertEvaluators(_v_e);
		gMax = _gMax;
	}

	virtual void basicGeneticOperators(Population<R>& p) = 0;

	virtual void exec(Population<R>& p, double timelimit, double target_f)
	{
		//ACHO Q FALTA APAGAR ALGUMA COISA NO FINAL

		//vector<vector<Evaluation<M>*> > e_pop;
		FitnessValues& e_pop = *new FitnessValues;

		for (int i = 0; i < p.size(); i++)
			e_pop.push_back(&v_e[0]->evaluate(p.at(i)));

		exec(p, e_pop, timelimit, target_f);

		for (int i = 0; i < e_pop.size(); i++)
		{
			//for (int e = 0; e < v_e.size(); e++)
			//{
			//	delete e_pop[i][e];
			//}
			//delete &e_pop[i];

			delete e_pop[i];
		}
		delete &e_pop;
	}

	virtual void exec(Population<R>& p, FitnessValues& e_pop, double timelimit, double target_f)
	{
		Timer tnow;

		cout << "exec: Non Sorting Genetic Algorithm Search " << endl;
		int N = p_0.size();

		Population<R> q = p;
		basicGeneticOperators(q);

		int g = 0;
		while ((g <= gMax) && (tnow.now() < timelimit))
		{
			cout << "Generation = " << g << endl;

			Population<R> r = p;

			for (int i = 0; i < q.size(); i++)
				r.push_back(q.at(i));

			//Start NonDominance Order by sets
			vector<Population<R>*> F;
			nonDominanceOrder(F, r);

			/*
			for(int i =0;i<F.size();i++)
				cout<<"F["<<i<<"] = "<<F[i]->size()<<endl;
			getchar();
			*/
			if ((g % 10) == 0)
			{
				cout << "=================================" << endl;
				for (int i = 0; i < F[0]->size(); i++)
				{
					Evaluation<M>& e1 = v_e[0]->evaluate(F[0]->at(i));
					Evaluation<M>& e2 = v_e[1]->evaluate(F[0]->at(i));
					Evaluation<M>& e3 = v_e[2]->evaluate(F[0]->at(i));
					cout << "Ind = " << i << endl;
					cout << " obj1 = " << e1.evaluation() << endl;
					cout << " obj2 = " << e2.evaluation() << endl;
					cout << " obj3 = " << e3.evaluation() << endl;
					delete &e1;
					delete &e2;
					delete &e3;
				}
				cout << "=================================" << endl;
			}

			Population<R> popTemp;
			int j = 0;

			vector<double> cD; //Crowding Distance

			while ((popTemp.size() + F[j]->size()) < N)
			{
				crowdingDistanceOrder(cD, *F[j], g);

				for (int i = 0; i < F[j]->size(); i++)
					popTemp.push_back(F[j]->at(i));
				j++;
			}

			vector<double> cDTemp;
			crowdingDistanceOrder(cDTemp, *F[j]);

			/*
			 for (int i = 0; i < F.size(); i++)
			 cout << "F[" << i << "] = " << F[i]->size() << endl;
			 for (int i = 0; i < cD.size(); i++)
			 cout << "cD[" << i << "] = " << cD[i] << endl;
			 for (int i = 0; i < cDTemp.size(); i++)
			 cout << "cDTemp[" << i << "] = " << cDTemp[i] << endl;
			 getchar();
			 */

			vector<pair<double, int> > cDOrdenated;
			for (int i = 0; i < cDTemp.size(); i++)
				cDOrdenated.push_back(make_pair(cDTemp[i], i));

			sort(cDOrdenated.begin(), cDOrdenated.end(), compara);

			int popTempSize = popTemp.size();
			for (int i = 0; i < (N - popTempSize); i++)
			{
				cD.push_back(cDOrdenated[i].first);
				popTemp.push_back(F[j]->at(cDOrdenated[i].second));
			}

			p.clear();
			p = popTemp;
			popTemp.clear();
			//cout << "p.size() = " << p.size() << endl;
			//cout << "q.size() antes selecao = " << q.size() << endl;
			q.clear();
			q = basicSelection(p, cD);
			//cout << "q.size() depois selecao= " << q.size() << endl;
			//cout << "p.size() = " << p.size() << endl;
			basicGeneticOperators(q);
			//cout << "q.size() depois basic genect = " << q.size() << endl;
			//getchar();

			for (int i = 0; i < F.size(); i++)
				F[i]->clear();

			r.clear();

			g++;
		}

	}

	void crowdingDistanceOrder(vector<double>& CD, const Population<R>& Fj)
	{
		int N = Fj.size();
		if (N > 0)
		{
			int CDOldSize = CD.size();
			for (int i = 0; i < N; i++)
				CD.push_back(0);

			for (int m = 0; m < v_e.size(); m++)
			{
				vector<pair<double, int> > fitness;

				for (int i = 0; i < N; i++)
				{
					Evaluation<M>& e = v_e[m]->evaluate(Fj.at(i));
					fitness.push_back(make_pair(e.evaluation(), i));
					delete &e;
				}

				sort(fitness.begin(), fitness.end(), compara);
				//Maior [0], Menor [N]

				CD[CDOldSize + fitness[0].second] = INFINITO;
				CD[CDOldSize + fitness[N - 1].second] = INFINITO;

				for (int i = (CDOldSize + 1); i < (CDOldSize + N - 1); i++)
				{
					if ((fitness[0].first - fitness[N - 1].first) < 0.000001)
						CD[CDOldSize + fitness[i].second] = INFINITO;
					else
						CD[CDOldSize + fitness[i].second] = CD[CDOldSize + fitness[i].second] + (fitness[i - 1].first - fitness[i + 1].first) / (fitness[0].first - fitness[N - 1].first);

				}

			}
		}
	}

	void crowdingDistanceOrder(vector<double>& CD, const Population<R>& Fj, int g)
	{
		int N = Fj.size();
		if (N > 0)
		{
			int CDOldSize = CD.size();
			for (int i = 0; i < N; i++)
				CD.push_back(0);

			for (int m = 0; m < v_e.size(); m++)
			{

				vector<pair<double, int> > fitness;

				for (int i = 0; i < N; i++)
				{
					Evaluation<M>& e = v_e[m]->evaluate(Fj.at(i));
					fitness.push_back(make_pair(e.evaluation(), i));
					delete &e;
				}

				sort(fitness.begin(), fitness.end(), compara);
				//Maior [0], Menor [N]


				CD[CDOldSize + fitness[0].second] = INFINITO;
				CD[CDOldSize + fitness[N - 1].second] = INFINITO;

				for (int i = 1; i < N - 1; i++)
				{
					if ((fitness[0].first - fitness[N - 1].first) < 0.000001)
						CD[CDOldSize + fitness[i].second] = INFINITO;
					else
						CD[CDOldSize + fitness[i].second] = CD[CDOldSize + fitness[i].second] + (fitness[i - 1].first - fitness[i + 1].first) / (fitness[0].first - fitness[N - 1].first);
				}

			}
		}
	}

	void nonDominanceOrder(vector<Population<R>*>& F, const Population<R>& p)
	{

		//vector<Population<R>*> U;


		Population<R> pAtual = p;
		Population<R>* F0 = new Population<R> ;
		F.push_back(F0);

		vector<int> nd;
		vector<int> deleteds;

		for (int i = 0; i < pAtual.size(); i++)
		{
			int nd = 0;

			for (int j = 0; j < pAtual.size(); j++)
			{
				if (j != i)
					if (pDominance.dominates(pAtual.at(j), pAtual.at(i)))
						nd++;
			}

			if (nd == 0)
			{
				F[0]->push_back(pAtual.at(i));
				deleteds.push_back(i);
			}
		}

		int nMax = p.size() / 2;
		int nAtual = F[0]->size();

		for (int i = 0; i < deleteds.size(); i++)
			delete &pAtual.remove(deleteds[i] - i);

		deleteds.clear();

		int k = 0;

		while ((F[k]->size() != 0) && (nAtual <= nMax))
		{
			k++;

			Population<R>* uTemp = new Population<R> ;
			F.push_back(uTemp);

			for (int i = 0; i < pAtual.size(); i++)
			{
				int nd = 0;

				for (int j = 0; j < pAtual.size(); j++)
				{
					if (j != i)
						if (pDominance.dominates(pAtual.at(j), pAtual.at(i)))
							nd++;
				}

				if (nd == 0)
				{
					F[k]->push_back(pAtual.at(i));
					deleteds.push_back(i);
				}
			}

			for (int i = 0; i < deleteds.size(); i++)
				delete &pAtual.remove(deleteds[i] - i);
			//cout << "deleteds.size = " << deleteds.size() << endl;
			//cout << "F[k].size = " << F[k]->size() << endl;
			//cout<<" pAtual.size "<<pAtual.size()<<endl;
			//getchar();
			nAtual += F[k]->size();

			deleteds.clear();
		}

	}

	virtual Population<R> basicSelection(const Population<R>& p, vector<double> cD)
	{
		Population<R> q;
		for (int i = 0; i < p.size(); i++)
		{
			int j = rg.rand(p.size());
			while (i == j)
				j = rg.rand(p.size());

			bool A = pDominance.dominates(p.at(i), p.at(j));
			if (A)
				q.push_back(p.at(i));

			bool B = pDominance.dominates(p.at(j), p.at(i));
			if (B)
				q.push_back(p.at(j));

			if (A == B)
			{
				if (cD[i] >= cD[j])
					q.push_back(p.at(i));
				else
					q.push_back(p.at(j));
			}
		}

		return q;
	}

};

#endif /*NONSORTINGGENETICALGORITHMII_HPP_*/
