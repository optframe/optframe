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

#ifndef OPTFRAME_EVALUATOR_HPP_
#define OPTFRAME_EVALUATOR_HPP_

#include "ADSManager.hpp"
#include "Evaluation.hpp"
#include "Move.hpp"
#include "MoveCost.hpp"
#include "Solution.hpp"

#include <iostream>

#include "Action.hpp"
#include "Direction.hpp"

#include "Solutions/ESolution.hpp" // TODO: remove.. just to enforce compilation errors.

#include "GeneralEvaluator.hpp"

//#define OPTFRAME_EPSILON 0.0001

using namespace std;
using namespace scannerpp;

namespace optframe {

//! \english The Evaluator class is responsible for the attribution of objective values for each Solution \endenglish \portuguese A classe Evaluator é responsável pela atribuição de valores objetivo para cada Solution \endportuguese

/*!
 \english
 The Evaluator class is responsible for the attribution of objective values for each Solution
 This is done by the method evaluate().
 \endenglish

 \portuguese
 A classe Evaluator é responsável pela atribuição de valores objetivo para cada Solution.
 Isto é feito através do método evaluate().
 \endportuguese
 */

// TODO: use XEv here
template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = pair<S, XEv>>
//template<XSolution S, XEvaluation XEv> // require explicitly XEv here...
class Evaluator : public Direction, public GeneralEvaluator<S, XEv, XSH>
{

protected:
   bool allowCosts; // move.cost() is enabled or disabled for this Evaluator
   evtype weight;   // defaults to 1

public:
   Evaluator(bool _allowCosts = true, evtype w = 1)
     : allowCosts(_allowCosts)
     , weight(w)
   {
   }

   virtual ~Evaluator()
   {
   }

   bool getAllowCosts()
   {
      return allowCosts;
   }

   void setAllowCosts(bool _allowCosts)
   {
      allowCosts = _allowCosts;
   }

   // TODO: remove weight
   evtype getWeight() const
   {
      return weight;
   }

   // TODO: remove weight
   void setWeight(const evtype& w)
   {
      weight = w;
   }

public:
   virtual XEv evaluate(const S&) = 0;

   // TODO: verify if 'e.outdated' must be required at all times, or just specific cases
   //virtual void reevaluate(XEv& e, const S& s) override
   virtual void reevaluate(XSH& se) override
   {
      S& s = se.first;
      XEv& e = se.second;
      if (e.outdated)
         e = evaluate(s);
   }

public:
   // Apply movement considering a previous XEv => Faster (used on CheckCommand and locally)
   // Update XEv 'e'
   //Move<S, XEv>* applyMoveReevaluate(XEv& e, Move<S, XEv>& m, S& s)
   Move<S, XEv>* applyMoveReevaluate(Move<S, XEv>& m, XSH& se)
   {
      // apply move and get reverse move
      Move<S, XEv>* rev = m.applyUpdate(se);
      // for now, must be not nullptr
      assert(rev != nullptr);
      // consolidate 'outdated' XEv data on 'e'
      reevaluate(se);

      // create pair
      return rev;
   }

   // Apply movement without considering a previous XEv => Slower.
   // Return new XEv 'e'
   pair<Move<S, XEv>*, XEv> applyMove(Move<S, XEv>& m, S& s)
   {
      // apply move and get reverse move
      Move<S, XEv>* rev = m.apply(s);
      // for now, must be not nullptr 
      assert(rev != nullptr);
      // TODO: include management for 'false' hasReverse()
      assert(m.hasReverse() && rev);
      // create pair
      return pair<Move<S, XEv>*, XEv>(rev, evaluate(s));
   }

