#pragma once

#include "BaseConcepts.hpp"
// testing BaseConcepts: compilation tests (these are NOT unit tests!)
// unit tests can be found on tests/ folder
//
// these tests may help understanding deep compilation errors caused by concepts

// ============================


// only for <assert> like stuff
#ifndef NDEBUG
// Example of valid struct satisfying solution properties

template <XRepresentation R, class ADS = _ADS>
struct IsSolution
{
   R &getR();
   ADS *getADSptr();

   IsSolution<R, ADS>& clone()
   {
      return *this; // TODO:
   }

   std::string toString() const
   {
      return "";
   }

   friend std::ostream& operator<<(std::ostream& os, const IsSolution<R,ADS>& me)
   {
      os << me.toString();
      return os;
   }
};

template <class T>
struct IsEvaluation
{ 
   T &evaluation(); // this is required!

   IsEvaluation<T>& clone()
   {
      return *this; // TODO:
   }

   std::string toString() const
   {
      return "";
   }

   friend std::ostream& operator<<(std::ostream& os, const IsEvaluation<T>& me)
   {
      os << me.toString();
      return os;
   }
};

// =======================================

// test BaseSolution concept
template<XBaseSolution<double> S>
class TestBaseSol
{
public:
};

template<XRepresentation R, XRSolution<R> XRS>
class TestXRSolution
{
public:
};

template<XSolution XS>
class TestXSol
{
public:
};

// test evaluation concept
template<XEvaluation XE>
class TestEv
{
public:
};


struct optframe_test_debug_testsol_issolution_disable_runtime
{
// test if following structure is valid
TestBaseSol<IsSolution<double>> test;
TestXRSolution<double, IsSolution<double>> testxrs;
TestEv<IsEvaluation<short>> testev;
};

// =========================================================

template<XSolution S, XEvaluation XEv, X2ESolution<S, XEv> X2ES>
class TestPopOrPareto
{
public:
};

// ----------------

// example of a Population element
template <XSolution S, XEvaluation XEv>
struct IsPopulation
{
};

// example of a Pareto element
template <XSolution S, XEvaluation XEv>
struct IsPareto
{
};

struct optframe_test_debug_testpareto_ispop_ispareto_disable_runtime
{
TestPopOrPareto<IsSolution<double>, IsEvaluation<int>, IsPopulation<IsSolution<double>,IsEvaluation<int>> > testLocal1;
TestPopOrPareto<IsSolution<double>, IsEvaluation<int>, IsPareto<IsSolution<double>,IsEvaluation<int>> > testLocal2;
};


#endif // NDEBUG clause
