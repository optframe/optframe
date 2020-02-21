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

	virtual void shake(pair<S, XEv>& p, unsigned int k_shake, double timelimit, double target_f)
	{
      S& s = p.first;
      Evaluation<>& e = p.second;
		Move<S, XEv>* move = vshake.at(k_shake)->validRandomMove(s);
		if(move)
		{
         Move<S, XEv>* rev = move->applyUpdate(e, s);
			Component::safe_delete(rev);
			evaluator.reevaluate(e, s); // refresh 'e'
			delete move;
		}
	}

	virtual pair<pair<S, Evaluation<>>, unsigned int> neighborhoodChange(const pair<S, XEv>& star, const pair<S, XEv>& p2, unsigned int k)
	{
      const S& s2 = p2.first;
      const XEv& e2 = p2.second;
      const S& sStar = star.first;
      const XEv& eStar = star.second;
      //
		if (evaluator.betterThan(e2, eStar))
		{
			// IMPROVEMENT!
			pair<S, Evaluation<>> p(s2.clone(), e2.clone());
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

   pair<S, XEv> genPair(double timelimit)
   {
      std::optional<S> sStar = constructive.generateSolution(timelimit);
		XEv eStar = evaluator.evaluate(*sStar);
      return make_pair(*sStar, eStar); 
   }

	//pair<S, Evaluation<>>* search(SOSC& sosc,  const S* _s = nullptr,  const Evaluation<>* _e = nullptr) override
   virtual std::optional<pair<S, XEv>> search(SOSC& sosc) override
	{
      double timelimit = sosc.timelimit;
      double target_f = sosc.target_f;
		if(timelimit == 0)
			timelimit = 36000; // 10 hours
		cout << id() << " search(target=" << target_f << ", timelimit=" << timelimit << ")" << endl;

		Timer tnow;

		//S& sStar = *constructive.generateSolution(sosc.timelimit);
		//Evaluation<>   eStar = evaluator.evaluate(sStar);
      //pair<S, XEv> star = input?*input:genPair(sosc.timelimit); // elvis
      pair<S, XEv> star = genPair(sosc.timelimit);
      S& sStar = star.first;
		Evaluation<>& eStar = star.second;

		if(Component::information)
			cout << "VNS starts: " << eStar.evaluation() << endl;

		while(evaluator.betterThan(target_f, eStar.evaluation()) && ((tnow.now()) < timelimit))
		{
			unsigned k = 0;

			while(k < vshake.size())
			{
            pair<S, XEv> p1 = star; // copy (how to automatically invoke clone?)
				////S& s = *new S(sStar); // implicit clone on copy constructor
				////Evaluation<>&   e = eStar.clone();

				shake(p1, k, timelimit, target_f);

				LocalSearch<S, XEv>& improve = buildSearch(k);

				improve.searchFrom(p1, sosc);
   
				delete& improve; // save trajectory history?

				pair<pair<S, XEv>, unsigned int> nc = neighborhoodChange(star, p1, k);

				sStar = nc.first.first;  // TODO: move?
				eStar = nc.first.second; // TODO: move?

				//delete& nc.first.first; // drop automatically?
				//delete& nc.first.second; // drop automatically?

				//delete& s; // drop automatically?
				//delete& e; // drop automatically?

				k = nc.second;
			}
		}

		return std::optional<pair<S, XEv>> (star);
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
