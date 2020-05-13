// Generator.hpp: coroutines support on OptFrame FCore library
// This is a very special (and experimental) class. We plan to drop it,
//    as soon as we have an "official" replacement.
//
// It was adapted from 'roger-dv' from: https://github.com/roger-dv/cpp20-coro-Generator
// According to @roger-dv, it was:
//   - created by rogerv on 9/29/19
//   - based on example code (but with significant cleanup) found in:
//       * Rainer Grimm, Concurrency with Modern C++ (Leanpub, 2017 - 2019), 207-209.
//
// From our side, we needed to fix some "experimental" stuff, on 12/may/2020,
//   and also adjust naming standards for FCore
// This works fine on gcc-10.1, using -fcoroutines flag and --std=c++20
//
#ifndef OPTFCORE_CORO_GENERATOR_HPP
#define OPTFCORE_CORO_GENERATOR_HPP

#include <coroutine> // works on c++20/gcc-10.1
#include <iostream>
#include <memory>

//
namespace optframe {

template<typename T>
class Generator final
{
public:
   struct promise_type; // forward declaration
   using handle_type = std::coroutine_handle<promise_type>;

private:
   handle_type coro;

public:
   explicit Generator(handle_type h)
     : coro(h)
   {
   }

   Generator(const Generator&) = delete;

   Generator(Generator&& oth) noexcept
     : coro(oth.coro)
   {
      oth.coro = nullptr;
   }

   Generator& operator=(const Generator&) = delete;

   Generator& operator=(Generator&& other) noexcept
   {
      coro = other.coro;
      other.coro = nullptr;
      return *this;
   }

   ~Generator()
   {
      if (coro) {
         coro.destroy();
      }
   }

   bool next()
   {
      coro.resume();
      return not coro.done();
   }

   T getValue()
   {
      return coro.promise().current_value;
   }

   // ============================
   // internal class: promise_type
   // ============================

   struct promise_type
   {
   private:
      T current_value{};
      friend class Generator;

   public:
      promise_type() = default;
      ~promise_type() = default;
      promise_type(const promise_type&) = delete;
      promise_type(promise_type&&) = delete;
      promise_type& operator=(const promise_type&) = delete;
      promise_type& operator=(promise_type&&) = delete;

      auto initial_suspend()
      {
         return std::suspend_always{};
      }

      auto final_suspend()
      {
         return std::suspend_always{};
      }

      auto get_return_object()
      {
         return Generator{ handle_type::from_promise(*this) };
      }

      auto return_void()
      {
         return std::suspend_never{};
      }

      auto yield_value(T some_value)
      {
         current_value = some_value;
         return std::suspend_always{};
      }

      void unhandled_exception()
      {
         std::exit(1);
      }
   };
};

} // namespace optframe

#endif //OPTFCORE_CORO_GENERATOR_HPP
