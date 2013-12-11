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

#include "../MultiObjSearch.hpp"
#include "../Evaluator.hpp"
#include "../Population.hpp"
#include "../NSSeq.hpp"
#include "../ParetoDominance.hpp"
#include "../ParetoDominanceWeak.hpp"
#include "../InitialPopulation.h"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class TwoPhaseParetoLocalSearch: public MultiObjSearch<R, ADS, DS>
{
	typedef vector<Evaluation<DS>*> FitnessValues;

private:
	vector<NSSeq<R, ADS, DS>*> neighbors;
	MultiEvaluator<R, ADS, DS>& mev;
	InitialPopulation<R, ADS>& init_pop;
	int init_pop_size;
	ParetoDominance<R, ADS, DS> pDominance;
	ParetoDominanceWeak<R, ADS, DS> pDominanceWeak;

public:

	TwoPhaseParetoLocalSearch(MultiEvaluator<R, ADS, DS>& _mev, InitialPopulation<R, ADS>& _init_pop, int _init_pop_size, vector<NSSeq<R, ADS, DS>*> _neighbors) :
			mev(_mev), init_pop(_init_pop), init_pop_size(_init_pop_size), neighbors(_neighbors), pDominance(ParetoDominance<R, ADS, DS>(*_mev.getEvaluators())), pDominanceWeak(ParetoDominanceWeak<R, ADS, DS>(*_mev.getEvaluators()))
	{
		////pDominance.insertEvaluators(*_mev.getEvaluators());
		////pDominanceWeak.insertEvaluators(*_mev.getEvaluators());
	}

	virtual ~TwoPhaseParetoLocalSearch()
	{
	}

	/*
	virtual void exec(Population<R, ADS>& p, double timelimit, double target_f)
	{
		//ACHO Q FALTA APAGAR ALGUMA COISA NO FINAL

		//vector<vector<Evaluation<DS>*> > e_pop;
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


	//virtual void exec(Population<R, ADS>& p_0, FitnessValues& e_pop, double timelimit, double target_f)
	virtual Pareto<R, ADS, DS>* search(double timelimit = 100000000, double target_f = 0, Pareto<R, ADS, DS>* _pf = NULL)
	{
		long tini = time(NULL);
		cout << "exec: Two Phase Pareto Local Search " << endl;
		int r = neighbors.size();

		Population<R, ADS> p_0 = init_pop.generatePopulation(init_pop_size);

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

		cout << "Number of Inicial Non-Dominated solutions = " << x_e.first.size();
		cout <<endl;

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

				NSIterator<R, ADS, DS>& it = neighbors[k - 1]->getIterator(p.at(0));
				it.first();//Primeiro vizinho

				//verifica se existe vizinho a ser gerado
				if (it.isDone())
				{
					delete &it;
				}
				else
				{

					Move<R, ADS, DS>* move = geraMovimentoValido(it, p.at(ind));
					//cout << "!it.isDone() = " << !it.isDone() << " aplly = " << move->canBeApplied(p.at(ind)) << endl;
					while ((!it.isDone()) && (move->canBeApplied(p.at(ind))))
					{

						Solution<R, ADS>& s = p.at(ind).clone();
						Move<R, ADS, DS>& mov_rev = move->apply(s);

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
			cout << "p_a.size() = " << p_a.size();
			cout << endl;
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

		vector<Evaluator<R, ADS, DS>*>* _v_e = mev.getEvaluators();
		if (!_v_e)
		{
			cout << "2PPLS::search error: not using separated evaluators!" << endl;
			exit(1);
		}
		vector<Evaluator<R, ADS, DS>*> v_e(*_v_e);
		delete _v_e;

		Pareto<R, ADS, DS>* pf = new Pareto<R, ADS, DS>;
		for(unsigned i=0; i<p_0.size(); i++)
		{
			Solution<R, ADS>* s = &p_0.at(i);
			vector<Evaluation<DS>*> e;
			for(unsigned ev=0; ev<v_e.size(); ev++)
			{
				Evaluator<R, ADS, DS>* evtr = v_e[ev];
				//evtr->evaluate(s);
				Evaluation<DS>& e1 = evtr->evaluate(*s);
				e.push_back(&e1);
			}
			pf->push_back(*s, e);
		}
		return pf;
	}

	Move<R, ADS, DS>* geraMovimentoValido(NSIterator<R, ADS, DS>& it, Solution<R, ADS>& s, int ind)
	{

		Move<R, ADS, DS>* move = NULL;

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

	Move<R, ADS, DS>* geraMovimentoValido(NSIterator<R, ADS, DS>& it, Solution<R, ADS>& s)
	{

		Move<R, ADS, DS>* move = NULL;

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
		vector<Evaluator<R, ADS, DS>*>* _v_e = mev.getEvaluators();
		if (!_v_e)
		{
			cout << "2PPLS::addSolution (1) error: not using separated evaluators!" << endl;
			exit(1);
		}
		vector<Evaluator<R, ADS, DS>*> v_e(*_v_e);
		delete _v_e;

		Evaluation<DS>& e = v_e[0]->evaluate(s);
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
		vector<Evaluator<R, ADS, DS>*>* _v_e = mev.getEvaluators();
		if (!_v_e)
		{
			cout << "2PPLS::addSolution (2) error: not using separated evaluators!" << endl;
			exit(1);
		}
		vector<Evaluator<R, ADS, DS>*> v_e(*_v_e);
		delete _v_e;

		Evaluation<DS>& e = v_e[0]->evaluate(s);
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
