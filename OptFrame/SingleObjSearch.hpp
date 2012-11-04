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

#ifndef OPTFRAME_SINGLE_OBJ_SEARCH_HPP_
#define OPTFRAME_SINGLE_OBJ_SEARCH_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Solution.hpp"
#include "Evaluation.hpp"

#include "OptFrameComponent.hpp"

template< class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY >
class SingleObjSearch :
      public OptFrameComponent
{
   typedef vector<Evaluation<M>*> FitnessValues;
   typedef const vector<const Evaluation<M>*> ConstFitnessValues;

public:

   SingleObjSearch()
   {
   }

   virtual ~SingleObjSearch()
   {
   }

   // search method try to find a feasible solution within timelimit, if there is no such solution it returns NULL.
   virtual pair<Solution<R, ADS>&, Evaluation<M>&>* search(double timelimit = 100000000, double target_f = 0) = 0;

   virtual string log()
   {
      return "Empty heuristic log.";
   }

   static string idComponent()
   {
	   stringstream ss;
	   ss << OptFrameComponent::idComponent() << "SingleObjSearch:";
	   return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

};

#endif /* OPTFRAME_SINGLE_OBJ_SEARCH_HPP_ */
