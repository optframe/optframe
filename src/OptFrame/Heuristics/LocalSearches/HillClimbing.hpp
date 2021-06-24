// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_HILLCLIMBING_HPP_
#define OPTFRAME_HILLCLIMBING_HPP_

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"

namespace optframe {

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class HillClimbing : public LocalSearch<XES, XEv>
{
private:
   sref<GeneralEvaluator<XES, XEv>> evaluator;
   sref<LocalSearch<XES, XEv>> ls;

public:
   HillClimbing(sref<GeneralEvaluator<XES, XEv>> _ev, sref<LocalSearch<XES, XEv>> _ls)
     : evaluator(_ev)
     , ls(_ls)
   {
   }

   virtual ~HillClimbing()
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
      XEv& e = se.second;
      //
      double timelimit = sosc.timelimit;

      long tini = time(nullptr);

      //Evaluation<>* e0 = &e.clone();
      XEv e0(e); // avoid that using return status on 'exec'

      ls->searchFrom(se, sosc);

      long tnow = time(nullptr);

      // while improvement is found
      //while ((evaluator.betterThan(e, e0)) && ((tnow - tini) < timelimit))
      //while ((e.betterStrict(e0)) && ((tnow - tini) < timelimit))
      while ((evaluator->betterStrict(e, e0)) && ((tnow - tini) < timelimit)) {
         //delete e0;
         //e0 = &e.clone();

         //   (*e0) = e;
         e0 = e;

         ls->searchFrom(se, sosc);

         tnow = time(nullptr);
      }

      return SearchStatus::NO_REPORT;
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
   }

   virtual string id() const
   {
      return idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearch<XES, XEv>::idComponent() << ":HC";
      return ss.str();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class HillClimbingBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~HillClimbingBuilder()
   {
   }

   virtual LocalSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      sptr<GeneralEvaluator<XES, XEv>> eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      string rest = scanner.rest();

      pair<sptr<LocalSearch<XES, XEv>>, std::string> method;
      method = hf.createLocalSearch(rest);

      sptr<LocalSearch<XES, XEv>> h = method.first;

      scanner = Scanner(method.second);

      return new HillClimbing<XES, XEv>(eval, h);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
      params.push_back(make_pair(LocalSearch<XES, XEv>::idComponent(), "local search"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == HillClimbing<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearchBuilder<S, XEv>::idComponent() << ":HC";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

}

#endif /*OPTFRAME_HILLCLIMBING_HPP_*/
