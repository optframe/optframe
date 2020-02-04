#ifndef OPTFRAME_MULTI_OBJ_VALUE_HPP
#define OPTFRAME_MULTI_OBJ_VALUE_HPP

#include <tuple>

#include "BaseSolution.h" // TODO: rename to BaseConcepts

//#include "Util/printable.h"

//namespace optframe{

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

   friend ostream& operator<<(ostream& os, const MOVCompare& me)
   {
      os << "MOVCompare(lt=" << me.lt << ";eq=" << me.eq << ";gt=" << me.gt << ")";
      return os;
   }
};

// typically, template parameter packs are: template<class... Params>
// here, parameter pack requires 'totally_ordered' elements (by concepts)
template<optframe::totally_ordered... AllObjTypes>
class MultiObjValue
{
protected:
   std::tuple<AllObjTypes...> objValues;

public:
   MultiObjValue(std::tuple<AllObjTypes...> allObjValues)
     : objValues(allObjValues)
   {
   }

   template<class... Args>
   MultiObjValue(Args&&... allObjValues)
     //: objValues(std::forward<Args>(allObjValues)...) // does it help?
     : objValues(allObjValues...)
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

   std::string toString() const
   {
      std::stringstream ss;
      // c++17 fold expression std::apply (helps a lot!). TODO: capture last value to avoid semi-colon
      ss << "MultiObjValue:(";
      std::apply([&](auto&&... args) { ((ss << args << ';'), ...); }, this->objValues);
      ss << ")";
      return ss.str();
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
   // todo: implement operators (really necessary? don't know yet.. maybe just on default Evaluation, but not all)
   // ==
   // +
   // ...
};

// ---------------------------------
// Compile-tests (validate concepts)
// => Disable these with NDEBUG <=
// ---------------------------------
// IMPORTANT: these are different from Unit Tests, because
// these help compile "to break" in advance, and if it progresses further,
// something else will break in perhaps an unexplicable way... good old C++ :)
// We still need to ensure proper unit testing on tests/ folder, for other reasons.

#ifndef NDEBUG
struct optframe_debug_example_test_mov
{
   struct s_empty
   {};

   int f()
   {
      std::tuple<int, char> foo(10, 'x');
      MultiObjValue<int, char> testMOV1(foo);
      MultiObjValue<int, char> testMOV2(20, 'y');
      //MultiObjValue<s_empty, char> testMOV2(s_empty(), 'y'); // ERROR: breaks 'totally_ordered' requirement
   };
};

// TODO: create unit tests to validate simple comparisons between MultiObjValue objects

#endif

//} // namespace optframe

#endif // OPTFRAME_MULTI_OBJ_VALUE_HPP