// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_DOMAIN_HPP_
#define OPTFRAME_DOMAIN_HPP_

// Provides standard (named) domains for optframe. Notably defines:
// <XS>, <XES>, <XRKf64>, <XMESf64>, <X2ESf64> and <X2MESf64>

#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
//

#include "BaseConcepts.hpp"
#include "Component.hpp"
#include "Evaluation.hpp"

// for test only
#ifndef NDEBUG
#include "Helper/EPopulation.hpp"
#include "Helper/MultiESolution.hpp"
#include "BasicPareto.hpp" // for testing only!
#endif

namespace optframe {
class Domain {
 public:
  // check that double type is 64-bits
  // https://stackoverflow.com/questions/752309/ensuring-c-doubles-are-64-bits

  template <typename T>
  struct is_f32
      : std::bool_constant<std::is_same_v<T, float> && sizeof(T) == 4 &&
                           std::numeric_limits<T>::is_iec559> {};

  template <typename T>
  struct is_f64
      : std::bool_constant<std::is_same_v<T, double> && sizeof(T) == 8 &&
                           std::numeric_limits<T>::is_iec559> {};

  template <typename T>
  struct is_i64
      : std::bool_constant<std::is_same_v<T, int64_t> && sizeof(T) == 8> {};

  // =========================================
  // also checking that type is vector<double>
  // (standard random-keys implementation)

  template <typename>
  struct is_rkf64 : std::false_type {};

  template <typename T, typename Alloc>
  struct is_rkf64<std::vector<T, Alloc>>
      : std::bool_constant<std::is_same_v<T, double> && sizeof(double) == 8 &&
                           std::numeric_limits<double>::is_iec559> {};

  // ===========

  template <typename>
  struct is_rkf32 : std::false_type {};

  template <typename T, typename Alloc>
  struct is_rkf32<std::vector<T, Alloc>>
      : std::bool_constant<std::is_same_v<T, float> && sizeof(float) == 4 &&
                           std::numeric_limits<float>::is_iec559> {};

  // ==================================================================
  // using SFINAE void_t to allow failure on other non-XESolution types
  // REMEMBER: XES = (S,E)

  template <typename XES, typename = void>
  struct is_XESf32 : std::false_type {};

  template <typename XES>
  struct is_XESf32<XES, std::void_t<typename XES::second_type>>
      : std::bool_constant<XESolution<XES> &&
                           is_f32<typename XES::second_type::objType>::value> {
  };

  //
  template <typename XES, typename = void>
  struct is_XESf64 : std::false_type {};

  template <typename XES>
  struct is_XESf64<XES, std::void_t<typename XES::second_type>>
      : std::bool_constant<XESolution<XES> &&
                           is_f64<typename XES::second_type::objType>::value> {
  };

  //
  template <typename XES, typename = void>
  struct is_XESi64 : std::false_type {};

  template <typename XES>
  struct is_XESi64<XES, std::void_t<typename XES::second_type>>
      : std::bool_constant<XESolution<XES> &&
                           is_i64<typename XES::second_type::objType>::value> {
  };

  // ============================================
  // same SFINAE strategy here...
  // REMEMBER: X2ES = 2^XES

  template <typename X2ES, typename = void, typename = void>
  struct is_X2ESf64 : std::false_type {};

  template <typename X2ES>
  struct is_X2ESf64<X2ES,
                    std::void_t<typename X2ES::value_type>,  // for type
                                                             // deduction
                    std::void_t<typename X2ES::value_type::second_type>>
      :  // for type deduction
         std::bool_constant<
             X2ESolution<X2ES, typename X2ES::value_type> &&
             is_f64<typename X2ES::value_type::second_type::objType>::value> {};

  // ===========
  // REMEMBER: XMES = (S,[E])

  template <typename XMES, typename = void, typename = void>
  struct is_XMESf64 : std::false_type {};

