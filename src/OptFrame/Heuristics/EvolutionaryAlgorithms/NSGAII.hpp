// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

#include "../../MultiObjSearch.hpp"
#include "../../Solution.hpp"
#include "../../Evaluator.hpp"
#include "../../Evaluation.hpp"
#include "../../Population.hpp"
#include "../../NSSeq.hpp"
#include "../../ParetoDominance.hpp"

namespace optframe
{
#define INFINITO 1000000000

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
struct IndividualNSGAII
{
	Solution<R, ADS>& s;
	MultiEvaluation* mev; // TODO: remove?
	int rank;
	double distance;

	bool isChild;
	bool isRandom;
	bool isCross;
	bool isMutation;

	int id; // for debug reasons
	int num_children; // for tests

	IndividualNSGAII(Solution<R, ADS>& _s) :
			s(_s)
	{
		mev = nullptr;
		rank = -1;
		distance = -1;
		id = -1;
		num_children = -1;

		isChild = false;
		isCross = false;
		isRandom = false;
		isMutation = false;
	}

	IndividualNSGAII(const IndividualNSGAII<R, ADS>& ind) :
			s(ind.s.clone())
	{
		mev = nullptr;
		if(ind.mev)
			mev = &ind.mev->clone();
		rank = ind.rank;
		distance = ind.distance;

		isChild = ind.isChild;
		id = ind.id;
		num_children = ind.num_children;

		isRandom = ind.isRandom;
		isCross = ind.isCross;
		isMutation = ind.isMutation;
	}

	virtual ~IndividualNSGAII()
	{
		delete &s;
		if(mev)
			delete mev;
	}

	void print() const
	{
		cout << "IndNSGAII: rank=" << rank << "\tdist=" << distance;
		cout << "\t[ ";
		if(mev)
			for(unsigned e = 0; e < mev->size(); e++)
				cout << mev->at(e).evaluation() << " ; ";
		cout << " ]";

		cout << "\tisChild=" << isChild << " Crs=" << isCross << " Rdm=" << isRandom << " Mut=" << isMutation << "\t |x|=" << num_children << endl;
		//cout << s.getR() << endl;
	}

	IndividualNSGAII<R, ADS>& clone() const
	{
		return *new IndividualNSGAII<R, ADS>(*this);
	}

