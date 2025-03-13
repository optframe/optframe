// SPDX-License-Identifier: MIT
// Copyright (C) 2023 - optional_view
// https://github.com/igormcoelho/optional_view

#ifndef OPVIEW_OPTIONAL_VIEW_HPP_  // NOLINT
#define OPVIEW_OPTIONAL_VIEW_HPP_

// #define OPTIONAL_VIEW_EXTENSIONS 1

// ====================================================================
// Optional View (optional_view::optional_view) - Likely C++11 or C++17 (for
// nullopt) Author: Igor M. Coelho MIT License (2023)
// ====================================================================

// What is an optional_view<T>:
// a non-owning reference to some existing type T, or empty (nullopt)
//
// This is useful in situations where you may pass or not some variable,
// typically resolved with T* (with or without some constness)
//
// Now I think that, std::optional<const T&> could solve the same problem,
// and optional references are typically crazy discussions... so, let's
// focus on optional_view, and give it proper semantics.
//
// This also prevents the costs of packing into std::optional when data is
// already loaded into any memory model, such as stack, heap, any smart
// pointer or even optional type (all abstracted away as T*)
// TODO: we could directly support conversions from safe types (uptr, sptr,),
// and avoid user to take pointer (and maybe even ban pointer interface here).
// Unsafe ref passing as T& is natural and should be kept.

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <optional>  // for std::nullopt

#define MOD_EXPORT
#else

import std;

// do NOT export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace opview {
//
MOD_EXPORT template <typename T>
class optional_view {
  using value_type = T;

 private:
  T* value;
  // OR... optional<T*>... BUT NO NEED OR THAT!

 public:
  optional_view() noexcept : value{nullptr} {}

  // do not accept pointer here
  // explicit optional_view(T* _value) : value{_value} {}

  // this is unsafe: but the risk is yours! (explicit or implicit)
  // NOLINTNEXTLINE
  optional_view(T& _value) : value{&_value} {}

  // cannot support rvalue due to non-ownership semantics
  // NOLINTNEXTLINE
  optional_view(T&& _value) = delete;

  // allow nullopt (explicit or implicit)
  // NOLINTNEXTLINE
  optional_view(std::nullopt_t data) : value{nullptr} {}

  // disallow nullptr
  // NOLINTNEXTLINE
  optional_view(std::nullptr_t data) = delete;

  // allow optional<T> for compatibility (explicit or implicit)
  // NOLINTNEXTLINE
  optional_view(std::optional<T>& op_data)
      : value{op_data ? &(*op_data) : nullptr} {}

  template <class X, typename = typename std::enable_if<
                         std::is_convertible<X*, T*>::value ||
                         std::is_same<X, T>::value>::type>
  optional_view(std::optional<X>& op_data)
      : value{op_data ? &(*op_data) : nullptr} {}

  // ===============================================

  // copy constructor
  optional_view(const optional_view<T>& other) : value{other.value} {}

  template <class X, typename = typename std::enable_if<
                         std::is_convertible<X*, T*>::value ||
                         std::is_same<X, T>::value>::type>
  optional_view(const optional_view<X>& other) : value{other.value} {}

  // disable move constructor
  optional_view(optional_view<T>&& other) = delete;

  ~optional_view() = default;

  // MUST delete all operator=
  // This is coherent to *_view behavior, and also prevent misleading issues
  // with possible rebind or not rebind... this is not needed on a view.
  optional_view<T>& operator=(const optional_view<T>&) = delete;

  optional_view<T>& operator=(optional_view<T>&&) = delete;

  // return raw pointer
  T* operator->() { return value; }

  // return raw pointer
  const T* operator->() const { return value; }

  // return dereferenced shared object
  T& operator*() { return *value; }

  // return dereferenced shared object
  const T& operator*() const { return *value; }

  // return dereferenced shared object
  T& get() { return *value; }

  // return dereferenced shared object
  const T& get() const { return *value; }

  // return dereferenced shared object
  operator T&() { return *value; }

  bool empty() const { return !(value); }

  // has some view?
  operator bool() { return (bool)value; }

#ifdef OPTIONAL_VIEW_EXTENSIONS
  void reset() noexcept { value = nullptr; }
#endif
};

MOD_EXPORT template <typename T>
using const_optional_view = optional_view<const T>;

}  // namespace opview

#endif  // OPVIEW_OPTIONAL_VIEW_HPP_ // NOLINT
