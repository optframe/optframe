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

#ifndef OPTFRAME_CLONE_CONSTRUCTIVE_HPP_
#define OPTFRAME_CLONE_CONSTRUCTIVE_HPP_

#include "ComponentBuilder.h"
#include "Constructive.hpp"
#include "Solution.hpp"

namespace optframe {

template<XSolution S>
class CloneConstructive : public Constructive<S>
{
   sptr<S> base;

public:
   CloneConstructive(sptr<S> _base)
     : base(&_base->clone())
   {
   }

   CloneConstructive(const S& _base)
     : base(&_base.clone())
   {
   }

   virtual ~CloneConstructive()
   {
      //delete &base;
   }

   virtual std::optional<S> generateSolution(double timelimit) override
   {
      S& s = base->clone();
      std::optional<S> retS(s); // TODO: what happens here? can we move at least??
      S sc = s;
      delete &s;
      //return new S(sc);
      return retS;
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (Constructive<S>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Constructive<S>::idComponent() << ":CloneConstructive";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class CloneConstructiveBuilder : public ComponentBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~CloneConstructiveBuilder()
   {
   }

   virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      sptr<S> s;
      hf.assign(s, *scanner.nextInt(), scanner.next()); // reads backwards!

      return new CloneConstructive<S>(s);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(S::idComponent(), "solution"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == CloneConstructive<S>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "CloneConstructive";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};
}

#endif /*OPTFRAME_CLONE_CONSTRUCTIVE_HPP_*/
