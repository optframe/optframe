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

#ifndef OPTFRAME_CS_HPP_
#define OPTFRAME_CS_HPP_

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSEnum.hpp"

namespace optframe {

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class CircularSearch : public LocalSearch<XES, XEv>
{
private:
   sref<GeneralEvaluator<XES, XEv>> eval;
   sref<NSEnum<XES, XEv>> ns;

   int initial_w;

public:
   CircularSearch(sref<GeneralEvaluator<XES, XEv>> _eval, sref<NSEnum<XES, XEv>> _nsEnum)
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

   virtual SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& sosc) override
   {
      //XSolution& s = se.first;
      //XEv& e = se.second;

      //double timelimit = sosc.timelimit;
      //double target_f = sosc.target_f;
      int Wmax = ns->size();

      int w = initial_w % Wmax;

      do {
         uptr<Move<XES, XEv>> m = ns->indexMove(w);

         //if (m->canBeApplied(s)) {
         if (m->canBeApplied(se)) {
            bool mayEstimate = false;
            ///MoveCost<>& cost = *eval->moveCost(m, se, mayEstimate);
            op<XEv> cost = eval->moveCost(*m, se, mayEstimate);

            //if (eval->isImprovement(*cost)) {
            //if (cost->isImprovingStrict()) {
            if (eval->isStrictImprovement(*cost)) {
               //double old_f = e.evaluation();

               m->applyUpdate(se);
               eval->reevaluate(se); // updates 'e'

               //cout << "CS improvement! w:" << w << " fo=" << e.evaluation() << " (antiga fo="<< old_f << ")" << endl << endl;

               initial_w = w + 1;

               return SearchStatus::IMPROVEMENT;
            }
         }

         w = (w + 1) % Wmax;
      } while (w != initial_w);
      return SearchStatus::NO_REPORT;
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearch<XES, XEv>::idComponent() << ":CS";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class CircularSearchBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~CircularSearchBuilder()
   {
   }

   virtual LocalSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      sptr<GeneralEvaluator<XES, XEv>> eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      sptr<NSEnum<XES, XEv>> nsenum;
      hf.assign(nsenum, *scanner.nextInt(), scanner.next()); // reads backwards!

      return new CircularSearch<XES, XEv>(eval, nsenum);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
      params.push_back(make_pair(NSEnum<XES, XEv>::idComponent(), "neighborhood structure"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == CircularSearch<XES, XEv>::idComponent();
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
