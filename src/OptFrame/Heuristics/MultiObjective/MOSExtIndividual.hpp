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

#ifndef OPTFRAME_MOS_EXTENDED_INDIVIDUAL_HPP_
#define OPTFRAME_MOS_EXTENDED_INDIVIDUAL_HPP_

#include <algorithm>

#include "../../MultiObjSearch.hpp"
#include "../../Solution.hpp"
#include "../../Evaluator.hpp"
#include "../../Evaluation.hpp"
#include "../../Population.hpp"
#include "../../NSSeq.hpp"
#include "../../ParetoDominance.hpp"

#include "MOSIndividual.hpp"

namespace optframe
{

// MultiObjSearch Extended Individual
template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MOSExtIndividual: public MOSIndividual<X, ADS, DS>
{
public:

	MOSIndividual<R>& parent;

	MOSExtIndividual(Solution<X, ADS>* s, MultiEvaluation<DS>* mev, MOSIndividual<R>* _parent) :
			MOSIndividual<X, ADS, DS>(s, mev), parent(*_parent)
	{
	}

	MOSExtIndividual(Solution<R, ADS>& s, MultiEvaluation<DS>& mev, MOSIndividual<R>* _parent) :
			MOSIndividual<X, ADS, DS>(s, mev), parent(*_parent)
	{
	}

	MOSExtIndividual(const MOSExtIndividual<R, X, ADS, DS>& ind) :
			MOSIndividual<X, ADS, DS>(&ind.s->clone(), &ind.mev->clone()), parent(ind.parent)
	{
		this->fitness = ind.fitness;
		this->diversity = ind.diversity;

		this->id = ind.id;
	}

	virtual ~MOSExtIndividual()
	{
	}

	virtual void print() const
	{
		cout << "MOSExtIndividual: parent=" << &parent << " fitness=" << this->fitness << "\t diversity=" << this->diversity;
		cout << "\t[ ";
		for(unsigned e = 0; e < this->mev->size(); e++)
			cout << this->mev->at(e).evaluation() << (e == this->mev->size() - 1 ? " " : " ; ");
		cout << " ]";
		cout << endl;
	}

	virtual MOSIndividual<X, ADS, DS>& clone() const
	{
		return *new MOSExtIndividual<R, X, ADS, DS>(*this);
	}

};

/*
template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MOSExtPopulation: public MOSPopulation<R>
{
public:

	vector<MOSExtIndividual<R, X>*> PX;

	MOSExtPopulation() :
			MOSPopulation<R>()
	{
	}

	MOSExtPopulation(const vector<MOSIndividual<R>*>& _PS, const vector<MOSExtIndividual<R, ADS, DS>*>& _PX) :
			MOSPopulation<R>(_PS), PX(_PX)
	{
	}

	virtual ~MOSExtPopulation()
	{
	}

	inline void setVector(vector<MOSIndividual<R>*>& v)
	{
		this->P = v;
	}

	inline void setVector(vector<MOSExtIndividual<R, X>*>& v)
	{
		PX = v;
	}

	// reconfigure GET methods for 'PX'
	inline vector<MOSIndividual<R>*>& getVector()
	{
		return getXVector();
	}

	inline vector<MOSIndividual<R>*>& getSVector()
	{
		return this->P;
	}

	inline vector<MOSExtIndividual<R, X>*>& getXVector()
	{
		return PX;
	}

	// reconfigure GET methods for 'PX'
	inline vector<MOSIndividual<R>*> getVector() const
	{
		return getXVector();
	}

	inline vector<MOSIndividual<R>*> getSVector() const
	{
		return this->P;
	}

	inline vector<MOSExtIndividual<R, X, ADS, DS>*> getXVector() const
	{
		return PX;
	}

	// reconfigure GET methods for 'PX'
	inline MOSIndividual<R>* at(unsigned id) const
	{
		return PX[id];
	}

	inline MOSIndividual<R>* atS(unsigned id) const
	{
		return this->P[id];
	}

	inline MOSExtIndividual<R, X, ADS, DS>* atX(unsigned id) const
	{
		return PX[id];
	}

	// reconfigure GET methods for 'PX'
	inline unsigned size() const
	{
		return PX.size();
	}

	inline unsigned sizeS() const
	{
		return this->P.size();
	}

	inline unsigned sizeX() const
	{
		return PX.size();
	}

	inline void add(MOSIndividual<R>* ind)
	{
		this->P.push_back(ind);
	}

	inline void add(MOSExtIndividual<R, X, ADS, DS>* ind)
	{
		PX.push_back(ind);
	}

	virtual void add(MOSPopulation<R>& Pop)
	{
		this->P.insert(this->P.end(), Pop.P.begin(), Pop.P.end());
	}

	virtual void add(MOSExtPopulation<R, X, ADS, DS>& Pop)
	{
		this->P.insert(this->P.end(), Pop.P.begin(), Pop.P.end());
		PX.insert(PX.end(), Pop.PX.begin(), Pop.PX.end());
	}

	inline void add(vector<MOSIndividual<R>*>& v)
	{
		this->P.insert(this->P.end(), v.begin(), v.end());
	}

	inline void add(vector<MOSExtIndividual<R, X, ADS, DS>*>& v)
	{
		PX.insert(PX.end(), v.begin(), v.end());
	}

	// reconfigure for both
	inline void clear()
	{
		PX.clear();
		this->P.clear();
	}

	// reconfigure for both
	virtual void free()
	{
		for(unsigned i = 0; i < this->P.size(); i++)
			if(this->P[i])
				delete this->P[i];
		this->P.clear();

		for(unsigned i = 0; i < PX.size(); i++)
			if(PX[i])
				delete PX[i];
		PX.clear();
	}
};
*/

}

#endif /*OPTFRAME_MOS_EXTENDED_INDIVIDUAL_HPP_*/
