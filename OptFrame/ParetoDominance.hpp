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
 * ParetoDominance.hpp
 *
 * LGPLv3
 *
 */

#ifndef OPTFRAME_PARETODOMINANCE_HPP_
#define OPTFRAME_PARETODOMINANCE_HPP_

#include "Solution.hpp"
#include "Evaluation.hpp"

#include "Evaluator.hpp"
#include "MultiEvaluator.hpp"
#include "Direction.hpp"

#include <iostream>
#include <cmath>

using namespace std;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class ParetoDominance
{
public:
	// TODO: make Evaluator inherit from Direction!
	vector<Direction*> v_d;
	MultiEvaluator<R, ADS>& mev;

public:

	ParetoDominance(MultiEvaluator<R, ADS>& _mev) :
			mev(_mev)
	{
	}

	virtual ~ParetoDominance()
	{
	}

//	void insertEvaluators(vector<Evaluator<R, ADS>*> _v_e)
//	{
//		mev.addEvaluator(_v_e);
//	}

//	vector<Evaluator<R, ADS>*> getEvaluators()
//	{
//		return v_e;
//	}

	MultiEvaluator<R, ADS>& getMultiEvaluator()
	{
		return mev;
	}

// true if 's1' dominates 's2'
	virtual bool dominates(const Solution<R, ADS>& s1, const Solution<R, ADS>& s2)
	{
		if (mev.size() == 0)
		{
			cout << "ParetoDominance::error! not implemented for direction only!" << endl;
			exit(1);
		}

		MultiEvaluation mev1(std::move(mev.evaluateSolution(s1)));
		MultiEvaluation mev2(std::move(mev.evaluateSolution(s2)));


		bool r = dominates(mev1, mev2);

		delete &mev1;
		delete &mev2;


		return r;
	}

	// true if 's1' dominates 's2'
	virtual bool dominates(const MultiEvaluation* mev1, const MultiEvaluation* mev2)
	{
		return dominates(*mev1, *mev2);
	}

	// true if 's1' dominates 's2'
	virtual bool dominates(const MultiEvaluation& mev1, const MultiEvaluation& mev2)
	{
		pair<int, int> betterEquals = checkDominates(mev1, mev2);
		int better = betterEquals.first;
		int equals = betterEquals.second;

		return ((better + equals == (int) mev1.size()) && (better > 0));
	}

	//return a pair of better and equals
	pair<int, int> checkDominates(const MultiEvaluation& mev1, const MultiEvaluation& mev2)
	{
		if ((mev1.size() != mev2.size()) || (mev1.size() == 0) || (mev2.size() == 0))
		{
			// TODO: throw exception!
			cout << "WARNING in ParetoDominance: different sizes or empty!" << endl;
			return make_pair(-1, -1);
		}

		int better = 0;
		int equals = 0;

		for (int eIndex = 0; eIndex < (int) mev1.size(); eIndex++)
		{
			if (mev.betterThan(mev1[eIndex], mev2[eIndex], eIndex))
				better++;

			if (mev.equals(mev1[eIndex], mev2[eIndex], eIndex))
				equals++;

//			if (abs(mev1[e].evaluation() - mev2[e].evaluation()) < 0.0001)
//

		}

		return make_pair(better, equals);
	}

	// returns pair: (true, if 's1' dominates 's2'; true, if 's2' dominates 's1')
	//virtual pair<bool, bool> birelation(const vector<Evaluation*>& v1, const vector<Evaluation*>& v2)
	//{
	//	bool b1 = dominates(v1, v2);
	//	bool b2 = dominates(v2, v1);
	//	return make_pair(b1, b2);
	//}
//
//	virtual pair<bool, bool> birelation(const MultiEvaluation& mev1, const MultiEvaluation& mev2)
//	{
//		pair<int, int> betterEquals = checkDominates(mev1, mev2);
//		int better = betterEquals.first;
//		int equals = betterEquals.second;
//
//		int N = mev1.size();
//		int better2 = N - better - equals;
//		// 'v1' dominates 'v2'?
//		bool b1 = (better + equals == N) && (better > 0);
//		// 'v2' dominates 'v1'?
//		bool b2 = (better2 + equals == N) && (better2 > 0);
//
//		return make_pair(b1, b2);
//	}

};

}

#endif /*OPTFRAME_PARETODOMINANCE_HPP_*/
