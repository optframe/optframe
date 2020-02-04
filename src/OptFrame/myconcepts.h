#ifndef OPTFRAME_MYCONCEPTS
#define OPTFRAME_MYCONCEPTS

// general concepts expected to exist on c++20

namespace optframe
{

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

// reference: https://en.cppreference.com/w/cpp/concepts/totally_ordered
template <class T>
concept bool totally_ordered =
  optframe::equality_comparable<T> &&
  requires(const std::remove_reference_t<T>& a,
           const std::remove_reference_t<T>& b) {
    { a <  b } -> bool;
    { a >  b } -> bool;
    { a <= b } -> bool;
    { a >= b } -> bool; //std::boolean;
  };


// TODO: insert some 'zero' concept here..  (HasZero)
// 'abs' concept is also important to perform safe comparisons (HasAbs)
// basic arithmetic is currently just +, - and *.  (division was never required to this day)
// will require totally_ordered concept because everything (we know) depends on it
// 'basic_arithmetics_assign' includes assignment operators +=, -= (weight scalar *= was dropped for now, too hard)
template <class T>
concept bool basic_arithmetics_assign = 
  optframe::totally_ordered<T> &&
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
template<optframe::totally_ordered T>
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