   // Movement cost based on reevaluation of 'e'
   //MoveCost<>* moveCost(XEv& e, Move<S, XEv>& m, S& s, bool allowEstimated = false)
   MoveCost<>* moveCost(Move<S, XEv>& m, XSH& se, bool allowEstimated = false)
   {
      // TODO: in the future, consider 'allowEstimated' parameter
      // TODO: in the future, consider 'e' and 's' as 'const', and use 'const_cast' to remove it.

      MoveCost<>* p = nullptr;
      if (allowCosts) {
         p = m.cost(se, allowEstimated);
      }

      // if p not null, do not update 's' => much faster (using cost)
      if (p) {
         return p;
      } else {
         // need to update 's' together with reevaluation of 'e' => slower (may perform reevaluation)

         // TODO: in the future, consider moves with nullptr reverse (must save original solution/evaluation)
         assert(m.hasReverse());

         XEv& e = se.second;

         XEv ev_begin(e); // copy
         //XEv ev_begin = e; //TODO: VITOR removing last evaluation
         // saving 'outdated' status to avoid inefficient re-evaluations
         //			bool outdated = e.outdated;
         // apply move to both XEv and Solution
         Move<S, XEv>* rev = applyMoveReevaluate(m, se);
         // get final values
         pair<evtype, evtype> e_end = make_pair(e.getObjFunction(), e.getInfMeasure());
         // get final values for lexicographic part
         vector<pair<evtype, evtype>> alternatives(e.getAlternativeCosts().size());
         for (unsigned i = 0; i < alternatives.size(); i++) {
            alternatives[i].first = e.getAlternativeCosts()[i].first;
            alternatives[i].second = e.getAlternativeCosts()[i].second;
         }

         // apply reverse move in order to get the original solution back
         //TODO - Why do not save ev at the begin? Extra evaluation
         //Even when reevaluate is implemented, It would be hard to design a strategy that is faster than copying previous evaluation//==================================================================
         //			Move<S, XEv>* ini = applyMoveReevaluate(e, *rev, s);
         //
         //			// if XEv wasn't 'outdated' before, restore its previous status
         //			if (!outdated)
         //				e.outdated = outdated;

         S& s = se.first;

         Move<S, XEv>* ini = rev->apply(s);
         // for now, must be not nullptr
         assert(ini != nullptr);
         // TODO: include management for 'false' hasReverse()
         assert(rev->hasReverse() && ini);

         e = std::move(ev_begin);
         //==================================================================

         // get original values (also could be calculated in the begin of function)
         pair<evtype, evtype> e_ini = make_pair(e.getObjFunction(), e.getInfMeasure());
         // do the same for lexicographic part
         for (unsigned i = 0; i < alternatives.size(); i++) {
            alternatives[i].first -= e.getAlternativeCosts()[i].first;
            alternatives[i].second -= e.getAlternativeCosts()[i].second;
         }
         // destroy reverse move
         delete rev;
         // destroy initial move
         delete ini;
         // create a MoveCost object...

         // TODO: try to move this MoveCost generation somewhere else.... 
         // perhaps, in Evaluation class, so that users can easily personalize it.
         // don't know for sure. (TODO)
         //p = new MoveCost<>(e_end.first - e_ini.first, e_end.second - e_ini.second, e.weight);
         p = new MoveCost<>(e_end.first - e_ini.first, e_end.second - e_ini.second);

         // ... and set the lexicographic costs
         p->setAlternativeCosts(alternatives);
         // return a MoveCost object pointer
         return p;
      }
   }

   // Movement cost based on complete evaluation (only on CheckCommand)
   // USE ONLY FOR VALIDATION OF CODE! OTHERWISE, USE MoveCost<>(e, m, s)
   MoveCost<>* moveCostComplete(Move<S, XEv>& m, S& s, bool allowEstimated = false)
   {
      // TODO: in the future, consider 'allowEstimated' parameter
      // TODO: in the future, consider 'e' and 's' as 'const', and use 'const_cast' to remove it.

      // TODO: in the future, consider moves with nullptr reverse (must save original solution/evaluation)
      assert(m.hasReverse());

      pair<Move<S, XEv>*, XEv> rev = applyMove(m, s);

      pair<Move<S, XEv>*, XEv> ini = applyMove(*rev.first, s);

      // Difference: new - original

      evtype obj = rev.second.getObjFunction() - ini.second.getObjFunction();
      evtype inf = rev.second.getInfMeasure() - ini.second.getInfMeasure();

      vector<pair<evtype, evtype>> alternatives(rev.second.getAlternativeCosts().size());

      for (unsigned i = 0; i < alternatives.size(); i++) {
         alternatives[i].first = rev.second.getAlternativeCosts()[i].first - ini.second.getAlternativeCosts()[i].first;
         alternatives[i].second = rev.second.getAlternativeCosts()[i].second - ini.second.getAlternativeCosts()[i].second;
      }

      MoveCost<>* p = new MoveCost<>(obj, inf);
      p->setAlternativeCosts(alternatives);

      delete rev.first;
      delete ini.first;

      return p;
   }

