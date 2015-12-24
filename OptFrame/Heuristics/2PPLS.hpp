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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class TwoPhaseParetoLocalSearch: public MultiObjSearch<R, ADS>
{
	typedef vector<Evaluation*> FitnessValues;

private:
	vector<NSSeq<R, ADS>*> neighbors;
	MultiEvaluator<R, ADS>& mev;
	InitialPopulation<R, ADS>& init_pop;
	int init_pop_size;
	ParetoDominance<R, ADS> pDominance;
	ParetoDominanceWeak<R, ADS> pDominanceWeak;

public:

	TwoPhaseParetoLocalSearch(MultiEvaluator<R, ADS>& _mev, InitialPopulation<R, ADS>& _init_pop, int _init_pop_size, vector<NSSeq<R, ADS>*> _neighbors) :
			mev(_mev), init_pop(_init_pop), init_pop_size(_init_pop_size), neighbors(_neighbors), pDominance(ParetoDominance<R, ADS>(*_mev.getEvaluators())), pDominanceWeak(ParetoDominanceWeak<R, ADS>(*_mev.getEvaluators()))
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

	//virtual void exec(Population<R, ADS>& p_0, FitnessValues& e_pop, double timelimit, double target_f)
	virtual Pareto<R, ADS>* search(double timelimit = 100000000, double target_f = 0, Pareto<R, ADS>* _pf = NULL)
	{
		Timer tnow;

		cout << "exec: Two Phase Pareto Local Search " << endl;
		int r = neighbors.size();

		Population<R, ADS> p_0 = init_pop.generatePopulation(init_pop_size);

		pair<Population<R, ADS>, vector<vector<bool> > > x_e;
		for (int ind = 0; ind < p_0.size(); ind++)
		{
			Solution<R, ADS>& s = p_0.at(ind).clone();
			if (!addSolution(x_e, s))
				delete &s;
		}

		Population<R, ADS> p = x_e.first;
		Population<R, ADS> p_a;

		cout << "Number of Inicial Non-Dominated solutions = " << x_e.first.size() << endl;

//		for (int i = 0; i < x_e.first.size(); i++)
//		{
//			vector<bool> neigh;
//			neigh.push_back(true);
//			for (int n = 1; n < r; n++)
//				neigh.push_back(false);
//			x_e.second.push_back(neigh);
//		}

		int k = 1;
		while ((k <= r) && (tnow.now() < timelimit))
		{
			cout << "k = " << k << endl;

			//Marca como visitados todos os vizinhos adcionados recentemente
			for (int pareto = 0; pareto < x_e.first.size(); pareto++)
			{
				int lastAdded = x_e.second.size() - pareto - 1;
				x_e.second[lastAdded][k - 1] = true;
			}

			//Gera todos os vizinhos de cada individuo da populacao
			for (int ind = 0; ind < p.size(); ind++)
			{
				//cout<<"2PPLS ind= "<<ind<<endl;

				NSIterator<R, ADS>& it = neighbors[k - 1]->getIterator(p.at(ind));
				it.first();		//Primeiro vizinho

				//verifica se existe vizinho a ser gerado
				if (it.isDone())
				{
					delete &it;
				}
				else
				{
					Move<R, ADS>* move = geraMovimentoValido(it, p.at(ind));

					while ((!it.isDone()) && (move->canBeApplied(p.at(ind))))
					{
						Solution<R, ADS>& s = p.at(ind).clone();
						Move<R, ADS>* mov_rev = move->apply(s);

						delete mov_rev;
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
							move = geraMovimentoValido(it, p.at(ind));

					}
				}

				if (it.isDone())
					delete &it;

			}

			cout << "p_a.size() = " << p_a.size();
			cout << " , x_e.first.size() = " << x_e.first.size() << endl;

			/*if (x_e.first.size() > 50)
			 {
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

			 vector<Evaluator<R, ADS>*>* _v_e = mev.getEvaluators();
			 if (!_v_e)
			 {
			 cout << "2PPLS::search error: not using separated evaluators!" << endl;
			 exit(1);
			 }

			 vector<Evaluator<R, ADS>*> v_e(*_v_e);
			 delete _v_e;

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
			 pf->push_back(*s, e);
			 }

			 vector<vector<Evaluation<>*> > vEval = pf->getParetoFront();
			 cout << "MO optimization finished! Printing Pareto Front!" << endl;
			 for (int i = 0; i < vEval.size(); i++)
			 {
			 cout << vEval[i][0]->getObjFunction() << "\t" << vEval[i][1]->getObjFunction() << endl;
			 }

			 }*/

			if (p_a.size() != 0)
			{
				p.clear();  // verificar se pode-se limpar a populacao p
				p = p_a;
				p_a.clear();
				k = 1;
			}
			else
			{
				k++;
				p.clear(); // verificar se pode-se limpar a populacao p
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

		vector<Evaluator<R, ADS>*>* _v_e = mev.getEvaluators();
		if (!_v_e)
		{
			cout << "2PPLS::search error: not using separated evaluators!" << endl;
			exit(1);
		}

		vector<Evaluator<R, ADS>*> v_e(*_v_e);
		delete _v_e;

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
			pf->push_back(*s, e);
		}

		cout << "Two Phase Pareto Local Search Finished" << endl;

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

	bool addSolution(Population<R, ADS>& p, Solution<R, ADS>& s)
	{
		vector<Evaluator<R, ADS>*>* _v_e = mev.getEvaluators();
		if (!_v_e)
		{
			cout << "2PPLS::addSolution (2) error: not using separated evaluators!" << endl;
			exit(1);
		}
		vector<Evaluator<R, ADS>*> v_e(*_v_e);
		delete _v_e;

		vector<double> fitnessNewInd;

		for (int evalIndex = 0; evalIndex < v_e.size(); evalIndex++)
		{
			Evaluation &e = v_e[evalIndex]->evaluate(s);

			if (!e.isFeasible())
			{
				delete &e;
				return false;
			}

			fitnessNewInd.push_back(e.evaluation());
			delete &e;
		}

		bool added = true;
		for (int ind = 0; ind < p.size(); ind++)
		{

			vector<double> popIndFitness = p.getFitness(ind);
			if (pDominanceWeak.dominates(popIndFitness, fitnessNewInd))
				return false;

			if (pDominance.dominates(fitnessNewInd, popIndFitness))
				delete &p.remove(ind);

//			if (pDominanceWeak.dominates(p.at(ind), s))
//				return false;
//
//			if (pDominance.dominates(s, p.at(ind)))
//				delete &p.remove(ind);

		}
		if (added == true)
			p.push_back(s, fitnessNewInd);

		return added;
	}

	bool addSolution(pair<Population<R, ADS>, vector<vector<bool> > >& p, Solution<R, ADS>& s)
	{

		vector<Evaluator<R, ADS>*>* _v_e = mev.getEvaluators();
		if (!_v_e)
		{
			cout << "2PPLS::addSolution (2) error: not using separated evaluators!" << endl;
			exit(1);
		}
		vector<Evaluator<R, ADS>*> v_e(*_v_e);
		delete _v_e;

		vector<double> fitnessNewInd;

		for (int evalIndex = 0; evalIndex < v_e.size(); evalIndex++)
		{
			Evaluation &e = v_e[evalIndex]->evaluate(s);

			if (!e.isFeasible())
			{
				delete &e;
				return false;
			}

			fitnessNewInd.push_back(e.evaluation());
			delete &e;
		}

		bool added = true;
		for (int ind = 0; ind < p.first.size(); ind++)
		{

			vector<double> popIndFitness = p.first.getFitness(ind);
			if (pDominanceWeak.dominates(popIndFitness, fitnessNewInd))
				return false;

			if (pDominance.dominates(fitnessNewInd, popIndFitness))
			{
				delete &p.first.remove(ind);
				p.second.erase(p.second.begin() + ind);
				ind--;
			}
		}

		if (added == true)
		{
			p.first.push_back(s, fitnessNewInd);
			vector<bool> neigh;
			for (int n = 0; n < neighbors.size(); n++)
				neigh.push_back(false);
			p.second.push_back(neigh);
		}

		return added;

	}

};

#endif /*TWOPHASEPARETOLOCALSEARCHPPLS_HPP_*/
