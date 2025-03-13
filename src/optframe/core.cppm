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

// TODO: #include <Core.hpp> directly here (excluding Component.hpp)
#include <OptFrame/Constructive.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Move.hpp>
//
#include <OptFrame/GeneralEvaluator.hpp>  // Must be part of Core too!
//
#include <OptFrame/NS.hpp>
//
#include <OptFrame/NSFind.hpp>  // Must be part of Core!
//
#include <OptFrame/NSIterator.hpp>  // Must be part of Core!
//
#include <OptFrame/Experimental/NSBlockIterator.hpp>  // Make this part of NSIterator... or remove it!
//
#include <OptFrame/NSSeq.hpp>
//
#include <OptFrame/NSEnumIterator.hpp>  // Must be part of Core!
//
#include <OptFrame/RandGen.hpp>  // Must be part of Core!
//
#include <OptFrame/NSEnum.hpp>
//
// #include <OptFrame/Timer.hpp>  // NOT part of Core!

// BASIC TESTS BELOW....

export void func(std::string s) { std::print("s = {}\n", s); }

export template <class X>
class A {
 public:
  void f(X x) { printf("old C...\n"); }
};
