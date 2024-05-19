// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_CONCEPTS_BASECONCEPTS_HPP_
#define OPTFRAME_CONCEPTS_BASECONCEPTS_HPP_

// =============================
// Basic concepts for OptFrame
// =============================
// XSolution
// XEvaluation
// XESolution
// XBaseSolution
// =============================

#include <cstddef>  // nullptr_t
#include <string>

// the default ADS type is 'int'
// adopting 'void' type would cause troubles in constructor/copy/move operations
// if not used, it can be ignored with few impacts (hoping compiler will help
// us!)
typedef std::nullptr_t OPTFRAME_DEFAULT_ADS;
typedef OPTFRAME_DEFAULT_ADS OptFrameADS;  // more beautiful :)
typedef OPTFRAME_DEFAULT_ADS _ADS;         // more beautiful :)

// Check if C++20 Concepts is supported
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)

// this is NOT official c++20 concepts... just some for OptFrame! (based on lite
// concepts g++ 7)
#include <OptFrame/Concepts/MyConcepts.hpp>

// may require some basic printing capabilities
// #include <OptFrame/printable/printable.hpp>

namespace optframe {

template <class T>
concept IsComplete = requires(T self) {
  {sizeof(self)};
};

template <typename T>
concept XOStreamable = requires(std::ostream& os, T value) {
  { os << value } -> std::convertible_to<std::ostream&>;
};

// Representation type just requires some Copy Constructive behavior
template <class R>
// concept bool Representation = true;
concept
#if __cplusplus <= 201703L  // after c++20, not required 'bool'
    XRepresentation = requires(R self) {
  {new R(self)};
};
#else
    XRepresentation = std::copy_constructible<R>;
#endif

// ========================

// requires Self& clone() method
// there may be some "Cloneable" in std future...
// TODO: change return to unique_ptr instead of reference
template <class Self>
concept HasClone = true;

template <class Self>
concept HasToString = requires(Self self) {
  { self.toString() } -> my_same_as<std::string>;
};

// TODO: should we require 'copy constructive' for 'XSolution'? or for
// 'XRepresentation'?

template <class Self>
concept XSolution = XRepresentation<Self>;

// -------
// Why is XSolutionOrIncomplete necessary?
// See imcoelho response on stackoverflow
// https://stackoverflow.com/questions/63429760/can-a-concept-be-checked-against-incomplete-type/69907133#69907133
// Trick to allow Solution type to be incomplete, during CRTP requirements
//
// Other possibility is to statically assert concept just after it becomes
// concrete (but definitive type needs to be known in advance).
//

template <class S>
concept XSolutionOrIncomplete = !IsComplete<S> || XSolution<S>;

// No more cloning!! finally! thanks concepts :)

// ==========================

// useless concept for 'Structure', there's no such Space... just a regular
// 'class'.
// template <class ADS>
// concept bool Structure = true;

// template <class S, XRepresentation R> // explored old gcc bug... "constrained
// concepts not allowed"
// note that "auto" is an "unconstrained concept", while "XRepresentation"
// forced a filter that cannot be done during concept definition.
template <class S, class R>
concept HasGetR = XRepresentation<R> &&(
    requires(S a) {
      { a.getR() } -> my_same_as<R&>;
    } ||
    requires(S a) {
      { a.getR() } -> my_same_as<R>;
    });

template <class S, class ADS = _ADS>
concept HasGetADS = requires(S a) {
  { a.getADSptr() } -> my_same_as<ADS*>;
};

// gcc bug allowed "XRepresentation R" here, but "error: a variable concept
// cannot be constrained" so, we should explicitly pass constraints on next line
// as: "XRepresentation<R>"
template <class S, class R, class ADS = _ADS>
concept XBaseSolution =
    XRepresentation<R> && HasGetR<S, R> && HasGetADS<S, ADS> && XSolution<S>;

// same gcc bug: "error: a variable concept cannot be constrained" for
// "XRepresentation R"
template <class Self, class R>
concept XRSolution = XRepresentation<R> && HasGetR<Self, R> && XSolution<Self>;

// ============================

// 'objval' means total order: basic arithmetics (+/-) and comparability
template <class T>
concept objval = optframe::basic_arithmetics<T> && optframe::comparability<T>;

//
// defining 'evgoal' concept: something that has +/- and some directed
// strong/weak comparison (betterThan/betterOrEquals)
//
// TODO(igormcoelho): OLD rename 'update' to 'add'?
// TODO(igormcoelho): OLD rename 'diff' to 'sub'?
//
template <class Self>
concept evgoal = (optframe::basic_arithmetics<Self> ||
                  (
                      requires(Self e, remove_ref<Self>& e2) {
                        { e.update(e2) } -> my_same_as<void>;
                      } &&
                      requires(Self e, const remove_ref<Self>& e2) {
                        { e.diff(e2) } -> my_same_as<Self>;
                      }));

template <class Self>
concept hasUpdateDiff = (
    requires(Self e, remove_ref<Self>& e2) {
      { e.update(e2) } -> my_same_as<void>;
    } &&
    requires(Self e, const remove_ref<Self>& e2) {
      { e.diff(e2) } -> my_same_as<Self>;
    });

template <class Self>
concept HasGetObj = requires(Self a) {
  typename Self::objType;  // requires 'objType' on XEvaluation...
  //
  // filtering 'objType' properties...
  // BROKEN for MultiEvaluator.. how to fix this?
  // requires optframe::objval<typename Self::objType>;
  //
  // objType must support basic arithmetics
  requires optframe::basic_arithmetics<typename Self::objType>;
  //
  // { a.evaluation() } -> my_convertible_to<typename Self::objType>;
  { a.evaluation() } -> std::convertible_to<typename Self::objType>;
  //
  //-> optframe::objval; // note that, if 'ObjType' becomes complex, one must
  // return a moveable copy, not reference of internal value
  // any totally ordered output is fine! (double, int, ParetoFront , etc...)
};

// Evaluation of any numeric/comparable type ObjType (can be int, double, or
// pareto-front-based set for objective space) decided by using generic "total
// ordered" concept, to avoid extra template ObjType this will finally solve
// dependency on different obj types (int, double, etc.) in same project it will
// also allow unification of Evaluation and MultiEvaluation into single same
// "thing" note that getObjValue and getInfeasibleValue are not necessary here,
// just getObj one can implement this way if preferred, separating or not both
// "values"... not mandatory anymore
template <class Self>
concept XEvaluation =  // sing obj. evaluation part (standard multi obj)
    (
        optframe::evgoal<Self>&& HasClone<Self>&& HasToString<Self>&&
            HasGetObj<Self>&& optframe::ostreamable<Self>&& requires(Self e) {
              typename Self::objType;
            } &&
        requires(Self e) {
          // variable 'outdated' is still useful for optimizations
          //{ e.outdated } -> my_convertible_to<bool>;
          //{ e.estimated } -> my_convertible_to<bool>;
          //
          // converted variable to methods!
          { e.isOutdated() } -> my_convertible_to<bool>;
          { e.invalidate() } -> my_same_as<void>;  // makes object outdated
          // There is no way to make object !outdated, but it doesn't matter
          // because it is an implementation-specific mechanism, not general.
          { e.isEstimated() } -> my_convertible_to<bool>;
          // WHY 'diff' here... not on evgoal already?
          // {e.diff(e)};
        });

// XSolution and XEvaluation are container-inspired "conceptual objects", to
// carry Representation and Objective Value One can even aggregate both in a
// single unified "thing", called XESolution

// =================== Notes on Pair ==================
// One Pair could be implemented in two ways:

// funny thing, Solution doesn't carry specific representation (yet)
// probably, because Representation itself is as abstract as a solution...
// in case of "ObjType" it's different, we may operate over it, perhaps:
// compare, perhaps "add", "subtract", ... to create costs
// template <class Self>
// concept bool XESolution = XSolution<Self> && XEvaluation<Self>;
// -----> now concept also allows pair<S, XEv> to represent composed space
// <-----
template <class Self>
concept XESolution = XSolution<Self> && requires(Self p) {
  typename Self::first_type;   // requires a "first_type" with some "XSolution
                               // properties"
  typename Self::second_type;  // requires a "second_type" with some
                               // "XEvaluation properties"
  // p.first;  // requires a XSolution variable named 'first'
  { p.first } -> my_convertible_to<typename Self::first_type>;
  // p.second; // requires a XEvaluation variable named 'second'
  { p.second } -> my_convertible_to<typename Self::second_type>;
  //
  // requires my_same_as<decltype(p.first), typename Self::first_type>;    //
  // not enough for reference and non-reference cases requires
  // my_same_as<decltype(p.second), typename Self::second_type>;
  // details on types
  requires XSolution<typename Self::first_type>;
  requires XEvaluation<typename Self::second_type>;
};

// =====================
// X2Solution Space: 2^S
// ---------------------
// This concept comprises Population-based or Pareto-based spaces
// One difference is that, even when Population is used, typically a single
// 'best' is returned, and
//  for Pareto-based space, solutions are not "totally-orderable", thus only
//  partially comparable, so multiple "choices" are desired.
// In other words, MO is usually not homeotelic (all tend to same direction),
// but heterotelic.
//   "homeo-" (sameness, identity,..), "telic" (goal or direction)
// Note that Dominance is a concept from Game Theory that implies "better than
// the alternative";
//  and more specifically that "it improves at least one, and it's not worse on
//  any".
// ---------------------
// not used for now...
// template <class Self, XSolution S>
// concept bool X2Solution = true; // nothing actually required (for now...).
// Maybe 'at' and 'size' are good candidates
// ---
// general one:
template <class Self, class P>
// concept bool XPowerSet = true;
concept XPowerSet = requires(Self a, size_t idx) {
  {
    a.size()
    } -> my_convertible_to<size_t>;  // could this be 'int' as well? TODO: test

  {
    // a.getP(idx) // abandoning 'getP' in favor of 'at'... for compatibility
    // with 'vector'
    //
    a.at(idx)
    } -> my_convertible_to<P>;
  //
  typename Self::value_type;
};

// TODO: powerset could return a 'size'(or 'count'), and perhaps a vector (or
// iterator) of objects for type P

// now redefining X2Solution more beautifully...
// too bad (x2), this explores a gcc bug!! "error: a variable concept cannot be
// constrained"
// template<class Self, XSolution S> // needs to fix this 'XSolution S'
template <class Self, class S>  // fixed "constrained variable concept" on line
                                // below... "XSolution<S>&&..."
concept X2Solution = XSolution<S> &&
    XPowerSet<Self, S>;  // Too bad, this is unused on OptFrame... :'(

// ---
// We will usually assume a X2ESolution type, since it carries solution sample
// and evaluation space together It is in fact: s \subseteq 2^S  plus its own
// evaluation (for each solution)...
// template <class Self, XSolution S, XEvaluation XEv> // unused S and XEv!
// (could be replaced directly by XSolution and XEvaluation down here...)
//
// The line below explores a gcc bug!! Needs to fix "XESolution XES"... cannot
// appear on definition!
// template<class Self, XESolution XES> // unused S and XEv! (could be replaced
// directly by XSolution and XEvaluation down here...) concept bool X2ESolution
// = X2Solution<Self, S>; concept bool X2ESolution = XPowerSet<Self, S> &&
// XPowerSet<Self, XEv>;
template <class Self,
          class XES>  // fixes gcc bug! "XESolution<XES>&&..." is good!
concept X2ESolution = XESolution<XES> && XPowerSet<Self, XES>;

// TODO1: should make exponential over a XESolution... not separated sets!!
// TODO2: require any container with operator*, instead of hardcoded 'uptr'....
// let's move on!

// concept bool X2ESolution = XPowerSet<Self, XSolution> && XPowerSet<Self,
// XEvaluation>; // TODO: may also include pair here, instead of 'merge'...
// concept bool X2ESolution = XPowerSet<Self, XSolution<Self>> &&
// XPowerSet<Self, XEv>; // TODO: may also include pair here, instead of
// 'merge'...

// TODO: create 'ideal' and 'nadir' point requirements for every MO type
// best would be to re-create int or double with these extra "properties"

// this has gcc bug... needs fixing! "error: a variable concept cannot be
// constrained"
// template<class Self, XSolution S, XEvaluation XEv>
//
// looks unused!!
// template<class Self, class S, class XEv> // fixes gcc bug (on line below)
// concept bool X2ESPareto = XSolution<S> && XEvaluation<XEv> &&
// X2ESolution<Self, S, XEv>; // TODO: require 'dominates' and 'weakDominates'
// here
//... optimization directions and 'ideal'/'nadir' points may also come here...

// this has gcc bug... needs fixing! "error: a variable concept cannot be
// constrained"
// template<class Self, XSolution S, XEvaluation XEv>
//
// looks unused!!
// template<class Self, class S, class XEv> // fixes gcc bug (on line below)
// concept bool X2ESPopulation = XSolution<S> && XEvaluation<XEv> &&
// X2ESolution<Self, S, XEv>; // TODO: require 'fitness' here, or some sort of
// evolutionary concepts...
// .... in the end, this may be a search in the 'population space'. One may
// adapt a SingleObjSearch to embed a PopulationalSearch (TODO: create
// PopulationalSearch)

// ====================

// Search Space for Optimization (definition) or, perhaps, Optimization Space
// A composition of searchable structures (XSolution) and
//   evaluation criteria to guide search on these structures,
//   being them single or multi objective.

// template <class Self, XSolution S, XEvaluation XEv> // TODO: should remove S
// and XEv, by changing X2ESolution concept... concept bool XSearch =
// XESolution<Self> || X2ESolution<Self, S, XEv>;
//
// same bug as before... cannot have "XESolution XES" during concept definition!
// template<class Self, XESolution XES> // TODO: should remove S and XEv, by
// changing X2ESolution concept...
template <class Self, class XES>  // TODO: should remove S and XEv, by changing
                                  // X2ESolution concept...
concept
    // XSearch = XESolution<XES> && (XESolution<Self> || X2ESolution<Self,
    // XES>);
    //
    //  XES must be of XESolution kind
    //  XES is either the BASE type for Self or the REAL type for Self
    XSearch = (XESolution<XES> && XESolution<Self>) ||
              (XESolution<XES> && X2ESolution<Self, XES>);

// -------------
// Maybe make evaluation values total_ordered...
//   so that we can have MinEvaluation and MaxEvaluation (also LexiMinEvaluation
//   and LexiMaxEvaluation)

// ===================

// template <class Self, class XSearch> // TODO: make this, if StopCriteria has
// XESolution
template <class Self>
concept XSearchMethod = true;

// ===================

template <class Self>
concept X01N = true;  // TODO: space for [0,1]^N random keys... N could be
                      // constexpr template, but better not.

// ===================
// XSEvaluation (Single evaluation)      - Requires Total Order
// XMEvaluation (Multi evaluation)       - Requires Set of Total Order
// XESSolution (ES = objective single)   - Requires Total Order
// XEMSolution (EM = objective multiple) - Requires Set of Total Order

template <class Self>
concept XSEvaluation =
    XEvaluation<Self> && comparability<typename Self::objType>;

// XMEvaluation mimics XPowerSet... but could not use it directly!

template <class Self>
concept XMEvaluation = XEvaluation<Self> &&
    requires(Self e, typename Self::objType m,
             typename Self::objType::value_type v,
             typename Self::objType::value_type::objType vm, size_t idx) {
  { m.size() } -> std::convertible_to<size_t>;
  { m[idx] } -> std::convertible_to<typename Self::objType::value_type>;
} && comparability<typename Self::objType::value_type::objType>;  // TOTAL ORDER

template <class Self>
concept XESSolution =
    XESolution<Self> && XSEvaluation<typename Self::second_type>;

template <class Self>
concept XEMSolution =
    XESolution<Self> && XMEvaluation<typename Self::second_type>;

template <class Self,
          class XMES>  // fixes gcc bug! "XESolution<XES>&&..." is good!
concept X2EMSolution = XEMSolution<XMES> && XPowerSet<Self, XMES>;

// ===================

// XFamily concept

template <class Self>
concept XFamily = requires(Self a) {
  { a.family() } -> my_same_as<std::string>;
};

// ========================

}  // namespace optframe

// compilation tests for concepts (these are NOT unit tests)
// TODO: put on unit tests or directly here (without #include "printable.h")
#include <OptFrame/Concepts/BaseConcepts.ctest.hpp>

#else  // No Concepts!!!

// disable
#define XEMSolution typename
#define XESolution typename
#define XRepresentation typename
#define XEvaluation typename
#define XSolution typename
#define XSolutionOrIncomplete typename  // TODO: remove
#define XSearchMethod typename
#define XESSolution typename

#endif  // __cpp_concepts

#endif  // OPTFRAME_CONCEPTS_BASECONCEPTS_HPP_
