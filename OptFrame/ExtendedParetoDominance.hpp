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

/*
 * ParetoDominance.hpp
 *
 * LGPLv3
 *
 */

#ifndef OPTFRAME_EXTENDED_PARETODOMINANCE_HPP_
#define OPTFRAME_EXTENDED_PARETODOMINANCE_HPP_

#include "Solution.hpp"
#include "Evaluation.hpp"

#include "Evaluator.hpp"
#include "Direction.hpp"

#include "Decoder.hpp"

#include <iostream>
#include <cmath>

using namespace std;

namespace optframe
{

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ExtendedParetoDominance
{
protected:
	// TODO: make Evaluator inherit from Direction!
	vector<Direction<DS>*> v_d;
	Decoder<R, X, ADS, DS>& decoder;

public:

	ExtendedParetoDominance(vector<Direction<DS>*> _v_d, Decoder<R, X, ADS, DS>& _decoder) :
			v_d(_v_d), decoder(_decoder)
	{
	}

	virtual ~ExtendedParetoDominance()
	{
	}

	virtual const MultiEvaluation<DS>& getBest(const vector<MultiEvaluation<DS>*>& vme)
	{
		//cout << "getBest() from " << vme.size();
		//cout << endl;

		if(vme.size() == 0)
		{
			cout << "ExtendedParetoDominance::error: no element!!" << endl;
			exit(1);
		}

		MultiEvaluation<DS>* best = vme.at(0);
		double best_f = 0;
		for(unsigned k = 0; k < v_d.size(); k++)
			best_f += best->at(k).evaluation();

		for(unsigned i = 1; i < vme.size(); i++)
		{
			double value = 0;
			for(unsigned k = 0; k < v_d.size(); k++)
				value += vme[i]->at(k).evaluation();

			if(value < best_f)
			{
				best_f = value;
				best = vme[i];
			}
		}

		/*
		 cout << "BEST IS:";
		 best->print();
		 cout << "FROM: (" << vme.size() << ")";
		 for(unsigned i=0; i<vme.size();i++)
		 vme[i]->print();
		 getchar();
		 */

		return *best;
	}

	// true if 's1' dominates 's2'
	virtual bool dominates(const Solution<R, ADS>& s1, const Solution<R, ADS>& s2)
	{
		//cout << "ExtendedParetoDominance::dominates()" << endl;

		pair<vector<Solution<X, ADS>*>, vector<MultiEvaluation<DS>*> > dec_vs1 = decoder.decode(s1);
		const MultiEvaluation<DS>& best_s1 = getBest(dec_vs1.second);
		pair<vector<Solution<X, ADS>*>, vector<MultiEvaluation<DS>*> > dec_vs2 = decoder.decode(s2);
		const MultiEvaluation<DS>& best_s2 = getBest(dec_vs2.second);

		//cout << "getBest ok!" << endl;

		vector<const Evaluation<DS>*> v1;
		vector<const Evaluation<DS>*> v2;

		for(unsigned k = 0; k < v_d.size(); k++)
		{
			v1.push_back(&best_s1.at(k));
			v2.push_back(&best_s2.at(k));
		}

		bool r = dominates(v1, v2);

		for(int i = 0; i < dec_vs1.second.size(); i++)
			delete dec_vs1.second.at(i);

		for(int i = 0; i < dec_vs2.second.size(); i++)
			delete dec_vs2.second.at(i);

		//cout << "finished s1, s2 dominance" << endl;
		return r;
	}

	// true if 's1' dominates 's2'
	virtual bool dominates(const vector<const Evaluation<DS>*> v1, const vector<const Evaluation<DS>*> v2)
	{
		if((v1.size() != v2.size()) || (v1.size() == 0) || (v2.size() == 0))
		{
			// TODO: throw exception!
			cout << "WARNING in ParetoDominance: different sizes or empty!" << endl;
			return false;
		}

		int better = 0;
		int equals = 0;

		if(v_d.size() != v1.size())
		{
			// TODO: throw exception!
			cout << "WARNING in ParetoDominance: different sizes." << endl;
			return false;
		}

		for(int e = 0; e < v1.size(); e++)
		{
			if(v_d[e]->betterThan(*v1[e], *v2[e]))
				better++;

			if(abs(v1[e]->evaluation() - v2[e]->evaluation()) < 0.0001)
				equals++;
		}

		return ((better + equals == v1.size()) && (better > 0));
	}

};

}

#endif /*OPTFRAME_EXTENDED_PARETODOMINANCE_HPP_*/
