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

#ifndef OPTFRAME_CS_HPP_
#define OPTFRAME_CS_HPP_

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSEnum.hpp"

namespace optframe {

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class CircularSearch : public LocalSearch<R, ADS, S>
{
private:
   Evaluator<S, XEv>& eval;
   NSEnum<S, XEv>& ns;

   int initial_w;

public:
   CircularSearch(Evaluator<S, XEv>& _eval, NSEnum<S, XEv>& _nsEnum)
     : eval(_eval)
     , ns(_nsEnum)
   {
      initial_w = 0;
   }

   virtual ~CircularSearch()
   {
   }

   virtual void exec(S& s, SOSC& sosc) override
   {
      Evaluation<> e = eval.evaluateSolution(s);
      exec(s, e, sosc);
   }

   virtual void exec(S& s, Evaluation<>& e, SOSC& sosc) override
   {
      //double timelimit = sosc.timelimit;
      //double target_f = sosc.target_f;
      int Wmax = ns.size();

      int w = initial_w % Wmax;

      do {
         Move<S, XEv>& m = *ns.indexMove(w);

         if (m.canBeApplied(s)) {
            bool mayEstimate = false;
            MoveCost<>& cost = *eval.moveCost(e, m, s, mayEstimate);

            if (eval.isImprovement(cost)) {
               //double old_f = e.evaluation();

               Component::safe_delete(m.applyUpdate(e, s));
               eval.reevaluateSolution(e, s); // updates 'e'

               //cout << "CS improvement! w:" << w << " fo=" << e.evaluation() << " (antiga fo="<< old_f << ")" << endl << endl;

               initial_w = w + 1;

               delete &cost;
               delete &m;
               return;
            }

            delete &cost;
         }

         delete &m;

         w = (w + 1) % Wmax;
      } while (w != initial_w);
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (LocalSearch<R, ADS, S>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearch<R, ADS, S>::idComponent() << ":CS";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class CircularSearchBuilder : public LocalSearchBuilder<R, ADS, S>
{
public:
   virtual ~CircularSearchBuilder()
   {
   }

   virtual LocalSearch<R, ADS, S>* build(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
   {
      Evaluator<S, XEv>* eval;
      hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

      NSEnum<S, XEv>* nsenum;
      hf.assign(nsenum, scanner.nextInt(), scanner.next()); // reads backwards!

      return new CircularSearch<R, ADS, S>(*eval, *nsenum);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(Evaluator<S, XEv>::idComponent(), "evaluation function"));
      params.push_back(make_pair(NSEnum<S, XEv>::idComponent(), "neighborhood structure"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == CircularSearch<R, ADS, S>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearchBuilder<R, ADS, S>::idComponent() << ":CS";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};
}

#endif /*OPTFRAME_CS_HPP_*/
