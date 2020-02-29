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

#ifndef OPTFRAME_MOVE_COST_HPP_
#define OPTFRAME_MOVE_COST_HPP_

#include <cmath>
#include <cstdlib>
#include <iostream>

#include "BaseConcepts.hpp"
#include "Component.hpp"
#include "Evaluation.hpp"

using namespace std;

namespace optframe {

// more than 'objval' we need to ensure arithmetics here... TODO: see that (same as Evaluation)
template<optframe::objval ObjType = evtype, XEvaluation XEv = Evaluation<ObjType>>
//template<class ObjType = evtype, XEvaluation XEv = Evaluation<ObjType>>
class MoveCost : public Component
{
protected:
   // objective function value (cost difference)
   ObjType objFunction;
   // infeasibility measure value (cost difference)
   ObjType infMeasure;
   // constant to mutiply infeasibility weight
   evtype weight;
   // flag indicating if Evaluation was outdated
   bool outdated;
   // flag indicating if cost is estimated
   bool estimated;
   // storing costs for lexicographic approaches
   vector<pair<evtype, evtype>> alternatives;

public:
   explicit MoveCost(ObjType obj, ObjType inf = 0, evtype w = 1, bool _outdated = true, bool _estimated = false)
     : objFunction(obj)
     , infMeasure(inf)
     , weight(w)
     , outdated(_outdated)
     , estimated(_estimated)
   {
   }

   MoveCost(const MoveCost& mc)
     : objFunction(mc.objFunction)
     , infMeasure(mc.infMeasure)
     , weight(mc.weight)
     , outdated(mc.outdated)
     , estimated(mc.estimated)
     , alternatives(mc.alternatives)
   {
   }

   virtual ~MoveCost()
   {
   }

   bool isEstimated() const
   {
      return estimated;
   }

   const vector<pair<evtype, evtype>>& getAlternativeCosts() const
   {
      return alternatives;
   }

   ObjType getObjFunctionCost() const
   {
      return objFunction;
   }

   ObjType getInfMeasureCost() const
   {
      return infMeasure;
   }

   void addAlternativeCost(const pair<evtype, evtype>& alternativeCost)
   {
      alternatives.push_back(alternativeCost);
   }

   void setAlternativeCosts(const vector<pair<evtype, evtype>>& alternativeCosts)
   {
      alternatives = alternativeCosts;
   }

   void setObjFunctionCost(ObjType obj)
   {
      objFunction = obj;
   }

   void setInfMeasureCost(ObjType inf)
   {
      infMeasure = inf;
   }

   ObjType cost() const
   {
      return objFunction + weight * infMeasure;
   }

   // update Evaluation with costs
   virtual void update(XEv& e) // TODO: put correct ObjType here
   {
      // update objective function value
      e.setObjFunction(e.getObjFunction() + objFunction);
      // update infeasibility measure value
      e.setInfMeasure(e.getInfMeasure() + infMeasure);
      // restore previous 'outdated' status, if Evaluation wasn't outdated before
      if (!outdated)
         e.outdated = outdated;

      // may also update lexicographic costs...
   }

   static string idComponent()
   {
      return "OptFrame:MoveCost";
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual void print() const
   {
      cout << fixed; // disable scientific notation
      cout << "Move cost = " << cost() << endl;
      if (alternatives.size() > 0) {
         cout << "Alternative Costs: ";
         for (unsigned i = 0; i < alternatives.size(); i++)
            cout << "(" << alternatives[i].first << ";" << alternatives[i].second << ") ";
         cout << endl;
      }
   }

   virtual MoveCost& operator=(const MoveCost& mc)
   {
      if (&mc == this) // auto ref check
         return *this;

      objFunction = mc.objFunction;
      infMeasure = mc.infMeasure;
      outdated = mc.outdated;
      estimated = mc.estimated;
      alternatives = mc.alternatives;
      weight = mc.weight;

      return *this;
   }

   virtual MoveCost& clone() const
   {
      return *new MoveCost(*this);
   }

   // TODO: THIS FRIEND WAS DESTROYING EVERYTHING!!
   //friend class MultiMoveCost; // TODO: remove! experiment for MO problems
};

#ifndef NDEBUG
struct optframe_debug_test_move_cost
{
   MoveCost<> testMoveCost;
};
#endif

} // namespace optframe

#endif /*OPTFRAME_MOVE_COST_HPP_*/
