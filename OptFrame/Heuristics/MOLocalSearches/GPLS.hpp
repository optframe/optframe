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

#ifndef GENERALPARETOLOCALSEARCH_HPP_
#define GENERALPARETOLOCALSEARCH_HPP_

#include <algorithm>

#include "../MultiObjSearch.hpp"
#include "../MOLocalSearch.hpp"
#include "../Evaluator.hpp"
#include "../Population.hpp"
#include "../NSSeq.hpp"
#include "../ParetoDominance.hpp"
#include "../ParetoDominanceWeak.hpp"
#include "../InitialPopulation.h"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class paretoManager2PPLS: public paretoManager<R, ADS>
{
private:
	int r;

public:
	pair<Pareto<R, ADS>, vector<vector<bool> > > x_e;
	Pareto<R, ADS> p_a;

	paretoManager2PPLS(MultiEvaluator<R, ADS>& _mev, int _r) :
			paretoManager<R, ADS>(_mev), r(_r)
	{

	}

	virtual ~paretoManager2PPLS()
	{
	}

	//Special addSolution used in the 2PPLS speedUp
	bool addSolution(Solution<R, ADS>* candidate, MultiEvaluation* mev)
	{
		vector<Evaluation*> fitnessNewInd = mev->getVector();

		bool added = true;
		for (int ind = 0; ind < x_e.first.size(); ind++)
		{
			vector<Evaluation*> popIndFitness = x_e.first.getIndEvaluations(ind);

			if (paretoManager<R, ADS>::domWeak.dominates(popIndFitness, fitnessNewInd))
				return false;

			if (paretoManager<R, ADS>::dom.dominates(fitnessNewInd, popIndFitness))
			{
				x_e.first.remove(ind);
				x_e.second.erase(x_e.second.begin() + ind);
				ind--;
			}

		}

		if (added == true)
		{
			x_e.first.push_back(candidate, fitnessNewInd);
			vector<bool> neigh;
			for (int n = 0; n < r; n++)
				neigh.push_back(false);
			x_e.second.push_back(neigh);

			paretoManager<R, ADS>::addSolution(p_a, candidate, mev);
		}

		return added;
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class GeneralParetoLocalSearch: public MultiObjSearch<R, ADS>
{
	typedef vector<Evaluation*> FitnessValues;

private:
	MultiEvaluator<R, ADS>& multiEval;
	InitialPopulation<R, ADS>& init_pop;
	int init_pop_size;
	vector<MOLocalSearch<R, ADS>*> vLS;
	paretoManager2PPLS<R, ADS> pMan2PPLS;

public:

	GeneralParetoLocalSearch(MultiEvaluator<R, ADS>& _mev, InitialPopulation<R, ADS>& _init_pop, int _init_pop_size, vector<MOLocalSearch<R, ADS>*> _vLS) :
			multiEval(_mev), init_pop(_init_pop), init_pop_size(_init_pop_size), vLS(_vLS), pMan2PPLS(paretoManager2PPLS<R, ADS>(_mev, _vLS.size()))
	{

	}

	virtual ~GeneralParetoLocalSearch()
	{
	}

	//virtual void exec(Population<R, ADS>& p_0, FitnessValues& e_pop, double timelimit, double target_f)
	virtual Pareto<R, ADS>* search(double timelimit = 100000000, double target_f = 0, Pareto<R, ADS>* _pf = NULL)
	{
		Timer tnow;

		cout << "exec: Two Phase Pareto Local Search with Pareto Manager (tL:" << timelimit << ")" << endl;
		int r = vLS.size();

		Pareto<R, ADS> p_0;
		Pareto<R, ADS> p;

		if (_pf == NULL)
		{
			cout << "Creating initial population using a constructive method..." << endl;
			Population<R, ADS> tempPop = init_pop.generatePopulation(init_pop_size);

			for (int ind = 0; ind < tempPop.size(); ind++)
			{
				Solution<R, ADS>* s = &tempPop.at(ind).clone();
				MultiEvaluation& mev = multiEval.evaluate(*s);
				pMan2PPLS.paretoManager<R, ADS>::addSolution(p_0, s, &mev);

				delete s;

			}

			cout << "Population generated with " << p_0.size() << " individuals" << endl;
			//			return new Pareto<R, ADS>(p_0);
		}
		else
		{
			cout << "Extracting Pareto _pf ..." << endl;
			//			vector<Solution<R, ADS>*> tempPop = _pf->getParetoSet();
			//			for (int i = 0; i < tempPop.size(); i++)
			//				p_0.push_back(tempPop[i]);
			p_0 = *_pf;
			cout << "Population extracted with " << p_0.size() << " individuals" << endl;
		}

		for (int ind = 0; ind < p_0.size(); ind++)
		{
			Solution<R, ADS>* s = &p_0.getNonDominatedSol(ind).clone();
			pMan2PPLS.paretoManager<R, ADS>::addSolution(s);
			delete s;
		}

		//It should be noticed why it does not work with the following codes
//		pMan2PPLS.x_e.first.clear();
//		pMan2PPLS.x_e.second.clear();
//		pMan2PPLS.x_e.first = p_0;
//		cout << pMan2PPLS.x_e.second << endl;
//		for (int i = 0; i < pMan2PPLS.x_e.first.size(); i++)
//		{
//			vector<bool> neigh;
//			for (int n = 0; n < r; n++)
//				neigh.push_back(false);
//			pMan2PPLS.x_e.second.push_back(neigh);
//		}

		p = pMan2PPLS.x_e.first;
		p_0.clear();
		pMan2PPLS.p_a.clear();

		cout << "Number of Inicial x_e non-dominated solutions = " << pMan2PPLS.x_e.first.size() << endl;

		int k = 1;

		while ((k <= r) && (tnow.now() < timelimit))
		{
			cout << "k = " << k << endl;

			//Marca como visitados todos os vizinhos que serao visitados
			for (int pareto = 0; pareto < pMan2PPLS.x_e.first.size(); pareto++)
			{
				int lastAdded = pMan2PPLS.x_e.second.size() - pareto - 1;
				pMan2PPLS.x_e.second[lastAdded][k - 1] = true;
			}

			//Gera todos os vizinhos de cada individuo da populacao
			for (int ind = 0; ind < p.size(); ind++)
			{
				Solution<R, ADS>* s = &p.getNonDominatedSol(ind).clone();
				vLS[k - 1]->exec(*s, pMan2PPLS, timelimit - tnow.now(), target_f);

				//				bool added = p.addSolution(pDominance, pDominanceWeak, x_e, s, r);
				//
				//				if (added)
				//					p.addSolution(pDominance, pDominanceWeak, p_a, s);

				delete s;
			}

			cout << "p_a.size() = " << pMan2PPLS.p_a.size();
			cout << " , x_e.first.size() = " << pMan2PPLS.x_e.first.size() << endl;

			if (pMan2PPLS.p_a.size() != 0)
			{
				p.clear();  // verificar se pode-se limpar a populacao p
				p = pMan2PPLS.p_a;
				pMan2PPLS.p_a.clear();
				k = 1;
			}
			else
			{
				k++;
				p.clear(); // verificar se pode-se limpar a populacao p
				p = pMan2PPLS.x_e.first;

				//speed-up - Thibauuuut Lust - Nice guy
				if (k <= r)
				{

					int removed = 0;
					for (int i = 0; i < pMan2PPLS.x_e.second.size(); i++)
					{
						if (pMan2PPLS.x_e.second[i][k - 1] == true)
						{
							p.remove(i - removed);
							removed++;
						}
					}
				}
				//end of the speed-up
			}
		}

		//		p = x_e.first;

		//		for (int i = 0; i < p_0.size(); i++)
		//		{
		//			for (int j = 0; j < p_0.size(); j++)
		//			{
		//				if ((i != j) && pDominanceWeak.dominates(p_0.at(i), p_0.at(j)))
		//				{
		//					cout << "ERRO DOMINANCIA" << endl;
		//					getchar();
		//				}
		//			}
		//		}
		//
		//		vector<Evaluator<R, ADS>*>* _v_e = mev.getEvaluators();
		//		if (!_v_e)
		//		{
		//			cout << "2PPLS::search error: not using separated evaluators!" << endl;
		//			exit(1);
		//		}
		//
		//		vector<Evaluator<R, ADS>*> v_e(*_v_e);
		//		delete _v_e;

		//		Pareto<R, ADS>* pf = new Pareto<R, ADS>;
		//
		//		for (unsigned i = 0; i < p_0.size(); i++)
		//		{
		//			Solution<R, ADS>* s = &p_0.at(i);
		//
		//			vector<Evaluation*> e;
		//			for (unsigned ev = 0; ev < v_e.size(); ev++)
		//			{
		//				Evaluator<R, ADS>* evtr = v_e[ev];
		//				//evtr->evaluate(s);
		//				Evaluation& e1 = evtr->evaluate(*s);
		//				e.push_back(&e1);
		//			}
		//			pf->push_back(s, e);
		//		}

		cout << "Two Phase Pareto Local Search Finished" << endl;

		return new Pareto<R, ADS>(pMan2PPLS.x_e.first);
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
			{
				move = &it.current();
			}
			else
			{
//				cout << "finished" << endl;
//				getchar();
				return NULL;
			}
		}

		return move;
	}

};

#endif /*GENERALPARETOLOCALSEARCH_HPP_*/

