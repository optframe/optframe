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

#ifndef TWOPHASEPARETOLOCALSEARCHPPLS_HPP_
#define TWOPHASEPARETOLOCALSEARCHPPLS_HPP_

#include <algorithm>

#include "../HTrajectory.hpp"
#include "../Evaluator.hpp"
#include "../Population.hpp"
#include "../NSSeq.hpp"
#include "../ParetoDominance.hpp"
#include "../ParetoDominanceWeak.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class TwoPhaseParetoLocalSearch: public HTrajectory<R, ADS, M>
{
	typedef vector<Evaluation<M>*> FitnessValues;

private:
	vector<NSSeq<R, ADS, M>*> neighbors;
	vector<Evaluator<R, ADS, M>*> v_e;
	ParetoDominance<R, ADS, M> pDominance;
	ParetoDominanceWeak<R, ADS, M> pDominanceWeak;

public:
	using HTrajectory<R, ADS, M>::exec; // prevents name hiding

	TwoPhaseParetoLocalSearch(vector<Evaluator<R, ADS, M>*> _v_e, vector<NSSeq<R, ADS, M>*> _neighbors) :
		v_e(_v_e), neighbors(_neighbors)
	{
		pDominance.insertEvaluators(_v_e);
		pDominanceWeak.insertEvaluators(_v_e);
	}

	virtual ~TwoPhaseParetoLocalSearch()
	{
	}

	virtual void exec(Population<R, ADS>& p, double timelimit, double target_f)
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

	virtual void exec(Population<R, ADS>& p_0, FitnessValues& e_pop, double timelimit, double target_f)
	{
		long tini = time(NULL);
		cout << "exec: Two Phase Pareto Local Search " << endl;
		int r = neighbors.size();

		Population<R, ADS> p = p_0;
		Population<R, ADS> p_a;
		pair<Population<R, ADS> , vector<vector<bool> > > x_e;

		//x_e.first = p_0;

		for (int ind = 0; ind < p_0.size(); ind++)
		{
			Solution<R, ADS>& s = p_0.at(ind).clone();
			if (!addSolution(x_e.first, s))
				delete &s;

		}

		cout << "Number of Inicial Non-Dominated solutions = " << x_e.first.size()<<endl;

		for (int i = 0; i < x_e.first.size(); i++)
		{
			vector<bool> neigh;
			neigh.push_back(true);
			for (int n = 1; n < r; n++)
				neigh.push_back(false);
			x_e.second.push_back(neigh);
		}

		int k = 1;
		while (k <= r)
		{
			cout << "k = " << k << endl;

			//Marca com visitados todos os vizinhos adcionados recentemente
			for (int pareto = 0; pareto < x_e.first.size(); pareto++)
			{
				int lastAdded = x_e.second.size() - pareto - 1;
				x_e.second[lastAdded][k - 1] = true;
			}

			//Gera todos os vizinhos de cada individuo da populacao
			for (int ind = 0; ind < p.size(); ind++)
			{

				NSIterator<R, ADS, M>& it = neighbors[k - 1]->getIterator(p.at(0).getR());
				it.first();//Primeiro vizinho

				//verifica se existe vizinho a ser gerado
				if (it.isDone())
				{
					delete &it;
				}
				else
				{

					Move<R, ADS, M>* move = geraMovimentoValido(it, p.at(ind));
					//cout << "!it.isDone() = " << !it.isDone() << " aplly = " << move->canBeApplied(p.at(ind)) << endl;
					while ((!it.isDone()) && (move->canBeApplied(p.at(ind))))
					{

						Solution<R, ADS>& s = p.at(ind).clone();
						Move<R, ADS, M>& mov_rev = move->apply(s);

						delete &mov_rev;
						delete move;

						if (!pDominanceWeak.dominates(p.at(ind), s))
						{
							bool added = addSolution(x_e, s);

							if (added)
								addSolution(p_a, s);
						}

						delete &s;

						it.next();
						if (!it.isDone())
							move = geraMovimentoValido(it, p.at(ind), ind);

					}
				}

				if (it.isDone())
					delete &it;

			}
			cout << "p_a.size() = " << p_a.size() << endl;
			//getchar();

			if (p_a.size() != 0)
			{
				p = p_a;
				p_a.clear();
				k = 1;
			}
			else
			{
				k++;
				p = x_e.first;

				//speed-up
				if (k <= r)
				{

					int removed = 0;
					for (int i = 0; i < x_e.second.size(); i++)
					{
						if (x_e.second[i][k - 1] == true)
						{
							delete &p.remove(i - removed);
							removed++;
						}
					}
				}
				//end of the speed-up
			}
		}

		p_0 = x_e.first;

		for (int i = 0; i < p_0.size(); i++)
		{
			for (int j = 0; j < p_0.size(); j++)
			{
				if ((i != j) && pDominanceWeak.dominates(p_0.at(i), p_0.at(j)))
				{
					cout << "ERRO DOMINANCIA" << endl;
					getchar();
				}
			}
		}

	}

	Move<R, ADS, M>* geraMovimentoValido(NSIterator<R, ADS, M>& it, Solution<R, ADS>& s, int ind)
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
			{
				move = &it.current();
			}
			else
			{
				delete &it;
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
				delete &p.remove(ind); //Esta perdendo o individuo retornado,tem problema? todo

		}
		if (added == true)
			p.push_back(s);

		return added;

	}

	bool addSolution(pair<Population<R, ADS> , vector<vector<bool> > >& p, Solution<R, ADS>& s)
	{
		Evaluation<M>& e = v_e[0]->evaluate(s);
		if (!e.isFeasible())
		{
			delete &e;
			return false;
		}
		delete &e;

		bool added = true;
		for (int ind = 0; ind < p.first.size(); ind++)
		{

			if (pDominanceWeak.dominates(p.first.at(ind), s))
				return false;

			if (pDominance.dominates(s, p.first.at(ind)))
			{
				delete &p.first.remove(ind);
				p.second.erase(p.second.begin() + ind);
				ind--;
			}
		}
		if (added == true)
		{
			p.first.push_back(s);
			vector<bool> neigh;
			for (int n = 0; n < neighbors.size(); n++)
				neigh.push_back(false);
			p.second.push_back(neigh);
		}

		return added;

	}

};

#endif /*TWOPHASEPARETOLOCALSEARCHPPLS_HPP_*/
