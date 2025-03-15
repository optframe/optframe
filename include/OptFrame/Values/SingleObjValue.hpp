// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_SINGLE_OBJ_VALUE_HPP
#define OPTFRAME_SINGLE_OBJ_VALUE_HPP

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <math.h> /* fabs */

// no includes (amazing class!)
// not anymore :(
#include <OptFrame/Concepts/MyConcepts.hpp>

#define MOD_EXPORT
#else

// SORRY! EVEN IN MODULE! MOVE THIS TO optframe.values!
// #include <math.h> /* fabs */

// ERROR: post-module-declaration imports must not be from header inclusion
// import std;
// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

// only ugly thing left...
#ifndef EVALUATION_TYPE
#define EVALUATION_TYPE double
#endif

// finally deprecated this UGLY thing
/*
#ifndef EVALUATION_ZERO
#define EVALUATION_ZERO 0.0001 // TODO: should be part of numerical 'concept'
too #endif

#ifndef EVALUATION_ABS
#define EVALUATION_ABS ::fabs // TODO: should be part of numerical 'concept' too
#endif
*/

namespace optframe {
// default to many methods (TODO: unify on SingleObjValue or just leave as it
// is)
MOD_EXPORT using evtype = EVALUATION_TYPE;
// Single Objective Value will use standard/default value
MOD_EXPORT using SingleObjValue = evtype;

class NumericBasics {
 public:
  // globally friend classes

  // prototype for numeric_zero function: should return zero of that type
  // template<optframe::basic_arithmetics T>
  // friend T numeric_zero();

  // doing outside, for clarity!
};

// ======================
// FOUR IMPORTANT METHODS
// ======================
// num_zero           => returns a copy of the numeric zero
// numeric_zero(x)    => sets x as the numeric zero
// numeric_is_zero(x) => checks if x is zero
// num_zero_precision => returns a copy of the numeric zero precision

// =========== num_zero ============

MOD_EXPORT template <ConceptsBasicArithmetics T>
constexpr inline typename std::enable_if<std::is_same<T, std::int32_t>::value,
                                         std::int32_t>::type
num_zero() {
  return 0;
}

MOD_EXPORT template <ConceptsBasicArithmetics T>
constexpr inline typename std::enable_if<std::is_same<T, std::int64_t>::value,
                                         std::int64_t>::type
num_zero() {
  return 0;
}

MOD_EXPORT template <ConceptsBasicArithmetics T>
constexpr inline
    typename std::enable_if<std::is_same<T, float>::value, float>::type
    num_zero() {
  return 0.0;
}

MOD_EXPORT template <ConceptsBasicArithmetics T>
constexpr inline
    typename std::enable_if<std::is_same<T, double>::value, double>::type
    num_zero() {
  return 0.0;
}

// ========== numeric_zero ============
// number zero for that arithmetic type

// stores numeric_zero value on 't'
MOD_EXPORT template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, std::int32_t>::value, void>::type
numeric_zero(T& t) {
  t = 0;
}

MOD_EXPORT template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, std::int64_t>::value, void>::type
numeric_zero(T& t) {
  t = 0;
}

MOD_EXPORT template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, float>::value, void>::type
numeric_zero(T& t) {
  t = 0.0;
}

// stores numeric_zero value on 't'
MOD_EXPORT template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, double>::value, void>::type
numeric_zero(T& t) {
  t = 0.0;
}

// =========== num_zero ============

MOD_EXPORT template <ConceptsBasicArithmetics T>
constexpr inline typename std::enable_if<std::is_same<T, std::int32_t>::value,
                                         std::int32_t>::type
num_zero_precision() {
  return 0;
}

MOD_EXPORT template <ConceptsBasicArithmetics T>
constexpr inline typename std::enable_if<std::is_same<T, std::int64_t>::value,
                                         std::int64_t>::type
num_zero_precision() {
  return 0;
}

MOD_EXPORT template <ConceptsBasicArithmetics T>
constexpr inline
    typename std::enable_if<std::is_same<T, double>::value, double>::type
    num_zero_precision() {
  double p = 0.0001;
  return p;
}

MOD_EXPORT template <ConceptsBasicArithmetics T>
constexpr inline
    typename std::enable_if<std::is_same<T, float>::value, float>::type
    num_zero_precision() {
  float p = 0.0001;
  return p;
}

// ===============
// is_zero section
// ===============
// define limits where value of type is to considered zero

MOD_EXPORT template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, std::int32_t>::value, bool>::type
numeric_is_zero(const T& t) {
  return t == 0;
}

MOD_EXPORT template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, std::int64_t>::value, bool>::type
numeric_is_zero(const T& t) {
  return t == 0;
}

MOD_EXPORT template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, double>::value, bool>::type
numeric_is_zero(const T& t) {
  return std::fabs(t) <= num_zero_precision<T>();
}

MOD_EXPORT template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, float>::value, bool>::type
numeric_is_zero(const T& t) {
  return std::fabs(t) <= num_zero_precision<T>();
}

}  // namespace optframe

#endif  // OPTFRAME_SINGLE_OBJ_VALUE_HPP
