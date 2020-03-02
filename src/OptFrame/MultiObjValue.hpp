#ifndef OPTFRAME_MULTI_OBJ_VALUE_HPP
#define OPTFRAME_MULTI_OBJ_VALUE_HPP

#include <assert.h>
#include <iostream>
#include <sstream>
#include <tuple>

#include "SingleObjValue.hpp" // must re-use some structures
#include "myconcepts.h"

//#include "Util/printable.h"

namespace optframe {

// Struct to handle comparison values for two MultiObjValue
// It could be a triple of ints... but too many tuples already on code below!! (largely complex)
// for simplicity, decided to create a struct to name each field
struct MOVCompare
{
   int lt{ 0 }; // count (<=>) -1
   int eq{ 0 }; // count (<=>) 0
   int gt{ 0 }; // count (<=>) +1

   void add(const MOVCompare& other)
   {
      lt += other.lt;
      eq += other.eq;
      gt += other.gt;
   }

   friend std::ostream& operator<<(std::ostream& os, const MOVCompare& me)
   {
      os << "MOVCompare(lt=" << me.lt << ";eq=" << me.eq << ";gt=" << me.gt << ")";
      return os;
   }
};

// typically, template parameter packs are: template<class... Params>
// here, parameter pack requires 'totally_ordered' elements (by concepts)
// IMPORTANT: it may be good to already require 'basic_arithmetics' on types (+,-) (and perhaps * in next phase...) which are historically useful
// if this is a bad decision, we can revert it some day :)
// basic arithmetics are useful to compute 'MoveCosts', infeasibility weights and many other things
template<optframe::basic_arithmetics... AllObjTypes>
//template<optframe::totally_ordered... AllObjTypes>
class MultiObjValue
{
   //protected:
   // should be public, to make it easier is_zero verifications
public:
   std::tuple<AllObjTypes...> objValues;

public:
   explicit MultiObjValue(std::tuple<AllObjTypes...> allObjValues)
     : objValues(allObjValues)
   {
   }

   template<class... Args>
   explicit MultiObjValue(Args&&... allObjValues)
     : objValues(std::forward<Args>(allObjValues)...) // does it help?
                                                      //: objValues(allObjValues...)
   {
   }

protected:
   // compare tuples and return "spaceship" comparison (-1,0,+1) to each value
   // magic solution adapted from: https://stackoverflow.com/questions/1198260/how-can-you-iterate-over-the-elements-of-an-stdtuple
   // base: I = 0
   template<std::size_t I = 0>
   inline typename std::enable_if<I == sizeof...(AllObjTypes), MOVCompare>::type
   comparet(const std::tuple<AllObjTypes...>& tOther) const
   {
      return MOVCompare(); // empty comparison struct MOVCompare
   }
   // continues.. I > 0
   template<std::size_t I = 0>
     inline typename std::enable_if < I<sizeof...(AllObjTypes), MOVCompare>::type
                                      comparet(const std::tuple<AllObjTypes...>& tOther) const
   {
      MOVCompare movc;
      // beware that '==', '<' and '>' may not be completely consistent, must sum individually
      if (std::get<I>(this->objValues) == std::get<I>(tOther))
         movc.eq += 1; // equals '0'
      if (std::get<I>(this->objValues) < std::get<I>(tOther))
         movc.lt += 1; // less '-1'
      if (std::get<I>(this->objValues) > std::get<I>(tOther))
         movc.gt += 1; // greater '+1'
      // get next recursive  (TODO: may pass by reference or move)
      MOVCompare movc2 = comparet<I + 1>(tOther);
      // aggregate information
      movc.add(movc2); // TODO: may get directly from method as 'move &&' and drop
      return movc;
   }

public:
   // too bad, no spaceship operator widely available (yet) to depend on (example: for basic types like int, double, etc)
   // will create a 'compare' helper here, for other methods
   // -1 less, 0 equals, 1 greater, -2 different
   // (not that 'different' means 'uncomparable' here in general sense.. for Pareto implementations we solve this in other manner)
   virtual int compare(const MultiObjValue& c) const
   {
      // get number of objectives (compile-time)
      constexpr int N = sizeof...(AllObjTypes);
      MOVCompare movc = comparet(c.objValues); // do not use 'comparev' here, will lose precious time
      if (movc.eq == N)
         return 0; // all equals
      if (movc.lt == N)
         return -1; // all less
      if (movc.gt == N)
         return +1; // all greater
      return -2;    // different/uncomparable (needs some Pareto-based or other specific rule to decide)
   }

   // comparev just "safely" exposes comparet
   // TODO: maybe this method may become 'constexpr'.. hard to unroll all loops and aggregate struct MOVCompare ??
   MOVCompare comparev(const MultiObjValue& c) const
   {
      // c++17 fold expression std::apply could be perhaps used here, with some indexing technique (TODO)
      return comparet(c.objValues);
   }

