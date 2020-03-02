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

#ifndef OPTFRAME_EVALUATION_HPP_
#define OPTFRAME_EVALUATION_HPP_

#include <assert.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "BaseConcepts.hpp" 
#include "Component.hpp"
#include "MultiObjValue.hpp"  // inserting this beforehand.. who knows!!!
#include "SingleObjValue.hpp" // basic value 'evtype' comes from here!

using namespace std;

namespace optframe {

//! \english The Evaluation class is a container class for the objective function value and the Memory structure M. \endenglish \portuguese A classe Evaluation é uma classe contêiner para o valor da função objetivo e a estrutura de Memória M. \endportuguese

/*!
 \english
 It is also possible to carry an infeasibility measure.
 The evaluation() method returns the sum of objFunction and infMeasure.
 \endenglish

 \portuguese
 Também é possível carregar uma medida de inviabilidade infMeasure.
 O método evaluation() retorna a soma da função objetivo objFunction e a infMeasure.
 \endportuguese
 */

//#include "Util/PackTypes.hpp"
//#define EVALUATION_TYPE PackTypes

// note: for multi-objective problems with distinct objective space types
// such as (int, evtype, long long) you can use PackTypes in Utils or overload
// manually each of the numeric operators +, -, *
//why this isn't a template????????????????
// found a solution using C++20 concepts: now it's an optional template :)

// TODO: this ObjType here can require more than 'totally_ordered',
// as it will require arithmetics (+,-,*) to calculate MoveCosts and Infeasibility
// Thus, we should use another concept here, composed (&&) with arithmetics.
// Good thing is that Evaluation class is generic for most uses...
// This is not the case for Pareto solutions, where composability may become more complex (MultiEvaluation)
// TODO: must see next how to deal with that (on Pareto side), but at least we have a very elegant solution now

// here comes the tricky part, 'totally_ordered' should be enough, but we endup needing arithmetics to compute costs
//template<optframe::totally_ordered ObjType = evtype>
// so we will proceed with basic arithmetics, +, - and *.
// this effectively discard 'string' and others (although comparable)

// THIS IS FOCUSED SINGLE DIRECTION PERSPECTIVE (TOTAL ORDER). SEE PARAM 'isMini'
template<optframe::basic_arithmetics ObjType = evtype>
class Evaluation final : public Component
{
protected:
   // ==== Objective Space type: pair<evtype, evtype> ====
   // objective function value (default = double)
   ObjType objFunction;
   // infeasibility measure value (default = double)
   ObjType infMeasure;
   // for lexicographic approaches, use these extra evaluation values
   vector<pair<ObjType, ObjType>> alternatives;

   // ==== Objective Space auxiliary information ====
   // LocalOptimum Status: mapping 'move.id()' to 'NeighborhoodStatus'
   // map<string, bool> localStatus; // TODO: REMOVE!
   // GlobalOptimumStatus (for exact methods only)
   enum GOS
   {
      gos_yes,
      gos_no,
      gos_unknown
   } gos;

   // LocalOptimumStatus
   enum LOS
   {
      los_yes,
      los_no,
      los_unknown
   }; // do not declare here (keep in ADS or R)

public:
   // boolean field to indicate if Evaluation needs an update
   bool outdated;
   // boolean field to indicate if Evaluation value is an estimation (not exact)
   bool estimated;

   // is minimization
   bool isMini {true}; 

   // ======================================
   // begin canonical part

   // TODO (IGOR): maybe create empty Evaluation with numeric_zero on 'obj'

   explicit Evaluation(const ObjType& obj, const ObjType& inf, const evtype& w = 1, bool _isMini = true)
     : objFunction(obj)
     , infMeasure(inf)
     , isMini(_isMini)
   {
      gos = gos_unknown;
      outdated = false;
      estimated = false;
   }

   explicit Evaluation(const ObjType& obj)
     : objFunction(obj)
   {
      //infMeasure = optframe::get_numeric_zero<ObjType>();
      optframe::numeric_zero(infMeasure);

      gos = gos_unknown;
      outdated = false;
      estimated = false;
   }

   explicit Evaluation()
   {
      optframe::numeric_zero(objFunction);
      optframe::numeric_zero(infMeasure);

      gos = gos_unknown;
      outdated = false;
      estimated = false;
   }   

   explicit Evaluation(const Evaluation<ObjType>& e)
     : objFunction(e.objFunction)
     , infMeasure(e.infMeasure)
     , alternatives(e.alternatives)
     , gos(e.gos)
     , outdated(e.outdated)
     , estimated(e.estimated)
     , isMini(e.isMini)
   {
   }

   virtual ~Evaluation()
   {
   }

   virtual Evaluation<ObjType>& operator=(const Evaluation<ObjType>& e)
   {
      if (&e == this) // auto ref check
         return *this;

      objFunction = e.objFunction;
      infMeasure = e.infMeasure;
      outdated = e.outdated;
      estimated = e.estimated;
      alternatives = e.alternatives;
      gos = e.gos;

      return *this;
   }

   virtual Evaluation<ObjType>& clone() const
   {
      return *new Evaluation(*this);
   }

