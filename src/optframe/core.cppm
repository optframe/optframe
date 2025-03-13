module;

#include <assert.h>
#include <stdio.h>

#include <string>

export module optframe.core;
export import optframe.semstream;
export import optframe.component;
export import optframe.concepts;
export import optframe.values;
export import optframe.timer;

export import std;

#include <OptFrame/Core.hpp>

// BASIC TESTS BELOW....

export void func(std::string s) { std::print("s = {}\n", s); }

export template <class X>
class A {
 public:
  void f(X x) { printf("old C...\n"); }
};
