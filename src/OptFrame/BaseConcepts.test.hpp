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

template <Representation R, Structure ADS = _ADS>
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

// test BaseSolution concept
template<XBaseSolution<double> S>
class TestBaseSol
{
public:
};

template<Representation R, XRSolution<R> XRS>
class TestXRSolution
{
public:
};

template<XSolution XS>
class TestXSol
{
public:
};

struct optframe_test_debug_testsol_issolution_disable_runtime
{
// test if following structure is valid
TestBaseSol<IsSolution<double>> test;
TestXRSolution<double, IsSolution<double>> testxrs;
};

// test evaluation concept
template<XEvaluation XE>
class TestEv
{
public:
};

#endif // NDEBUG clause
