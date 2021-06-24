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
