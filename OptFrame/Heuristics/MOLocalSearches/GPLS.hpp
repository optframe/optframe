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
#include "../Heuristics/MOLocalSearches/MOBestImprovement.hpp"
#include "../Evaluator.hpp"
#include "../Population.hpp"
#include "../NSSeq.hpp"
#include "../ParetoDominance.hpp"
#include "../ParetoDominanceWeak.hpp"
#include "../InitialPopulation.h"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
struct gplsStructure
{
	Pareto<R, ADS> x_e;
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
//	pair<Pareto<R, ADS>, vector<vector<bool> > > x_e;
//	Pareto<R, ADS> p_a;
	gplsStructure<R, ADS> gplsData;

	paretoManager2PPLS(MultiEvaluator<R, ADS>& _mev, int _r) :
			paretoManager<R, ADS>(_mev), r(_r)
	{

	}

	virtual ~paretoManager2PPLS()
	{
	}

	//Special addSolution used in the 2PPLS speedUp
	bool addSolution(Solution<R, ADS>* candidate, MultiEvaluation* candidateMev)
	{

		bool added = true;
		for (int ind = 0; ind < gplsData.x_e.size(); ind++)
		{
			MultiEvaluation popIndFitness = gplsData.x_e.getIndMultiEvaluation(ind);

			if (paretoManager<R, ADS>::domWeak.dominates(popIndFitness, *candidateMev))
				return false;

			if (paretoManager<R, ADS>::dom.dominates(*candidateMev, popIndFitness))
			{
				gplsData.x_e.erase(ind);
				gplsData.nsParetoOptimum.erase(gplsData.nsParetoOptimum.begin() + ind);
				gplsData.newSol.erase(gplsData.newSol.begin() + ind);
				ind--;
			}

		}

		if (added)
		{
			gplsData.x_e.push_back(candidate, candidateMev);
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

		cout << "exec: General 2PPLS with Pareto Manager (tL:" << timelimit << ")" << endl;
		int r = vLS.size();

		gplsStructure<R, ADS> gPLSData;
		Pareto<R, ADS>* p_0;
		Pareto<R, ADS> p;

		if (_pf == NULL)
		{
			cout << "Creating initial population using a constructive method..." << endl;
			Population<R, ADS> tempPop = init_pop.generatePopulation(init_pop_size);

			for (int ind = 0; ind < tempPop.size(); ind++)
			{
				pMan2PPLS.paretoManager<R, ADS>::addSolution(*p_0, &tempPop.at(ind));
			}

			cout << "Population generated with " << p_0->size() << " individuals!" << endl;
		}
		else
		{
			cout << "Extracting Pareto Front given as parameters..." << endl;
			p_0 = _pf;

			cout << "Population extracted with " << p_0->size() << " individuals!" << endl;
		}
		pMan2PPLS.gplsData.x_e = *p_0;
		p = pMan2PPLS.gplsData.x_e;
		delete p_0;

		//Initializing auxiliar data structures -- Pareto Optimum and NewSol (guides auxiliar population p_a)
		for (int i = 0; i < pMan2PPLS.gplsData.x_e.size(); i++)
		{
			vector<bool> neigh;
			for (int n = 0; n < r; n++)
				neigh.push_back(false);
			pMan2PPLS.gplsData.nsParetoOptimum.push_back(neigh);
			pMan2PPLS.gplsData.newSol.push_back(true);
		}

		cout << "Number of Inicial x_e non-dominated solutions = " << pMan2PPLS.gplsData.x_e.size() << endl;

		int k = 1;
		cout << "Starting search with k = " << k << endl;
		while ((k <= r) && (tnow.now() < timelimit))
		{

			for (int ind = 0; ind < pMan2PPLS.gplsData.x_e.size(); ind++)
			{
				//All individuals from NS k-1 will be visited
				string localSearchId = vLS[k - 1]->id();
				//Speed-up only if it is an exaustive search through the whole NS
				if (localSearchId == "OptFrame:MOLocalSearch:MO-BI")
					pMan2PPLS.gplsData.nsParetoOptimum[ind][k - 1] = true;
				//Ensure that all individuals are maked are old solutions - Only the new ones will be marked as true
				pMan2PPLS.gplsData.newSol[ind] = false;
			}

			//Run local search for each individual of the population - Pareto Manager, pMan2PPLS, updates population
			for (int ind = 0; ind < p.size(); ind++)
				vLS[k - 1]->exec(p.getNonDominatedSol(ind), p.getIndMultiEvaluation(ind), pMan2PPLS, timelimit - tnow.now(), target_f);

			p.clear();

			//Updated current Pareto p with the individuals added in this current iteration
			for (int ind = 0; ind < pMan2PPLS.gplsData.x_e.size(); ind++)
				if (pMan2PPLS.gplsData.newSol[ind])
					p.push_back(&pMan2PPLS.gplsData.x_e.getNonDominatedSol(ind), &pMan2PPLS.gplsData.x_e.getIndMultiEvaluation(ind));

			if (p.size() != 0)
			{
				k = 1;
			}
			else
			{
				k++;
				p = pMan2PPLS.gplsData.x_e;

				//speed-up - Thibauuuut Lust - Nice guy
				if (k <= r)
				{

					int removed = 0;
					for (int i = 0; i < pMan2PPLS.gplsData.nsParetoOptimum.size(); i++)
						if (pMan2PPLS.gplsData.nsParetoOptimum[i][k - 1] == true)
						{
							p.erase(i - removed);
							removed++;
						}

				}
				//end of the speed-up

				//Another option for speed-up, which only add instead of delete
				//for (int ind = 0; ind < pMan2PPLS.gplsData.x_e.size(); ind++)
				//	if (pMan2PPLS.gplsData.nsParetoOptimum[ind][k-1] == false) //speed-up - Thibauuuut Lust - Nice guy
				//		p.push_back(&pMan2PPLS.gplsData.x_e.getNonDominatedSol(ind), &pMan2PPLS.gplsData.x_e.getIndMultiEvaluation(ind));

			}
			cout << "p.size() = " << p.size() << "\t pMan2PPLS.gplsData.x_e.size() = " << pMan2PPLS.gplsData.x_e.size();
			cout << "\t k = " << k << endl;
		}

		Pareto<R, ADS>* pReturn = new Pareto<R, ADS>(pMan2PPLS.gplsData.x_e);
		p.clear();
		pMan2PPLS.gplsData.x_e.clear();

		cout << "General Two-Phase Pareto Local Search Finished" << endl;

		return pReturn;
	}

};

#endif /*GENERALPARETOLOCALSEARCH_HPP_*/

