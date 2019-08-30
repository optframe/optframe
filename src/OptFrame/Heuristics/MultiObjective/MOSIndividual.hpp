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

#ifndef OPTFRAME_MOS_INDIVIDUAL_HPP_
#define OPTFRAME_MOS_INDIVIDUAL_HPP_

#include <algorithm>

#include "../../MultiObjSearch.hpp"
#include "../../Solution.hpp"
#include "../../Evaluator.hpp"
#include "../../Evaluation.hpp"
#include "../../Population.hpp"
#include "../../NSSeq.hpp"
#include "../../ParetoDominance.hpp"

namespace optframe
{

// MultiObjSearch Individual
template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MOSIndividual
{
public:

	Solution<R, ADS>* s;
	MultiEvaluation<DS>* mev; // temporarily kept without evaluation

	double fitness;
	double diversity;
	int id;           // each element should have an unique identifier inside a population

	// individuals with same evaluation values (used for compression of population)
	vector<MOSIndividual<R, ADS, DS>*> copies;

	MOSIndividual(Solution<R, ADS>* _s, MultiEvaluation<DS>* _mev) :
			s(_s), mev(_mev)
	{
		fitness = -1;
		diversity = -1;

		id = -1;
	}

	MOSIndividual(Solution<R, ADS>& _s, MultiEvaluation<DS>& _mev) :
			s(&_s.clone()), mev(&_mev.clone())
	{
		fitness = -1;
		diversity = -1;

		id = -1;
	}

	MOSIndividual(const MOSIndividual<R, ADS>& ind) :
			s(&ind.s->clone()), mev(&ind.mev->clone())
	{
		fitness = ind.fitness;
		diversity = ind.diversity;

		id = ind.id;
	}

	virtual ~MOSIndividual()
	{
		if(s)
			delete s;
		if(mev)
			delete mev;
	}

	virtual bool betterThan(const MOSIndividual<R, ADS>& ind) const
	{
		// assuming minimization of fitness and maximization of diversity
		if(fitness < ind.fitness)
			return true;
		if((fitness == ind.fitness))
			return diversity > ind.diversity;
		return false;
	}

	virtual bool betterFitness(const MOSIndividual<R, ADS>& ind) const
	{
		// assuming minimization of fitness
		return fitness < ind.fitness;
	}

	virtual bool betterDiversity(const MOSIndividual<R, ADS>& ind) const
	{
		// assuming maximization of diversity
		return diversity > ind.diversity;
	}

	// assuming minimization of fitness
	virtual bool minFitness() const
	{
		return true;
	}

	// assuming maximization of diversity
	virtual bool maxDiversity() const
	{
		return true;
	}

	virtual void print() const
	{
		cout << "MOSIndividual #" << id << " fitness=" << fitness << "\t diversity=" << diversity;
		cout << "\t[ ";
		for(unsigned e = 0; e < mev->size(); e++)
			cout << mev->at(e).evaluation() << (e == mev->size() - 1 ? " " : " ; ");
		cout << " ]";
		cout << endl;
	}

	virtual MOSIndividual<R, ADS, DS>& clone() const
	{
		return *new MOSIndividual<R, ADS, DS>(*this);
	}

	static void compress(vector<MOSIndividual<R, ADS, DS>*>& P)
	{
		for(int s = 0; s < ((int) P.size()) - 1; s++)
			for(int j = s + 1; j < ((int) P.size()); j++)
				if(P[s]->mev->sameValues(*P[j]->mev))
				{
					P[s]->copies.push_back(P[j]);
					P.erase(P.begin() + j);
					j--;
				}
	}

	static void spreadToCopies(vector<MOSIndividual<R, ADS, DS>*>& P)
	{
		for(unsigned s = 0; s < P.size(); s++)
			for(int j = 0; j < P[s]->copies.size(); j++)
			{
				P[s]->copies[j]->fitness = P[s]->fitness;
				P[s]->copies[j]->distance = P[s]->distance;
			}
	}

};

/*
template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MOSPopulation
{
protected:
	vector<MOSIndividual<R, ADS, DS>*> P;

public:
	MOSPopulation()
	{
	}

	MOSPopulation(MOSIndividual<R, ADS, DS>* ind)
	{
		P.push_back(ind);
	}

	MOSPopulation(const vector<MOSIndividual<R, ADS, DS>*>& _P) :
			P(_P)
	{
	}

	virtual ~MOSPopulation()
	{
	}

	virtual void setVector(vector<MOSIndividual<R, ADS, DS>*>& v)
	{
		P = v;
	}

	virtual vector<MOSIndividual<R, ADS, DS>*>& getVector()
	{
		return P;
	}

	virtual vector<MOSIndividual<R, ADS, DS>*> getVector() const
	{
		return P;
	}

	virtual MOSIndividual<R, ADS, DS>* at(unsigned id) const
	{
		return P[id];
	}

	virtual unsigned size() const
	{
		return P.size();
	}

	virtual void add(MOSIndividual<R, ADS, DS>* ind)
	{
		P.push_back(ind);
	}

	virtual void add(MOSPopulation<R, ADS, DS>& Pop)
	{
		P.insert(P.end(), Pop.P.begin(), Pop.P.end());
	}

	virtual void add(vector<MOSIndividual<R, ADS, DS>*>& v)
	{
		P.insert(P.end(), v.begin(), v.end());
	}

	virtual void clear()
	{
		P.clear();
	}

	virtual void free()
	{
		for(unsigned i = 0; i < P.size(); i++)
			if(P[i])
				delete P[i];
		P.clear();
	}
};
*/

}

#endif /*OPTFRAME_MOS_INDIVIDUAL_HPP_*/
