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

#ifndef OPTFRAME_IEVALUATION_HPP_
#define OPTFRAME_IEVALUATION_HPP_

#include <assert.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "BaseConcepts.hpp" 

#include "MultiObjValue.hpp"  // inserting this beforehand.. who knows!!!
#include "SingleObjValue.hpp" // basic value 'evtype' comes from here!

using namespace std;

namespace optframe {

template<optframe::basic_arithmetics ObjType = evtype>
class IEvaluation
{
public:

   virtual ObjType evaluation() const = 0; // too bad, cannot just put deduced type here (virtual function)

   virtual string toString() const = 0;
};

} // namespace optframe

#endif /*OPTFRAME_IEVALUATION_HPP_*/
