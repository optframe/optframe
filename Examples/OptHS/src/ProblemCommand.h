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

#include <OptFrame/Scanner++/Scanner.hpp>

#include "Evaluation.h"
#include "Representation.h"
#include "Solution.h"

#include "ProblemInstance.h"

#include "Evaluator.h"

#include "ConstructiveRandom.h"
#include "NSSwap.h"

#include <OptFrame/HeuristicFactory.hpp>

using namespace scannerpp;
using namespace optframe;

namespace OptHS {

class OptHSProblemCommand
{
public:
   ProblemInstance* p;

   OptHSProblemCommand()
   {
      p = NULL;
   }

   virtual ~OptHSProblemCommand()
   {
      if (p)
         delete p;
   }

   string id()
   {
      return "problem.OptHS";
   }

   bool registerComponent(Component& component, string type, string name, HeuristicFactory<SolutionOptHS>& hf, map<string, string>& dictionary)
   {
      int idx = hf.addComponent(component, type);
      stringstream ss;
      ss << type << " " << idx;
      return true; //defineText(name, ss.str(), dictionary);
   }

   bool load(string members, HeuristicFactory<SolutionOptHS>& hf, map<string, string>& dictionary, map<string, vector<string>>& ldictionary)
   {
      Scanner scanner(members);

      p = new ProblemInstance(scanner);

      // add everything to the HeuristicFactory 'hf'

      ConstructiveRandom& is = *new ConstructiveRandom(*p, hf.getRandGen());
      hf.addComponent(is);

      OptHSEvaluator& eval = *new OptHSEvaluator(*p);
      hf.addComponent(eval, Evaluator<SolutionOptHS>::idComponent());

      NSSwap* ns = new NSSwap(*p, hf.getRandGen());
      hf.addComponent(*ns);

      cout << "problem '" << members << "' loaded successfully" << endl;

      return true;
   }

   bool unload(HeuristicFactory<SolutionOptHS>& factory, map<string, string>& dictionary, map<string, vector<string>>& ldictionary)
   {
      if (p)
         delete p;
      p = NULL;

      cout << "problem instance for OptHS unloaded successfully (use 'drop_all' if you want to remove all components)" << endl;

      return true;
   }
};

}

#endif /*OptHS_PROBLEMMODULE_HPP_*/
