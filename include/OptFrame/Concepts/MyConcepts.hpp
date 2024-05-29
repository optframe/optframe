// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_CONCEPTS_MYCONCEPTS_HPP_
#define OPTFRAME_CONCEPTS_MYCONCEPTS_HPP_

// general concepts expected to exist on c++20

#include <memory>    // unique_ptr
#include <optional>  // optional
#include <vector>    // for vsref

// "not null shared pointer" library (local copy!)
#include <OptFrame/nnptr/nnshared.hpp>
// "optional_view" library (local copy!)
#include <OptFrame/opview/optional_view.hpp>

// Check if C++20 Concepts is supported
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)

#if __cplusplus <= 201703L  // after c++20, #include<concepts>
// no concepts library, yet...
#define AUTO_CONCEPTS
#else

#include <concepts>
#define AUTO_CONCEPTS auto
#endif

#endif  // cpp_concepts

// =========== OUTSIDE optframe scope (to make error tracking easier...)
// ========== "not null shared pointer" shortcuts
template <class R>
using sref = nnptr::NNShared<R>;
// NOLINTNEXTLINE
#define sref_copy(X) nnptr::copy(X)
//
template <class R>
using vsref = std::vector<sref<R>>;
//
template <class R>
using sptr = std::shared_ptr<R>;
//
template <class R>
using vsptr = std::vector<sptr<R>>;
//
template <class R>
using op_view = opview::optional_view<R>;
//
template <class R>
using cop_view = opview::const_optional_view<R>;

typedef void OPTFRAME_DEFAULT_PROBLEM;

namespace optframe {

// helper: remove_ref = std::remove_reference_t
template <class Self>
using remove_ref = std::remove_reference_t<Self>;

// notes: on c++17, we have 'bool' on concept definition, but not on c++20
// warning: the ‘bool’ keyword is not allowed in a C++20 concept definition
// use macro __cplusplus == 201703L to verify that

// #define BOOL_OR_EMPTY(b) (__cplusplus <= 201703L ? bool :  std::cout <<
// "C++17\n";
//    else if (__cplusplus == 201402L) std::cout << "C++14\n";)

// default renaming of std::unique_ptr (too long!!)
template <class T>
using uptr = std::unique_ptr<T>;

// default renaming of std::observer_ptr (too long!!)
// template<class T>
// using optr = std::observer_ptr<T>;

// default renaming of std::optional (too long!!)
template <class T>
using op = std::optional<T>;

// id type for move id, neighborhood id, etc.
using id_type = std::size_t;

// ================ BEGIN CONCEPTS ================
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)

// https://en.cppreference.com/w/cpp/concepts/boolean

template <class Me, class Other>
concept my_same_as = std::is_same_v<Me, Other> && std::is_same_v<Other, Me>;

// https://en.cppreference.com/w/cpp/concepts/convertible_to
/*
  template <class From, class To>
concept convertible_to =
  std::is_convertible_v<From, To> &&
  requires(std::add_rvalue_reference_t<From> (&f)()) {
    static_cast<To>(f());
  };
  */
template <class From, class To>
concept my_convertible_to = std::is_convertible_v<From, To> &&
    requires(std::add_rvalue_reference_t<From> (&f)()) {
  static_cast<To>(f());
};

// https://en.cppreference.com/w/cpp/concepts/equality_comparable
template <class T, class U>
concept __WeaklyEqualityComparableWith =  // exposition only
    requires(const std::remove_reference_t<T>& t,
             const std::remove_reference_t<U>& u) {
  { t == u } -> my_same_as<bool>;  // std::boolean;
  { t != u } -> my_same_as<bool>;  // std::boolean;
  { u == t } -> my_same_as<bool>;  // std::boolean;
  { u != t } -> my_same_as<bool>;  // std::boolean;
};
//

template <class T>
concept equality_comparable = __WeaklyEqualityComparableWith<T, T>;

//
//

