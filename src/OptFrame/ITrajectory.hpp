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

#ifndef OPTFRAME_ITRAJECTORY_HPP_
#define OPTFRAME_ITRAJECTORY_HPP_

#include "SearchOutput.hpp"
#include "StopCriteria.hpp"

namespace optframe {

// 'XES' is the "base concept" for the primary search component.
// 'XSH' is the primary search type ('best' has type XSH)
// 'XES2' is the "base concept" for the secondary search component.
// 'XSH2' is the secondary search type ('incumbent' has type XSH2)
// for trajectory methods, XSH2 = XSH
template<XESolution XES, XSearch<XES> XSH = XES>
class ITrajectory // do not inherit here!
{
   using XEv = typename XES::second_type;

public:
   // global search method (maybe, someday, create some abstract IGlobalSearch.. not now)
   virtual SearchOutput<XES, XSH> search(const StopCriteria<XEv>& stopCriteria) = 0;

   // virtual method with search signature for trajectory methods
   virtual SearchOutput<XES, XSH> searchBy(
     std::optional<XSH>& _best,
     std::optional<XSH>& _inc,
     const StopCriteria<XEv>& stopCriteria) = 0;
};

} // namespace optframe

#endif /* OPTFRAME_ITRAJECTORY_HPP_ */
