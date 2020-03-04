#ifndef OPTFRAME_MYCONCEPTS
#define OPTFRAME_MYCONCEPTS

// general concepts expected to exist on c++20

#include <memory>   // unique_ptr
#include <optional> // optional

namespace optframe
{
   
// default renaming of std::unique_ptr (too long!!)
template<class T>
using uptr = std::unique_ptr<T>;

// default renaming of std::optional (too long!!)
template<class T>
using op = std::optional<T>;

//https://en.cppreference.com/w/cpp/concepts/boolean
/*
template<class B>
  concept boolean =
    std::movable<std::remove_cvref_t<B>> &&
    requires(const std::remove_reference_t<B>& b1,
             const std::remove_reference_t<B>& b2, const bool a) {
      { b1 } -> std::convertible_to<bool>;
      { !b1 } -> std::convertible_to<bool>;
      { b1 && b2 } -> std::same_as<bool>;
      { b1 &&  a } -> std::same_as<bool>;
      {  a && b2 } -> std::same_as<bool>;
      { b1 || b2 } -> std::same_as<bool>;
      { b1 ||  a } -> std::same_as<bool>;
      {  a || b2 } -> std::same_as<bool>;
      { b1 == b2 } -> std::convertible_to<bool>;
      { b1 ==  a } -> std::convertible_to<bool>;
      {  a == b2 } -> std::convertible_to<bool>;
      { b1 != b2 } -> std::convertible_to<bool>;
      { b1 !=  a } -> std::convertible_to<bool>;
      {  a != b2 } -> std::convertible_to<bool>;
    };
   */

// https://en.cppreference.com/w/cpp/concepts/equality_comparable
template<class T, class U>
  concept bool __WeaklyEqualityComparableWith = // exposition only
    requires(const std::remove_reference_t<T>& t,
             const std::remove_reference_t<U>& u) {
      { t == u } -> bool; //std::boolean;
      { t != u } -> bool; //std::boolean;
      { u == t } -> bool; //std::boolean;
      { u != t } -> bool; //std::boolean;
    };
//

template < class T >
concept bool equality_comparable = __WeaklyEqualityComparableWith<T, T>;

//
//


// partial order only requires basic comparison >= and <=, but it's 
// not guaranteed that it's a total order... cannot guarantee it here just on operators
// One should guarantee that all elements are comparable... can we guarantee that?
//
// Total Order (for binary relation <=): antisymmetric, transitive, and connex relation
// antisymmetry: if a<=b and b<=a, then a=b
// transitive: if a<=b and b <= c, then a <= b
// connexity: a <= b or b <= a 

// IF WE REPLACE 'antisymmetry' with 'reflexity', we have a 'Total Preorder' (symbol below):
//  <
//  ~

// 'Total Preorder' is the complement of 'Strict Weak Order' (not used here: 'irreflexive', 'asymmetric' and 'transitive')
// Below we describe general 'Partial Orders'

// weak dominance is anti-symmetric
// weak dominance is transitive
// weak dominance DOESN'T have connexity, e.g., see (min,min): (1,5) and (2,4)

// Partial Order (for binary relation <=>): antisymmetric, transitive, PLUS (reflexive or irreflexive)
// if 'reflexive', a <=> a is always valid    (the typical '<=' syntax)
// if 'irreflexive', a <=> a is always false  (the typical '<' syntax)

// weak dominance is always 'reflexive'.


template <class T>
//concept bool totally_ordered =
//concept bool partially_ordered =
concept bool comparability =
  optframe::equality_comparable<T> &&
  requires(const std::remove_reference_t<T>& a,
           const std::remove_reference_t<T>& b) {
    { a <  b } -> bool;
    { a >  b } -> bool;
    { a <= b } -> bool;
    { a >= b } -> bool;
  };

// The 'comparability' name was 'totally_ordered' (as C++ proposal), but it's neither partial or total...
// one can easily return 'false' to all operators, meaning that these are incomparable. 
// Thus 'comparability' allows for both 'comparable' and 'incomparable' situations.
//
// for partial order, only 'weak partial' (<=) and 'strong partial' (<) would be necessary
// the rest can be defined in terms of < and <=.
// a < b
// a <= b
// a > b  === !(a <= b)
// a >= b === !(a < b)


// Total Order: a Partial Order which is valid for all possible objects (not verified here)
// Partial Order (requires being transitive, antisymmetric) may be two kinds: weak or strong.
// a Weak/Non-Strict Partial Order is ALWAYS valid to itself (reflexive), and a Strong/Strict Partial is NEVER (irreflexive).

// reference: https://en.cppreference.com/w/cpp/concepts/totally_ordered
//concept bool totally_ordered =
//  optframe::equality_comparable<T> &&
//  requires(const std::remove_reference_t<T>& a,
//           const std::remove_reference_t<T>& b) {
//    { a <  b } -> bool;
//    { a >  b } -> bool;
//    { a <= b } -> bool;
//    { a >= b } -> bool; //std::boolean;
//  };


// TODO: insert some 'zero' concept here..  (HasZero)
// 'abs' concept is also important to perform safe comparisons (HasAbs)
// basic arithmetic is currently just +, - and *.  (division was never required to this day)
// 'basic_arithmetics_assign' includes assignment operators +=, -= (weight scalar *= was dropped for now, too hard)
template <class T>
concept bool basic_arithmetics_assign = 
// optframe::totally_ordered<T> &&
//  optframe::comparable<T> &&
  //
//  requires(const std::remove_reference_t<T>& a,
  requires(std::remove_reference_t<T>& a,
           const std::remove_reference_t<T>& b) {
    { a += b } -> std::remove_reference_t<T>&;
    { a -= b } -> std::remove_reference_t<T>&;
    //{ a *= b } -> std::remove_reference_t<T>&; // useful, but too hard to do now
    ////{ a / b } -> std::remove_reference_t<T>&;  // NOT actually necessary (until today!)
  };


template <class T>
concept bool basic_arithmetics = 
  optframe::basic_arithmetics_assign<T> &&
  requires(const std::remove_reference_t<T>& a,
           const std::remove_reference_t<T>& b) {
    { a + b } -> std::remove_reference_t<T>; // requires const a
    { a - b } -> std::remove_reference_t<T>; // requires const a
    //{ a * b } -> std::remove_reference_t<T>; // useful, but too hard now... must provide multiplication by scalar to do 'weights'
    //{ a / b } -> std::remove_reference_t<T>;  // NOT actually necessary (until today!)
  };
  /*
  && 
  requires(std::remove_reference_t<T>& a) {
    { optframe::get_numeric_zero<T>() } -> std::remove_reference_t<T>&;
  };
  */

template <class T>
concept bool extended_arithmetics = 
  optframe::basic_arithmetics<T> &&
  requires(const std::remove_reference_t<T>& a,
           const std::remove_reference_t<T>& b) {
    { a * b } -> std::remove_reference_t<T>; // useful for weighted computation
    //{ a / b } -> std::remove_reference_t<T>;  // NOT actually necessary (until today!)
  };  


// capability to move to ostream&
template <class Self>
concept bool ostreamable = 
  requires(std::ostream& os, 
           const std::remove_reference_t<Self>& obj) {
    { os << obj } -> std::ostream&;
  };

} // namespace optframe

