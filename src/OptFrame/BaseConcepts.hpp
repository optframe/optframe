#ifndef OPTFRAME_BASE_CONCEPTS_HPP_
#define OPTFRAME_BASE_CONCEPTS_HPP_

// Basic concepts for OptFrame
// XSolution
// XEvaluation
// XESolution
// XBaseSolution

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
concept bool Representation = optframe::ostreamable<R>;


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
concept bool XSolution = HasClone<Self> && HasToString<Self> && Representation<Self>;

// ==========================


template <class ADS>
concept bool Structure = true;

template <class S, Representation R>
concept bool HasGetR = requires(S a)
{
   {
      a.getR()
   }
   ->R &;
};

template <class S, Structure ADS = _ADS>
concept bool HasGetADS = requires(S a)
{
   {
      a.getADSptr()
   }
   ->ADS *;
};

template <class S, Representation R, Structure ADS = _ADS>
concept bool XBaseSolution = HasGetR<S, R> &&HasGetADS<S, ADS> && XSolution<S>;


template <class Self, Representation R>
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

// compilation tests for concepts (these are NOT unit tests)
#include "BaseConcepts.test.hpp"

#endif // OPTFRAME_SOLUTION_CONCEPTS_HPP_