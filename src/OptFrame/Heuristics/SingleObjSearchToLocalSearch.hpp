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

#ifndef OPTFRAME_SINGLE_OBJ_SEARCH_TO_LOCAL_SEARCH_HPP_
#define OPTFRAME_SINGLE_OBJ_SEARCH_TO_LOCAL_SEARCH_HPP_

#include <math.h>
#include <vector>

#include "../Constructive.hpp"
#include "../Evaluation.hpp"
#include "../Evaluator.hpp"
#include "../LocalSearch.hpp"
#include "../SingleObjSearch.hpp"

namespace optframe {

template<XESolution XES, XEvaluation XEv=Evaluation<>>
class SingleObjSearchToLocalSearch : public LocalSearch<XES, XEv>
{
protected:
   Evaluator<XES, XEv>& evaluator;
   SingleObjSearch<XES>& sios;

public:
   SingleObjSearchToLocalSearch(Evaluator<XES, XEv>& _evaluator, SingleObjSearch<XES>& _sios)
     : evaluator(_evaluator)
     , sios(_sios)
   {
   }

   virtual ~SingleObjSearchToLocalSearch()
   {
   }

   // DEPRECATED
	//virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
	//{
	//	Evaluation<> e = std::move(ev.evaluate(s));
	//	exec(s, e, stopCriteria);
	//}

	virtual void searchFrom(XES& se, const StopCriteria<XEv>& sosc) override
	{
      XSolution& s = se.first;
      XEv& e = se.second;

      // will ignore 'se'
      // maybe ALL SingleObjSearch should inherit from LocalSearch!
      // maybe 'LocalSearch' should become 'Search', and 'SingleObjSearch' -> 'GlobalSearch'... must think!

      std::optional<XES> r = make_optional(se);
      sios.search(r, sosc); // TODO: store flag and return

      if (r) {
         s = r->first;
         e = r->second;
         //delete r;  // no need
      }
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << SingleObjSearch<XES>::idComponent() << "SingleObjSearchToLocalSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual void print() const
   {
      cout << "SingleObjSearchToLocalSearch with sios: ";
      sios.print();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class SingleObjSearchToLocalSearchBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~SingleObjSearchToLocalSearchBuilder()
   {
   }

   virtual LocalSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      Evaluator<XES, XEv>* eval;
      hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

      string rest = scanner.rest();

      pair<SingleObjSearch<XES>*, std::string> method;
      method = hf.createSingleObjSearch(rest);

      SingleObjSearch<XES>* h = method.first;

      scanner = Scanner(method.second);

      return new SingleObjSearchToLocalSearch<XES, XEv>(*eval, *h);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
      params.push_back(make_pair(SingleObjSearch<XES>::idComponent(), "single obj search"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == SingleObjSearchToLocalSearch<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearchBuilder<S, XEv>::idComponent() << "SingleObjSearchToLocalSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};
}

#endif
