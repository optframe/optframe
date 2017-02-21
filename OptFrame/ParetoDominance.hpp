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
#include "Direction.hpp"

#include <iostream>
#include <cmath>

using namespace std;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class ParetoDominance
{
protected:
	// TODO: make Evaluator inherit from Direction!
	vector<Evaluator<R, ADS>*> v_e;
	vector<Direction*> v_d;

public:

	ParetoDominance(vector<Evaluator<R, ADS>*> _v_e) :
			v_e(_v_e)
	{
	}

	ParetoDominance(vector<Direction*> _v_d) :
			v_d(_v_d)
	{
	}

	virtual ~ParetoDominance()
	{
	}

	void insertEvaluators(vector<Evaluator<R, ADS>*> _v_e)
	{
		v_e = _v_e;
	}

	vector<Evaluator<R, ADS>*> getEvaluators()
	{
		return v_e;
	}

	// true if 's1' dominates 's2'
	virtual bool dominates(const Solution<R, ADS>& s1, const Solution<R, ADS>& s2)
	{
		if (v_e.size() == 0)
		{
			cout << "ParetoDominance::error! not implemented for direction only!" << endl;
			exit(1);
		}

		vector<Evaluation*> v1;
		vector<Evaluation*> v2;

		for (int e = 0; e < v_e.size(); e++)
		{
			v1.push_back(&v_e[e]->evaluate(s1));
			v2.push_back(&v_e[e]->evaluate(s2));
		}

		bool r = dominates(v1, v2);

		for (int e = 0; e < v_e.size(); e++)
		{
			delete v1[e];
			delete v2[e];
		}

		return r;
	}

	// true if 's1' dominates 's2'
	virtual bool dominates(const MultiEvaluation* mev1, const MultiEvaluation* mev2)
	{
		return dominates(mev1->getVector(), mev2->getVector());
	}

	// true if 's1' dominates 's2'
	virtual bool dominates(const MultiEvaluation& mev1, const MultiEvaluation& mev2)
	{
		return dominates(mev1.getVector(), mev2.getVector());
	}

	//return a pair of better and equals
	pair<int, int> checkDominates(const vector<Evaluation*>& v1, const vector<Evaluation*>& v2)
	{
		if ((v1.size() != v2.size()) || (v1.size() == 0) || (v2.size() == 0))
		{
			// TODO: throw exception!
			cout << "WARNING in ParetoDominance: different sizes or empty!" << endl;
			return make_pair(-1, -1);
		}

		int better = 0;
		int equals = 0;

		// TODO: make inheritance!
		if (v_e.size() == v1.size())
		{
			for (int e = 0; e < v1.size(); e++)
			{
				if (v_e[e]->betterThan(*v1[e], *v2[e]))
					better++;

				if (abs(v1[e]->evaluation() - v2[e]->evaluation()) < 0.0001)
					equals++;
			}

		}
		else // TODO: make inheritance!
		{
			if (v_d.size() != v1.size())
			{
				// TODO: throw exception!
				cout << "WARNING in ParetoDominance: different sizes." << endl;
				return make_pair(-1, -1);
			}

			for (int e = 0; e < v1.size(); e++)
			{
				if (v_d[e]->betterThan(*v1[e], *v2[e]))
					better++;

				if (abs(v1[e]->evaluation() - v2[e]->evaluation()) < 0.0001)
					equals++;
			}
		}

		return make_pair(better, equals);
	}

	// true if 's1' dominates 's2'
	virtual bool dominates(const vector<Evaluation*>& v1, const vector<Evaluation*>& v2)
	{
		pair<int, int> betterEquals = checkDominates(v1, v2);
		int better = betterEquals.first;
		int equals = betterEquals.second;

		return ((better + equals == v1.size()) && (better > 0));
	}

	// returns pair: (true, if 's1' dominates 's2'; true, if 's2' dominates 's1')
	//virtual pair<bool, bool> birelation(const vector<Evaluation*>& v1, const vector<Evaluation*>& v2)
	//{
	//	bool b1 = dominates(v1, v2);
	//	bool b2 = dominates(v2, v1);
	//	return make_pair(b1, b2);
	//}

	virtual pair<bool, bool> birelation(const vector<Evaluation*>& v1, const vector<Evaluation*>& v2)
	{
		int N = v1.size();
		if ((N != v2.size()) || (N == 0) || (v2.size() == 0))
		{
			// TODO: throw exception!
			cout << "WARNING in ParetoDominance: different sizes or empty!" << endl;
			return make_pair(false, false);
		}

		int better = 0;
		int equals = 0;

		// TODO: make inheritance!
		if (v_e.size() == v1.size())
		{
			for (int e = 0; e < v1.size(); e++)
			{
				if (v_e[e]->betterThan(*v1[e], *v2[e]))
					better++;

				if (abs(v1[e]->evaluation() - v2[e]->evaluation()) < 0.0001)
					equals++;
			}
		}
		else // TODO: make inheritance!
		{
			if (v_d.size() != v1.size())
			{
				// TODO: throw exception!
				cout << "WARNING in ParetoDominance: different sizes." << endl;
				return make_pair(false, false);
			}

			for (int e = 0; e < v1.size(); e++)
			{
				if (v_d[e]->betterThan(*v1[e], *v2[e]))
					better++;

				if (abs(v1[e]->evaluation() - v2[e]->evaluation()) < 0.0001)
					equals++;
			}
		}

		int better2 = N - better - equals;
		// 'v1' dominates 'v2'?
		bool b1 = (better + equals == N) && (better > 0);
		// 'v2' dominates 'v1'?
		bool b2 = (better2 + equals == N) && (better2 > 0);

		return make_pair(b1, b2);
	}

	virtual pair<bool, bool> birelation(const MultiEvaluation& mev1, const MultiEvaluation& mev2)
	{
		return birelation(mev1.getVector(), mev2.getVector());
	}
};

}

#endif /*OPTFRAME_PARETODOMINANCE_HPP_*/
