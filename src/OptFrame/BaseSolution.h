#ifndef OPTFRAME_SOLUTION_CONCEPTS_H_
#define OPTFRAME_SOLUTION_CONCEPTS_H_

// this should be renamed to BaseConcepts instead of BaseSolution... TODO:

#include<cstddef> // nullptr_t
#include<string>

// this is NOT official c++20 concepts... just some for OptFrame! (based on lite concepts g++ 7)
#include "myconcepts.h"

// the default ADS type is 'int'
// adopting 'void' type would cause troubles in constructor/copy/move operations
// if not used, it can be ignored with few impacts (hoping compiler will help us!)
typedef std::nullptr_t OPTFRAME_DEFAULT_ADS;
typedef OPTFRAME_DEFAULT_ADS OptFrameADS; // more beautiful :)
typedef OPTFRAME_DEFAULT_ADS _ADS;        // more beautiful :)

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
concept bool XSolution = HasClone<Self> &&HasToString<Self>;

// ==========================

template <class R>
concept bool Representation = true;

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
//concept bool BaseSolution = HasGetR<S, R> &&HasGetADS<S, ADS> && XSolution<S>;
concept bool BaseSolution = HasGetR<S, R> &&HasGetADS<S, ADS> && XSolution<S>;


template <class Self, Representation R>
concept bool RSolution = HasGetR<Self, R> && XSolution<Self>;

// ============================

template <class Self>
concept bool HasGetObj = requires(Self a)
{
   {
      //a.getObj()
      a.evaluation() // keeping standard name since OptFrame's early times
   }
   //->ObjType &;
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
concept bool XEvaluation = HasClone<Self> && HasToString<Self> && HasGetObj<Self>;

// XSolution and XEvaluation are container-inspired "conceptual objects", to carry Representation and Objective Value
// One can even aggregate both in a single unified "thing", called XESolution


// funny thing, Solution doesn't carry specific representation (yet)
// probably, because Representation itself is as abstract as a solution... 
// in case of "ObjType" it's different, we may operate over it, perhaps: compare, perhaps "add", "subtract", ... to create costs
template <class Self>
concept bool XESolution = XSolution<Self> && XEvaluation<Self>;


// ============================

// only for <assert> like stuff
#ifndef NDEBUG
// Example of valid struct satisfying solution properties

template <Representation R, Structure ADS = _ADS>
struct IsSolution
{
   R &getR();
   ADS *getADSptr();

   IsSolution<R, ADS>& clone()
   {
      return *this; // TODO:
   }

   std::string toString()
   {
      return "";
   }
};

// test BaseSolution concept
template<BaseSolution<double> S>
class TestBaseSol
{
public:
};

struct optframe_test_debug_testsol_issolution_disable_runtime
{
// test if following structure is valid
TestBaseSol<IsSolution<double>> test;
};

// test evaluation concept
template<XEvaluation XE>
class TestEv
{
public:
};

#endif // NDEBUG clause

#endif // OPTFRAME_SOLUTION_CONCEPTS_H_