/*
github.com/optframe/nnshared
MIT License
commit 3c95c006eabed95da29ac0be4476f77c5af96934 (HEAD -> master, origin/master,
origin/HEAD) Author: Igor Machado <igor.machado@gmail.com> Date:   Thu Jun 17
23:40:11 2021 -0300 differences from gsl::not_null
*/

//
//#include <gsl/gsl> // nnptr (local copy)
#include <memory> // shared_ptr

// ====================================================================
// Not Null Shared Pointer (nn::shared)
// Authors: Igor M. Coelho & Fellipe Pessanha
// MIT License (2021)
// ====================================================================

// This library depends on...

// ====================================================================
// nnptr: an alternative to gsl::nnptr, but with NDEBUG removing all
// 'nnptr' during Release
// ====================================================================

// =============
// For nn::nnptr
// =============

#include <algorithm>    // for forward
#include <iosfwd>       // for ptrdiff_t, nullptr_t, ostream, size_t
#include <memory>       // for shared_ptr, unique_ptr
#include <system_error> // for hash
#include <type_traits>  // for enable_if_t, is_convertible, is_assignable

// ======================
// begin nn::nnptr part
// ======================

// =============================================
// Same assumptions are valid from gsl::nnptr
// =============================================
//
// nn::nnptr restricts a pointer to only hold non-null values (enforced during
// Debug time, not Release)
//
// Has zero size overhead over T (during both Debug and Release)
//
// For nnptr<T>, if T is a pointer (i.e. T == U*) then
// - allow construction from U*
// - disallow construction from nullptr_t
// - disallow default construction
// - ensure construction from null U* fails
// - allow implicit conversion to U*
//

// ==============================================
// code for nnptr (compatible with gsl::nnptr)
// reference:
// https://github.com/microsoft/GSL/blob/b26f6d5ec7b043f9d459c1dfdd6da4d930d4e9b4/include/gsl/pointers
// date: 17-jun-2021
// ==============================================

namespace nn {

namespace details {
template <typename T, typename = void>
struct is_comparable_to_nullptr : std::false_type {};

template <typename T>
struct is_comparable_to_nullptr<
    T, std::enable_if_t<std::is_convertible<
           decltype(std::declval<T>() != nullptr), bool>::value>>
    : std::true_type {};
} // namespace details

template <class T> class nnptr {
public:
  static_assert(details::is_comparable_to_nullptr<T>::value,
                "T cannot be compared to nullptr.");

  template <typename U,
            typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  constexpr nnptr(U &&u) : ptr_(std::forward<U>(u)) {
#ifndef NDEBUG
    if (ptr_ == nullptr)
      std::terminate();
#endif
  }

  template <
      typename = std::enable_if_t<!std::is_same<std::nullptr_t, T>::value>>
  constexpr nnptr(T u) : ptr_(std::move(u)) {
#ifndef NDEBUG
    if (ptr_ == nullptr)
      std::terminate();
#endif
  }

  template <typename U,
            typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  constexpr nnptr(const nnptr<U> &other) : nnptr(other.get()) {}

  nnptr(const nnptr &other) = default;
  nnptr &operator=(const nnptr &other) = default;
  constexpr std::conditional_t<std::is_copy_constructible<T>::value, T,
                               const T &>
  get() const {
#ifndef NDEBUG
    if (ptr_ == nullptr)
      std::terminate();
#endif
    return ptr_;
  }

  constexpr operator T() const { return get(); }
  constexpr decltype(auto) operator->() const { return get(); }
  constexpr decltype(auto) operator*() const { return *get(); }

  // prevents compilation when someone attempts to assign a null pointer
  // constant
  nnptr(std::nullptr_t) = delete;
  nnptr &operator=(std::nullptr_t) = delete;

  // unwanted operators...pointers only point to single objects!
  nnptr &operator++() = delete;
  nnptr &operator--() = delete;
  nnptr operator++(int) = delete;
  nnptr operator--(int) = delete;
  nnptr &operator+=(std::ptrdiff_t) = delete;
  nnptr &operator-=(std::ptrdiff_t) = delete;
  void operator[](std::ptrdiff_t) const = delete;

private:
  T ptr_;
};

template <class T> auto make_nnptr(T &&t) noexcept {
  return nnptr<std::remove_cv_t<std::remove_reference_t<T>>>{
      std::forward<T>(t)};
}

template <class T>
std::ostream &operator<<(std::ostream &os, const nnptr<T> &val) {
  os << val.get();
  return os;
}

template <class T, class U>
auto operator==(const nnptr<T> &lhs,
                const nnptr<U> &rhs) noexcept(noexcept(lhs.get() == rhs.get()))
    -> decltype(lhs.get() == rhs.get()) {
  return lhs.get() == rhs.get();
}
// should prevent comparisons with nullptr_t (from igor)
template <class T>
bool operator==(const nnptr<T> &lhs, std::nullptr_t nptr) = delete;

template <class T, class U>
auto operator!=(const nnptr<T> &lhs,
                const nnptr<U> &rhs) noexcept(noexcept(lhs.get() != rhs.get()))
    -> decltype(lhs.get() != rhs.get()) {
  return lhs.get() != rhs.get();
}
// should prevent comparisons with nullptr_t (from igor)
template <class T>
bool operator!=(const nnptr<T> &lhs, std::nullptr_t nptr) = delete;

template <class T, class U>
auto operator<(const nnptr<T> &lhs,
               const nnptr<U> &rhs) noexcept(noexcept(lhs.get() < rhs.get()))
    -> decltype(lhs.get() < rhs.get()) {
  return lhs.get() < rhs.get();
}

template <class T, class U>
auto operator<=(const nnptr<T> &lhs,
                const nnptr<U> &rhs) noexcept(noexcept(lhs.get() <= rhs.get()))
    -> decltype(lhs.get() <= rhs.get()) {
  return lhs.get() <= rhs.get();
}

template <class T, class U>
auto operator>(const nnptr<T> &lhs,
               const nnptr<U> &rhs) noexcept(noexcept(lhs.get() > rhs.get()))
    -> decltype(lhs.get() > rhs.get()) {
  return lhs.get() > rhs.get();
}

template <class T, class U>
auto operator>=(const nnptr<T> &lhs,
                const nnptr<U> &rhs) noexcept(noexcept(lhs.get() >= rhs.get()))
    -> decltype(lhs.get() >= rhs.get()) {
  return lhs.get() >= rhs.get();
}

// more unwanted operators
template <class T, class U>
std::ptrdiff_t operator-(const nnptr<T> &, const nnptr<U> &) = delete;
template <class T>
nnptr<T> operator-(const nnptr<T> &, std::ptrdiff_t) = delete;
template <class T>
nnptr<T> operator+(const nnptr<T> &, std::ptrdiff_t) = delete;
template <class T>
nnptr<T> operator+(std::ptrdiff_t, const nnptr<T> &) = delete;

} // namespace nn

