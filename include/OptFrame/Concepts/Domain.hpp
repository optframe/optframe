// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_DOMAIN_HPP_
#define OPTFRAME_DOMAIN_HPP_

// Provides standard (named) domains for optframe. Notably defines:
// <XS>, <XES>, <XRKf64>, <XMESf64>, <X2ESf64> and <X2MESf64>

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
// important stuff
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>

#define MOD_EXPORT
#else

// Cannot IMPORT here... already done on .cppm of 'concepts'
// import std;

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {
MOD_EXPORT class Domain {
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
  struct is_i32
      : std::bool_constant<std::is_same_v<T, std::int32_t> && sizeof(T) == 4> {
  };

  template <typename T>
  struct is_i64
      : std::bool_constant<std::is_same_v<T, std::int64_t> && sizeof(T) == 8> {
  };

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
      : std::bool_constant<
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
            XESolution<XES> &&
#endif
            is_f32<typename XES::second_type::objType>::value> {
  };

  //
  template <typename XES, typename = void>
  struct is_XESf64 : std::false_type {};

  template <typename XES>
  struct is_XESf64<XES, std::void_t<typename XES::second_type>>
      : std::bool_constant<
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
            XESolution<XES> &&
#endif
            is_f64<typename XES::second_type::objType>::value> {
  };

  //
  template <typename XES, typename = void>
  struct is_XESi64 : std::false_type {};

  template <typename XES>
  struct is_XESi64<XES, std::void_t<typename XES::second_type>>
      : std::bool_constant<
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
            XESolution<XES> &&
#endif
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
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
             X2ESolution<X2ES, typename X2ES::value_type> &&
#endif
             is_f64<typename X2ES::value_type::second_type::objType>::value> {
  };

  // ===========
  // REMEMBER: XMES = (S,[E])

  template <typename XMES, typename = void, typename = void>
  struct is_XMESf64 : std::false_type {};

  template <typename XMES>
  struct is_XMESf64<XMES, std::void_t<typename XMES::second_type>,
                    std::void_t<typename XMES::second_type::XEv>>
      : std::bool_constant<
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
            XESolution<XMES> &&
#endif
            is_f64<typename XMES::second_type::XEv::objType>::value> {
  };

  // for i32 multievaluation

  template <typename XMES, typename = void, typename = void>
  struct is_XMESi32 : std::false_type {};

  template <typename XMES>
  struct is_XMESi32<XMES, std::void_t<typename XMES::second_type>,
                    std::void_t<typename XMES::second_type::XEv>>
      : std::bool_constant<
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
            XESolution<XMES> &&
#endif
            is_i32<typename XMES::second_type::XEv::objType>::value> {
  };

  // ===========
  // REMEMBER: X2MES = 2^(S,[E])

  template <typename X2MES, typename = void, typename = void, typename = void>
  struct is_X2MESf64 : std::false_type {};

  template <typename X2MES>
  struct is_X2MESf64<X2MES, std::void_t<typename X2MES::value_type>,
                     std::void_t<typename X2MES::value_type::second_type>,
                     std::void_t<typename X2MES::value_type::second_type::XEv>>
      : std::bool_constant<
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
            X2ESolution<X2MES, typename X2MES::value_type> &&
#endif
            is_f64<
                typename X2MES::value_type::second_type::XEv::objType>::value> {
  };
  //

  // ==================================================================
  // using SFINAE void_t to allow failure on other non-XESolution types
  // REMEMBER: X2S = 2^S

  template <typename X2S, typename = void>
  struct is_X2S : std::false_type {};

  template <typename X2S>
  struct is_X2S<X2S, std::void_t<typename X2S::value_type>>
      : std::bool_constant<
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
            X2Solution<X2S, typename X2S::value_type>
#else
            true
#endif
            > {
  };
  //
  // =========================================
  // RK-specific with population

  // XRKf64Ef64 = (RKf64, Ef64)
  template <typename XES, typename = void>
  struct is_XRKf64Ef64 : std::false_type {};

  template <typename XES>
  struct is_XRKf64Ef64<XES, std::void_t<typename XES::second_type>>
      : std::bool_constant<
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
            XESolution<XES> &&
#endif
            is_rkf64<typename XES::first_type>::value &&
            is_f64<typename XES::second_type::objType>::value> {
  };

  // XRKf64Ei32 = (RKf64, Ei32)
  template <typename XES, typename = void>
  struct is_XRKf64Ei32 : std::false_type {};

  template <typename XES>
  struct is_XRKf64Ei32<XES, std::void_t<typename XES::second_type>>
      : std::bool_constant<
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
            XESolution<XES> &&
#endif
            is_rkf64<typename XES::first_type>::value &&
            is_i32<typename XES::second_type::objType>::value> {
  };

  // XRKf64EMi32 = (RKf64, [Ei32])  [aka, MultiEvaluation<int>]
  template <typename XES, typename = void>
  struct is_XRKf64EMi32 : std::false_type {};

  template <typename XES>
  struct is_XRKf64EMi32<XES, std::void_t<typename XES::second_type>>
      : std::bool_constant<
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
            XESolution<XES> &&
#endif
            is_rkf64<typename XES::first_type>::value &&
            is_XMESi32<XES>::value> {
  };

  // X2RKf64Ef64 = 2^(RKf64, Ef64)

  template <typename X2ES, typename = void, typename = void>
  struct is_X2RKf64Ef64 : std::false_type {};

  template <typename X2ES>
  struct is_X2RKf64Ef64<X2ES,
                        std::void_t<typename X2ES::value_type>,  // for type
                                                                 // deduction
                        std::void_t<typename X2ES::value_type::second_type>>
      :  // for type deduction
         std::bool_constant<
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
             X2ESolution<X2ES, typename X2ES::value_type> &&
#endif
             is_XRKf64Ef64<typename X2ES::value_type>::value> {
  };

  // X2RKf64Ei32 = 2^(RKf64, Ei32)

  template <typename X2ES, typename = void, typename = void>
  struct is_X2RKf64Ei32 : std::false_type {};

  template <typename X2ES>
  struct is_X2RKf64Ei32<X2ES,
                        std::void_t<typename X2ES::value_type>,  // for type
                                                                 // deduction
                        std::void_t<typename X2ES::value_type::second_type>>
      :  // for type deduction
         std::bool_constant<
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
             X2ESolution<X2ES, typename X2ES::value_type> &&
#endif
             is_XRKf64Ei32<typename X2ES::value_type>::value> {
  };

  // X2RKf64EMi32 = 2^(RKf64, [Ei32])

  template <typename X2ES, typename = void, typename = void>
  struct is_X2RKf64EMi32 : std::false_type {};

  template <typename X2ES>
  struct is_X2RKf64EMi32<X2ES,
                         std::void_t<typename X2ES::value_type>,  // for type
                                                                  // deduction
                         std::void_t<typename X2ES::value_type::second_type>>
      :  // for type deduction
         std::bool_constant<
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
             X2ESolution<X2ES, typename X2ES::value_type> &&
#endif
             is_XRKf64EMi32<typename X2ES::value_type>::value> {
  };

  // ===============

  template <typename X>
  constexpr static std::string_view getNamedDomain() {
    if constexpr (is_rkf64<X>::value) return "<XRKf64>";
    if constexpr (is_rkf32<X>::value)
      return "<XRKf32>";
    else if constexpr (is_XRKf64Ef64<X>::value)
      return "<XRKf64Ef64>";
    else if constexpr (is_X2RKf64Ef64<X>::value)
      return "<X2RKf64Ef64>";
    else if constexpr (is_XRKf64Ei32<X>::value)
      return "<XRKf64Ei32>";
    else if constexpr (is_X2RKf64Ei32<X>::value)
      return "<X2RKf64Ei32>";
    else if constexpr (is_XRKf64EMi32<X>::value)
      return "<XRKf64EMi32>";
    else if constexpr (is_X2RKf64EMi32<X>::value)
      return "<X2RKf64EMi32>";
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
    else if constexpr (is_XMESi32<X>::value)
      return "<XMESi32>";
    else if constexpr (is_X2MESf64<X>::value)
      return "<X2MESf64>";

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
    else if constexpr (XESolution<X>)
      return "<XES>";
    else if constexpr (is_X2S<X>::value)
      return "<X2S>";
    else if constexpr (XSolution<X>)
      return "<XS>";
    return "";
#else
    return "<X?>";
#endif
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

}  // namespace optframe

#endif  // OPTFRAME_DOMAIN_HPP_
