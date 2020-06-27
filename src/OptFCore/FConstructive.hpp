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

#ifndef OPTFRAME_FCORE_FCONSTRUCTIVE_HPP_
#define OPTFRAME_FCORE_FCONSTRUCTIVE_HPP_

#include <functional>

#include "../OptFrame/Constructive.hpp"

namespace optframe {

template< XSolution S >
class FConstructive final : public Constructive<S>
{
public:

   std::optional<S> (*fGenerate)(double); // constructive

   FConstructive(
      std::optional<S> (*_fGenerate)(double)
   ):
   fGenerate { _fGenerate }
   {
   }

   virtual std::optional<S> generateSolution(double timelimit)
   {
      return fGenerate(timelimit);
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FConstructive";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FCORE_FCONSTRUCTIVE_HPP_*/
