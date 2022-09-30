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

#ifndef OPTFRAME_INITIALPOPULATION_HPP_
#define OPTFRAME_INITIALPOPULATION_HPP_

#include "../../Component.hpp"
#include "../../Constructive.hpp"
#include "../../MultiSolution.hpp"

#include "EA.h"

using namespace std;

namespace optframe {

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class InitialMultiSolution : public Component
  , public EA
{
public:
   virtual ~InitialMultiSolution()
   {
   }

   virtual MultiSolution<S>& generatePopulation(unsigned populationSize) = 0;

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (Component::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":" << EA::family() << ":InitialPopulation";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicInitialMultiSolution : public InitialMultiSolution<S>
{
public:
   Constructive<S>& constructive;

   BasicInitialMultiSolution(Constructive<S>& _constructive)
     : constructive(_constructive)
   {
   }

   virtual ~BasicInitialMultiSolution()
   {
   }

   virtual MultiSolution<S>& generatePopulation(unsigned populationSize)
   {
      MultiSolution<S>* p = new MultiSolution<S>;
      for (unsigned i = 0; i < populationSize; i++)
         p->push_back(&constructive.generateSolution());
      return *p;
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (InitialMultiSolution<S>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << InitialMultiSolution<S>::idComponent() << ":BasicInitialPopulation";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicInitialPopulationBuilder : public ComponentBuilder<R, ADS>
{
public:
   virtual ~BasicInitialPopulationBuilder()
   {
   }

   virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
   {
      Constructive<S>* c;
      hf.assign(c, *scanner.nextInt(), scanner.next()); // reads backwards!

      return new BasicInitialMultiSolution<S>(*c);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      //params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
      params.push_back(make_pair(InitialSearch<XES, XEv>::idComponent(), "constructive heuristic"));
      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == BasicInitialMultiSolution<S>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << ComponentBuilder<R, ADS>::idComponent() << "" << EA::family() << ":BasicInitialPopulation";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};
}

#endif /*OPTFRAME_INITIALPOPULATION_HPP_*/
