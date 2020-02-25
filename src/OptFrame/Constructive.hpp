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

#ifndef OPTFRAME_CONSTRUCTIVE_HPP_
#define OPTFRAME_CONSTRUCTIVE_HPP_

#include "BaseConcepts.hpp"
#include "Component.hpp"
//#include "Solution.hpp"
//#include "Solutions/CopySolution.hpp"

namespace optframe {

//template<class R, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<R, ADS> S = CopySolution<R, ADS>>
template<XSolution S>
class Constructive : public Component
{
public:
   virtual ~Constructive()
   {
   }

   // timelimit in seconds, accepting fractions (millisecs, ...)
   // may or may not generate valid solution in time
   virtual std::optional<S> generateSolution(double timelimit) = 0;

   virtual bool compatible(std::string s)
   {
      return (s == idComponent()) || (Component::compatible(s));
   }

   static std::string idComponent()
   {
      std::stringstream ss;
      ss << Component::idComponent() << ":Constructive";
      return ss.str();
   }

   virtual std::string id() const
   {
      return idComponent();
   }
};
} // namespace optframe

#endif /*OPTFRAME_CONSTRUCTIVE_HPP_*/
