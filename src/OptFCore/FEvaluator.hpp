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

#ifndef OPTFRAME_FCORE_FEVALUATOR_HPP_
#define OPTFRAME_FCORE_FEVALUATOR_HPP_

#include <functional>

#include "../OptFrame/Evaluator.hpp"

namespace optframe {

template<
  XESolution XES,                                                         // ESolution Type
  bool Minimizing                                                        // is minimization
  >
class FEvaluator final : public Evaluator<typename XES::first_type, typename XES::second_type, XES>
{
   using S = typename XES::first_type;
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective

public:

   XEv (*fEvaluate)(const S&); // evaluation function

   FEvaluator(
      XEv (*_fEvaluate)(const S&)
   )
   :
   fEvaluate{ _fEvaluate }
   {
   }

   virtual XEv evaluate(const S& s)
   {
      return fEvaluate(s);
   }

   //virtual constexpr bool isMinimization() const
   // note that c++-10.1 accepts this on --std=c++17, which is a mistake (should only exist on c++20)
   virtual bool isMinimization() const
   {
      return Minimizing;
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FEvaluator";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FCORE_FEVALUATOR_HPP_*/