  template <typename XMES>
  struct is_XMESf64<XMES, std::void_t<typename XMES::second_type>,
                    std::void_t<typename XMES::second_type::XEv>>
      : std::bool_constant<
            XESolution<XMES> &&
            is_f64<typename XMES::second_type::XEv::objType>::value> {};

  // ===========
  // REMEMBER: X2MES = 2^(S,[E])

  template <typename X2MES, typename = void, typename = void, typename = void>
  struct is_X2MESf64 : std::false_type {};

  template <typename X2MES>
  struct is_X2MESf64<X2MES, std::void_t<typename X2MES::value_type>,
                     std::void_t<typename X2MES::value_type::second_type>,
                     std::void_t<typename X2MES::value_type::second_type::XEv>>
      : std::bool_constant<
            X2ESolution<X2MES, typename X2MES::value_type> &&
            is_f64<
                typename X2MES::value_type::second_type::XEv::objType>::value> {
  };
  //

  template <typename X>
  constexpr static std::string_view getNamedDomain() {
    if constexpr (is_rkf64<X>::value) return "<XRKf64>";
    if constexpr (is_rkf32<X>::value)
      return "<XRKf32>";
    else if constexpr (is_XESf64<X>::value)
      return "<XESf64>";
    else if constexpr (is_XESf32<X>::value)
      return "<XESf32>";
    else if constexpr (is_XESi64<X>::value)
      return "<XESi64>";
    else if constexpr (is_X2ESf64<X>::value)
      return "<X2ESf64>";
    else if constexpr (is_XMESf64<X>::value)
      return "<XMESf64>";
    else if constexpr (is_X2MESf64<X>::value)
      return "<X2MESf64>";
    else if constexpr (XESolution<X>)
      return "<XES>";
    else if constexpr (XSolution<X>)
      return "<XS>";
    return "";
  }

  template <typename X>
  static std::string_view getAlternativeDomain(std::string_view defaultDom) {
    constexpr std::string_view named = getNamedDomain<X>();
    if (defaultDom == named)
      return "";
    else
      return named;
  }
};

#ifndef NDEBUG
static_assert(Domain::getNamedDomain<void*>() == std::string_view{"<XS>"});
static_assert(Domain::getNamedDomain<std::vector<float>>() ==
              std::string_view{"<XRKf32>"});
static_assert(Domain::getNamedDomain<std::vector<double>>() ==
              std::string_view{"<XRKf64>"});
static_assert(
    Domain::getNamedDomain<std::pair<void*, optframe::Evaluation<float>>>() ==
    std::string_view("<XESf32>"));
static_assert(
    Domain::getNamedDomain<std::pair<void*, optframe::Evaluation<double>>>() ==
    std::string_view("<XESf64>"));
using TestXES1 = std::pair<void*, optframe::Evaluation<>>;
static_assert(Domain::getNamedDomain<TestXES1>() ==
              std::string_view("<XESf64>"));
static_assert(
    Domain::getNamedDomain<std::pair<void*, optframe::Evaluation<int64_t>>>() ==
    std::string_view("<XESi64>"));
static_assert(
    Domain::getNamedDomain<std::pair<void*, optframe::Evaluation<int>>>() ==
    std::string_view("<XES>"));
static_assert(Domain::getNamedDomain<MultiESolution<TestXES1>>() ==
              std::string_view("<X2ESf64>"));
static_assert(Domain::getNamedDomain<EPopulation<TestXES1>>() ==
              std::string_view("<X2ESf64>"));
using EMSolution_Test1 = std::pair<void*, MultiEvaluation<double>>;
static_assert(Domain::getNamedDomain<EMSolution_Test1>() ==
              std::string_view("<XMESf64>"));
static_assert(Domain::getNamedDomain<BasicPareto<EMSolution_Test1>>() ==
              std::string_view("<X2MESf64>"));
#endif

}  // namespace optframe

#endif  // OPTFRAME_DOMAIN_HPP_
