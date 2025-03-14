// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_NEIGHBORHOOD_EXPLORATION_HPP_
#define OPTFRAME_NEIGHBORHOOD_EXPLORATION_HPP_

// C++
#include <iostream>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/Search/LocalSearch.hpp>
#include <OptFrame/Search/SingleObjSearch.hpp>

// using namespace std;

namespace optframe {

// This is  NEx: Neighborhood Exploration

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH = XES>
#else
template <typename XES, typename XSH = XES>
#endif
struct RichMove {
  using XEv = typename XES::second_type;
  uptr<Move<XES, XSH>> move;
  XEv cost;
  SearchStatus status;

  RichMove() {}

  RichMove(RichMove&& rmove) : move(std::move(rmove.move)) {}

  RichMove& operator=(RichMove&& rmove) {
    // self-reference
    if (&rmove == this) return *this;
    move = std::move(rmove.move);
    cost = std::move(rmove.cost);
    status = rmove.status;
    return *this;
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH = XES>
#else
template <typename XES, typename XSH = XES>
#endif
class NeighborhoodExploration : public LocalSearch<XES, XSH> {
 public:
  using XEv = typename XES::second_type;
  NeighborhoodExploration() {}

  virtual ~NeighborhoodExploration() {}

  // implementation of a "default" local search for this NEx
  SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& stop) override {
    bool improved = false;
    // searching new move
    op<RichMove<XES, XSH>> movec = searchMove(se, stop);
    // check if move exists
    if (!movec) return SearchStatus::NO_REPORT;
    //
    // accept if it's improving by flag (avoid double verification and extra
    // Evaluator class here) using !!e idiom, to check against value 0
    while (!!(movec->status & SearchStatus::IMPROVEMENT)) {
      improved = true;
      // apply move to solution
      movec->move->apply(se);
      // update cost
      movec->cost.update(se.second);
      // searching new move
      movec = searchMove(se, stop);
      // check if move exists
      if (!movec) return SearchStatus::IMPROVEMENT;
    }
    // finished search
    return improved ? SearchStatus::IMPROVEMENT : SearchStatus::NO_REPORT;
  }

  // Output move may be nullptr. Otherwise it's a pair of Move and its Cost.
  virtual op<RichMove<XES, XSH>> searchMove(const XES& se,
                                            const StopCriteria<XEv>& stop) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":NEx";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = XES>
#else
template <typename S, typename XEv = Evaluation<>, typename XES = pair<S, XEv>,
          typename X2ES = MultiESolution<XES>, typename XSH = XES>
#endif
class NeighborhoodExplorationBuilder : public ComponentBuilder<XES> {
 public:
  virtual ~NeighborhoodExplorationBuilder() {}

  virtual NeighborhoodExploration<XES, XSH>* build(Scanner& scanner,
                                                   HeuristicFactory<XES>& hf,
                                                   string family = "") = 0;

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            string family = "") override {
    return build(scanner, hf, family);
  }

  vector<pair<string, string>> parameters() override = 0;

  bool canBuild(string) override = 0;

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << "NEx";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#include <OptFrame/Concepts/BaseConcepts.ctest.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>

namespace optframe {

struct TestSimpleRich {
  uptr<int> ptr;

  TestSimpleRich() {}

  TestSimpleRich(TestSimpleRich&& tsr) : ptr(std::move(tsr.ptr)) {}
};

using TestRichMove = RichMove<IsESolution<int>>;

// static_assert(std::is_destructible<TestRichMove>);

struct TestCompilationRichMove {
  /*
   op<RichMove<IsESolution<int>, IsEvaluation<int>>> getRichOptional()
   {
      op<RichMove<IsESolution<int>, IsEvaluation<int>>> empty;
      //
      RichMove<IsESolution<int>, IsEvaluation<int>> rmove;
      //return make_optional(rmove);
      return op<RichMove<IsESolution<int>,
   IsEvaluation<int>>>(std::move(rmove));
   }
   */

  op<TestSimpleRich> getRichOptional() {
    op<TestSimpleRich> empty;
    //
    TestSimpleRich simple;
    return op<TestSimpleRich>(std::move(simple));
  }

  void test() { auto rmv = getRichOptional(); }
};

}  // namespace optframe

#endif /* OPTFRAME_NEIGHBORHOOD_EXPLORATION_HPP_ */