// partial order only requires basic comparison >= and <=, but it's
// not guaranteed that it's a total order... cannot guarantee it here just on
// operators One should guarantee that all elements are comparable... can we
// guarantee that?
//
// Total Order (for binary relation <=): antisymmetric, transitive, and connex
// relation antisymmetry: if a<=b and b<=a, then a=b transitive: if a<=b and b
// <= c, then a <= b connexity: a <= b or b <= a

// IF WE REPLACE 'antisymmetry' with 'reflexity', we have a 'Total Preorder'
// (symbol below):
//  <
//  ~

// 'Total Preorder' is the complement of 'Strict Weak Order' (not used here:
// 'irreflexive', 'asymmetric' and 'transitive') Below we describe general
// 'Partial Orders'

// weak dominance is anti-symmetric
// weak dominance is transitive
// weak dominance DOESN'T have connexity, e.g., see (min,min): (1,5) and (2,4)

// Partial Order (for binary relation <=>): antisymmetric, transitive, PLUS
// (reflexive or irreflexive) if 'reflexive', a <=> a is always valid    (the
// typical '<=' syntax) if 'irreflexive', a <=> a is always false  (the typical
// '<' syntax)

// weak dominance is always 'reflexive'.

template <class T>
concept comparability = optframe::equality_comparable<T> &&
    requires(const std::remove_reference_t<T>& a,
             const std::remove_reference_t<T>& b) {
  { a < b } -> my_same_as<bool>;
  { a > b } -> my_same_as<bool>;
  { a <= b } -> my_same_as<bool>;
  { a >= b } -> my_same_as<bool>;
};

// The 'comparability' name was 'totally_ordered' (as C++ proposal), but it's
// neither partial or total... one can easily return 'false' to all operators,
// meaning that these are incomparable. Thus 'comparability' allows for both
// 'comparable' and 'incomparable' situations.
//
// for partial order, only 'weak partial' (<=) and 'strong partial' (<) would be
// necessary the rest can be defined in terms of < and <=. a < b a <= b a > b
// === !(a <= b) a >= b === !(a < b)

// Total Order: a Partial Order which is valid for all possible objects (not
// verified here) Partial Order (requires being transitive, antisymmetric) may
// be two kinds: weak or strong. a Weak/Non-Strict Partial Order is ALWAYS valid
// to itself (reflexive), and a Strong/Strict Partial is NEVER (irreflexive).

// reference: https://en.cppreference.com/w/cpp/concepts/totally_ordered
// concept bool totally_ordered =
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
// basic arithmetic is currently just +, - and *.  (division was never required
// to this day) 'basic_arithmetics_assign' includes assignment operators +=, -=
// (weight scalar *= was dropped for now, too hard)

template <class T>
concept basic_arithmetics_assign = requires(
    std::remove_reference_t<T>& a, const std::remove_reference_t<T>& b) {
  { a += b } -> my_same_as<T>;
  { a -= b } -> my_same_as<T>;
}
|| requires(std::remove_reference_t<T>& a,
            const std::remove_reference_t<T>& b) {
  { a += b } -> my_same_as<T&>;
  { a -= b } -> my_same_as<T&>;
};

template <class T>
concept basic_arithmetics = optframe::basic_arithmetics_assign<T> &&
    requires(const std::remove_reference_t<T>& a,
             const std::remove_reference_t<T>& b) {
  // { a + b } -> my_same_as<T>;
  { a + b } -> std::convertible_to<T>;
  // { a - b } -> my_same_as<T>;
  { a - b } -> std::convertible_to<T>;
};

template <class T>
concept extended_arithmetics = optframe::basic_arithmetics<T> &&
    requires(const std::remove_reference_t<T>& a,
             const std::remove_reference_t<T>& b) {
  {
    a* b
    } -> my_same_as<T>;  // std::remove_reference_t<T>; // useful for weighted
                         // computation
                         //{ a / b } -> std::remove_reference_t<T>;  // NOT
                         // actually necessary (until
                         // today!)
};

// capability to move to ostream&
template <class Self>
concept ostreamable = requires(std::ostream& os,
                               // const std::remove_reference_t<Self>& obj) {
                               const Self& obj) {
  {os << obj};
};

