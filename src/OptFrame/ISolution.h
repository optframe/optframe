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

#ifndef OPTFRAME_ISOLUTION_HPP_
#define OPTFRAME_ISOLUTION_HPP_

#include <cstdlib>
#include <iostream>

// for debug purposes
#include <assert.h>

#include "BaseConcepts.hpp" // concepts c++20

#include "IEvaluation.h"

namespace optframe
{

// ISolution should compatible with XSolution space
class ISolution
{
public:
	virtual std::string toString() const = 0;

   virtual ~ISolution()
   {
   }
};

// IRSolution should compatible with XRSolution space
template<XRepresentation R>
class IRSolution : public ISolution
{
public:
   virtual R& getR() = 0; // too bad, virtual cannot return deduced type XRepresentation (cannot remove template here)
	virtual std::string toString() const = 0;

   virtual ~IRSolution()
   {
   }
};


// IESolution should compatible with XESolution space (XSolution and XEvaluation)
template<XSolution S, XEvaluation XEv>
class IESolution : public ISolution
{
public:
   //S first;    // non-static declared with placeholder XSolution (requires template here!)
   //XEv second; // non-static declared with placeholder XEvaluation (requires template here!)

   S& first;    // non-static declared with placeholder XSolution (requires template here!)
   XEv& second; // non-static declared with placeholder XEvaluation (requires template here!)

   // give me the references of something YOU own, not me...
   explicit IESolution(S& s, XEv& e) :
      first(s), second(e)
   {
   }

/*
   explicit IESolution(S&& s, XEv&& e) :
      first(s), second(e)
   {
   }
*/

   virtual ~IESolution()
   {
   }
};



} // namespace optframe

#endif /* OPTFRAME_ISOLUTION_HPP_ */
