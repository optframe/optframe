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

#ifndef OPTFRAME_POPULATION_MANAGEMENT_HPP_
#define OPTFRAME_POPULATION_MANAGEMENT_HPP_

#include "MOSIndividual.hpp"
#include "../../Component.hpp"
#include "../EvolutionaryAlgorithms/GeneralCrossover.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class PopulationManagement: Component
{
public:

	PopulationManagement()
	{
	}

	virtual ~PopulationManagement()
	{
	}

	// create a new population
	virtual vector<MOSIndividual<R, ADS, DS>*> initialize(unsigned pSize) = 0;

	// create next population
	virtual vector<MOSIndividual<R, ADS, DS>*> createNext(unsigned target_size, const vector<const MOSIndividual<R, ADS, DS>*>& P) = 0;

	virtual void print() const
	{
		cout << "PopulationManagement" << endl;
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class BasicPopulationManagement: public PopulationManagement<R, ADS, DS>
{
public:
	InitialPopulation<R, ADS>& initPop;
	vector<NS<RepCARP>*> mutations;
	double mutationRate; // probability of applying a mutation
	vector<GeneralCrossover<R, ADS>*> crossovers;
	double renewRate; // percentage of children population to be entirely reconstructed
	RandGen& rg;

	BasicPopulationManagement(InitialPopulation<R, ADS>& _initPop, vector<NS<RepCARP>*> _mutations, double _mutationRate, vector<GeneralCrossover<RepCARP>*> _crossovers, double _renewRate, RandGen& _rg) :
			initPop(_initPop), mutations(_mutations), mutationRate(_mutationRate), crossovers(_crossovers), renewRate(_renewRate), rg(_rg)
	{
		if(renewRate > 1)
			renewRate = 1;
		if(renewRate < 0)
			renewRate = 0;
	}

	virtual ~BasicPopulationManagement()
	{
	}

	virtual MOSIndividual<R, ADS, DS>* createIndividual(Solution<R, ADS>* s)
	{
		MultiEvaluation<DS>* mev = NULL;

		// no evaluation here!

		return new MOSIndividual<R, ADS, DS>(s, mev);
	}

	virtual vector<MOSIndividual<R, ADS, DS>*> initialize(unsigned pSize)
	{
		Population<R, ADS> *p = &initPop.generatePopulation(pSize);

		vector<MOSIndividual<R, ADS, DS>*> r;
		for(unsigned i = 0; i < p->size(); i++)
			r.push_back(createIndividual(&p->at(i)));

		p->clearNoKill();
		delete p;

		return r;
	}

	// multiple crossovers in a binary tournament and apply many mutations
	virtual vector<MOSIndividual<R, ADS, DS>*> createNext(unsigned target_size, const vector<const MOSIndividual<R, ADS, DS>*>& P)
	{
		vector<MOSIndividual<R, ADS, DS>*> children;

		unsigned size_renew = renewRate * target_size;

		if(size_renew > 0)
			children = initialize(size_renew);

		unsigned rest = target_size - size_renew;

		vector<MOSIndividual<R, ADS, DS>*> pool = binaryTournment(rest, P);

		vector<MOSIndividual<R, ADS, DS>*> crossMut = basicCrossoverMutation(pool);

		children.insert(children.end(), crossMut.begin(), crossMut.end());

		if(children.size() != target_size)
			if(Component::warning)
			{
				cout << "WARNING: BasicPopulationManagement::createNext() tried to create population of " << target_size << " but got " << children.size();
				cout << endl;
			}

		return children;
	}

	vector<const MOSIndividual<R, ADS, DS>*> binaryTournment(unsigned poolSize, const vector<const MOSIndividual<R, ADS, DS>*>& P)
	{
		vector<const MOSIndividual<R, ADS, DS>*> pool;

		for(unsigned t = 1; t <= poolSize; t++)
		{
			int i = rg.rand(P.size());
			int j = i;
			while(i == j)
				j = rg.rand(P.size());

			if(P[i]->betterThan(*P[j]))
				pool.push_back(P[i]);
			else
				pool.push_back(P[j]);
		}

		return pool;
	}

	vector<MOSIndividual<R, ADS, DS>*> basicCrossoverMutation(const vector<const MOSIndividual<R, ADS, DS>*>& pool)
	{
		vector<MOSIndividual<R, ADS, DS>*> children;

		if(pool.size() == 0)
			return children;

		for(unsigned i = 0; i < pool.size(); i++)
		{
			unsigned j = i + 1;
			if(j == pool.size())
				j = 0;

			pair<Solution<R, ADS>*, Solution<R, ADS>*> crossChildren;

			if(crossovers.size() > 0)
			{
				int crossId = rg.rand(crossovers.size());
				crossChildren = crossovers[crossId].cross(*pool[i], *pool[j]);
			}
			else
				crossChildren = make_pair(&pool[i]->clone(), &pool[j]->clone());

			vector<Solution<R, ADS>*> vChildren(2, NULL);
			vChildren[0] = crossChildren.first;
			vChildren[1] = crossChildren.second;

			for(unsigned c = 0; c < 2; c++)
			{
				if((mutations.size() > 0) && rg.randP(mutationRate)) // mutation rate
				{
					int neigh = rg.rand(mutations.size());
					Move<R, ADS, DS>* move = mutations[neigh]->validMove(*vChildren[c]);

					if(move)
					{
						delete &move->apply(*vChildren[c]);
						delete move;
						move = NULL;
					}
				}

				children.push_back(vChildren[c]);
			}
		}

		return children;
	}
};

}

#endif /*OPTFRAME_POPULATION_MANAGEMENT_HPP_*/
