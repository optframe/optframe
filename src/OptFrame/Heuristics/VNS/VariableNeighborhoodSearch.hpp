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

#include "../../Constructive.hpp"
#include "../../SingleObjSearch.hpp"
#include "../../Evaluator.hpp"
#include "../../NS.hpp"
#include "../../NSSeq.hpp"
#include "../../LocalSearch.hpp"
#include "../../Timer.hpp"

#include "VNS.h"

namespace optframe
{

template<XSolution S, XEvaluation XEv=Evaluation<>>
class VariableNeighborhoodSearch: public VNS, public SingleObjSearch<S, XEv>
{
protected:
	Evaluator<S, XEv>& evaluator;
	Constructive<S>& constructive;
	vector<NS<S, XEv>*> vshake;
	vector<NSSeq<S, XEv>*> vsearch;

public:

	VariableNeighborhoodSearch(Evaluator<S, XEv>& _evaluator, Constructive<S>& _constructive, vector<NS<S, XEv>*> _vNS, vector<NSSeq<S, XEv>*> _vNSSeq) :
		evaluator(_evaluator), constructive(_constructive), vshake(_vNS), vsearch(_vNSSeq)
	{
	}

	virtual ~VariableNeighborhoodSearch()
	{
	}

	//virtual void improvement(S& s, Evaluation<>& e, double timelimit, double target_f) = 0;

	virtual void shake(S& s, Evaluation<>& e, unsigned int k_shake, double timelimit, double target_f)
	{
		Move<S, XEv>* move = vshake.at(k_shake)->validrandomMove(s);
		if(move)
		{
         Move<S, XEv>* rev = move->applyUpdate(e, s);
			Component::safe_delete(rev);
			evaluator.reevaluateSolution(e, s); // refresh 'e'
			delete move;
		}
	}

	virtual pair<pair<S&, Evaluation<>&>, unsigned int> neighborhoodChange(const S& sStar, const Evaluation<>& eStar, const S& s2, const Evaluation<>& e2, unsigned int k)
	{
		if (evaluator.betterThan(e2, eStar))
		{
			// IMPROVEMENT!
			pair<S&, Evaluation<>&> p(s2.clone(), e2.clone());
			if(Component::information)
				cout << "VNS: improvement at NS " << k << " => " << e2.evaluation() << endl;
			return make_pair(p, 0); // k <- 0
		}
		else
		{
			pair<S&, Evaluation<>&> p(sStar.clone(), eStar.clone());
			return make_pair(p, k+1);
		}
	}

	virtual LocalSearch<S, XEv>& buildSearch(unsigned k_search) = 0;

	pair<S, Evaluation<>>* search(SOSC& sosc,  const S* _s = nullptr,  const Evaluation<>* _e = nullptr) override
	{
      double timelimit = sosc.timelimit;
      double target_f = sosc.target_f;
		if(timelimit == 0)
			timelimit = 36000; // 10 hours
		cout << id() << " search(target=" << target_f << ", timelimit=" << timelimit << ")" << endl;

		Timer tnow;

		S& sStar = *constructive.generateSolution(sosc.timelimit);
		Evaluation<>   eStar = evaluator.evaluateSolution(sStar);

		if(Component::information)
			cout << "VNS starts: " << eStar.evaluation() << endl;

		while(evaluator.betterThan(target_f, eStar.evaluation()) && ((tnow.now()) < timelimit))
		{
			unsigned k = 0;

			while(k < vshake.size())
			{
				S& s = *new S(sStar); // implicit clone on copy constructor
				Evaluation<>&   e = eStar.clone();

				shake(s, e, k, timelimit, target_f);

				LocalSearch<S, XEv>& improve = buildSearch(k);
				improve.exec(s, e, sosc);
				delete& improve; // save trajectory history?

				pair<pair<S&, Evaluation<>&>, unsigned int> nc = neighborhoodChange(sStar, eStar, s, e, k);

				sStar = nc.first.first;
				eStar = nc.first.second;

				delete& nc.first.first;
				delete& nc.first.second;

				delete& s;
				delete& e;

				k = nc.second;
			}
		}

		return new pair<S, Evaluation<>> (sStar, eStar);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<S, XEv>::idComponent() << VNS::family();
		//ss << SingleObjSearch<S, XEv>::idComponent() << VNS::family << "VariableNeighborhoodSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_VNS_HPP_*/
