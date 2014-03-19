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

	Solution<R, ADS>& s;
	MultiEvaluation<DS>& mev;

	double fitness;
	double diversity;

	int id; // for debug reasons

	MOSIndividual(Solution<R, ADS>* _s, MultiEvaluation<DS>* _mev) :
			s(*_s), mev(*_mev)
	{
		fitness = -1;
		diversity = -1;

		id = -1;
	}

	MOSIndividual(Solution<R, ADS>& _s, MultiEvaluation<DS>& _mev) :
			s(_s.clone()), mev(_mev.clone())
	{
		fitness = -1;
		diversity = -1;

		id = -1;
	}

	MOSIndividual(const MOSIndividual<R, ADS>& ind) :
			s(ind.s.clone()), mev(ind.mev.clone())
	{
		fitness = ind.rank;
		diversity = ind.distance;

		id = ind.id;
	}

	virtual ~MOSIndividual()
	{
		delete &s;
		delete &mev;
	}

	virtual bool betterThan(const MOSIndividual<R, ADS>& ind)
	{
		// assuming minimization of fitness and maximization of diversity
		if(fitness < ind.fitness)
			return true;
		if((fitness == ind.fitness))
			return diversity > ind.diversity;
		return false;
	}

	virtual void print() const
	{
		cout << "MOSIndividual: fitness=" << fitness << "\t diversity=" << diversity;
		cout << "\t[ ";
		for(unsigned e = 0; e < mev.size(); e++)
			cout << mev.at(e).evaluation() << (e == mev.size() - 1 ? " " : " ; ");
		cout << " ]";
		cout << endl;
	}

	virtual MOSIndividual<R, ADS>& clone() const
	{
		return *new MOSIndividual<R, ADS>(*this);
	}

};

}

#endif /*OPTFRAME_MOS_INDIVIDUAL_HPP_*/
