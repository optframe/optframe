// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#include "OptFrameComponent.hpp"
#include "ComponentBuilder.h"

#include "Solution.hpp"
#include "Population.hpp"
#include "Evaluation.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class LocalSearch: public OptFrameComponent
{
   typedef vector<Evaluation<M>*> FitnessValues;
   typedef const vector<const Evaluation<M>*> ConstFitnessValues;

public:

   LocalSearch()
   {
   }

   virtual ~LocalSearch()
   {
   }

   // core methods

   // no-optimization
   Solution<R, ADS>& search(const Solution<R, ADS>& s, double timelimit = 100000000, double target_f = 0)
   {
      Solution<R, ADS>& s2 = s.clone();
      exec(s2, timelimit, target_f);
      return s2;
   }

   // optimizated version
   pair<Solution<R, ADS>&, Evaluation<M>&>& search(const Solution<R, ADS>& s, const Evaluation<M>& e, double timelimit = 100000000, double target_f = 0)
   {
      Solution<R, ADS>& s2 = s.clone();
      Evaluation<M>& e2 = e.clone();
      exec(s2, e2, timelimit, target_f);
      return *new pair<Solution<R, ADS>&, Evaluation<M>&> (s2, e2);
   }


   // core methods

   // 1
   virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f) = 0;

   // 2
   virtual void exec(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit, double target_f) = 0;


   static string idComponent()
   {
	   stringstream ss;
	   ss << OptFrameComponent::idComponent() << "LocalSearch:";
	   return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class LocalSearchBuilder : public ComponentBuilder<R, ADS, M>
{
public:
	virtual ~LocalSearchBuilder()
	{
	}

	virtual LocalSearch<R, ADS, M>* build(Scanner& scanner, HeuristicFactory<R, ADS, M>& hf, string family = "") = 0;

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS, M>::idComponent() << "LocalSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


#endif /* OPTFRAME_LOCAL_SEARCH_HPP_ */
