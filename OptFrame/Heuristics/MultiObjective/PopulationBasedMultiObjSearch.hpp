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

#ifndef OPTFRAME_POPULATION_BASED_MOS_HPP_
#define OPTFRAME_POPULATION_BASED_MOS_HPP_

#include "../../MultiObjSearch.hpp"
#include "../../Timer.hpp"

#include "MOSIndividual.hpp"
#include "FitnessAssignment.hpp"
#include "DiversityManagement.hpp"
#include "Elitism.hpp"
#include "PopulationManagement.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class PopulationBasedMultiObjSearch: public MultiObjSearch<R, ADS, DS>
{
protected:
	MultiEvaluator<R, ADS, DS>& mevr;
	PopulationManagement<R, ADS, DS>& popMan;

	unsigned popSize;

	// stopping criteria (-1: indicates unused)
	int maxGen;  // total number of generations
	int maxIter; // generations without improvement

public:
	PopulationBasedMultiObjSearch(MultiEvaluator<R, ADS, DS>& _mevr, PopulationManagement<R, ADS, DS>& _popMan, unsigned _popSize, int _maxIter, int _maxGen = 100000000) :
			mevr(_mevr), popMan(_popMan), popSize(_popSize), maxGen(_maxGen), maxIter(_maxIter)
	{
	}

	virtual ~PopulationBasedMultiObjSearch()
	{
	}

	void evaluate(MOSPopulation<R, ADS, DS>& Pop)
	{
		vector<MOSIndividual<R, ADS, DS>*>& P = Pop.P;
		for(unsigned s = 0; s < P.size(); s++)
			P[s]->mev = &mevr.evaluate(P[s]->s);
	}

	virtual void assignFitness(MOSPopulation<R, ADS, DS>& P) = 0;

	virtual void assignDiversity(MOSPopulation<R, ADS, DS>& P) = 0;

	virtual void select(unsigned popSize, MOSPopulation<R, ADS, DS>& P, MOSPopulation<R, ADS, DS>& archive) = 0;

	virtual void freePopulation(MOSPopulation<R, ADS, DS>& P, MOSPopulation<R, ADS, DS>& archive) = 0;

	Pareto<R, ADS, DS>* search(double timelimit = 100000000, double target_f = 0, Pareto<R, ADS, DS>* _pf = NULL)
	{
		Timer timer;

		cout << "Population Based Multi Objective Search search()" << endl;

		MOSPopulation<R, ADS, DS> archive;

		MOSPopulation<R, ADS, DS>& P = popMan.initialize(popSize);
		evaluate(P);
		assignFitness(P);
		assignDiversity(P);

		MOSPopulation<R, ADS, DS>& Q = popMan.createNext(popSize, P);
		evaluate(Q);

		int t = 0;
		int tImp = 0;
		vector<double> bestObj(mevr.nObjectives);
		for(unsigned i = 0; i < bestObj.size(); i++)
			bestObj[i] = mevr.worst(i);

		while((timer.now() < timelimit) && (t <= maxGen) && (tImp <= maxIter))
		{
			P.add(Q);

			assignFitness(P);
			assignDiversity(P);

			select(popSize, P, archive);

			// archive is updated
			// unused already free'd

			bool improved = false;
			for(unsigned s = 0; s < archive.P.size(); s++)
				for(unsigned i = 0; i < bestObj.size(); i++)
					if(mevr.betterThan(i, archive.P[s]->mev->at(i).evaluation(), bestObj[i]))
					{
						// IMPROVEMENT IN ONE OBJECTIVE
						improved = true;
						bestObj[i] = archive.P[s]->mev->at(i).evaluation();
					}

			if(improved)
			{
				tImp = -1;
				cout << "t=" << t << " improved bounds: " << bestObj << endl;
			}

			// generating next population
			MOSPopulation<R, ADS, DS>& Q2 = popMan.createNext(popSize, P);
			Q.P.clear();
			Q.add(Q2);
			delete &Q2;
			evaluate(Q);
			t++;
			tImp++;
		}

		freePopulation(P, archive);

		Pareto<R, ADS, DS>* pf = new Pareto<R, ADS, DS>;
		for(unsigned i = 0; i < archive.P.size(); i++)
		{
			Solution<R, ADS>* s = &archive.P.at(i)->s;
			MultiEvaluation<DS>* mev = archive.P.at(i)->mev;
			pf->push_back(s, mev);
		}

		delete& P;
		delete& archive;

		return pf;
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ClassicNSGAII: public PopulationBasedMultiObjSearch<R, ADS, DS>
{
protected:
	FitnessAssignment<R, ADS, DS>* fa;
	DiversityManagement<R, ADS, DS>* dm;
	Elitism<R, ADS, DS>* elitism;


public:
	ClassicNSGAII(MultiEvaluator<R, ADS, DS>& muev, PopulationManagement<R, ADS, DS>& popMan, unsigned popSize, int maxIter, int maxGen = 100000000) :
			PopulationBasedMultiObjSearch<R, ADS, DS>(muev, popMan, popSize, maxIter, maxGen)
	{
		if(muev.nObjectives == 2)
			fa = new BiObjNonDominatedSort<R, ADS, DS>(muev.getDirections());
		else
			fa = new NonDominatedSort<R, ADS, DS>(muev.getDirections());

		dm = new CrowdingDistance<R, ADS, DS>(muev.getDirections());

		elitism = new NSGAIISelection<R, ADS, DS>;
	}

	virtual ~ClassicNSGAII()
	{
		delete fa;
		delete dm;
		delete elitism;
	}

	using PopulationBasedMultiObjSearch<R, ADS, DS>::search;

	virtual void assignFitness(MOSPopulation<R, ADS, DS>& P)
	{
		fa->assignFitnessAll(P);
	}

	virtual void assignDiversity(MOSPopulation<R, ADS, DS>& P)
	{
		dm->assignDiversityAll(P);
	}

	virtual void select(unsigned popSize, MOSPopulation<R, ADS, DS>& P, MOSPopulation<R, ADS, DS>& archive)
	{
		elitism->select(popSize, P, archive);
	}

	virtual void freePopulation(MOSPopulation<R, ADS, DS>& P, MOSPopulation<R, ADS, DS>& archive)
	{
		elitism->free(P, archive);
	}

};

}

#endif /*OPTFRAME_POPULATION_BASED_MOS_HPP_*/
