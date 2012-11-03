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

#include "../HTrajectory.hpp"
#include "../Evaluator.hpp"
#include "../Population.hpp"
#include "../NSSeq.hpp"
#include "../ParetoDominance.hpp"
#include "../ParetoDominanceWeak.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class MOVNSLevels: public HTrajectory<R, ADS, M>
{
	typedef vector<Evaluation<M>*> FitnessValues;

private:
	vector<NSSeq<R, ADS, M>*> neighbors;
	vector<Evaluator<R, ADS, M>*> v_e;
	ParetoDominance<R, ADS, M> pDominance;
	ParetoDominanceWeak<R, ADS, M> pDominanceWeak;
	RandGen& rg;
	int levelMax;
	int iterMax;

public:
	using HTrajectory<R, ADS, M>::exec; // prevents name hiding

	MOVNSLevels(vector<Evaluator<R, ADS, M>*> _v_e, vector<NSSeq<R, ADS, M>*> _neighbors, RandGen& _rg, int _iterMax, int _levelMax) :
		v_e(_v_e), neighbors(_neighbors), rg(_rg)
	{
		pDominance.insertEvaluators(_v_e);
		pDominanceWeak.insertEvaluators(_v_e);
		levelMax = _levelMax;
		iterMax = _iterMax;
	}

	virtual ~MOVNSLevels()
	{
	}

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

	virtual void exec(Population<R, ADS>& p_0, FitnessValues& e_pop, double timelimit, double target_f)
	{
		Timer tnow;
		cout << "exec: MOVNS " << endl;
		int r = neighbors.size();

		Population<R, ADS> D;
		for (int ind = 0; ind < p_0.size(); ind++)
		{
			Solution<R, ADS>& s = p_0.at(ind).clone();
			if (!addSolution(D, s))
				delete &s;
		}

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
				Move<R, ADS, M>* move = &(neighbors[neigh]->move(s1));

				while (!(move->canBeApplied(s1)))
				{
					delete move;
					move = &(neighbors[neigh]->move(s1));
				}

				Move<R, ADS, M>& mov_rev = move->apply(s1);
				delete &mov_rev;
				delete move;
			}

			int neigh = rg.rand(neighbors.size());

			NSIterator<R, ADS, M>& it = neighbors[neigh]->getIterator(s1.getR());
			it.first();//Primeiro vizinho

			//verifica se existe vizinho a ser gerado
			if (it.isDone())
			{
				delete &it;
			}
			else
			{

				Move<R, ADS, M>* move = geraMovimentoValido(it, s1);
				//cout << "!it.isDone() = " << !it.isDone() << " aplly = " << move->canBeApplied(p.at(ind)) << endl;
				while ((!it.isDone()) && (move->canBeApplied(s1)))
				{
					Solution<R, ADS>& s2 = s1.clone();
					Move<R, ADS, M>& mov_rev = move->apply(s2);
					delete &mov_rev;
					delete move;

					bool added = addSolution(D, s2);
					if (added)
					{
						cout << "Sol ADCIONADA NA POOL" << endl;
						cout << "Conjunto eficiente size = " << D.size() << endl;
						perturbation = 1;
						perturbationLevel = 1;
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
	}

	Move<R, ADS, M>* geraMovimentoValido(NSIterator<R, ADS, M>& it, Solution<R, ADS>& s)
	{

		Move<R, ADS, M>* move = NULL;

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

	bool addSolution(Population<R, ADS>& p, Solution<R, ADS>& s)
	{
		Evaluation<M>& e = v_e[0]->evaluate(s);
		if (!e.isFeasible())
		{
			delete &e;
			return false;
		}
		delete &e;

		bool added = true;
		for (int ind = 0; ind < p.size(); ind++)
		{

			if (pDominanceWeak.dominates(p.at(ind), s))
				return false;

			if (pDominance.dominates(s, p.at(ind)))
				delete &p.remove(ind);

		}
		if (added == true)
			p.push_back(s);

		return added;
	}

};

#endif /*MULTIOBJECTIVEVNSLEVELS_HPP_*/
