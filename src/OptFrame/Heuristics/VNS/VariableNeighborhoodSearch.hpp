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

template<XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class VariableNeighborhoodSearch: public VNS, public SingleObjSearch<XES>
{
protected:
	GeneralEvaluator<XES, XEv>& evaluator;
	InitialSearch<XES, XEv>& constructive;
	vector<NS<XES, XEv, XSH>*> vshake;
	vector<NSSeq<XES, XEv, XSH>*> vsearch;

public:

	VariableNeighborhoodSearch(GeneralEvaluator<XES, XEv>& _evaluator, InitialSearch<XES, XEv>& _constructive, vector<NS<XES, XEv>*> _vNS, vector<NSSeq<XES, XEv, XSH>*> _vNSSeq) :
		evaluator(_evaluator), constructive(_constructive), vshake(_vNS), vsearch(_vNSSeq)
	{
	}

	virtual ~VariableNeighborhoodSearch()
	{
	}

	//virtual void improvement(XSolution& s, Evaluation<>& e, double timelimit, double target_f) = 0;

	virtual void shake(XES& se, unsigned int k_shake, const StopCriteria<XEv>& sosc)
	{
      //double timelimit = sosc.timelimit;
      //XEv target_f(sosc.target_f); // BROKEN
      //
      //XSolution& s = se.first;
      //Evaluation<>& e = se.second;
		uptr<Move<XES, XEv>> move = vshake.at(k_shake)->validRandomMove(se);
		if(move)
		{
         move->applyUpdate(se);
			evaluator.reevaluate(se); // refresh 'e'
		}
	}

	virtual pair<XES, unsigned int> neighborhoodChange(const XES& star, const XES& p2, unsigned int k)
	{
      //const XSolution& s2 = p2.first;
      const XEv& e2 = p2.second;
      //const XSolution& sStar = star.first;
      const XEv& eStar = star.second;
      //
		//if (evaluator.betterThan(e2, eStar))
      //if (e2.betterStrict(eStar))
      if (evaluator.betterStrict(e2, eStar))
		{
			// IMPROVEMENT!
			//XES p(s2.clone(), e2.clone()); // TODO: avoid leak!!
         XES p = p2; // copy! AVOID


			if(Component::information)
         {
				cout << "VNS: improvement at NS " << k << " => " << e2.evaluation() << endl;
            //e2.print();
            //eStar.print();
         }
			return make_pair(p, 0); // k <- 0
		}
		else
		{
			//XES p(sStar.clone(), eStar.clone()); // TODO: avoid leak!
         XES p = star; // COPY: AVOID!
			return make_pair(p, k+1);
		}
	}

	virtual LocalSearch<XES, XEv>& buildSearch(unsigned k_search) = 0;

/*
   XES genPair(double timelimit)
   {
      std::optional<S> sStar = constructive.generateSolution(timelimit);
		XEv eStar = evaluator.evaluate(*sStar);
      return make_pair(*sStar, eStar); 
   }
*/   

	//pair<S, Evaluation<>>* search(StopCriteria<XEv>& sosc,  const S* _s = nullptr,  const Evaluation<>* _e = nullptr) override
   //virtual std::optional<XES> search(StopCriteria<XEv>& sosc) override
   SearchStatus search(op<XES>& star, const StopCriteria<XEv>& sosc) override
	{
      double timelimit = sosc.timelimit;
      //XEv target_f(sosc.target_f); // BROKEN
		//cout << id() << " search(target=" << target_f << ", timelimit=" << timelimit << ")" << endl;
      cout << id() << " search(" << "timelimit=" << timelimit << ")" << endl; // TODO: 'stop'.toString()

		Timer tnow;

		//XSolution& sStar = *constructive.generateSolution(sosc.timelimit);
		//Evaluation<>   eStar = evaluator.evaluate(sStar);
      //XES star = input?*input:genPair(sosc.timelimit); // elvis
      //star = star?:genPair(sosc.timelimit);
      star = star?:constructive.initialSearch(sosc);
      //
      XSolution& sStar = star->first;
		Evaluation<>& eStar = star->second;

		if(Component::information)
			cout << "VNS starts: " << eStar.evaluation() << endl;

		while((tnow.now() < timelimit))//  && evaluator.betterThan(target_f, eStar))
		{
			unsigned k = 0;
         //cout << "VNS k=0 initial target = " << target_f << " timelimit = " << timelimit << endl;
         //cout << eStar.evaluation() << endl;
         //cout << evaluator.betterThan(target_f, eStar) << endl;
         //cout << evaluator.betterThan(8000, 7962.15) << endl;

			while(k < vshake.size())
			{
            XES p1 = *star; // copy (how to automatically invoke clone?)
				////XSolution& s = *new S(sStar); // implicit clone on copy constructor
				////Evaluation<>&   e = eStar.clone();

				shake(p1, k, sosc);

				LocalSearch<XES, XEv>& improve = buildSearch(k);

				improve.searchFrom(p1, sosc);
   
				delete& improve; // save trajectory history?

				pair<XES, unsigned int> nc = neighborhoodChange(*star, p1, k);

				sStar = nc.first.first;  // TODO: move?
				eStar = nc.first.second; // TODO: move?

				//delete& nc.first.first; // drop automatically?
				//delete& nc.first.second; // drop automatically?

				//delete& s; // drop automatically?
				//delete& e; // drop automatically?

            if(k != nc.second)
            {
               //cout << "VNS k change from " << k << " to " << nc.second << endl;
            }
				k = nc.second;
			}
		}

      //if (sosc.target && evaluator.betterThan(star->second, sosc.target->second))
      //if (star->second.betterStrict(sosc.target_f))
      if (evaluator.betterStrict(star->second, sosc.target_f))
      {
			cout << "VNS exit by target_f: " << star->second.evaluation() << " better than " << sosc.target_f.evaluation() << endl;
         //cout << "isMin: " << evaluator.isMinimization() << endl;
         cout << "isMin: " << star->second.isMini << endl;
      }

      if (((tnow.now()) >= timelimit))
      {
			cout << "VNS exit by timelimit: " << timelimit << endl;
      }

		//return std::optional<XES> (star);
      return SearchStatus::VALID_SOL;
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<XES>::idComponent() << VNS::family();
		//ss << SingleObjSearch<XES>::idComponent() << VNS::family << "VariableNeighborhoodSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_VNS_HPP_*/
