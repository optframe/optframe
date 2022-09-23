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

#ifndef OPTFRAME_COMPONENT_BUILDER_H_
#define OPTFRAME_COMPONENT_BUILDER_H_

#include "Component.hpp"
#include "Evaluation.hpp"
#include "MultiESolution.hpp"
#include "Solution.hpp"
#include "Solutions/CopySolution.hpp"

#include "Scanner++/Scanner.hpp"

using namespace scannerpp;

namespace optframe {

template<XSolution S, XEvaluation XEv, XESolution XES, X2ESolution<XES> X2ES>
class HeuristicFactory;

// TODO: maybe use CRTP pattern! (at least from LocalSearch and SingleObjSearch)
template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class ComponentBuilder : public Component
{
public:
   virtual ~ComponentBuilder()
   {
   }

   virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") = 0;

   virtual vector<pair<string, string>> parameters() = 0;

   virtual bool canBuild(string) = 0;

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":ComponentBuilder:";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

}

#endif /*OPTFRAME_COMPONENT_BUILDER_H_*/
