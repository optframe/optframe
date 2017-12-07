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

#ifndef OPTFRAME_VNS_HPP_
#define OPTFRAME_VNS_HPP_

#include <math.h>
#include <vector>

#include "../../Constructive.h"
#include "../../SingleObjSearch.hpp"
#include "../../Evaluator.hpp"
#include "../../NS.hpp"
#include "../../NSSeq.hpp"
#include "../../LocalSearch.hpp"
#include "../../Timer.hpp"

#include "VNS.h"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class VariableNeighborhoodSearch: public VNS, public SingleObjSearch<R, ADS>
{
protected:
	Evaluator<R, ADS>& evaluator;
	Constructive<R, ADS>& constructive;
	vector<NS<R, ADS>*> vshake;
	vector<NSSeq<R, ADS>*> vsearch;

public:

	VariableNeighborhoodSearch(Evaluator<R, ADS>& _evaluator, Constructive<R, ADS>& _constructive, vector<NS<R, ADS>*> _vNS, vector<NSSeq<R, ADS>*> _vNSSeq) :
		evaluator(_evaluator), constructive(_constructive), vshake(_vNS), vsearch(_vNSSeq)
	{
	}

	virtual ~VariableNeighborhoodSearch()
	{
	}

	//virtual void improvement(Solution<R, ADS>& s, Evaluation& e, double timelimit, double target_f) = 0;

	virtual void shake(Solution<R, ADS>& s, Evaluation& e, unsigned int k_shake, double timelimit, double target_f)
	{
		Move<R, ADS>* move = vshake.at(k_shake)->validMove(s);
		if(move)
		{
			Component::safe_delete(move->apply(e, s));
			evaluator.evaluate(e, s); // refresh 'e'
			delete move;
		}
	}

	virtual pair<pair<Solution<R, ADS>&, Evaluation&>, unsigned int> neighborhoodChange(const Solution<R, ADS>& sStar, const Evaluation& eStar, const Solution<R, ADS>& s2, const Evaluation& e2, unsigned int k)
	{
		if (evaluator.betterThan(e2, eStar))
		{
			// IMPROVEMENT!
			pair<Solution<R, ADS>&, Evaluation&> p(s2.clone(), e2.clone());
			if(Component::information)
				cout << "VNS: improvement at NS " << k << " => " << e2.evaluation() << endl;
			return make_pair(p, 0); // k <- 0
		}
		else
		{
			pair<Solution<R, ADS>&, Evaluation&> p(sStar.clone(), eStar.clone());
			return make_pair(p, k+1);
		}
	}

	virtual LocalSearch<R, ADS>& buildSearch(unsigned k_search) = 0;

	pair<Solution<R, ADS>&, Evaluation&>* search(double timelimit = 100000000, double target_f = 0,  const Solution<R, ADS>* _s = nullptr,  const Evaluation* _e = nullptr)
	{
		if(timelimit == 0)
			timelimit = 36000; // 10 hours
		cout << id() << " search(target=" << target_f << ", timelimit=" << timelimit << ")" << endl;

		Timer tnow;

		Solution<R, ADS>& sStar = constructive.generateSolution();
		Evaluation&   eStar = evaluator.evaluate(sStar);

		if(Component::information)
			cout << "VNS starts: " << eStar.evaluation() << endl;

		while(evaluator.betterThan(target_f, eStar.evaluation()) && ((tnow.now()) < timelimit))
		{
			unsigned k = 0;

			while(k < vshake.size())
			{
				Solution<R, ADS>& s = sStar.clone();
				Evaluation&   e = eStar.clone();

				shake(s, e, k, timelimit, target_f);

				LocalSearch<R, ADS>& improve = buildSearch(k);
				improve.exec(s, e, timelimit, target_f);
				delete& improve; // save trajectory history?

				pair<pair<Solution<R, ADS>&, Evaluation&>, unsigned int> nc = neighborhoodChange(sStar, eStar, s, e, k);

				sStar = nc.first.first;
				eStar = nc.first.second;

				delete& nc.first.first;
				delete& nc.first.second;

				delete& s;
				delete& e;

				k = nc.second;
			}
		}

		return new pair<Solution<R, ADS>&, Evaluation&> (sStar, eStar);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS>::idComponent() << VNS::family();
		//ss << SingleObjSearch<R, ADS>::idComponent() << VNS::family << "VariableNeighborhoodSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_VNS_HPP_*/