   // end canonical part
   // ======================================
   // begin Evaluation methods

   ObjType getObjFunction() const
   {
      return objFunction;
   }

   ObjType getInfMeasure() const
   {
      return infMeasure;
   }

   const vector<pair<ObjType, ObjType>>& getAlternativeCosts() const
   {
      return alternatives;
   }

   void setObjFunction(ObjType obj)
   {
      objFunction = obj;
   }

   void setInfMeasure(ObjType inf)
   {
      infMeasure = inf;
   }

   void addAlternativeCost(const pair<ObjType, ObjType>& alternativeCost)
   {
      alternatives.push_back(alternativeCost);
   }

   void setAlternativeCosts(const vector<pair<ObjType, ObjType>>& alternativeCosts)
   {
      alternatives = alternativeCosts;
   }

   // -----------------
   // for local optimum
   // -----------------

   // TODO: remove! LOS management is now on NSSeq and NSSeqIterators
   /*
	bool getLocalOptimumStatus(string moveId)
	{
		return localStatus[moveId];
	}

	void setLocalOptimumStatus(string moveId, bool status)
	{
		localStatus[moveId] = status;
	}
	*/

   // ------------------
   // for global optimum
   // ------------------

   GOS getGlobalOptimumStatus()
   {
      return gos;
   }

   void setGlobalOptimumStatus(GOS status)
   {
      gos = status;
   }

   ObjType evaluation() const
   {
      return objFunction + infMeasure;
   }

   // ========= TAKEN FROM MoveCost =======

   // update Evaluation with costs
   virtual void update(Evaluation<ObjType>& evCost)
   {
      // this task was performed before by MoveCost... now unifying in Evaluation
      // how to do this?
      assert(false);
   }

   // this strictly better than parameter 'e' (for mini, 'this' < 'e')
   virtual bool betterStrict(const Evaluation<ObjType>& e)
   {
      return isMini? evaluation() < e.evaluation() : evaluation() > e.evaluation();
   }

   // this non-strictly better than parameter 'e' (for mini, 'this' <= 'e')
   virtual bool betterOrEquals(const Evaluation<ObjType>& e)
   {
      return isMini? evaluation() <= e.evaluation() : evaluation() >= e.evaluation();
   }


   virtual bool isEstimated()
   {
      return false; // TODO: created good flag here??
   }

   // =========
   // finish MoveCost
   // =========

   // leave option to rewrite tolerance (or consider lexicographic values)
   virtual bool isFeasible() const
   {
      //return optframe::numeric_is_zero<ObjType>(infMeasure);
      // IMPORTANT: numeric_is_zero can come from anywhere!
      return optframe::numeric_is_zero(infMeasure);
      //return (EVALUATION_ABS(infMeasure) <= optframe::get_numeric_zero<ObjType>()); // deprecated
   }

   // ======================
   // Object specific part
   // ======================

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":Evaluation";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual void print() const
   {
      cout << toString() << endl;
   }

   virtual string toString() const
   {
      stringstream ss;
      ss << fixed; // disable scientific notation
      ss << "Evaluation function value = " << evaluation();
      ss << (isFeasible() ? " " : " (not feasible) ");
      ss << (outdated ? " OUTDATED " : " ");
      ss << (estimated ? " ESTIMATED " : " ");
      if (alternatives.size() > 0) {
         ss << " alternative costs: ";
         for (unsigned i = 0; i < alternatives.size(); i++)
            ss << "(" << alternatives[i].first << ";" << alternatives[i].second << ") ";
      }
      // ss << endl;

      return ss.str();
   }

   friend std::ostream& operator<<(std::ostream& os, const Evaluation& me)
   {
      os << me.toString();
      return os;
   }
};



















// Weighted and support to lexicographic computation
// WLxEvaluation: depend on extended_arithmetics (+, - and *)
template<optframe::extended_arithmetics ObjType = evtype>
class WLxEvaluation final : public Component
{
protected:
   // ==== Objective Space type: pair<evtype, evtype> ====
   // objective function value (default = double)
   ObjType objFunction;
   // infeasibility measure value (default = double)
   ObjType infMeasure;
   // for lexicographic approaches, use these extra evaluation values
   vector<pair<ObjType, ObjType>> alternatives;

   // ==== Objective Space auxiliary information ====
   // LocalOptimum Status: mapping 'move.id()' to 'NeighborhoodStatus'
   // map<string, bool> localStatus; // TODO: REMOVE!
   // GlobalOptimumStatus (for exact methods only)
   enum GOS
   {
      gos_yes,
      gos_no,
      gos_unknown
   } gos;

   // LocalOptimumStatus
   enum LOS
   {
      los_yes,
      los_no,
      los_unknown
   }; // do not declare here (keep in ADS or R)

public:
   // boolean field to indicate if Evaluation needs an update
   bool outdated;
   // boolean field to indicate if Evaluation value is an estimation (not exact)
   bool estimated;
   // constant to mutiply infeasibility weight
   ObjType weight;

   // ======================================
   // begin canonical part

