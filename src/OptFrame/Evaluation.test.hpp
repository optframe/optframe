#pragma once

#include "Evaluation.hpp"

// =========================================
// Compilation tests for Evaluation concepts
// These are NOT unit tests... 
// Unit Tests are on tests/ folder
// =========================================

// testing default evaluation (NDEBUG=0)
#ifndef NDEBUG

namespace optframe {

// TestTArithMO_is_zero2 is a XEvaluation class

template<optframe::basic_arithmetics ObjType>
class TestTArithMO_is_zero2
{
public:
   ObjType infMeasureX;

   bool outdated; // this is required by XEvaluation
   bool estimated; // this is required by XEvaluation

   bool betterStrict(const TestTArithMO_is_zero2<ObjType>& e); // required
   bool betterNonStrict(const TestTArithMO_is_zero2<ObjType>& e); // required
   bool equals(const TestTArithMO_is_zero2<ObjType>& e); // required
   bool isStrictImprovement();
   bool isNonStrictImprovement();


   void update(const TestTArithMO_is_zero2<ObjType>& e); // required
   TestTArithMO_is_zero2<ObjType> diff(const TestTArithMO_is_zero2<ObjType>& e); // required


   bool f()
   {
      return optframe::numeric_is_zero<ObjType>(infMeasureX);
   }

   string toString() const
   {
      return "";
   }

   TestTArithMO_is_zero2& clone()
   {
      return *this;
   }

   ObjType evaluation() const
   {
      ObjType o;
      return o;
   }

   friend ostream& operator<<(ostream& os, const TestTArithMO_is_zero2& me)
   {
      os << me.toString();
      return os;
   }
};

struct optframe_test_debug_testev_evaluation_disable_runtime
{
   // test if following structure is valid
   TestEv<Evaluation<double>> test_double;
   TestEv<Evaluation<int>> test_int;
   TestEv<Evaluation<>> test_default;
   TestEv<Evaluation<SingleObjValue>> test_sov; // single obj value
   TestEv<TestTArithMO_is_zero<MultiObjValue<int, double>>> Tmo;
   TestEv<TestTArithMO_is_zero2<MultiObjValue<int, double>>> Tmo2;
   TestEv<Evaluation<MultiObjValue<int, double>>> test_mov; // multi obj value
};

struct optframe_debug_test_evaluation
{
   Evaluation<> testEvaluation;
};

} // namespace optframe

#endif // NDEBUG