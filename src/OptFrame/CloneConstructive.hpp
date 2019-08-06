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

template<Representation R, Structure ADS = OPTFRAME_DEFAULT_ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>>
class CloneConstructive : public Constructive<R, ADS, S>
{
   S& base;

public:
   CloneConstructive(S& _base)
     : base(_base.clone())
   {
   }

   virtual ~CloneConstructive()
   {
      delete &base;
   }

   virtual S* generateSolution(double timelimit) override
   {
      S& s = base.clone();
      S sc = s;
      delete &s;
      return new S(sc);
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (Constructive<R, ADS, S>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Constructive<R, ADS, S>::idComponent() << ":CloneConstructive";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<Representation R, Structure ADS = OPTFRAME_DEFAULT_ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>>
class CloneConstructiveBuilder : public ComponentBuilder<R, ADS, S>
{
public:
   virtual ~CloneConstructiveBuilder()
   {
   }

   virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
   {
      S* s;
      hf.assign(s, scanner.nextInt(), scanner.next()); // reads backwards!

      return new CloneConstructive<R, ADS, S>(*s);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(S::idComponent(), "solution"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == CloneConstructive<R, ADS, S>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << ComponentBuilder<R, ADS, S>::idComponent() << "CloneConstructive";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};
}

#endif /*OPTFRAME_CLONE_CONSTRUCTIVE_HPP_*/
