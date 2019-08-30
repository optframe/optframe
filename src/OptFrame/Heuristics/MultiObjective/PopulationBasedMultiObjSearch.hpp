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

#ifndef OPTFRAME_POPULATION_BASED_MOS_HPP_
#define OPTFRAME_POPULATION_BASED_MOS_HPP_

#include "../../MultiObjSearch.hpp"
#include "../../Timer.hpp"

#include "MOSIndividual.hpp"
#include "FitnessAssignment.hpp"
#include "DiversityManagement.hpp"
#include "Elitism.hpp"
#include "PopulationManagement.hpp"
#include "MOSSelection.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class PopulationBasedMultiObjSearch: public MultiObjSearch<R, ADS, DS>
{
public:
	MultiDirection<DS>& mDir;

	PopulationBasedMultiObjSearch(MultiDirection<DS>& _mDir) :
			mDir(_mDir)
	{
	}

	virtual ~PopulationBasedMultiObjSearch()
	{
	}

	virtual vector<double> initializeBounds()
	{
		vector<double> best(mDir.nObjectives);
		for(unsigned i = 0; i < mDir.nObjectives; i++)
			best[i] = mDir.worst(i);
		return best;
	}

	// return true if any value was improved
	virtual bool updateBounds(vector<MOSIndividual<R, ADS, DS>*>& P, vector<double>& best)
	{
		bool improved = false;
		for(unsigned s = 0; s < P.size(); s++)
			for(unsigned i = 0; i < best.size(); i++)
				if(mDir.betterThan(i, P[s]->mev->at(i).evaluation(), best[i]))
				{
					best[i] = P[s]->mev->at(i).evaluation();
					improved = true;
				}
		return improved;
	}

	// assign fitness to subset 'g' that belongs to 'P'
	virtual void assignFitness(vector<MOSIndividual<R, ADS, DS>*>& g, const vector<const MOSIndividual<R, ADS, DS>*>& P) = 0;

	// assign diversity to subset 'g' that belongs to 'P'
	virtual void assignDiversity(vector<MOSIndividual<R, ADS, DS>*>& g, const vector<const MOSIndividual<R, ADS, DS>*>& P) = 0;

	// update archive based on current population
	virtual void updateArchive(const vector<const MOSIndividual<R, ADS, DS>*>& P, vector<MOSIndividual<R, ADS, DS>*>& archive) = 0;
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NSPopulationBasedMultiObjSearch: public PopulationBasedMultiObjSearch<R, ADS, DS>
{
protected:
	MultiDirection<DS>& mDir;
	PopulationManagement<R, ADS, DS>& popMan;
	unsigned popSize;

	// stopping criteria (-1: indicates unused)
	int maxGen;  // total number of generations
	int maxIter; // generations without improvement

public:
	NSPopulationBasedMultiObjSearch(MultiDirection<DS>& _mDir, PopulationManagement<R, ADS, DS>& _popMan, unsigned _popSize, int _maxIter, int _maxGen = 100000000) :
			PopulationBasedMultiObjSearch<R, ADS, DS>(_mDir), mDir(_mDir), popMan(_popMan), popSize(_popSize), maxGen(_maxGen), maxIter(_maxIter)
	{
	}

	virtual ~NSPopulationBasedMultiObjSearch()
	{
	}

	// evaluate and return best values for population
	virtual vector<double> evaluate(vector<MOSIndividual<R, ADS, DS>*>& Pop) = 0;

	virtual void assignFitness(vector<MOSIndividual<R, ADS, DS>*>& g, const vector<const MOSIndividual<R, ADS, DS>*>& P) = 0;

	virtual void assignDiversity(vector<MOSIndividual<R, ADS, DS>*>& g, const vector<const MOSIndividual<R, ADS, DS>*>& P) = 0;

	virtual void select(unsigned popSize, vector<MOSIndividual<R, ADS, DS>*>& P, vector<MOSIndividual<R, ADS, DS>*>& archive) = 0;

	virtual void freePopulation(vector<MOSIndividual<R, ADS, DS>*>& P, vector<MOSIndividual<R, ADS, DS>*>& archive) = 0;

	Pareto<R, ADS, DS>* search(double timelimit = 100000000, double target_f = 0, Pareto<R, ADS, DS>* _pf = nullptr)
	{
		Timer timer;

		cout << "Population Based Multi Objective Search search()" << endl;

		vector<MOSIndividual<R, ADS, DS>*> archive;

		vector<MOSIndividual<R, ADS, DS>*>& P = popMan.initialize(popSize);
		evaluate(P);
		vector<const MOSIndividual<R, ADS, DS>*> Pconst(P.begin(), P.end());
		assignFitness(P, Pconst);
		assignDiversity(P, Pconst);

		vector<MOSIndividual<R, ADS, DS>*>& Q = popMan.createNext(popSize, Pconst);
		vector<double> bestQ = evaluate(Q);

		int t = 0;
		int tImp = 0;

		vector<double> bestObj(mDir.nObjectives);
		for(unsigned i = 0; i < bestObj.size(); i++)
			bestObj[i] = mDir.worst(i);

		while((timer.now() < timelimit) && (t <= maxGen) && (tImp <= maxIter))
		{
			vector<double> bestP = evaluate(P);
			P.insert(P.end(), Q.begin(), Q.end());
			for(unsigned i = 0; i < bestP.size(); i++)
				if(mDir.betterThan(i, bestQ[i], bestP[i]))
					bestP[i] = bestQ[i];

			Pconst = vector<const MOSIndividual<R, ADS, DS>*>(P.begin(), P.end());
			assignFitness(P, Pconst);
			assignDiversity(P, Pconst);

			select(popSize, P, archive);

			// archive is updated
			// unused already free'd

			bool improved = false;
			for(unsigned s = 0; s < archive.size(); s++)
				for(unsigned i = 0; i < bestObj.size(); i++)
					if(mDir.betterThan(i, bestP[i], bestObj[i]))
					{
						// IMPROVEMENT IN ONE OBJECTIVE
						improved = true;
						bestObj[i] = bestP[i];
					}

			if(improved)
			{
				tImp = -1;
				cout << "t=" << t << " improved bounds: " << bestObj << endl;
			}

			// generating next population
			Pconst = vector<const MOSIndividual<R, ADS, DS>*>(P.begin(), P.end());
			Q = popMan.createNext(popSize, Pconst);
			bestQ = evaluate(Q);
			t++;
			tImp++;
		}

		freePopulation(P, archive);

		Pareto<R, ADS, DS>* pf = new Pareto<R, ADS, DS>;
		for(unsigned i = 0; i < archive.size(); i++)
		{
			Solution<R, ADS>* s = archive.at(i)->s;
			MultiEvaluation<DS>* mev = archive.at(i)->mev;
			pf->push_back(s, mev);
		}

		delete &P;
		delete &archive;

		return pf;
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ClassicNSGAII: public NSPopulationBasedMultiObjSearch<R, ADS, DS>
{
protected:
	MultiEvaluator<R, ADS, DS>& mevr;
	FitnessAssignment<R, ADS, DS>* fa;
	DiversityManagement<R, ADS, DS>* dm;
	MOSSelection<R, ADS, DS>* sel;

public:
	ClassicNSGAII(MultiEvaluator<R, ADS, DS>& _mevr, PopulationManagement<R, ADS, DS>& _popMan, unsigned popSize, int maxIter, int maxGen = 100000000) :
			NSPopulationBasedMultiObjSearch<R, ADS, DS>(_mevr, _popMan, popSize, maxIter, maxGen), mevr(_mevr)
	{
		if(mevr.nObjectives == 2)
			fa = new BiObjNonDominatedSort<R, ADS, DS>(mevr.getDirections());
		else
			fa = new NonDominatedSort<R, ADS, DS>(mevr.getDirections());

		dm = new CrowdingDistance<R, ADS, DS>(mevr.getDirections());

		sel = new NSGAIISelection<R, ADS, DS>;
	}

	virtual ~ClassicNSGAII()
	{
		delete fa;
		delete dm;
		delete sel;
	}

	using NSPopulationBasedMultiObjSearch<R, ADS, DS>::search;

	vector<double> evaluate(vector<MOSIndividual<R, ADS, DS>*>& P)
	{
		vector<double> best(mevr.nObjectives);
		for(unsigned i = 0; i < best.size(); i++)
			best[i] = mevr.worst(i);

		for(unsigned s = 0; s < P.size(); s++)
			if(!P[s]->mev) // not evaluated (nullptr)
			{
				P[s]->mev = &mevr.evaluate(*P[s]->s);
				for(unsigned i = 0; i < best.size(); i++)
					if(mevr.betterThan(i, P[s]->mev->at(i).evaluation(), best[i]))
						best[i] = P[s]->mev->at(i).evaluation();
			}

		return best;
	}

	virtual void assignFitness(vector<MOSIndividual<R, ADS, DS>*>& g, const vector<const MOSIndividual<R, ADS, DS>*>& P)
	{
		fa->assignFitnessGroup(g, P);
	}

	virtual void assignDiversity(vector<MOSIndividual<R, ADS, DS>*>& g, const vector<const MOSIndividual<R, ADS, DS>*>& P)
	{
		dm->assignDiversityGroup(g, P);
	}

	virtual void updateArchive(const vector<const MOSIndividual<R, ADS, DS>*>& P, vector<MOSIndividual<R, ADS, DS>*>& archive)
	{
		// NO ARCHIVING IN NSGA-II
	}

	virtual void select(unsigned popSize, vector<MOSIndividual<R, ADS, DS>*>& P, vector<MOSIndividual<R, ADS, DS>*>& archive)
	{
		sel->select(popSize, P, archive);
	}

	virtual void freePopulation(vector<MOSIndividual<R, ADS, DS>*>& P, vector<MOSIndividual<R, ADS, DS>*>& archive)
	{
		sel->free(P, archive);
	}

};

}

#endif /*OPTFRAME_POPULATION_BASED_MOS_HPP_*/
