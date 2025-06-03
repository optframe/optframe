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

#ifndef PCAP_INITIALSOLUTION_RandomInitalSolution_HPP_
#define PCAP_INITIALSOLUTION_RandomInitalSolution_HPP_

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

class PCAPInitialSolutionRandom : public Constructive<SolutionPCAP> {
 private:
  PCAPProblemInstance& pPCAP;
  RandGen& rg;
  // Your private vars

 public:
  PCAPInitialSolutionRandom(PCAPProblemInstance& _pPCAP, RandGen& _rg)
      :  // If necessary, add more parameters
        pPCAP(_pPCAP),
        rg(_rg) {}

  std::optional<SolutionPCAP> generateSolution(double timelimit) override {
    RepPCAP newRep;
    bool med[pPCAP.nCidades];
    for (int i = 0; i < pPCAP.nCidades; i++) {
      med[i] = false;
    }

    for (int i = 0; i < pPCAP.nMedianas; i++) {
      int x = rg.rand(pPCAP.nCidades);
      while (med[x] == true) x = rg.rand(pPCAP.nCidades);

      newRep.first.push_back(x);
    }

    for (int i = 0; i < pPCAP.nCidades; i++) {
      newRep.second.push_back(-1);

      for (int j = 0; j < pPCAP.nMedianas; j++) {
        if (i == newRep.first[j]) newRep.second[i] = j;
      }

      if (newRep.second[i] == -1) newRep.second[i] = rg.rand(pPCAP.nMedianas);
    }

    return make_optional(SolutionPCAP(newRep));
  }
};

#endif /*PCAP_INITIALSOLUTION_RandomInitalSolution_HPP_*/
