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

#ifndef OPTFRAME_FXCORE_FXNS_HPP_
#define OPTFRAME_FXCORE_FXNS_HPP_

#include <functional>

#include <OptFrame/NS.hpp>

namespace optframe {

template<
  XESolution XES,                        // ESolution Type
  uptr<Move<XES>> (*fRandom)(const XES&) // fApply
  >
class FxNS final : public NS<XES, typename XES::second_type>
{
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective

public:
   virtual uptr<Move<XES, XEv, XSH>> randomMove(const XES& se) override
   {
      return fRandom(se);
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FxNS";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FXCORE_FXNS_HPP_*/
