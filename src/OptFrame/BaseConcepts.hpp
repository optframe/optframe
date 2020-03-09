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

namespace optframe
{

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
concept bool XSolution = XRepresentation<Self>;//HasClone<Self> && HasToString<Self> && XRepresentation<Self>;
// No more cloning!! finally! thanks concepts :)

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

// defining 'objval' concept: objective value must have +/- (for costs) and comparisons
template <class T>
concept bool objval = 
  optframe::basic_arithmetics<T> && optframe::comparability<T>;

// defining 'evgoal' concept: something that has +/- and some directed strong/weak comparison (betterThan/betterOrEquals)
template <class Self>
concept bool evgoal = 
  (optframe::basic_arithmetics<Self>  ||
      (
         requires(Self e, 
               std::remove_reference_t<Self>& e2)
            {
               { e.update(e2) } -> void; // TODO: rename to 'add'
            }
      && 
         requires(Self e, 
               const std::remove_reference_t<Self>& e2)
            {
               { e.diff(e2) } -> Self; // TODO: rename to 'sub'
            }
      )
   ) && 
//      requires(Self e) 
        requires(Self e, 
           const std::remove_reference_t<Self>& e2)
      {
         { e.betterStrict(e2) }    -> bool; // strict compare towards goal, e.g., '<'
         { e.betterNonStrict(e2) } -> bool; // non-strict compare towards goal, e.g., '<='
         { e.equals(e2) }          -> bool; // compare towards goal, e.g., '=='
      }
      && 
        requires(Self e)
      {
         { e.isStrictImprovement()} -> bool;
         { e.isNonStrictImprovement()} -> bool;
      };
//------

template <class Self>
concept bool HasGetObj = requires(Self a)
{
   {
      //a.getObj()
      a.evaluation() // keeping standard name since OptFrame's early times
   }
   ->optframe::objval; // note that, if 'ObjType' becomes complex, one must return a moveable copy, not reference of internal value
   // any totally ordered output is fine! (double, int, ParetoFront , etc...)
};

// Evaluation of any numeric/comparable type ObjType (can be int, double, or pareto-front-based set for objective space)
// decided by using generic "total ordered" concept, to avoid extra template ObjType
// this will finally solve dependency on different obj types (int, double, etc.) in same project
// it will also allow unification of Evaluation and MultiEvaluation into single same "thing"
// note that getObjValue and getInfeasibleValue are not necessary here, just getObj
// one can implement this way if preferred, separating or not both "values"... not mandatory anymore
template <class Self>
concept bool XEvaluation = // sing obj. evaluation part (standard multi obj)
   (optframe::evgoal<Self> && HasClone<Self> && HasToString<Self> && HasGetObj<Self> && optframe::ostreamable<Self> &&
      requires(Self e)
               {
                  // variable 'outdated' is still useful for optimizations
                  {
                     e.outdated
                  }
                  -> bool; 
                  {
                     //e.update(XEvaluation<Self>) // TODO
                     e.estimated
                  } 
                  -> bool;
               }
   ) || // classic multiobj (MultiEvaluation) - TODO: remove this option
   (
      requires(Self e, size_t idx)
               {
                  // variable 'outdated' is still useful for optimizations
                  {
                     e.atObjVal(idx)
                  }
                  -> optframe::objval; 
               }
   );

// XSolution and XEvaluation are container-inspired "conceptual objects", to carry Representation and Objective Value
// One can even aggregate both in a single unified "thing", called XESolution


// funny thing, Solution doesn't carry specific representation (yet)
// probably, because Representation itself is as abstract as a solution... 
// in case of "ObjType" it's different, we may operate over it, perhaps: compare, perhaps "add", "subtract", ... to create costs
//template <class Self>
//concept bool XESolution = XSolution<Self> && XEvaluation<Self>;
// -----> now concept also allows pair<S, XEv> to represent composed space <-----
template <class Self>
concept bool XESolution = XSolution<Self> && //(XSolution<Self> && XEvaluation<Self>)  || 
               requires(Self a)
               {
                  // also allowing as pair<S, XEv>
                  {
                     a.first
                  }
                  -> XSolution&; 
                  {
                     a.second
                  }
                  -> XEvaluation&; 
               };


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
// not used for now...
//template <class Self, XSolution S>
//concept bool X2Solution = true; // nothing actually required (for now...). Maybe 'at' and 'size' are good candidates
// ---
// general one:
template <class Self, class P>
//concept bool XPowerSet = true; 
concept bool XPowerSet = requires(Self a, size_t idx)
   {
      { a.size() } -> size_t;
      { a.getP(idx) } -> P; // TODO: rename to 'at' (compatible with 'vector'), and return reference
   };
// TODO: powerset could return a 'size'(or 'count'), and perhaps a vector (or iterator) of objects for type P

// now redefining X2Solution more beautifully...
template <class Self, XSolution S>
concept bool X2Solution = XPowerSet<Self, S>; // Too bad, this is unused on OptFrame... :'(

// ---
// We will usually assume a X2ESolution type, since it carries solution sample and evaluation space together
// It is in fact: s \subseteq 2^S  plus its own evaluation (for each solution)... 
//template <class Self, XSolution S, XEvaluation XEv> // unused S and XEv! (could be replaced directly by XSolution and XEvaluation down here...)
template <class Self, XESolution XES> // unused S and XEv! (could be replaced directly by XSolution and XEvaluation down here...)
 //concept bool X2ESolution = X2Solution<Self, S>; 
//concept bool X2ESolution = XPowerSet<Self, S> && XPowerSet<Self, XEv>;
concept bool X2ESolution = XPowerSet<Self, XES>;

// TODO1: should make exponential over a XESolution... not separated sets!!
// TODO2: require any container with operator*, instead of hardcoded 'uptr'.... let's move on!

//concept bool X2ESolution = XPowerSet<Self, XSolution> && XPowerSet<Self, XEvaluation>; // TODO: may also include pair here, instead of 'merge'...
//concept bool X2ESolution = XPowerSet<Self, XSolution<Self>> && XPowerSet<Self, XEv>; // TODO: may also include pair here, instead of 'merge'...

// TODO: create 'ideal' and 'nadir' point requirements for every MO type
// best would be to re-create int or double with these extra "properties"

template <class Self, XSolution S, XEvaluation XEv>
concept bool X2ESPareto = X2ESolution<Self, S, XEv>; // TODO: require 'dominates' and 'weakDominates' here
//... optimization directions and 'ideal'/'nadir' points may also come here...

template <class Self, XSolution S, XEvaluation XEv>
concept bool X2ESPopulation = X2ESolution<Self, S, XEv>; // TODO: require 'fitness' here, or some sort of evolutionary concepts...
// .... in the end, this may be a search in the 'population space'. One may adapt a SingleObjSearch to embed a PopulationalSearch (TODO: create PopulationalSearch)

// ====================

// Search Space for Optimization (definition) or, perhaps, Optimization Space
// A composition of searchable structures (XSolution) and
//   evaluation criteria to guide search on these structures,
//   being them single or multi objective.


//template <class Self, XSolution S, XEvaluation XEv> // TODO: should remove S and XEv, by changing X2ESolution concept...
//concept bool XSearch = XESolution<Self> || X2ESolution<Self, S, XEv>; 
template <class Self, XESolution XES> // TODO: should remove S and XEv, by changing X2ESolution concept...
concept bool XSearch = XESolution<Self> || X2ESolution<Self, XES>; 

// -------------
// Maybe make evaluation values total_ordered...
//   so that we can have MinEvaluation and MaxEvaluation (also LexiMinEvaluation and LexiMaxEvaluation)

// ===================

//template <class Self, class XSearch> // TODO: make this, if StopCriteria has XESolution
template <class Self>
concept bool XSearchMethod = true; 

template <class Self, XEvaluation XEv, XSearchMethod XM>
concept bool XStopCriteria = 
               requires(Self a, std::remove_reference_t<XEv>& e, std::remove_reference_t<XM>& m)
               {
                  { a.shouldStop(e, m) } -> bool; 
               };

// bugged code (too bad!!) - TODO: fix gcc to have this simplified version (only Self on XStopCriteria)
/*
template <class Self>
concept bool XStopCriteria =  requires(Self a, const XEvaluation& e, XSearchMethod& m)
                              {
                                 { a.shouldStop(e, m) } -> bool; 
                              };
*/
// gcc-7 bug
/*
BaseConcepts.hpp:260:39: internal compiler error: in synthesize_implicit_template_parm, at cp/parser.c:38874
                requires(Self a, const XEvaluation& e, XSearchMethod& m)
                                       ^~~~~
Please submit a full bug report,
with preprocessed source if appropriate.
See <file:///usr/share/doc/gcc-7/README.Bugs> for instructions.
*/

// gcc-8 bug
/*
BaseConcepts.hpp:266:55: internal compiler error: in synthesize_implicit_template_parm, at cp/parser.c:39141
 concept bool XStopCriteria2 =  requires(Self a, const XEvaluation& e, XSearchMethod& m)
                                                       ^~~~~~~~~~~
Please submit a full bug report,
with preprocessed source if appropriate.
See <file:///usr/share/doc/gcc-8/README.Bugs> for instructions.
*/

// ===================

template <class Self>
concept bool X01N = true; // TODO: space for [0,1]^N random keys... N could be constexpr template, but better not.

// ===================

} // namespace optframe

// compilation tests for concepts (these are NOT unit tests)
#include "BaseConcepts.ctest.hpp"

#endif // OPTFRAME_SOLUTION_CONCEPTS_HPP_