namespace std {
template <class T> struct hash<nn::nnptr<T>> {
  std::size_t operator()(const nn::nnptr<T> &value) const {
    return hash<T>{}(value.get());
  }
};
} // namespace std

// ======================
// begin nn::shared part
// ======================

namespace nn {
//
template <typename T> class shared {
  using shared_type = T;

public:
  nnptr<std::shared_ptr<T>> data_;

  // this constructor can be used to "move" into new shared_ptr versions
  // this requires a copy constructor over the type X (must be concrete)
  template <
      class X,
      typename =
          typename std::enable_if<std::is_convertible<X *, T *>::value>::type,
      typename =
          typename std::enable_if<std::is_copy_constructible<X>::value>::type>
  shared(X &&other) : data_{std::shared_ptr<T>{new X(other)}} {}

  // this is for existing references (must have copy constructor)
  template <
      class X,
      typename =
          typename std::enable_if<std::is_convertible<X *, T *>::value>::type,
      typename =
          typename std::enable_if<std::is_copy_constructible<X>::value>::type>
  shared(const X &other) : data_{std::shared_ptr<T>{new X(other)}} {}

  shared(const shared<T> &other) : data_{other.data_} {}

  shared(const shared<T> &&corpse) : data_{corpse.data_} {}

  shared(std::shared_ptr<T> &data) : data_{data} {}

  shared(T *data) : data_{std::shared_ptr<T>{data}} {}

  // disallow explicit nullptr
  shared(std::nullptr_t data) = delete;

  T *operator->() { return data_.get().get(); }

  T &operator*() {
    // return dereferenced shared object
    return *data_;
  }

  T &get() {
    // return dereferenced shared object
    return *data_;
  }

  operator T &() {
    // return dereferenced shared object
    return *data_;
  }

  template <class Y, typename = typename std::enable_if<
                         std::is_convertible<T *, Y *>::value>::type>
  operator shared<Y>() // explicit? not necessary... (until now)
  {
    std::shared_ptr<Y> py = data_.get(); // remove encapsulation from 'nnptr'
    return py;
  }
};

} // namespace nn