	static void printResume(const vector<IndividualNSGAII<R, ADS>*>& v)
	{
		int count_child = 0;
		int count_cross = 0;
		int count_random = 0;
		int count_mutation = 0;
		for(unsigned i=0; i<v.size(); i++)
		{
			if(v[i]->isChild)
				count_child++;
			if(v[i]->isRandom)
				count_random++;
			if(v[i]->isCross)
				count_cross++;
			if(v[i]->isMutation)
				count_mutation++;
		}
		cout << "|S|=" << v.size();
                cout << "\tchild=" << count_child << "(" << (count_child*100/v.size()) << "%) |";
                cout << "\trandom=" << count_random << "(" << (count_random*100/v.size()) << "%)";
                cout << "\tcross=" << count_cross << "(" << (count_cross*100/v.size()) << "%)";
                cout << "\tmutation=" << count_mutation << "(" << (count_mutation*100/v.size()) << "%)";
		cout << endl;
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class NSGAII: public MultiObjSearch<R, ADS >
{
	typedef vector<Evaluation*> FitnessValues;

private:
	vector<Evaluator<R, ADS>*> v_e;

	InitialMultiSolution<R, ADS>& init_pop;
	int init_pop_size;

	ParetoDominance<R, ADS> pDominance;
	int gMax;

	static bool compara(pair<double, int> p1, pair<double, int> p2)
	{
		return p1.first > p2.first;
	}

protected:
	RandGen& rg;
public:

	//using Heuristic<R, ADS >::exec; // prevents name hiding

	NSGAII(vector<Evaluator<R, ADS>*> _v_e, InitialMultiSolution<R, ADS>& _init_pop, int _init_pop_size, int _gMax, RandGen& _rg) :
		v_e(_v_e), init_pop(_init_pop), init_pop_size(_init_pop_size), pDominance(ParetoDominance<R, ADS>(_v_e)), rg(_rg)
	{
		pDominance.insertEvaluators(_v_e);
		gMax = _gMax;
	}

	virtual ~NSGAII()
	{
	}

	virtual void basicGeneticOperators(Population<R, ADS>& p) = 0;

	/*
	virtual void exec(Population<R, ADS>& p, double timelimit, double target_f)
	{
		//ACHO Q FALTA APAGAR ALGUMA COISA NO FINAL

		//vector<vector<Evaluation*> > e_pop;
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
	*/

	//virtual void exec(Population<R, ADS>& p, FitnessValues& e_pop, double timelimit, double target_f)
	virtual Pareto<R, ADS>* search(double timelimit = 100000000, double target_f = 0, Pareto<R, ADS>* _pf = nullptr)
	{
		Timer tnow;

		cout << "exec: Non Sorting Genetic Algorithm Search " << endl;

		Population<R, ADS> p = init_pop.generatePopulation(init_pop_size);
		int N = p.size();

		Population<R, ADS> q = p;
		basicGeneticOperators(q);

		int g = 0;
		while ((g <= gMax) && (tnow.now() < timelimit))
		{
			cout << "Generation = " << g << endl;

			Population<R, ADS> r = p;

			for (int i = 0; i < q.size(); i++)
				r.push_back(q.at(i));

			//Start NonDominance Order by sets
			vector<Population<R, ADS>*> F;
			nonDominanceOrder(F, r);

			Population<R, ADS> popTemp;
			int j = 0;

			vector<double> cD; //Crowding Distance

			while ((popTemp.size() + F[j]->size()) < N)
			{
				crowdingDistanceOrder(cD, *F[j]);

				for (int i = 0; i < F[j]->size(); i++)
					popTemp.push_back(F[j]->at(i));
				j++;
			}

			vector<double> cDTemp;
			crowdingDistanceOrder(cDTemp, *F[j]);

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

			q.clear();
			q = basicSelection(p, cD);

			basicGeneticOperators(q);

			for (int i = 0; i < F.size(); i++)
				F[i]->clear();

			r.clear();

			g++;
		}


		Pareto<R, ADS>* pf = new Pareto<R, ADS>;
		for(unsigned i=0; i<p.size(); i++)
		{
			Solution<R, ADS>* s = &p.at(i);
			vector<Evaluation*> e;
			for(unsigned ev=0; ev<v_e.size(); ev++)
			{
				Evaluator<R, ADS>* evtr = v_e[ev];
				//evtr->evaluate(s);
				Evaluation& e1 = evtr->evaluate(*s);
				e.push_back(&e1);
			}
			pf->push_back(*s, e);
		}
		return pf;
	}

	void crowdingDistanceOrder(vector<double>& CD, const Population<R, ADS>& Fj)
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
					Evaluation& e = v_e[m]->evaluate(Fj.at(i));
					fitness.push_back(make_pair(e.evaluation(), i));
					delete &e;
				}

				sort(fitness.begin(), fitness.end(), compara);

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


	void nonDominanceOrder(vector<Population<R, ADS>*>& F, const Population<R, ADS>& p)
	{

		Population<R, ADS> pAtual = p;
		Population<R, ADS>* F0 = new Population<R, ADS> ;
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

			Population<R, ADS>* uTemp = new Population<R, ADS> ;
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

			nAtual += F[k]->size();

			deleteds.clear();
		}

	}

	virtual Population<R, ADS> basicSelection(const Population<R, ADS>& p, vector<double> cD)
	{
		Population<R, ADS> q;
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

}

#endif /*NONSORTINGGENETICALGORITHMII_HPP_*/
