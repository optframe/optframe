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

#ifndef MULTIOBJECTIVEVNS_HPP_
#define MULTIOBJECTIVEVNS_HPP_

#include <algorithm>

#include "../../MultiObjSearch.hpp"

#include "../../Evaluator.hpp"
#include "../../Population.hpp"
#include "../../NSSeq.hpp"
#include "../../ParetoDominance.hpp"
#include "../../ParetoDominanceWeak.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class MOVNS: public MultiObjSearch<R, ADS>
{
	typedef vector<Evaluation*> FitnessValues;

private:
	vector<NSSeq<R, ADS>*> neighbors;
	vector<Evaluator<R, ADS>*> v_e;
	ParetoDominance<R, ADS> pDominance;
	ParetoDominanceWeak<R, ADS> pDominanceWeak;
	RandGen& rg;
	Pareto<R, ADS> pfMethod;

public:

	MOVNS(vector<Evaluator<R, ADS>*> _v_e, vector<NSSeq<R, ADS>*> _neighbors, RandGen& _rg) :
			v_e(_v_e), neighbors(_neighbors), rg(_rg), pDominance(ParetoDominance<R, ADS>(_v_e)), pDominanceWeak(ParetoDominanceWeak<R, ADS>(_v_e))
	{

	}

	virtual ~MOVNS()
	{
	}

	virtual void exec(Population<R, ADS>& p_0, FitnessValues& e_pop, double timelimit, double target_f)
	{
		Timer tnow;
		cout << "exec: MOVNS" << endl;
		int r = neighbors.size();

		Population<R, ADS> D;
		for (int ind = 0; ind < p_0.size(); ind++)
		{
			Solution<R, ADS>& s = p_0.at(ind).clone();
			if (!pfMethod.addSolution(pDominance, pDominanceWeak, D, s))
				delete &s;
		}
		cout << "Number of Inicial Non-Dominated solutions = " << D.size() << endl;

		vector<bool> visited;
		for (int ind = 0; ind < D.size(); ind++)
			visited.push_back(false);

		while (tnow.now() < timelimit)
		{
			cout << "Conjunto eficiente size = " << D.size() << endl;
			int ind = rg.rand(D.size());
			if (visited[ind] == true)
				ind = rg.rand(D.size());
			visited[ind] = true;

			int neigh = rg.rand(neighbors.size());
			Move<R, ADS>* move = &(neighbors[neigh]->move(D.at(ind)));

			while (!(move->canBeApplied(D.at(ind))))
			{
				delete move;
				move = &(neighbors[neigh]->move(D.at(ind)));
			}

			Solution<R, ADS>& s1 = D.at(ind).clone();
			Move<R, ADS>& mov_rev = move->apply(s1);
			delete &mov_rev;

			NSIterator<R, ADS>& it = neighbors[neigh]->getIterator(s1.getR());
			it.first(); //Primeiro vizinho

			//verifica se existe vizinho a ser gerado
			if (it.isDone())
			{
				delete &it;
			}
			else
			{

				Move<R, ADS>* move = geraMovimentoValido(it, s1);
				//cout << "!it.isDone() = " << !it.isDone() << " aplly = " << move->canBeApplied(p.at(ind)) << endl;
				while ((!it.isDone()) && (move->canBeApplied(s1)))
				{
					Solution<R, ADS>& s2 = s1.clone();
					Move<R, ADS>& mov_rev = move->apply(s2);
					delete &mov_rev;
					delete move;

					bool added = pfMethod.addSolution(pDominance, pDominanceWeak, D, s2);

					if (added)
						cout << "Sol ADCIONADA NA POOL" << endl;
					delete &s2;

					it.next();
					if (!it.isDone())
						move = geraMovimentoValido(it, s1);
				}
			}

			if (it.isDone())
				delete &it;
			delete &s1;

			int verifica = -1;
			for (int v = 0; v < D.size(); v++)
			{
				if (visited[v] == false)
				{
					v = D.size();
					verifica = 0;
				}
			}
			if (verifica == -1)
				for (int v = 0; v < D.size(); v++)
					visited[v] = false;

		}

		p_0 = D;
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

}

#endif /*MULTIOBJECTIVEVNS_HPP_*/
