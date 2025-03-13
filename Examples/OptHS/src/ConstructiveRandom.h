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

#ifndef OptHS_INITIALSOLUTION_Random_HPP_
#define OptHS_INITIALSOLUTION_Random_HPP_

#include <OptFrame/Core/Constructive.hpp>
// #include "../../OptFrame/Util/TestSolution.hpp"

#include <stdlib.h>

#include <OptFrame/Core/RandGen.hpp>
#include <algorithm>
#include <list>

#include "Evaluator.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

namespace OptHS {

class ConstructiveRandom : public Constructive<SolutionOptHS> {
 private:
  ProblemInstance& p;
  RandGen& rg;

 public:
  ConstructiveRandom(ProblemInstance& _p, RandGen& _rg) : p(_p), rg(_rg) {}

  virtual ~ConstructiveRandom() {}

  std::optional<SolutionOptHS> generateSolution(double timelimit) override {
    RepOptHS newRep;

    for (unsigned i = 0; i < p.members.size() - 1; i++)
      for (unsigned j = i + 1; j < p.members.size(); j++)
        newRep.push_back(make_pair(p.members[i], p.members[j]));

    rg.shuffle(newRep);  // shuffle elements of newRep

    return make_optional(SolutionOptHS(newRep));
  }
};

}  // namespace OptHS

#endif /*OptHS_INITIALSOLUTION_Random_HPP_*/
