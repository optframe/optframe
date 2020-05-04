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

#ifndef VARIABLENEIGHBORHOODDESCENTUPDATEADS_HPP_
#define VARIABLENEIGHBORHOODDESCENTUPDATEADS_HPP_

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSEnum.hpp"

#include "VND.h"

namespace optframe {

//template<XRepresentation R, class ADS, XBaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
template<XRepresentation R, class ADS, XBaseSolution<R,ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class VariableNeighborhoodDescentUpdateADS : public LocalSearch<XES, XEv>
{
private:
   GeneralEvaluator<XES, XEv>& ev;
   ADSManager<R, ADS, S>& adsMan;
   vector<LocalSearch<XES, XEv>*> lsList;

public:
   VariableNeighborhoodDescentUpdateADS(GeneralEvaluator<XES, XEv>& _ev, ADSManager<R, ADS, S>& _adsMan, vector<LocalSearch<XES, XEv>*> _lsList)
     : ev(_ev)
     , adsMan(_adsMan)
     , lsList(_lsList)
   {
   }

   virtual ~VariableNeighborhoodDescentUpdateADS()
   {
   }

   // DEPRECATED
	//virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
	//{
	//	Evaluation<> e = std::move(ev.evaluate(s));
	//	exec(s, e, stopCriteria);
	//}

	virtual SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& sosc) override
	{
      //S& s = se.first;
      //XEv& e = se.second;
      //
      double timelimit = sosc.timelimit;
      
      //XEv target_f(sosc.target_f); // 'target_f' will break... removing

      long tini = time(nullptr);

      int r = lsList.size();

      int k = 1;

      long tnow = time(nullptr);
      while ((k <= r) && ((tnow - tini) < timelimit)) //&& ev.betterThan(target_f, e))
      {
         
         // avoiding heap
         //S* s0 = &s.clone();
         //Evaluation<>* e0 = &e.clone();
         //S s0(s); // enough to clone?
         //XEv e0(e);
         pair<S, XEv> p0 = se; // enough to clone?

         lsList[k - 1]->searchFrom(p0, sosc);

         //if (ev.betterThan(p0, se)) {
         //if (p0.second.betterStrict(se.second)) {
         if (ev.betterStrict(p0.second, se.second)) {
            se = p0;
            //delete s0; // no need
            //delete e0; // no need
            k = 1;
         } else {
            //Find move ID
            string localSearchID = lsList[k - 1]->toString();
            unsigned found = localSearchID.find("OptFrame");
            string moveID = localSearchID.substr(found);
            adsMan.setNeighLocalOptimum(se.first, moveID);

            //delete s0; // no need
            //delete e0; // no need

            k = k + 1;
         }
         //
         ev.reevaluate(se);

         tnow = time(nullptr);
      }
      return SearchStatus::NO_REPORT;
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearch<XES, XEv>::idComponent() << ":VNDUpdateADS";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual string toString() const
   {
      stringstream ss;
      ss << "VND: [ ";
      for (unsigned i = 0; i < lsList.size(); i++) {
         ss << lsList[i]->toString();
         if (i != lsList.size() - 1)
            ss << ",";
      }
      ss << "]";

      return ss.str();
   }
};

///template<XESolution XES, XEvaluation XEv = Evaluation<>>
//template<XRepresentation R, class ADS, XBaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
// passing 'S' manually, for safety
template<XRepresentation R, class ADS, XBaseSolution<R,ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class VariableNeighborhoodDescentUpdateADSBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~VariableNeighborhoodDescentUpdateADSBuilder()
   {
   }

   virtual LocalSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      GeneralEvaluator<XES, XEv>* eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      ADSManager<R, ADS, S>* adsMan;
      hf.assign(adsMan, *scanner.nextInt(), scanner.next()); // reads backwards!

      vector<LocalSearch<XES, XEv>*> hlist;
      hf.assignList(hlist, *scanner.nextInt(), scanner.next()); // reads backwards!

      return new VariableNeighborhoodDescentUpdateADS<R, ADS, S, XEv>(*eval, *adsMan, hlist);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));

      params.push_back(make_pair(ADSManager<R, ADS, S>::idComponent(), "ADSManager function"));

      stringstream ss;
      ss << LocalSearch<XES, XEv>::idComponent() << "[]";
      params.push_back(make_pair(ss.str(), "list of local searches"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == VariableNeighborhoodDescentUpdateADS<R, ADS, S, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearchBuilder<S, XEv>::idComponent() << ":VNDUpdateADS";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};
}

#endif /*VARIABLENEIGHBORHOODDESCENTUPDATEADS_HPP_*/
