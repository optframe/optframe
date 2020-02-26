#ifndef OPTFRAME_BASE_CONCEPTS_HPP_
#define OPTFRAME_BASE_CONCEPTS_HPP_

// =============================
// Basic concepts for OptFrame
// =============================
// XSolution
// XEvaluation
// XESolution
// XBaseSolution
// =============================

#include<cstddef> // nullptr_t
#include<string>

// this is NOT official c++20 concepts... just some for OptFrame! (based on lite concepts g++ 7)
#include "myconcepts.h"

// may require some basic printing capabilities
#include "Util/printable.h"

// the default ADS type is 'int'
// adopting 'void' type would cause troubles in constructor/copy/move operations
// if not used, it can be ignored with few impacts (hoping compiler will help us!)
typedef std::nullptr_t OPTFRAME_DEFAULT_ADS;
typedef OPTFRAME_DEFAULT_ADS OptFrameADS; // more beautiful :)
typedef OPTFRAME_DEFAULT_ADS _ADS;        // more beautiful :)


// general concept, just requiring operator<< for basic printability
template <class R>
//concept bool Representation = true;
concept bool XRepresentation = optframe::ostreamable<R>;


// ========================

// requires Self& clone() method
// there may be some "Cloneable" in std future...
// TODO: change return to unique_ptr instead of reference
template <class Self>
concept bool HasClone = requires(Self self)
{
   {
      self.clone()
   }
   ->Self &;
};

template <class Self>
concept bool HasToString = requires(Self self)
{
   {
      self.toString()
   }
   ->std::string;
};

template <class Self>
concept bool XSolution = HasClone<Self> && HasToString<Self> && XRepresentation<Self>;

// ==========================


// useless concept for 'Structure', there's no such Space... just a regular 'class'.
//template <class ADS>
//concept bool Structure = true;

template <class S, XRepresentation R>
concept bool HasGetR = requires(S a)
{
   {
      a.getR()
   }
   ->R &;
};

template <class S, class ADS = _ADS>
concept bool HasGetADS = requires(S a)
{
   {
      a.getADSptr()
   }
   ->ADS *;
};

template <class S, XRepresentation R, class ADS = _ADS>
concept bool XBaseSolution = HasGetR<S, R> &&HasGetADS<S, ADS> && XSolution<S>;


template <class Self, XRepresentation R>
concept bool XRSolution = HasGetR<Self, R> && XSolution<Self>;

// ============================

template <class Self>
concept bool HasGetObj = requires(Self a)
{
   {
      //a.getObj()
      a.evaluation() // keeping standard name since OptFrame's early times
   }
   ->optframe::totally_ordered; // note that, if 'ObjType' becomes complex, one must return a moveable copy, not reference of internal value
   // any totally ordered output is fine! (double, int, ParetoFront , etc...)
};

// Evaluation of any numeric/comparable type ObjType (can be int, double, or pareto-front-based set for objective space)
// decided by using generic "total ordered" concept, to avoid extra template ObjType
// this will finally solve dependency on different obj types (int, double, etc.) in same project
// it will also allow unification of Evaluation and MultiEvaluation into single same "thing"
// note that getObjValue and getInfeasibleValue are not necessary here, just getObj
// one can implement this way if preferred, separating or not both "values"... not mandatory anymore
template <class Self>
concept bool XEvaluation = HasClone<Self> && HasToString<Self> && HasGetObj<Self> && optframe::ostreamable<Self>;

// XSolution and XEvaluation are container-inspired "conceptual objects", to carry Representation and Objective Value
// One can even aggregate both in a single unified "thing", called XESolution


// funny thing, Solution doesn't carry specific representation (yet)
// probably, because Representation itself is as abstract as a solution... 
// in case of "ObjType" it's different, we may operate over it, perhaps: compare, perhaps "add", "subtract", ... to create costs
template <class Self>
concept bool XESolution = XSolution<Self> && XEvaluation<Self>;

// =====================
// X2Solution Space: 2^S
// ---------------------
// This concept comprises Population-based or Pareto-based spaces
// One difference is that, even when Population is used, typically a single 'best' is returned, and
//  for Pareto-based space, solutions are not "totally-orderable", thus only partially comparable, so multiple "choices" are desired.
// In other words, MO is usually not homeotelic (all tend to same direction), but heterotelic.
//   "homeo-" (sameness, identity,..), "telic" (goal or direction)
// Note that Dominance is a concept from Game Theory that implies "better than the alternative"; 
//  and more specifically that "it improves at least one, and it's not worse on any".
// ---------------------
template <class Self, XSolution S>
concept bool X2Solution = true; // nothing actually required (for now...). Maybe 'at' and 'size' are good candidates
// ---
// We will usually assume a X2ESolution type, since it carries solution sample and evaluation space together
// It is in fact: s \subseteq 2^S  plus its own evaluation (for each solution)... 
template <class Self, XSolution S, XEvaluation XEv>
concept bool X2ESolution = X2Solution<Self, S>; // nothing extra required by now

// TODO: create 'ideal' and 'nadir' point requirements for every MO type
// best would be to re-create int or double with these extra "properties"

template <class Self, XSolution S, XEvaluation XEv>
concept bool X2ESPareto = X2ESolution<Self, S, XEv>; // TODO: require 'dominates' and 'weakDominates' here
//... optimization directions and 'ideal'/'nadir' points may also come here...

template <class Self, XSolution S, XEvaluation XEv>
concept bool X2ESPopulation = X2ESolution<Self, S, XEv> // TODO: require 'fitness' here, or some sort of evolutionary concepts...
// .... in the end, this may be a search in the 'population space'. One may adapt a SingleObjSearch to embed a PopulationalSearch

// other idea, one may adapt a MultiObjSearch to embed a ParetoSearch method.

// compilation tests for concepts (these are NOT unit tests)
#include "BaseConcepts.test.hpp"

#endif // OPTFRAME_SOLUTION_CONCEPTS_HPP_