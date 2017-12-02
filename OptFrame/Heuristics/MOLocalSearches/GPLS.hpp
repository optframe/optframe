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

#include "../../MultiObjSearch.hpp"
#include "../../MOLocalSearch.hpp"
#include "../../Heuristics/MOLocalSearches/MOBestImprovement.hpp"
#include "../../Evaluator.hpp"
#include "../../Population.hpp"
#include "../../NSSeq.hpp"
#include "../../ParetoDominance.hpp"
#include "../../ParetoDominanceWeak.hpp"
#include "../../InitialPareto.h"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
struct gplsStructure
{
	vector<vector<bool> > nsParetoOptimum;
	vector<bool> newSol;

	gplsStructure()
	{
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class paretoManager2PPLS: public paretoManager<R, ADS>
{
private:
	int r;

public:
	gplsStructure<R, ADS> gplsData;
//	Pareto<R, ADS> x_e;

	paretoManager2PPLS(MultiEvaluator<R, ADS>& _mev, int _r) :
			paretoManager<R, ADS>(_mev), r(_r)
	{

	}

	virtual ~paretoManager2PPLS()
	{
	}

	//Special addSolution used in the GPLS speedUp
	bool addSolution(Pareto<R, ADS>& p, const Solution<R, ADS>& candidate, const MultiEvaluation& candidateMev)
	{
		bool added = true;
		for (unsigned ind = 0; ind < p.size(); ind++)
		{
			const MultiEvaluation& popIndFitness = p.getIndMultiEvaluation(ind);

			if (paretoManager<R, ADS>::domWeak.dominates(popIndFitness, candidateMev))
				return false;

			if (paretoManager<R, ADS>::dom.dominates(candidateMev, popIndFitness))
			{
				p.erase(ind);
				gplsData.nsParetoOptimum.erase(gplsData.nsParetoOptimum.begin() + ind);
				gplsData.newSol.erase(gplsData.newSol.begin() + ind);
				ind--;
			}

		}

		if (added)
		{
			p.push_back(candidate, candidateMev);
			vector<bool> neigh;
			for (int n = 0; n < r; n++)
				neigh.push_back(false);
			gplsData.nsParetoOptimum.push_back(neigh);
			gplsData.newSol.push_back(added);
		}

		return added;
	}

};

//General Two-Phase Pareto Local Search
// "We call this general local search approach to multiobjective problems the Pareto local search (PLS)."
//  A Dynasearch Neighborhood for the Bicriteria Traveling Salesman Problem , by Angel (2004) and Paquete(2004)
// "Generalization in the multiobjective case of the most simple metaheuristic: the hill-climbing method. "
//  Speed-up techniques for solving large-scale biobjective TSP, by Lust (2010)
template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class GeneralParetoLocalSearch: public MOLocalSearch<R, ADS>
{
	typedef vector<Evaluation*> FitnessValues;

private:
//	MultiEvaluator<R, ADS>& multiEval;
	InitialPareto<R, ADS>& init_pareto;
	int init_pop_size;
	vector<MOLocalSearch<R, ADS>*> vLS;
	paretoManager2PPLS<R, ADS> pMan2PPLS;

public:

	GeneralParetoLocalSearch(MultiEvaluator<R, ADS>& _mev, InitialPareto<R, ADS>& _init_pareto, int _init_pop_size, vector<MOLocalSearch<R, ADS>*> _vLS) :
			init_pareto(_init_pareto), init_pop_size(_init_pop_size), vLS(_vLS), pMan2PPLS(paretoManager2PPLS<R, ADS>(_mev, _vLS.size()))
	{

	}

	virtual ~GeneralParetoLocalSearch()
	{
	}

	virtual void exec(Pareto<R, ADS>& p, Solution<R, ADS>* s, paretoManager<R, ADS>* pManager, MOSC& stopCriteria)
	{

	}

	virtual void exec(Pareto<R, ADS>& p, Solution<R, ADS>* s, MultiEvaluation* sMev, paretoManager<R, ADS>* pManager, MOSC& stopCriteria)
	{

	}

	//virtual void exec(Population<R, ADS>& p_0, FitnessValues& e_pop, double timelimit, double target_f)
	virtual Pareto<R, ADS>* search(MOSC& stopCriteria, Pareto<R, ADS>* _pf = NULL)
	{
		Timer tnow;

		cout << "exec: General 2PPLS with Pareto Manager (tL:" << stopCriteria.timelimit << ")" << endl;
		int r = vLS.size();

		gplsStructure<R, ADS> gPLSData;
		Pareto<R, ADS> x_e;

		if (_pf == NULL)
		{
			cout << "Creating initial population using a initial pareto method:" << init_pop_size << endl;
			if (tnow.now() < stopCriteria.timelimit)
				x_e = std::move(init_pareto.generatePareto(init_pop_size, stopCriteria.timelimit - tnow.now()));

			cout << "Population generated with " << x_e.size() << " individuals!" << endl;
		}
		else
		{
			cout << "Extracting Pareto Front given as parameter..." << endl;
			x_e = std::move(*_pf); //check this move with AIIIGOR

			cout << "Extracting PF contains " << x_e.size() << " individuals." << endl;
		}

		//TODO - create a new method that does not copy
		//instead, it updated p during loop and do not perform neighborhood search in indivuals that were dominated
		//copy x_e to p
		Pareto<R, ADS> p = x_e;

		//Initializing auxiliar data structures -- Pareto Optimum and NewSol (guides auxiliar population p_a)
		for (int i = 0; i < (int) x_e.size(); i++)
		{
			vector<bool> neigh;
			for (int n = 0; n < r; n++)
				neigh.push_back(false);
			pMan2PPLS.gplsData.nsParetoOptimum.push_back(neigh);
			pMan2PPLS.gplsData.newSol.push_back(true);
		}

		cout << "Number of initial x_e non-dominated solutions = " << x_e.size() << endl;

		int k = 0;
		cout << "Starting search with k = " << k << endl;
		while ((k < r) && (tnow.now() < stopCriteria.timelimit))
		{

			for (int ind = 0; ind < (int) x_e.size(); ind++)
			{
				//All individuals from NS k-1 will be visited
				string localSearchId = vLS[k]->id();
				//Speed-up only if it is an exaustive search through the whole NS
				if (localSearchId == "OptFrame:MOLocalSearch:MO-BI")
					pMan2PPLS.gplsData.nsParetoOptimum[ind][k] = true;
				//Ensure that all individuals are maked as old solutions - Only the new ones will be marked as true
				pMan2PPLS.gplsData.newSol[ind] = false;
			}

			//Run local search for each individual of the population - Pareto Manager, pMan2PPLS, updates population
			MOSC* stopCriteriaLS = new MOSC;
			stopCriteriaLS->timelimit = stopCriteria.timelimit;

			for (int ind = 0; ind < (int) p.size(); ind++)
				vLS[k]->exec(x_e, &p.getNonDominatedSol(ind), &p.getIndMultiEvaluation(ind), &pMan2PPLS, *stopCriteriaLS);
			delete stopCriteriaLS;
//			for(int e=0;e<x_e.size();e++)
//			{
//				x_e.getIndMultiEvaluation(e).print();
//			}
//			getchar();

			p.clear();

			//Updated current Pareto p with the individuals added in this current iteration
			for (int ind = 0; ind < (int) x_e.size(); ind++)
				if (pMan2PPLS.gplsData.newSol[ind])
				{
					Solution<R, ADS>* sNew = new Solution<R, ADS>(x_e.getNonDominatedSol(ind));
					MultiEvaluation* mevNew = new MultiEvaluation(x_e.getIndMultiEvaluation(ind));
					p.add_ind(sNew, mevNew);
				}

			if (p.size() != 0)
			{
				k = 0;
			}
			else
			{
				k++;
				p = x_e;
				//speed-up - Thibauuuut Lust - Nice guy
				if (k < r)
				{

					int removed = 0;
					for (int i = 0; i < (int) pMan2PPLS.gplsData.nsParetoOptimum.size(); i++)
						if (pMan2PPLS.gplsData.nsParetoOptimum[i][k - 1] == true)
						{
							p.erase(i - removed);
							removed++;
						}

				}
				//end of the speed-up

				//Another option for speed-up, which only add instead of delete
				//for (int ind = 0; ind < pMan2PPLS.gplsData.x_e.size(); ind++)
				//	if (pMan2PPLS.gplsData.nsParetoOptimum[ind][k] == false) //speed-up - Thibauuuut Lust - Nice guy
				//		p.push_back(&pMan2PPLS.gplsData.x_e.getNonDominatedSol(ind), &pMan2PPLS.gplsData.x_e.getIndMultiEvaluation(ind));

			}
			cout << "p.size() = " << p.size();
			cout<<"\t pMan2PPLS.x_e.size() = " << x_e.size();
			cout << "\t k = " << k << endl;
		}

		Pareto<R, ADS>* pReturn = new Pareto<R, ADS>(std::move(x_e));
//		pReturn->print();
		p.clear();

		//checking possible dominance problems -- TODO - Remove for a faster code
		pMan2PPLS.checkDominance(*pReturn);

		cout << "General Two-Phase Pareto Local Search finished with " << pReturn->size() << " non-dominated solutions." << endl;


		return pReturn;
	}

};

#endif /*GENERALPARETOLOCALSEARCH_HPP_*/

