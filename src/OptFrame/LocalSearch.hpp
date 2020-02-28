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

#ifndef OPTFRAME_LOCAL_SEARCH_HPP_
#define OPTFRAME_LOCAL_SEARCH_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "Solution.hpp"
#include "Evaluation.hpp"

#include "Component.hpp"
#include "NSIterator.hpp"
#include "SingleObjSearch.hpp"

namespace optframe
{

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XSE = pair<S, XEv>>
class LocalSearch: public Component
{
   typedef vector<XEv*> FitnessValues;
   typedef const vector<const XEv*> ConstFitnessValues;

public:

   LocalSearch()
   {
   }

   virtual ~LocalSearch()
   {
   }

   // core methods 
   
   //(search: DEPRECATED)
   /*
   // no-optimization
   S& search(const S& s, const StopCriteria<XEv>& stopCriteria)
   {
      S& s2 = s.clone();
      exec(s2, stopCriteria);
      return s2;
   }
   */

   // copy-based version (TODO: deprecate this?)
   XSE lsearch(const XSE& se, const StopCriteria<XEv>& stopCriteria)
   {
      //S& s2 = s.clone();
      //XEv& e2 = e.clone();
      XSE p2 = se; // implicit 'clone' here ??
      searchFrom(p2, stopCriteria);
      //return *new pair<S&, XEv&> (s2, e2);
      return p2;
   }
   

   // core methods

   // 1
   //virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria) = 0;

   // keeping only this method, for simplification
   // 2
   //virtual void exec(pair<S, XEv>& se, const StopCriteria<XEv>& stopCriteria) = 0;
   // TODO: return SearchStatus
   virtual void searchFrom(XSE& se, const StopCriteria<XEv>& stopCriteria) = 0;
   // TODO: perhaps return 'bool' or FLAG indicating possible changes on solution (UNKNOWN, CHANGED, NOCHANGE, IMPROVED, ...)

   // optional: set local optimum status (LOS)
   virtual void setLOS(LOS los, string nsid, S& s, XEv& e)
   {
   }

   // optional: get local optimum status (LOS)
   virtual LOS getLOS(string nsid, S& s, XEv& e)
   {
	   return los_unknown;
   }

   virtual bool compatible(string s)
   {
	   return ( s == idComponent() ) || ( Component::compatible(s) );
   }

   static string idComponent()
   {
	   stringstream ss;
	   ss << Component::idComponent() << ":LocalSearch";
	   return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

};


template<XSolution S, XEvaluation XEv = Evaluation<>, X2ESolution<S, XEv> X2ES = MultiESolution<S, XEv>>
class LocalSearchBuilder : public ComponentBuilder<S, XEv, X2ES>
{
public:
	virtual ~LocalSearchBuilder()
	{
	}

	virtual LocalSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, X2ES>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, X2ES>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<S, XEv, X2ES>::idComponent() << "LocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}


#endif /* OPTFRAME_LOCAL_SEARCH_HPP_ */
