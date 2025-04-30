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

#ifndef TSP_INITIALSOLUTION_Random_HPP_
#define TSP_INITIALSOLUTION_Random_HPP_

#include <stdlib.h>

#include <algorithm>
#include <list>
//
#include <OptFrame/printable/printable.hpp>
using namespace optframe;
//
#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Search/InitialSearch.hpp>
// #include "../../OptFrame/Util/TestSolution.hpp" // DEPRECATED

#include <OptFrame/Core/RandGen.hpp>

#include "Evaluator.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

using namespace std;

namespace TSP {

// class RandomInitialSolutionTSP: public Constructive<SolutionTSP>
class RandomInitialSolutionTSP : public InitialSearch<ESolutionTSP> {
 private:
  sref<ProblemInstance> pI;
  sref<GeneralEvaluator<ESolutionTSP>> eval;
  sref<RandGen> rg;

  // Your private vars

 public:
  RandomInitialSolutionTSP(sref<ProblemInstance> _pI,
                           sref<GeneralEvaluator<ESolutionTSP>> _eval,
                           sref<RandGen> _rg)
      : pI(_pI),
        eval(_eval),
        rg(_rg)  // If necessary, add more parameters
  {
    // Put the rest of your code here
  };

  virtual ~RandomInitialSolutionTSP() {}

  // std::optional<SolutionTSP> generateSolution(double timelimit) override
  std::pair<std::optional<ESolutionTSP>, SearchStatus> initialSearch(
      const StopCriteria<>& sosc) override {
    RepTSP newRep(pI->n);

    vector<int> r(pI->n);
    for (unsigned int i = 0; i < r.size(); i++) r[i] = i;

    rg->shuffle(r);  // shuffle elements of r

    for (unsigned int i = 0; i < newRep.size(); i++) newRep[i] = r[i];

    // return new CopySolution<RepTSP>(newRep);
    // return make_optional(SolutionTSP(newRep));
    ESolutionTSP se(newRep, EvaluationTSP());
    eval->reevaluate(se);
    return make_pair(make_optional(se), SearchStatus::NO_REPORT);
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << InitialSearch<ESolutionTSP>::idComponent()
       << ":RandomInitialSolution";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (InitialSearch<ESolutionTSP>::compatible(s));
  }
};

}  // namespace TSP

#endif /*TSP_INITIALSOLUTION_Random_HPP_*/
