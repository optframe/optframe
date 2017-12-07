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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class LocalSearch: public Component
{
   typedef vector<Evaluation*> FitnessValues;
   typedef const vector<const Evaluation*> ConstFitnessValues;

public:

   LocalSearch()
   {
   }

   virtual ~LocalSearch()
   {
   }

   // core methods

   // no-optimization
   Solution<R, ADS>& search(const Solution<R, ADS>& s, SOSC& stopCriteria)
   {
      Solution<R, ADS>& s2 = s.clone();
      exec(s2, stopCriteria);
      return s2;
   }

   // optimizated version
   pair<Solution<R, ADS>&, Evaluation&>& search(const Solution<R, ADS>& s, const Evaluation& e, SOSC& stopCriteria)
   {
      Solution<R, ADS>& s2 = s.clone();
      Evaluation& e2 = e.clone();
      exec(s2, e2, stopCriteria);
      return *new pair<Solution<R, ADS>&, Evaluation&> (s2, e2);
   }


   // core methods

   // 1
   virtual void exec(Solution<R, ADS>& s, SOSC& stopCriteria) = 0;

   // 2
   virtual void exec(Solution<R, ADS>& s, Evaluation& e, SOSC& stopCriteria) = 0;

   // optional: set local optimum status (LOS)
   virtual void setLOS(LOS los, string nsid, Solution<R, ADS>& s, Evaluation& e)
   {
   }

   // optional: get local optimum status (LOS)
   virtual LOS getLOS(string nsid, Solution<R, ADS>& s, Evaluation& e)
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


template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class LocalSearchBuilder : public ComponentBuilder<R, ADS>
{
public:
	virtual ~LocalSearchBuilder()
	{
	}

	virtual LocalSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS>::idComponent() << "LocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}


#endif /* OPTFRAME_LOCAL_SEARCH_HPP_ */