   bool operator==(const MultiObjValue& c) const
   {
      return (compare(c) == 0);
   }
   bool operator!=(const MultiObjValue& c) const
   {
      // consistency
      return !this->operator==(c);
   }
   bool operator>(const MultiObjValue& c) const
   {
      return (compare(c) == 1);
   }
   bool operator>=(const MultiObjValue& c) const
   {
      // do NOT make it !operator<, because it may break consistency... ensure it's REALLY >=
      return (compare(c) >= 0);
   }
   bool operator<(const MultiObjValue& c) const
   {
      return (compare(c) == -1);
   }
   bool operator<=(const MultiObjValue& c) const
   {
      // do NOT make it !operator<, because it may break consistency... ensure it's REALLY >=
      int comp = compare(c); // beware that comp may be -2
      return (comp == -1) || (comp == 0);
   }

   // now, going to arithmetic operators

protected:
   // using same technique as 'comparet'
   // to simplify, 'operate_t' will operate over myself with += , -= and *=
   template<char OP, std::size_t I = 0>
   inline typename std::enable_if<I == sizeof...(AllObjTypes), void>::type
   operatet(const std::tuple<AllObjTypes...>& tOther)
   {
      // nothing to do (no objetives to compute with I==sizeof out of boundaries)
   }
   // continues.. I > 0
   template<char OP, std::size_t I = 0>
     inline typename std::enable_if < I<sizeof...(AllObjTypes), void>::type
                                      operatet(const std::tuple<AllObjTypes...>& tOther)
   {
      // TODO: use 'OP' as consexpr to directly evaluate desired operation (will perform on compile-time)
      // WARNING: doing this on runtime (compiler may be smart and help us!). It would very likely...
      if (OP == '+')
         std::get<I>(this->objValues) += std::get<I>(tOther);
      if (OP == '-')
         std::get<I>(this->objValues) -= std::get<I>(tOther);
      // useful, but not supporting scalar multiplication yet!! by int, double.. on practice, we cannot do 'weights' yet, too hard..
      //if(OP == '*')
      //   std::get<I>(this->objValues) *= std::get<I>(tOther);
   }

public:
   MultiObjValue& operator+=(const MultiObjValue& c)
   {
      operatet<'+'>(c.objValues);
      return *this;
   }

   MultiObjValue& operator-=(const MultiObjValue& c) 
   {
      operatet<'-'>(c.objValues);
      return *this;
   }

   // dropping now to avoid scalar mult support
   //MultiObjValue& operator*=(const MultiObjValue& c)
   //{
   //   operatet<'*'>(c.objValues);
   //   return *this;
   //}

   // division is not required!

   MultiObjValue operator+(const MultiObjValue& c) const
   {
      return MultiObjValue(*this) += c;
   }

   MultiObjValue operator-(const MultiObjValue& c) const
   {
      return MultiObjValue(*this) -= c;
   }

   //MultiObjValue operator*(const MultiObjValue& c) const
   //{
   //   return MultiObjValue(*this) *= c;
   //}

   // finally, must ensure this class is printable
   std::string toString() const
   {
      std::stringstream ss;
      // c++17 fold expression std::apply (helps a lot!). TODO: capture last value to avoid semi-colon
      ss << "MultiObjValue:(";
      std::apply([&](auto&&... args) { ((ss << args << ';'), ...); }, this->objValues);
      ss << ")";
      return ss.str();
   }

   friend std::ostream& operator<<(std::ostream& os, const MultiObjValue& me)
   {
      os << me.toString();
      return os;
   }

