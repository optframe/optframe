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

#ifndef OPTFRAME_FCORE_FMOVE_HPP_
#define OPTFRAME_FCORE_FMOVE_HPP_

#include <functional>

#include "../OptFrame/Move.hpp"


namespace optframe {

template<class M, XESolution XES, class fApply = nullptr_t, class fCanBeApplied = nullptr_t, XEvaluation XEv = Evaluation<>, XSearch<XES> XSH = XES>
class FMove final : public Move<XES, XEv, XSH>
{
public:
   M m; // internal structure for move

   FMove(const M& _m) :
      m(_m)
   {
   }

   virtual bool canBeApplied(const XES& se)
   {
      return false;
   }

   virtual uptr<Move<XES, XEv, XSH>> apply(XSH& se)
   {
      return nullptr;
   }

   virtual bool operator==(const Move<XES, XEv, XSH>& m) const
   {
      return false;
   }

   bool operator!=(const Move<XES, XEv, XSH>& m) const
   {
      return !(*this == m);
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FMove";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   // use 'operator<<' for M
   virtual void print() const
   {
      std::cout << m << std::endl;
   }
};

} // namespace optframe

#endif /*OPTFRAME_FCORE_FMOVE_HPP_*/
