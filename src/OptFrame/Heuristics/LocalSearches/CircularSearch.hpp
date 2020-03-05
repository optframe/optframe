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

template<XSolution S, XEvaluation XEv = Evaluation<>>
class CircularSearch : public LocalSearch<S, XEv>
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

   // DEPRECATED
	//virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
	//{
	//	Evaluation<> e = std::move(ev.evaluate(s));
	//	exec(s, e, stopCriteria);
	//}

	virtual void searchFrom(pair<S, XEv>& se, const StopCriteria<XEv>& sosc) override
	{
      S& s = se.first;
      //XEv& e = se.second;
      
      //double timelimit = sosc.timelimit;
      //double target_f = sosc.target_f;
      int Wmax = ns.size();

      int w = initial_w % Wmax;

      do {
         uptr<Move<S, XEv>> m = ns.indexMove(w);

         if (m->canBeApplied(s)) {
            bool mayEstimate = false;
            ///MoveCost<>& cost = *eval.moveCost(m, se, mayEstimate);
            op<XEv> cost = eval.moveCost(*m, se, mayEstimate);

            if (eval.isImprovement(*cost)) {
               //double old_f = e.evaluation();

               m->applyUpdate(se);
               eval.reevaluate(se); // updates 'e'

               //cout << "CS improvement! w:" << w << " fo=" << e.evaluation() << " (antiga fo="<< old_f << ")" << endl << endl;

               initial_w = w + 1;

               return;
            }
         }

         w = (w + 1) % Wmax;
      } while (w != initial_w);
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (LocalSearch<S, XEv>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearch<S, XEv>::idComponent() << ":CS";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class CircularSearchBuilder : public LocalSearchBuilder<S, XEv, X2ES>
{
public:
   virtual ~CircularSearchBuilder()
   {
   }

   virtual LocalSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      Evaluator<S, XEv>* eval;
      hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

      NSEnum<S, XEv>* nsenum;
      hf.assign(nsenum, scanner.nextInt(), scanner.next()); // reads backwards!

      return new CircularSearch<S, XEv>(*eval, *nsenum);
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
      return component == CircularSearch<S, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearchBuilder<S, XEv>::idComponent() << ":CS";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};
}

#endif /*OPTFRAME_CS_HPP_*/
