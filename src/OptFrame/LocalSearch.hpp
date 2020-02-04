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

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class LocalSearch: public Component
{
   typedef vector<Evaluation<>*> FitnessValues;
   typedef const vector<const Evaluation<>*> ConstFitnessValues;

public:

   LocalSearch()
   {
   }

   virtual ~LocalSearch()
   {
   }

   // core methods

   // no-optimization
   S& search(const S& s, SOSC& stopCriteria)
   {
      S& s2 = s.clone();
      exec(s2, stopCriteria);
      return s2;
   }

   // optimizated version
   pair<S&, Evaluation<>&>& search(const S& s, const Evaluation<>& e, SOSC& stopCriteria) override
   {
      S& s2 = s.clone();
      Evaluation<>& e2 = e.clone();
      exec(s2, e2, stopCriteria);
      return *new pair<S&, Evaluation<>&> (s2, e2);
   }


   // core methods

   // 1
   virtual void exec(S& s, SOSC& stopCriteria) = 0;

   // 2
   virtual void exec(S& s, Evaluation<>& e, SOSC& stopCriteria) = 0;

   // optional: set local optimum status (LOS)
   virtual void setLOS(LOS los, string nsid, S& s, Evaluation<>& e)
   {
   }

   // optional: get local optimum status (LOS)
   virtual LOS getLOS(string nsid, S& s, Evaluation<>& e)
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


template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class LocalSearchBuilder : public ComponentBuilder<R, ADS, S>
{
public:
	virtual ~LocalSearchBuilder()
	{
	}

	virtual LocalSearch<R, ADS, S>* build(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS, S>::idComponent() << "LocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}


#endif /* OPTFRAME_LOCAL_SEARCH_HPP_ */
