#ifndef OPTFRAME_SINGLE_OBJ_VALUE_HPP
#define OPTFRAME_SINGLE_OBJ_VALUE_HPP

#include <math.h> /* fabs */

// no includes (amazing class!)
// not anymore :(
#include <OptFrame/Concepts/MyConcepts.hpp>

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
using evtype = EVALUATION_TYPE;
// Single Objective Value will use standard/default value
using SingleObjValue = evtype;

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

// template <optframe::basic_arithmetics T>
template <ConceptsBasicArithmetics T>
constexpr inline
    typename std::enable_if<std::is_same<T, int32_t>::value, int32_t>::type
    num_zero() {
  return 0;
}

template <ConceptsBasicArithmetics T>
constexpr inline
    typename std::enable_if<std::is_same<T, int64_t>::value, int64_t>::type
    num_zero() {
  return 0;
}

template <ConceptsBasicArithmetics T>
constexpr inline
    typename std::enable_if<std::is_same<T, float>::value, float>::type
    num_zero() {
  return 0.0;
}

// template <optframe::basic_arithmetics T>
template <ConceptsBasicArithmetics T>
constexpr inline
    typename std::enable_if<std::is_same<T, double>::value, double>::type
    num_zero() {
  return 0.0;
}

// ========== numeric_zero ============
// number zero for that arithmetic type

// stores numeric_zero value on 't'
// template <optframe::basic_arithmetics T>
template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, int32_t>::value, void>::type
numeric_zero(T& t) {
  t = 0;
}

template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, int64_t>::value, void>::type
numeric_zero(T& t) {
  t = 0;
}

template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, float>::value, void>::type
numeric_zero(T& t) {
  t = 0.0;
}

// stores numeric_zero value on 't'
// template <optframe::basic_arithmetics T>
template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, double>::value, void>::type
numeric_zero(T& t) {
  t = 0.0;
}

// =========== num_zero ============

// template <optframe::basic_arithmetics T>
template <ConceptsBasicArithmetics T>
constexpr inline
    typename std::enable_if<std::is_same<T, int32_t>::value, int32_t>::type
    num_zero_precision() {
  return 0;
}

template <ConceptsBasicArithmetics T>
constexpr inline
    typename std::enable_if<std::is_same<T, int64_t>::value, int64_t>::type
    num_zero_precision() {
  return 0;
}

// template <optframe::basic_arithmetics T>
template <ConceptsBasicArithmetics T>
constexpr inline
    typename std::enable_if<std::is_same<T, double>::value, double>::type
    num_zero_precision() {
  double p = 0.0001;
  return p;
}

template <ConceptsBasicArithmetics T>
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

// template <optframe::basic_arithmetics T>
template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, int32_t>::value, bool>::type
numeric_is_zero(const T& t) {
  return t == 0;
}

template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, int64_t>::value, bool>::type
numeric_is_zero(const T& t) {
  return t == 0;
}

// template <optframe::basic_arithmetics T>
template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, double>::value, bool>::type
numeric_is_zero(const T& t) {
  return ::fabs(t) <= num_zero_precision<T>();
}

template <ConceptsBasicArithmetics T>
inline typename std::enable_if<std::is_same<T, float>::value, bool>::type
numeric_is_zero(const T& t) {
  return ::fabs(t) <= num_zero_precision<T>();
}

}  // namespace optframe

#endif  // OPTFRAME_SINGLE_OBJ_VALUE_HPP
