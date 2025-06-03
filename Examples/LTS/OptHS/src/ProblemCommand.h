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

#ifndef OptHS_PROBLEMMODULE_HPP_
#define OptHS_PROBLEMMODULE_HPP_

#include <iostream>
//
#include <OptFrame/printable/printable.hpp>
using namespace optframe;
//
#include <OptFrame/Hyper/HeuristicFactory.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>

#include "ConstructiveRandom.h"
#include "Evaluation.h"
#include "Evaluator.h"
#include "NSSwap.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

using namespace scannerpp;
using namespace optframe;

namespace OptHS {

class OptHSProblemCommand {
 public:
  ProblemInstance* p;

  OptHSProblemCommand() { p = NULL; }

  virtual ~OptHSProblemCommand() {
    if (p) delete p;
  }

  string id() { return "problem.OptHS"; }

  bool registerComponent(sref<Component> component, string type, string name,
                         HeuristicFactory<ESolutionOptHS>& hf,
                         std::map<std::string, std::string>& dictionary) {
    int idx = hf.addComponent(component, type);
    std::stringstream ss;
    ss << type << " " << idx;
    return true;  // defineText(name, ss.str(), dictionary);
  }

  bool load(string members, HeuristicFactory<ESolutionOptHS>& hf,
            std::map<std::string, std::string>& dictionary,
            std::map<std::string, std::vector<std::string>>& ldictionary) {
    Scanner scanner(members);

    // NOLINTNEXTLINE
    p = new ProblemInstance(scanner);

    // add everything to the HeuristicFactory 'hf'

    sref<ConstructiveRandom> is = new ConstructiveRandom(*p, hf.getRandGen());
    hf.addComponent(is, "OptFrame:Constructive");

    sref<OptHSEvaluator> eval = new OptHSEvaluator(*p);
    hf.addComponent(eval,
                    Evaluator<SolutionOptHS, EvaluationOptHS>::idComponent());

    sref<NSSwap> ns = new NSSwap(*p, hf.getRandGen());
    hf.addComponent(ns, "OptFrame:NS");

    std::cout << "problem '" << members << "' loaded successfully" << std::endl;

    return true;
  }

  bool unload(HeuristicFactory<ESolutionOptHS>& factory,
              std::map<std::string, std::string>& dictionary,
              std::map<std::string, std::vector<std::string>>& ldictionary) {
    if (p) delete p;
    p = NULL;

    std::cout
        << "problem instance for OptHS unloaded successfully (use 'drop_all' "
           "if you want to remove all components)"
        << std::endl;

    return true;
  }
};

}  // namespace OptHS

#endif /*OptHS_PROBLEMMODULE_HPP_*/
