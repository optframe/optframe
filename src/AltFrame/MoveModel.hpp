#pragma once

#include<memory>
#include<optional>

// ===================
// Move 'Concept' here
// ===================

// Python: "don't ask for permission, ask for forgiveness"
// We cannot do that for 'undo', because if it's not 'undoable', we cannot go back (would require a copy)
// We could do this with 'apply', and return a bool (or an std::optional with disabled exception handling)
// But to prevent "simmetry" loss, it's better and simpler to keep four methods:
//
// some 'apply'
// some 'canBeApplied' ('canApply' can DEFAULT to 'return true;', while 'apply' may still fail and return 'false')
// some 'undo'
// some 'canUndo'
//
// An extra method for 'cost' is also quite welcome, as usual
//
// ==================
// maybe an arrow operator-> can bring iteroperability with older unique_ptr Move strategy (with abstract implementation)


using id_type = int;

template<class X>
using uptr = std::unique_ptr<X>;

template<class X>
using op = std::optional<X>;

template <class Self, class XES>
//concept bool totally_ordered =
//concept bool partially_ordered =
concept 
#if __cplusplus <= 201703L // after c++20, not required 'bool'
  bool 
#endif
 IsMove =
  requires(Self m, XES& se) {
     //typename Self::xes_type; // not asking this, because of unique_ptr
    { m->apply(se) };
  };

/*
       {
          e.atObjVal(idx)
       }
       ->optframe::objval;

template<class Self>
concept 
#if __cplusplus <= 201703L // after c++20, not required 'bool'
  bool 
#endif
 XESolution = XSolution<Self>&& //(XSolution<Self> && XEvaluation<Self>)  ||
  requires(Self p)
{
   typename Self::first_type;  // requires a "first_type" with some "XSolution properties"
   typename Self::second_type; // requires a "second_type" with some "XEvaluation properties"
   {
      p.first
   }
   ->my_convertible_to<typename Self::first_type>;
   {
      p.second
   }
   ->my_convertible_to<typename Self::second_type>;
   //
   XSolution<typename Self::first_type>;
   XEvaluation<typename Self::second_type>;
};
*/

//