// ====================
// BEGIN compile tests 
// (disable with NDEBUG)
// ====================

#ifndef NDEBUG
// class that passes total order
class ConceptsTestClassTotalOrder
{
public:

   // consistency operator ("spaceship") only on gcc 10 (TODO: Ubuntu currently on gcc-7 and gcc-8)
   // auto operator<=>(const ConceptsTestClassTotalOrder& c) const

   bool operator==(const ConceptsTestClassTotalOrder& c) const
   {
      return true;
   }
   bool operator!=(const ConceptsTestClassTotalOrder& c) const
   {
      return !this->operator==(c);
   }
   bool operator>(const ConceptsTestClassTotalOrder& c) const
   {
      return true;
   }
   bool operator>=(const ConceptsTestClassTotalOrder& c) const
   {
      return true;
   }
   bool operator<(const ConceptsTestClassTotalOrder& c) const
   {
      return true;
   }
   bool operator<=(const ConceptsTestClassTotalOrder& c) const
   {
      return true;
   }
};

// testing total order for some random classes
template<optframe::comparability T>
struct MyConceptsTestTotalOrder
{
};

struct MyConceptsLocalTestBed
{
   MyConceptsTestTotalOrder<double> t_double;
   MyConceptsTestTotalOrder<int> t_int;
   MyConceptsTestTotalOrder<ConceptsTestClassTotalOrder> t_class;
   // practical test
   bool test()
   {
      ConceptsTestClassTotalOrder t_class1;
      ConceptsTestClassTotalOrder t_class2;
      return t_class1 < t_class2;
   }
};

// ========================================
// test if arithmetics compile (at least!)

class ConceptsTestClassArithmetics: public ConceptsTestClassTotalOrder
{
public:

   ConceptsTestClassArithmetics& operator+=(const ConceptsTestClassArithmetics& c)
   {
      return *this;
   }

   ConceptsTestClassArithmetics& operator-=(const ConceptsTestClassArithmetics& c)
   {
      return *this;
   }

   // multiplication by scalar (int, double, etc) is not very easy now, so dropping for a while
   //ConceptsTestClassArithmetics& operator*=(const ConceptsTestClassArithmetics& c)
   //{
   //   return *this;
   //}
   // division is not required!

   ConceptsTestClassArithmetics operator+(const ConceptsTestClassArithmetics& c) const
   {
      return ConceptsTestClassArithmetics(*this) += c;
   }

   ConceptsTestClassArithmetics operator-(const ConceptsTestClassArithmetics& c) const
   {
      return ConceptsTestClassArithmetics(*this) -= c;
   }

   // multiplication by scalar (int, double, etc) is not very easy now, so dropping for a while
   //ConceptsTestClassArithmetics operator*(const ConceptsTestClassArithmetics& c) const
   //{
   //   return ConceptsTestClassArithmetics(*this) *= c;
   //}
};

// testing total order for some random classes
template<optframe::basic_arithmetics T>
struct MyConceptsTestArithmetics
{
};

struct MyConceptsLocalTestBedArithmetics
{
   MyConceptsTestArithmetics<double> t_double;
   MyConceptsTestArithmetics<int> t_int;
   MyConceptsTestArithmetics<ConceptsTestClassArithmetics> t_class;
   // practical test
   ConceptsTestClassArithmetics test()
   {
      ConceptsTestClassArithmetics t_class1;
      ConceptsTestClassArithmetics t_class2;
      t_class1 += t_class2;       // one operation
      return t_class1 + t_class2; // another operation
   }
};

#endif // NDEBUG

#endif // MYCONCEPTS