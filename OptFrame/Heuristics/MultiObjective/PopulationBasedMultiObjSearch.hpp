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

	FitnessAssignment<R, ADS, DS>& fa;
	DiversityManagement<R, ADS, DS>& dm;
	Elitism<R, ADS, DS>& elitism;
	PopulationManagement<R, ADS, DS>& popMan;

	unsigned popSize;

	// stopping criteria (-1: indicates unused)
	int maxGen;  // total number of generations
	int maxIter; // generations without improvement

public:
	PopulationBasedMultiObjSearch(MultiEvaluator<R, ADS, DS>& _mevr, FitnessAssignment<R, ADS, DS>& _fa, DiversityManagement<R, ADS, DS>& _dm, Elitism<R, ADS, DS>& _elitism, PopulationManagement<R, ADS, DS>& _popMan, unsigned _popSize, int _maxIter, int _maxGen = 100000000) :
			mevr(_mevr), fa(_fa), dm(_dm), elitism(_elitism), popMan(_popMan), popSize(_popSize), maxGen(_maxGen), maxIter(_maxIter)
	{
	}

	virtual ~PopulationBasedMultiObjSearch()
	{
	}

	void evaluate(vector<MOSIndividual<R, ADS, DS>*>& P)
	{
		for(unsigned s = 0; s < P.size(); s++)
			P[s]->mev = &mevr.evaluate(P[s]->s);
	}


	virtual Pareto<R, ADS, DS>* search(double timelimit = 100000000, double target_f = 0, Pareto<R, ADS, DS>* _pf = NULL)
	{
		Timer timer;

		cout << "Population Based Multi Objective Search search() with" << endl;
		cout << "FitnessAssignment: ";
		fa.print();
		cout << "DiversityManagement: ";
		dm.print();
		cout << "Elitism: ";
		elitism.print();
		cout << "|Population| = " << popSize << endl;
		cout << "PopulationManagement: ";
		popMan.print();

		vector<MOSIndividual<R, ADS, DS>*> archive;

		vector<MOSIndividual<R, ADS, DS>*> P = popMan.initialize(popSize);
		evaluate(P);
		fa.assignFitnessAll(P);
		dm.assignDiversity(P);

		vector<const MOSIndividual<R, ADS, DS>*> Pconst(P.begin(), P.end());
		vector<MOSIndividual<R, ADS, DS>*> Q = popMan.createNext(popSize, Pconst);
		evaluate(Q);

		int t = 0;
		int tImp = 0;
		vector<double> bestObj(mevr.nObjectives);
		for(unsigned i = 0; i < bestObj.size(); i++)
			bestObj[i] = mevr.worst(i);

		while((timer.now() < timelimit) && (t <= maxGen) && (tImp <= maxIter))
		{
			P.insert(P.end(), Q.begin(), Q.end());

			fa.assignFitnessAll(P);
			dm.assignDiversity(P);

			elitism.select(popSize, P, archive);

			// archive is updated
			// unused already free'd

			bool improved = false;
			for(unsigned s = 0; s < archive.size(); s++)
				for(unsigned i = 0; i < bestObj.size(); i++)
					if(mevr.betterThan(i, archive[s]->mev->at(i).evaluation(), bestObj[i]))
					{
						// IMPROVEMENT IN ONE OBJECTIVE
						improved = true;
						bestObj[i] = archive[s]->mev->at(i).evaluation();
					}

			if(improved)
			{
				tImp = -1;
				cout << "t=" << t << " improved bounds: " << bestObj << endl;
			}

			// generating next population
			Pconst = vector<const MOSIndividual<R, ADS, DS>*>(P.begin(), P.end());
			Q = popMan.createNext(popSize, Pconst);
			evaluate(Q);
			t++;
			tImp++;
		}

		elitism.free(P, archive);

		Pareto<R, ADS, DS>* pf = new Pareto<R, ADS, DS>;
		for(unsigned i = 0; i < archive.size(); i++)
		{
			Solution<R, ADS>* s = &archive.at(i)->s;
			MultiEvaluation<DS>* mev = archive.at(i)->mev;
			pf->push_back(s, mev);
		}
		return pf;
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ClassicNSGAII: public PopulationBasedMultiObjSearch<R, ADS, DS>
{
public:
	ClassicNSGAII(MultiEvaluator<R, ADS, DS>& muev, InitialPopulation<R, ADS>& initPop, vector<NS<RepCARP>*> mutations, double mutationRate, vector<GeneralCrossover<RepCARP>*> crossovers, double renewRate, RandGen& rg, unsigned popSize, int maxIter, int maxGen = 100000000) :
			PopulationBasedMultiObjSearch<R, ADS, DS>(muev, (muev.nObjectives == 2 ? (FitnessAssignment<R, ADS, DS>&)*new BiObjNonDominatedSort<R, ADS, DS>(muev.getDirections()) : (FitnessAssignment<R, ADS, DS>&)*new NonDominatedSort<R, ADS, DS>(muev.getDirections())), *new CrowdingDistance<R, ADS, DS>(muev.getDirections()), *new NSGAIISelection<R, ADS, DS>, *new BasicPopulationManagement<R, ADS, DS>(initPop, mutations, mutationRate, crossovers, renewRate, rg), popSize, maxIter, maxGen)
	{
	}

	virtual ~ClassicNSGAII()
	{
		delete &this->fa;
		delete &this->dm;
		delete &this->elitism;
		delete &this->popMan;
	}

	using PopulationBasedMultiObjSearch<R, ADS, DS>::search;
};

}

#endif /*OPTFRAME_POPULATION_BASED_MOS_HPP_*/