#endif  // cpp_concepts

}  // namespace optframe

// ====================
// BEGIN compile tests
// (disable with NDEBUG)
// ====================

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)

#ifndef NDEBUG
// class that passes total order
class ConceptsTestClassTotalOrder {
 public:
  // consistency operator ("spaceship") only on gcc 10 (TODO: Ubuntu currently
  // on gcc-7 and gcc-8) auto operator<=>(const ConceptsTestClassTotalOrder& c)
  // const

  bool operator==(const ConceptsTestClassTotalOrder& c) const { return true; }
  bool operator!=(const ConceptsTestClassTotalOrder& c) const {
    return !this->operator==(c);
  }
  bool operator>(const ConceptsTestClassTotalOrder& c) const { return true; }
  bool operator>=(const ConceptsTestClassTotalOrder& c) const { return true; }
  bool operator<(const ConceptsTestClassTotalOrder& c) const { return true; }
  bool operator<=(const ConceptsTestClassTotalOrder& c) const { return true; }
};

// testing total order for some random classes
template <optframe::comparability T>
struct MyConceptsTestTotalOrder {};

struct MyConceptsLocalTestBed {
  MyConceptsTestTotalOrder<double> t_double;
  MyConceptsTestTotalOrder<int> t_int;
  MyConceptsTestTotalOrder<ConceptsTestClassTotalOrder> t_class;
  // practical test
  bool test() {
    ConceptsTestClassTotalOrder t_class1;
    ConceptsTestClassTotalOrder t_class2;
    return t_class1 < t_class2;
  }
};

// ========================================
// test if arithmetics compile (at least!)

class ConceptsTestClassArithmetics : public ConceptsTestClassTotalOrder {
 public:
  ConceptsTestClassArithmetics& operator+=(
      const ConceptsTestClassArithmetics& c) {
    return *this;
  }

  ConceptsTestClassArithmetics& operator-=(
      const ConceptsTestClassArithmetics& c) {
    return *this;
  }

  // multiplication by scalar (int, double, etc) is not very easy now, so
  // dropping for a while
  // ConceptsTestClassArithmetics& operator*=(const
  // ConceptsTestClassArithmetics& c)
  //{
  //   return *this;
  //}
  // division is not required!

  ConceptsTestClassArithmetics operator+(
      const ConceptsTestClassArithmetics& c) const {
    return ConceptsTestClassArithmetics(*this) += c;
  }

  ConceptsTestClassArithmetics operator-(
      const ConceptsTestClassArithmetics& c) const {
    return ConceptsTestClassArithmetics(*this) -= c;
  }

  // multiplication by scalar (int, double, etc) is not very easy now, so
  // dropping for a while
  // ConceptsTestClassArithmetics operator*(const ConceptsTestClassArithmetics&
  // c) const
  //{
  //   return ConceptsTestClassArithmetics(*this) *= c;
  //}
};

// testing total order for some random classes
template <optframe::basic_arithmetics T>
struct MyConceptsTestArithmetics {};

struct MyConceptsLocalTestBedArithmetics {
  MyConceptsTestArithmetics<double> t_double;
  MyConceptsTestArithmetics<int> t_int;
  MyConceptsTestArithmetics<ConceptsTestClassArithmetics> t_class;
  // practical test
  ConceptsTestClassArithmetics test() {
    ConceptsTestClassArithmetics t_class1;
    ConceptsTestClassArithmetics t_class2;
    t_class1 += t_class2;        // one operation
    return t_class1 + t_class2;  // another operation
  }
};

#endif  // NDEBUG

#define ConceptsBasicArithmetics optframe::basic_arithmetics
#define ConceptsComparability optframe::comparability

#else  // NO CONCEPTS

// disable!
#define ConceptsBasicArithmetics typename
#define ConceptsComparability typename

#endif  // cpp_concepts

#endif  // OPTFRAME_CONCEPTS_MYCONCEPTS_HPP_
