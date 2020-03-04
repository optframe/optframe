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

#ifndef OPTFRAME_COPY_SOLUTION_HPP_
#define OPTFRAME_COPY_SOLUTION_HPP_

#include <cstdlib>
#include <iostream>

// for debug purposes
#include <assert.h>

// basic elements of an OptFrame Component
#include "../Component.hpp"

#include "../BaseConcepts.hpp"
#include "../Util/printable.h"

// will require 'R', thus should require basic printability here


// copy-based solution (NO POINTER IS USED HERE)
// requires copy-constructor and move-constructor (for R and ADS)
// ADS requires empty constructor (but not R)

namespace optframe
{

//template<XRepresentation R, class ADS = nullptr_t>
template<XRepresentation R, class ADS = _ADS>
class CopySolution : public Component
{
public:
   // allow direct access to internals (no issue)
	R r;     // representation
	ADS ads; // auxiliary data structure

   // copy constructor for R (ADS requires empty constructor)
	explicit CopySolution(const R& _r) :
			r(_r), ads(ADS())
	{
	}

   // copy constructor for R and ADS
   CopySolution(const R& _r, const ADS& _ads) :
			r(_r), ads(_ads)
	{
	}

	// move constructor for R and ADS
	CopySolution(R&& _r, ADS&& _ads) :
			r(_r), ads(_ads)
	{
	}

   // copy constructor
	CopySolution(const CopySolution<R, ADS>& s) :
			CopySolution(s.r, s.ads)
	{
	}

   // move constructor
	CopySolution(CopySolution<R, ADS> && s) :
			CopySolution(s.r, s.ads)
	{
	}


	CopySolution<R, ADS>& operator=(const CopySolution<R, ADS>& s)
	{
		if (&s == this) // auto ref check
			return *this;

		r = s.r;
      ads = s.ads;

		return *this;
	}

	CopySolution<R, ADS>& operator=(CopySolution<R, ADS> && s) noexcept
	{
		r = std::move(s.r);
      ads = std::move(s.ads);

		return *this;
	}

   // -----------------------------------
   // getters and setters for R and ADS 
   // according to XBaseSolution concept
   // -----------------------------------

   R& getR()
   {
      return r;   
   }

   const R& getR() const
   {
      return r;   
   }

   ADS& getADS()
   {
      return ads;   
   }

   const ADS& getADS() const
   {
      return ads;   
   }

   ADS* getADSptr()
   {
      return &ads;   
   }

   const ADS* getADSptr() const
   {
      return &ads;   
   }

   // ==================================================

	// destructor for Solution (must free R and ADS objects)
	virtual ~CopySolution()
	{
	}

	// ==================
	// end canonical part
	// ==================


   // shouldn't clone here!

   // TODO: remove!!!
	CopySolution<R, ADS>& clone() const
	{
   	return *new CopySolution<R, ADS>(*this);
	}



	// =================
	// begin Object part
	// =================

	static string idComponent()
	{
		std::stringstream ss;
		ss << Component::idComponent() << ":CopySolution";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual string toString() const
	{
		std::stringstream ss;
		ss << "CopySolution: " << r;
		//ss << "ADS: " << ads;DecoderRandomKeys
		return ss.str();
	}

   friend std::ostream& operator<<(std::ostream& os, const CopySolution<R, ADS>& s)
   {
      os << s.toString();
      return os;
   }
};

template<XRepresentation R, class ADS = _ADS, XBaseSolution<R,ADS> S = CopySolution<R,ADS>>
struct _Testing
{
   S s;
};

}

#endif /* OPTFRAME_COPY_SOLUTION_HPP_ */