   // this has some scope issue, if we want it to be in optframe:: only... must let access protected free
   /*
   friend inline bool
   numeric_is_zero(const MultiObjValue<AllObjTypes...>& tOther)
   {
      //return compare_zero<Args...>(tOther);
      // c++17 fold apply
      return std::apply([](auto... v) { return (optframe::numeric_is_zero(v) && ...); }, tOther.objValues);
   }
   */
};

/*
// ==================================================
// define when a MultiObjValue may be considered zero
// base: I = 0
template<class... AllObjTypes, std::size_t I = 0>
inline typename std::enable_if<I == sizeof...(AllObjTypes), bool>::type
compare_zero(const std::tuple<AllObjTypes...>& tOther)
{
   return true; // empty I out of bounds ust be true
}
// continues.. I > 0
template<class... AllObjTypes, std::size_t I = 0>
  inline typename std::enable_if < I<sizeof...(AllObjTypes), bool>::type
                                   compare_zero(const std::tuple<AllObjTypes...>& tOther)
{
   // every element must be zero
   if (!optframe::numeric_is_zero(std::get<I>(tOther)))
      return false;
   // get next recursive (TODO: may try to short-circuit with AND &&)
   return compare_zero<AllObjTypes..., I + 1>(tOther);
}


//template<class T, class... Args>
//inline typename std::enable_if<std::is_same<T, MultiObjValue<Args...>>::value, bool>::type
//numeric_is_zero(const MultiObjValue<Args...>& tOther)
//{
 //  return compare_zero<Args...>(tOther);
//}
*/

//template<optframe::basic_arithmetics... Args, class T = optframe::MultiObjValue<Args...>>
template<optframe::basic_arithmetics... Args>
inline bool
//inline typename std::enable_if<std::is_same<std::remove_reference_t<T>, optframe::MultiObjValue<Args...>>::value, bool>::type
numeric_is_zero(const optframe::MultiObjValue<Args...>& tOther)
//numeric_is_zero(const T& tOther)
{
   //return compare_zero<Args...>(tOther);
   // c++17 fold apply
   return std::apply([](auto... v) { return (optframe::numeric_is_zero(v) && ...); }, tOther.objValues);
   // only issue is PROTECTED info. let it PUBLIC for now
}

// TODO: find a better way to capture like this!
/*
template<optframe::basic_arithmetics... Args, class T = MultiObjValue<Args...>>
//inline const T
inline typename std::enable_if<std::is_same<std::remove_reference_t<T>, optframe::MultiObjValue<Args...>>::value, T>::type
numeric_zero()
{
   std::tuple<Args...> tdef; // default values (TODO: iterate to define zero)
   return T(tdef);
}
// This conflicts with double definition and MultiObjValue<double> ... don't know how to solve it, without passing as parameter value (to avoid ambiguity!)
*/

/*
template<optframe::basic_arithmetics... Args, class T = MultiObjValue<Args...>>
//inline const T
inline typename std::enable_if<std::is_same<std::remove_reference_t<T>, optframe::MultiObjValue<Args...>>::value, void>::type
numeric_zero(MultiObjValue<Args...>& t)
{
   std::tuple<Args...> tdef; // default values (TODO: iterate to define zero)
   t.objValues = tdef;
}
*/

template<
template<typename...> class T,
typename... Args2
>
inline typename std::enable_if<
                                       std::is_same<
                                               T<Args2...>, MultiObjValue<Args2...>
                                       >::value, void
                              >::type
numeric_zero(T<Args2...>& t)
{
   std::tuple<Args2...> tdef; // default values (TODO: iterate to define zero)
   t.objValues = tdef;
}

// -----------------

template<optframe::basic_arithmetics T>
inline T
get_numeric_zero()
{
   T t;
   optframe::numeric_zero<T>(t);
   return t;
}


/*
template<class... Args>
inline bool
numeric_is_zero(const MultiObjValue<Args...> tOther)
{
   return numeric_is_zero(tOther);
}
*/

// ---------------------------------
// Compile-tests (validate concepts)
// => Disable these with NDEBUG <=
// ---------------------------------
// IMPORTANT: these are different from Unit Tests, because
// these help compile "to break" in advance, and if it progresses further,
// something else will break in perhaps an unexplicable way... good old C++ :)
// We still need to ensure proper unit testing on tests/ folder, for other reasons.

#ifndef NDEBUG

// TestTArithMO_is_zero should be an XEvaluation type

template<optframe::basic_arithmetics ObjType>
class TestTArithMO_is_zero
{
public:
   ObjType infMeasureX;

   bool outdated; // required for XEvaluation
   bool estimated; // required for XEvaluation
   bool betterStrict(const TestTArithMO_is_zero<ObjType>& e); // required
   bool betterOrEquals(const TestTArithMO_is_zero<ObjType>& e); // required
   void update(const TestTArithMO_is_zero<ObjType>& e); // required


   void f()
   {
      assert(optframe::numeric_is_zero(infMeasureX));
   }

   std::string toString() const
   {
      return "";
   }

   TestTArithMO_is_zero& clone()
   {
      return *this;
   }

   ObjType evaluation() const
   {
      ObjType t;
      return t;
   }

   friend ostream& operator<<(ostream& os, const TestTArithMO_is_zero& me)
   {
      os << me.toString();
      return os;
   }
};

struct optframe_debug_example_test_mov
{
   struct s_empty
   {};

   void f()
   {
      std::tuple<int, char> foo(10, 'x');
      MultiObjValue<int, char> testMOV1(foo);
      MultiObjValue<int, char> testMOV2(20, 'y');

      MultiObjValue<int, double> testMOV3(0, 0.000001);
      assert(optframe::numeric_is_zero(testMOV3)); // should be zero!

      MultiObjValue<int, double> num0;
      //optframe::numeric_zero<int, double>(num0);
      optframe::numeric_zero(num0);
      assert(optframe::numeric_is_zero(num0)); // should be zero!

      TestTArithMO_is_zero<MultiObjValue<int, double>> Tmo;

      //MultiObjValue<s_empty, char> testMOV2(s_empty(), 'y'); // ERROR: breaks 'totally_ordered' requirement
   };
};

// TODO: create unit tests to validate simple comparisons between MultiObjValue objects

#endif

} // namespace optframe

#endif // OPTFRAME_MULTI_OBJ_VALUE_HPP