   WLxEvaluation(const ObjType& obj, const ObjType& inf, const evtype& w = 1)
     : objFunction(obj)
     , infMeasure(inf)
     , weight(w)
   {
      gos = gos_unknown;
      outdated = false;
      estimated = false;
   }

   WLxEvaluation(const ObjType& obj)
     : objFunction(obj)
   {
      weight = 1;
      //infMeasure = optframe::get_numeric_zero<ObjType>();
      optframe::numeric_zero(infMeasure);

      gos = gos_unknown;
      outdated = false;
      estimated = false;
   }

   WLxEvaluation(const WLxEvaluation& e)
     : objFunction(e.objFunction)
     , infMeasure(e.infMeasure)
     , alternatives(e.alternatives)
     , gos(e.gos)
     , outdated(e.outdated)
     , estimated(e.estimated)
     , weight(e.weight)
   {
   }

   virtual ~WLxEvaluation()
   {
   }

   virtual WLxEvaluation& operator=(const WLxEvaluation& e)
   {
      if (&e == this) // auto ref check
         return *this;

      objFunction = e.objFunction;
      infMeasure = e.infMeasure;
      outdated = e.outdated;
      estimated = e.estimated;
      weight = e.weight;
      alternatives = e.alternatives;
      gos = e.gos;

      return *this;
   }

   virtual WLxEvaluation& clone() const
   {
      return *new WLxEvaluation(*this);
   }

   // end canonical part
   // ======================================
   // begin Evaluation methods

   ObjType getObjFunction() const
   {
      return objFunction;
   }

   ObjType getInfMeasure() const
   {
      return infMeasure;
   }

   evtype getWeight() const
   {
      return weight;
   }

   const vector<pair<ObjType, ObjType>>& getAlternativeCosts() const
   {
      return alternatives;
   }

   void setObjFunction(ObjType obj)
   {
      objFunction = obj;
   }

   void setInfMeasure(ObjType inf)
   {
      infMeasure = inf;
   }

   void setWeight(const evtype& w)
   {
      weight = w;
   }

   void addAlternativeCost(const pair<ObjType, ObjType>& alternativeCost)
   {
      alternatives.push_back(alternativeCost);
   }

   void setAlternativeCosts(const vector<pair<ObjType, ObjType>>& alternativeCosts)
   {
      alternatives = alternativeCosts;
   }

   // -----------------
   // for local optimum
   // -----------------

   // TODO: remove! LOS management is now on NSSeq and NSSeqIterators
   /*
	bool getLocalOptimumStatus(string moveId)
	{
		return localStatus[moveId];
	}

	void setLocalOptimumStatus(string moveId, bool status)
	{
		localStatus[moveId] = status;
	}
	*/

   // ------------------
   // for global optimum
   // ------------------

   GOS getGlobalOptimumStatus()
   {
      return gos;
   }

   void setGlobalOptimumStatus(GOS status)
   {
      gos = status;
   }

   // evaluation = objFunction + weight*infMeasure
   // note that, if 'evtype' becomes complex, one must return a moveable copy, not reference of internal value
   ObjType evaluation() const
   {
      return objFunction + weight * infMeasure;
   }

   // ========= TAKEN FROM MoveCost =======

   // update Evaluation with costs
   virtual void update(Evaluation<ObjType>& evCost)
   {
      // this task was performed before by MoveCost... now unifying in Evaluation
      // how to do this?
      assert(false);
   }

   virtual bool isEstimated()
   {
      return false; // TODO: created good flag here??
   }

   // =========
   // finish MoveCost
   // =========


   // leave option to rewrite tolerance (or consider lexicographic values)
   virtual bool isFeasible() const
   {
      //return optframe::numeric_is_zero<ObjType>(infMeasure);
      // IMPORTANT: numeric_is_zero can come from anywhere!
      return optframe::numeric_is_zero(infMeasure);
      //return (EVALUATION_ABS(infMeasure) <= optframe::get_numeric_zero<ObjType>()); // deprecated
   }

   // ======================
   // Object specific part
   // ======================

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":WLxEvaluation";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual void print() const
   {
      cout << toString() << endl;
   }

   virtual string toString() const
   {
      stringstream ss;
      ss << fixed; // disable scientific notation
      ss << "WLxEvaluation function value = " << evaluation();
      ss << (isFeasible() ? " " : " (not feasible) ");
      ss << " w= " << weight << " ";
      ss << (outdated ? " OUTDATED " : " ");
      ss << (estimated ? " ESTIMATED " : " ");
      if (alternatives.size() > 0) {
         ss << " alternative costs: ";
         for (unsigned i = 0; i < alternatives.size(); i++)
            ss << "(" << alternatives[i].first << ";" << alternatives[i].second << ") ";
      }
      // ss << endl;

      return ss.str();
   }

   friend std::ostream& operator<<(std::ostream& os, const WLxEvaluation& me)
   {
      os << me.toString();
      return os;
   }

};

} // namespace optframe

// Compilation tests for XEvaluation concepts
// These are NOT unit tests... Unit Tests are on tests/ folder
#include "Evaluation.test.hpp"

#endif /*OPTFRAME_EVALUATION_HPP_*/
