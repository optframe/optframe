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

#ifndef OPTFRAME_INITIAL_SEARCH_HPP_
#define OPTFRAME_INITIAL_SEARCH_HPP_

#include "BaseConcepts.hpp"
#include "Component.hpp"
#include "StopCriteria.hpp"

namespace optframe {

template<XESolution XES, XSearch<XES> XSH = XES, XSearchMethod XM = Component> // defaults to single obj.
class InitialSearch : public Component
{
public:
   virtual ~InitialSearch()
   {
   }

   // timelimit in seconds, accepting fractions (millisecs, ...)
   // may or may not generate valid solution in time
   virtual std::optional<XSH> initialSearch(const StopCriteria<XSH, XM>& stop) = 0;

   virtual bool compatible(std::string s)
   {
      return (s == idComponent()) || (Component::compatible(s));
   }

   static std::string idComponent()
   {
      std::stringstream ss;
      ss << Component::idComponent() << ":InitialSearch";
      return ss.str();
   }

   virtual std::string id() const
   {
      return idComponent();
   }
};
} // namespace optframe

#endif /*OPTFRAME_INITIAL_SEARCH_HPP_*/
