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
   int lt; // count (<=>) -1
   int eq; // count (<=>) 0
   int gt; // count (<=>) +1
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
   inline typename std::enable_if<I == sizeof...(AllObjTypes), std::vector<int>>::type
   comparet(const std::tuple<AllObjTypes...>& tOther) const
   {
      return std::vector<int>(); // empty vector
   }
   // continues.. I > 0
   template<std::size_t I = 0>
     inline typename std::enable_if < I<sizeof...(AllObjTypes), std::vector<int>>::type
                                      comparet(const std::tuple<AllObjTypes...>& tOther) const
   {
      int comp = 0; // equals
      if(std::get<I>(this->objValues) < std::get<I>(tOther))
         comp = -1; // less
      if(std::get<I>(this->objValues) > std::get<I>(tOther))
         comp = 1; // greater
   
      vector<int> vv(1, comp);
      vector<int> vv2 = comparet<I + 1>(tOther);

      vv.insert(vv.end(), vv2.begin(), vv2.end());
      return vv;
   }

public:
   // too bad, no spaceship operator widely available (yet) to depend on (example: for basic types like int, double, etc)
   // will create a 'compare' helper here, for other methods

   virtual std::vector<int> compare(const MultiObjValue& c) const
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
      return true;
   }
   bool operator!=(const MultiObjValue& c) const
   {
      return !this->operator==(c);
   }
   bool operator>(const MultiObjValue& c) const
   {
      return true;
   }
   bool operator>=(const MultiObjValue& c) const
   {
      return true;
   }
   bool operator<(const MultiObjValue& c) const
   {
      return true;
   }
   bool operator<=(const MultiObjValue& c) const
   {
      return true;
   }
   // todo: implement operators
   // ==
   // +
   // ...
};

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
#endif

//} // namespace optframe

#endif // OPTFRAME_MULTI_OBJ_VALUE_HPP