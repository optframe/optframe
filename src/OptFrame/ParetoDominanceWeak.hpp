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

/*
 * ParetoDominanceSWeak.hpp
 *
 * LGPLv3
 *
 */

#ifndef OPTFRAME_PARETODOMINANCE_WEAK_HPP_
#define OPTFRAME_PARETODOMINANCE_WEAK_HPP_

#include "ParetoDominance.hpp"
#include "MultiEvaluation.hpp"

#include <iostream>
#include <cmath>

using namespace std;

namespace optframe
{

template<XSolution S, XEvaluation XEv = Evaluation<>, XEvaluation XMEv = MultiEvaluation<>>
class ParetoDominanceWeak: public ParetoDominance<S, XEv, XMEv>
{
public:
	using ParetoDominance<S, XEv, XMEv>::dominates;

	ParetoDominanceWeak(MultiEvaluator<S, XEv, XMEv>& _mev) :
			ParetoDominance<S, XEv, XMEv>(_mev)
	{
	}

	virtual ~ParetoDominanceWeak()
	{
	}

	virtual bool dominates(const MultiEvaluation<>& mev1, const MultiEvaluation<>& mev2)
	{
		pair<int,int> betterEquals = ParetoDominance<S, XEv, XMEv>::checkDominates(mev1,mev2);
		int better = betterEquals.first;
		int equals = betterEquals.second;

		return (((unsigned) better + equals ==  mev1.size()));
	}

};

}

#endif /*OPTFRAME_PARETODOMINANCE_WEAK_HPP_*/
