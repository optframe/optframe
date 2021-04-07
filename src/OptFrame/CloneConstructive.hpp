// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_CLONE_CONSTRUCTIVE_HPP_
#define OPTFRAME_CLONE_CONSTRUCTIVE_HPP_

#include "Constructive.hpp"
#include "Solution.hpp"
#include "ComponentBuilder.h"

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
