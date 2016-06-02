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

#ifndef MULTIOBJECTIVEVNSLEVELS_HPP_
#define MULTIOBJECTIVEVNSLEVELS_HPP_

#include <algorithm>

#include "../../MultiObjSearch.hpp"
#include "../../Evaluator.hpp"
#include "../../Population.hpp"
#include "../../NSSeq.hpp"
#include "../../ParetoDominance.hpp"
#include "../../ParetoDominanceWeak.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class MOVNSLevels: public MultiObjSearch<R, ADS>
{
	typedef vector<Evaluation*> FitnessValues;

private:
	vector<NSSeq<R, ADS>*> neighbors;
	vector<Evaluator<R, ADS>*> v_e;
	InitialPopulation<R, ADS>& init_pop;
	int init_pop_size;

	ParetoDominance<R, ADS> pDominance;
	ParetoDominanceWeak<R, ADS> pDominanceWeak;
	RandGen& rg;
	int levelMax;
	int iterMax;
	Pareto<R, ADS> pfMethod;

public:
	//using HTrajectory<R, ADS>::exec; // prevents name hiding

	MOVNSLevels(vector<Evaluator<R, ADS>*> _v_e, InitialPopulation<R, ADS>& _init_pop, int _init_pop_size, vector<NSSeq<R, ADS>*> _neighbors, RandGen& _rg, int _iterMax, int _levelMax) :
			v_e(_v_e), init_pop(_init_pop), init_pop_size(_init_pop_size), neighbors(_neighbors), rg(_rg), pDominance(ParetoDominance<R, ADS>(_v_e)), pDominanceWeak(ParetoDominanceWeak<R, ADS>(_v_e))
	{
		levelMax = _levelMax;
		iterMax = _iterMax;
	}

	virtual ~MOVNSLevels()
	{
	}

	/*
	 virtual void exec(Population<R, ADS>& p, double timelimit, double target_f)
	 {
	 FitnessValues& e_pop = *new FitnessValues;

	 for (int i = 0; i < p.size(); i++)
	 e_pop.push_back(&v_e[0]->evaluate(p.at(i)));

	 exec(p, e_pop, timelimit, target_f);

	 for (int i = 0; i < e_pop.size(); i++)
	 delete e_pop[i];

	 delete &e_pop;
	 }
	 */

	//virtual void exec(Population<R, ADS>& p_0, FitnessValues& e_pop, double timelimit, double target_f)
	virtual Pareto<R, ADS>* search(double timelimit = 100000000, double target_f = 0, Pareto<R, ADS>* _pf = NULL)
	{
		Timer tnow;
		cout << "exec: MOVNS " << endl;
		int r = neighbors.size();

		Population<R, ADS> p_0 = init_pop.generatePopulation(init_pop_size);

		cout << "population generated" << endl;

		Population<R, ADS> D;
		for (int ind = 0; ind < p_0.size(); ind++)
		{
			Solution<R, ADS>& s = p_0.at(ind).clone();
			if (!pfMethod.addSolution(pDominance, pDominanceWeak, D, s))
				delete &s;
		}
		cout << "Initial efficient set size = " << D.size() << endl;
		vector<bool> visited;
		for (int ind = 0; ind < D.size(); ind++)
			visited.push_back(false);

		int perturbation = 1;
		int perturbationLevel = 1;

		while (tnow.now() < timelimit)
		{
			//cout << "Conjunto eficiente size = " << D.size() << endl;
			int ind = rg.rand(D.size());
			if (visited[ind] == true)
				ind = rg.rand(D.size());
			visited[ind] = true;

			if (perturbation == iterMax)
			{
				if (perturbationLevel < levelMax)
					perturbationLevel++;
				cout << "perturbationLevel = " << perturbationLevel << endl;
			}

			Solution<R, ADS>& s1 = D.at(ind).clone();
			for (int m = 0; m < perturbationLevel; m++)
			{
				int neigh = rg.rand(neighbors.size());
				Move<R, ADS>* move = &(neighbors[neigh]->move(s1));

				int maxTries = 500000;
				int tries = 0;
				//cout<<"trying to find move"<<endl;
				while (!(move->canBeApplied(s1)) && tnow.now() < timelimit && tries < maxTries)
				{
					delete move;
					move = &(neighbors[neigh]->move(s1));
					tries++;
				}
				//cout<<"found to find move"<<endl;
				if (tries < maxTries)
				{
					Move<R, ADS>* mov_rev = move->apply(s1);
					delete mov_rev;
				}
				delete move;
			}

			int neigh = rg.rand(neighbors.size());
			//cout<<"Current neighborhood is: " <<neigh<<endl;
			NSIterator<R, ADS>& it = neighbors[neigh]->getIterator(s1);
			it.first();		//Primeiro vizinho

			//verifica se existe vizinho a ser gerado
			if (it.isDone())
			{
				delete &it;
			}
			else
			{

				Move<R, ADS>* move = geraMovimentoValido(it, s1);
				//cout << "!it.isDone() = " << !it.isDone() << " aplly = " << move->canBeApplied(p.at(ind)) << endl;
				int nMoves = 0;
				while ((!it.isDone()) && (move->canBeApplied(s1)) && tnow.now() < timelimit)
				{
					nMoves++;
					if ((nMoves % 50000) == 0)
					{
						cout << "Iterator Moves = " << nMoves << endl;
					}
					Solution<R, ADS>& s2 = s1.clone();
					Move<R, ADS>* mov_rev = move->apply(s2);
					delete mov_rev;
					delete move;

					bool added = pfMethod.addSolution(pDominance, pDominanceWeak, D, s2);
					if (added)
					{
						cout << "New solution added to the pareto front! \t ";
						cout << "Efficient set size = " << D.size() << endl;

						perturbation = 1;
						perturbationLevel = 1;
						//getchar();
					}
					delete &s2;

					it.next();
					if (!it.isDone())
						move = geraMovimentoValido(it, s1);
				}
			}

			perturbation++;

			if (it.isDone())
				delete &it;
			delete &s1;

			int verifica = -1;
			for (int v = 0; v < D.size(); v++)
			{
				if (visited[v] == false)
				{
					v = D.size(); //sai do loop
					verifica = 0;
				}
			}
			if (verifica == -1)
				for (int v = 0; v < D.size(); v++)
					visited[v] = false;

		}

		p_0 = D;

//		cout << "finishing" << endl;
//		for (int i = 0; i < p_0.size(); i++)
//		{
//			vector<double> indFitness = p_0.getFitness(i);
//			//cout << indFitness[0] << "\t" << indFitness[1] << endl;
//			cout << indFitness << endl;
//		}
//		getchar();

		Pareto<R, ADS>* pf = new Pareto<R, ADS>;
		for (unsigned i = 0; i < p_0.size(); i++)
		{
			Solution<R, ADS>* s = &p_0.at(i);
			vector<Evaluation*> e;
			for (unsigned ev = 0; ev < v_e.size(); ev++)
			{
				Evaluator<R, ADS>* evtr = v_e[ev];
				//evtr->evaluate(s);
				Evaluation& e1 = evtr->evaluate(*s);
				e.push_back(&e1);
			}
			pf->push_back(s, e);
		}
		return pf;
	}

	Move<R, ADS>* geraMovimentoValido(NSIterator<R, ADS>& it, Solution<R, ADS>& s)
	{

		Move<R, ADS>* move = NULL;

		if (it.isDone())
			return NULL;
		else
			move = &it.current();

		while (!move->canBeApplied(s))
		{
			delete move;
			it.next();

			if (!it.isDone())
				move = &it.current();
			else
			{
				return NULL;
			}
		}

		return move;
	}

};

#endif /*MULTIOBJECTIVEVNSLEVELS_HPP_*/
