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

#ifndef OPTFRAME_IPOPULATIONAL_HPP_
#define OPTFRAME_IPOPULATIONAL_HPP_

#include "Population.hpp"
#include "StopCriteria.hpp"

namespace optframe {

// 'XES' is the "base concept" for the primary search component.
// 'XSH' is the primary search type ('best' has type XSH)
// 'XES2' is the "base concept" for the secondary search component.
// 'XSH2' is the secondary search type ('incumbent' has type XSH2)
template<XESolution XES, XSearch<XES> XSH = XES, XESolution XES2 = XES>
class IPopulational // do not inherit here!
{
   using XEv = typename XES::second_type;
   using XSH2 = Population<XES2>;

public:
   // global search method (maybe, someday, create some abstract IGlobalSearch.. not now)
   virtual SearchStatus search(const StopCriteria<XEv>& stopCriteria) = 0;

   // virtual method with search signature for populational methods
   virtual SearchStatus searchBy(
     std::optional<XSH>& _best,
     std::optional<XSH2>& _inc,
     const StopCriteria<XEv>& stopCriteria) = 0;
};

} // namespace optframe

#endif /* OPTFRAME_IPOPULATIONAL_HPP_ */
