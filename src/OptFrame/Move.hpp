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

#ifndef OPTFRAME_MOVE_HPP_
#define OPTFRAME_MOVE_HPP_

#include "Evaluation.hpp"
#include "MoveCost.hpp"
#include "Solution.hpp"
#include "Solutions/CopySolution.hpp"

#include "Component.hpp"

#include "MultiEvaluation.hpp"
#include "MultiMoveCost.hpp"

//#include "Action.hpp"

using namespace std;

typedef void OPTFRAME_DEFAULT_PROBLEM;

namespace optframe {

// it used to be a 'solution space' here:
// not anymore: template<XSolution S, XEvaluation XEv = Evaluation<>>
// Now, 'XR' is a representation space (free of constraints),
// which can be considered equivalent to the "solution space" (in some cases).
// This is useful to remove all possible overheads (including clone(), toString())
// from a very basic structure 'XR' (if user wants to).
// This is also useful when solution is composed by smaller "parts",
// and each part operates with independent moves (thus a fraction of real "solution space").
// Efficient components (like Move) should use 'XR' instead of 'XSolution' (and equivalents).
//template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = std::pair<S, XEv> >
//template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XSH = std::pair<S, XEv>>
template<XESolution XES, XEvaluation XEv2 = Evaluation<>, XSearch<XES> XSH = XES>
// BREAK TIME!! ONLY 'XES' shall pass... can we finish 'S' here?
// Finally, now it's possible to abolish XEv (XEv2) passing here, and deduce from 'XES::second_type'.
class Move : public Component
{
   // using XEv = decltype(declval<XSH>.second); // error: insufficient contextual information to determine type
   using XEv = XES::second_type; // This works!!!
   // TOO BAD: we really need to pass XEv here
public:
   virtual ~Move()
   {
   }

   // move id
   virtual id_type mid()
   {
      return 0;
   }

   virtual bool canBeApplied(const XES& se) = 0;

   // returns true if the apply returns a non-null pointer
   virtual bool hasReverse()
   {
      return true; // TODO: make it pure virtual "= 0"
   }

   // apply move directly to solution structure (only XSolution required)
   virtual uptr<Move<XES, XEv, XSH>> apply(XSH& se) = 0;

   // apply move to solution structure and updated objective space component (XSolution and XEvaluation)
   virtual uptr<Move<XES, XEv, XSH>> applyUpdate(XSH& se)
   {
      //XSolution& s = se.first;
      XEv& e = se.second;
      // boolean 'outdated' indicates that Evaluation needs update (after Solution change)
      // note that even if the reverse move is applied, the Evaluation will continue with
      // the outdated status set to true. So more efficient approaches may rewrite this
      // method, or use  efficient re-evaluation by means of the 'cost' method.
      e.outdated = true;
      // apply the move to R and ADS, saving the reverse (or undo) move
      uptr<Move<XES, XEv, XSH>> rev = apply(se);
      // update neighborhood local optimum status TODO:deprecated
      updateNeighStatus(se);

      // return reverse move (or null)
      return rev;
   }

/*
   // TODO: remove and unify on a single method (just varying XEv)
   virtual Move<XES, XEv, XSH>* applyMEV(MultiEvaluation<>& mev, XES& s)
   {
      // boolean 'outdated' indicates that Evaluation needs update (after Solution change)
      // note that even if the reverse move is applied, the Evaluation will continue with
      // the outdated status set to true. So more efficient approaches may rewrite this
      // method, or implement efficient re-evaluation by means of the 'cost' method.
      for (unsigned nE = 0; nE < mev.size(); nE++)
         mev.setOutdated(nE, true);
      // apply the move to R and ADS, saving the reverse (or undo) move
      Move<XES, XEv, XSH>* rev = apply(s);
      // update neighborhood local optimum status TODO:deprecated
      updateNeighStatus(s);
      // return reverse move (or null)
      return rev;
   }
*/


/*
   // TODO: remove and unify on a single method (just varying XEv)
   virtual uptr<Move<XES, XEv, XSH>> applyMEVUpdate(MultiEvaluation<>& mev, XES& se)
   {
      for (unsigned nE = 0; nE < mev.size(); nE++)
         mev.at(nE).outdated = true;
      // apply the move to R and ADS, saving the reverse (or undo) move
      uptr<Move<XES, XEv, XSH>> rev = apply(se);
      // update neighborhood local optimum status TODO:deprecated
      updateNeighStatus(se);

      // return reverse move (or null)
      return rev;
   }
*/


   // TODO: coming in one of the next versions..
   //virtual pair<Move<XES, XEv, XSH>&, MoveCost<>*> apply(const Evaluation<>& e, R& r, ADS& ads) = 0;

   // ================== cost calculation

   // Returns a XEvaluation object containing the difference
   virtual op<XEv> cost(const XES& se, bool allowEstimated)
   {
      return std::nullopt;
   }

   // experiment for multi objective problems
   virtual MultiMoveCost<>* costMEV(const MultiEvaluation<>& mev, const XES& se, bool allowEstimated)
   {
      return nullptr;
   }

   // ================== move independence and local search marking

   virtual bool independentOf(const Move<XES, XEv, XSH>& m)
   {
      // example: in VRP, move1 changes one route and move2 changes another... they are independent.
      // move1.isIndependent(move2) should return true.
      // by default, it is false (no move is independent)
      return false;
   }

   // ================== local search marking

   // TODO: deprecated. replaced by updateLOS?
   //virtual void updateNeighStatus(ADS* ads)
   virtual void updateNeighStatus(XSH& se)
   {
   }

   // TODO: force before apply(R,ADS) and after apply(S)?
   // TODO: think about how this fits a general 'XR' structure... maybe better on 'XEv' than 'XR' itself.
   virtual void updateLOS(XSH& s, XEv& e)
   {
   }

   // TODO: rethink!
   virtual bool isPartialLocalOptimum(const XES& se)
   {
      // the idea is to use this flag to ignore moves that are useless,
      // given that the solution is already in a (full) local optimum (or partial).

      return false;
   }

   // TODO: do we need clone() here?
   // It may be useful (and necessary!) to handle multiple applications of same Move... let's see!

   // ================== basic comparison

   virtual bool operator==(const Move<XES, XEv, XSH>& m) const = 0;

   bool operator!=(const Move<XES, XEv, XSH>& m) const
   {
      return !(*this == m);
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":Move";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual void print() const = 0;
};

} // namespace optframe

#endif /*OPTFRAME_MOVE_HPP_*/