   // used on FirstImprovement
   // Accept and apply move if it improves parameter moveCost
   bool acceptsImprove(Move<S, XEv>& m, XSH& se, MoveCost<>* mc = nullptr, bool allowEstimated = false)
   {
      // TODO: in the future, consider 'allowEstimated' parameter

      S& s = se.first;
      XEv& e = se.second;

      // initialize MoveCost pointer
      MoveCost<>* p = nullptr;
      // try to get a cost (should consider estimated moves in the future)
      if (allowCosts) {
         p = m.cost(se, allowEstimated);
      }

      // if p not null => much faster (using cost)
      if (p) {
         // verify if m is an improving move
         if (isImprovement(*p)) {
            // apply move and get reverse
            Move<S, XEv>* rev = m.apply(s);
            if (rev)
               delete rev;
            // update XEv with MoveCost
            p->update(e);
            // destroy MoveCost
            delete p;
            return true;
         } else {
            // destroy MoveCost
            delete p;
            return false;
         }
      } else {
         // need to update 's' together with reevaluation of 'e' => slower (may perform reevaluation)

         // TODO: in the future, consider moves with nullptr reverse (must save original solution/evaluation)
         assert(m.hasReverse());

         // saving previous evaluation
         XEv ev_begin(e);
         // saving 'outdated' status to avoid inefficient re-evaluations
         //			bool outdated = e.outdated;
         // get original obj function values
         pair<evtype, evtype> e_begin = make_pair(e.getObjFunction(), e.getInfMeasure());
         // get original values for lexicographic part
         vector<pair<evtype, evtype>> alt_begin(e.getAlternativeCosts().size());
         for (unsigned i = 0; i < alt_begin.size(); i++) {
            alt_begin[i].first = e.getAlternativeCosts()[i].first;
            alt_begin[i].second = e.getAlternativeCosts()[i].second;
         }
         // apply move to both XEv and Solution
         Move<S, XEv>* rev = applyMoveReevaluate(m, se);
         // TODO: check outdated and estimated!
         MoveCost mcost(e.getObjFunction() - e_begin.first, e.getInfMeasure() - e_begin.second, 1, false, false);
         // guarantee that alternative costs have same size
         assert(alt_begin.size() == e.getAlternativeCosts().size());
         // compute alternative costs
         for (unsigned i = 0; i < alt_begin.size(); i++)
            mcost.addAlternativeCost(make_pair(e.getAlternativeCosts()[i].first - alt_begin[i].first, e.getAlternativeCosts()[i].second - alt_begin[i].second));

         // check if it is improvement
         if (isImprovement(mcost)) {
            // delete reverse move
            if (rev)
               delete rev;
            return true;
         }

         // must return to original situation

         // apply reverse move in order to get the original solution back
         //TODO - Vitor, Why apply Move with e is not used???
         //			Even when reevaluate is implemented, It would be hard to design a strategy that is faster than copying previous evaluation
         //==================================================================
         //pair<Move<S, XEv>*, XEv> ini = applyMove(*rev, s);

         // if XEv wasn't 'outdated' before, restore its previous status
         //			if (!outdated)
         //				e.outdated = outdated;

         // go back to original evaluation
         //			e = ini.second;
         //			delete ini.first;

         Move<S, XEv>* ini = rev->apply(s);
         // for now, must be not nullptr
         assert(ini != nullptr);
         // TODO: include management for 'false' hasReverse()
         assert(rev->hasReverse() && ini);
         e = std::move(ev_begin);
         delete ini;
         //==================================================================

         delete rev;

         return false;
      }
   }

   // ============ betterThan ===========

   // From Direction:
   //virtual inline bool betterThan(const MoveCost<>& mc1, const MoveCost<>& mc2)
   using Direction::betterThan;


   // not good option! CONST and possible re-evaluations (on 's')
   virtual bool betterThan(const XSH& p1, const XSH& p2)
   {
      return betterThan(p1.first, p2.first);
   }

   // Note that these parameters are NON-CONST... so, they can be updated if necessary!
   virtual bool betterThan(XSH& se1, XSH& se2)
   {
      XEv& e1 = se1.second;
      XEv& e2 = se2.second;
      if(e1.outdated)
         e1 = evaluate(se1.first);
      if(e2.outdated)
         e2 = evaluate(se2.first);
      bool r = Direction::betterThan(e1, e2);
      return r;
   }

   //! abstract method betterThan: true when a < b for minimization problems; and true when a > b for maximization problems.
   /*!
	 betterThan is the method in OptFrame used to guide the search methods into the solution space.
	 with betterThan the search methods are able to compare good and poor solutions, in one of the two directions: minimization and maximization.
	 It must be implemented by the final user in one of these ways:
	 - for minimization problems, returns a < b;
	 - for maximization problems, returns a > b.
	 */
   //virtual bool betterThan(evtype a, evtype b) = 0;
   virtual bool betterThan(const S& s1, const S& s2)
   {
      XEv e1 = evaluate(s1);
      XEv e2 = evaluate(s2);
      bool r = Direction::betterThan(e1, e2);
      return r;
   }

   virtual bool betterThan(const XEv& e1, const XEv& e2)
   {
      return Direction::betterThan(e1, e2);
   }


   // ============= Component ===============

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (Direction::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":Evaluator";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XRepresentation R, XEvaluation XEv = Evaluation<>>
class BasicEvaluator : public Evaluator<RSolution<R>, XEv>
{
public:
   // only representation
   virtual XEv evaluate(const R&) = 0;

private:
   /* Use this if don't need ADS */
   /* Use isto se você não precisa do ADS */

   using ADS = OPTFRAME_DEFAULT_ADS;

   XEv evaluate(const RSolution<R>& s) override
   {
      return evaluate(s.getR());
   }
};
}

#endif /*OPTFRAME_EVALUATOR_HPP_*/
