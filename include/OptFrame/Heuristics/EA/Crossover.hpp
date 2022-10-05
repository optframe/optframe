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

#ifndef OPTFRAME_GENERAL_CROSSOVER_HPP_
#define OPTFRAME_GENERAL_CROSSOVER_HPP_

// C++
#include <vector>
//
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Helper/Solution.hpp>

#include "EA.h"

#ifndef _OPTFRAME_DBG_CROSSOVER_
#ifdef OPTFRAME_DEBUG
#define _OPTFRAME_DBG_CROSSOVER_
#else
#define _OPTFRAME_DBG_CROSSOVER_ while (false)
#endif /* OPTFRAME_DEBUG */
#endif /* _OPTFRAME_DBG_CROSSOVER_ */

namespace optframe {

template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class Crossover : public Component, public EA {
 public:
  virtual ~Crossover() {
  }

  virtual pair<S*, S*> cross(const S&, const S&) = 0;

  virtual bool compatible(string s) {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":" << EA::family() << ":Crossover";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }
};

//temporary fix for the true basic genetic algorithm! I will revisit this in the future to perform a proper naming convention
template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class SimpleCrossover {
 protected:
  using Individual = S;
  //using Chromossome = R;
  using Fitness = XEv*;  //nullptr means there's no evaluation
  using VPopulation = std::vector<pair<Individual, Fitness>>;

 public:
  SimpleCrossover() = default;
  virtual ~SimpleCrossover() = default;

  virtual VPopulation cross(const std::vector<Individual*>& parents) = 0;
};

/**********************/
/* CROSSOVER EXAMPLES */
/**********************/

//receives two parents to return offspring with user programmed operator
template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class TwoParentsCrossover : public SimpleCrossover<S, XEv> {
 protected:
  using Individual = S;
  //using Chromossome = R;
  using Fitness = XEv*;  //nullptr means there's no evaluation
  using VPopulation = std::vector<std::pair<Individual, Fitness>>;

 public:
  TwoParentsCrossover() = default;
  virtual ~TwoParentsCrossover() = default;

  virtual VPopulation cross(const Individual& parent1, const Individual& parent2) = 0;

  virtual VPopulation cross(const std::vector<Individual*>& parents) {
    assert(parents.size() == 2);
    assert(parents[0] != nullptr);
    assert(parents[1] != nullptr);
    return cross(*parents[0], *parents[1]);
  }
};

}  // namespace optframe

#endif /*OPTFRAME_GENERAL_CROSSOVER_HPP_*/
