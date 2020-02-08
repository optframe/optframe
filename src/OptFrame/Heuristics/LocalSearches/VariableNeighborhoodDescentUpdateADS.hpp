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

//template<Representation R, class ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
template<Representation R, class ADS, BaseSolution<R,ADS> S, XEvaluation XEv = Evaluation<>>
class VariableNeighborhoodDescentUpdateADS : public LocalSearch<S, XEv>
{
private:
   Evaluator<S, XEv>& ev;
   ADSManager<R, ADS, S>& adsMan;
   vector<LocalSearch<S, XEv>*> lsList;

public:
   VariableNeighborhoodDescentUpdateADS(Evaluator<S, XEv>& _ev, ADSManager<R, ADS, S>& _adsMan, vector<LocalSearch<S, XEv>*> _lsList)
     : ev(_ev)
     , adsMan(_adsMan)
     , lsList(_lsList)
   {
   }

   virtual ~VariableNeighborhoodDescentUpdateADS()
   {
   }

   virtual void exec(S& s, SOSC& sosc) override
   {
      Evaluation<> e = ev.evaluate(s);

      exec(s, e, sosc);
   }

   virtual void exec(S& s, Evaluation<>& e, SOSC& sosc) override
   {
      double timelimit = sosc.timelimit;
      double target_f = sosc.target_f;
      long tini = time(nullptr);

      int r = lsList.size();

      int k = 1;

      long tnow = time(nullptr);
      while (ev.betterThan(target_f, e.evaluation()) && (k <= r) && ((tnow - tini) < timelimit)) {
         S* s0 = &s.clone();
         Evaluation<>* e0 = &e.clone();

         lsList[k - 1]->exec(*s0, *e0, sosc);

         if (ev.betterThan(*s0, s)) {
            s = *s0;
            e = *e0;
            delete s0;
            delete e0;
            k = 1;
         } else {
            //Find move ID
            string localSearchID = lsList[k - 1]->toString();
            unsigned found = localSearchID.find("OptFrame");
            string moveID = localSearchID.substr(found);
            adsMan.setNeighLocalOptimum(s, moveID);

            delete s0;
            delete e0;

            k = k + 1;
         }
         ev.reevaluate(e, s);

         tnow = time(nullptr);
      }
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (LocalSearch<S, XEv>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearch<S, XEv>::idComponent() << ":VNDUpdateADS";
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

///template<XSolution S, XEvaluation XEv = Evaluation<>>
//template<Representation R, class ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
// passing 'S' manually, for safety
template<Representation R, class ADS, BaseSolution<R,ADS> S, XEvaluation XEv = Evaluation<>>
class VariableNeighborhoodDescentUpdateADSBuilder : public LocalSearchBuilder<S, XEv>
{
public:
   virtual ~VariableNeighborhoodDescentUpdateADSBuilder()
   {
   }

   virtual LocalSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv>& hf, string family = "")
   {
      Evaluator<S, XEv>* eval;
      hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

      ADSManager<R, ADS, S>* adsMan;
      hf.assign(adsMan, scanner.nextInt(), scanner.next()); // reads backwards!

      vector<LocalSearch<S, XEv>*> hlist;
      hf.assignList(hlist, scanner.nextInt(), scanner.next()); // reads backwards!

      return new VariableNeighborhoodDescentUpdateADS<R, ADS, S, XEv>(*eval, *adsMan, hlist);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(Evaluator<S, XEv>::idComponent(), "evaluation function"));

      params.push_back(make_pair(ADSManager<R, ADS, S>::idComponent(), "ADSManager function"));

      stringstream ss;
      ss << LocalSearch<S, XEv>::idComponent() << "[]";
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
