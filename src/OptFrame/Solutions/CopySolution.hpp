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

#include "../BaseSolution.h"
#include "../Util/printable.h"

// will require 'R', thus should require basic printability here


// copy-based solution

namespace optframe
{

template<Representation R, Structure ADS = _ADS>
class CopySolution : public Component
{
protected:
	R* r;     // representation
	ADS* ads; // auxiliary data structure

public:

	CopySolution(R* _r, ADS* _ads = nullptr) :
			r(_r), ads(_ads)
	{
		assert(r);
	}

	CopySolution(const R& _r) :
			r(new R(_r)), ads(nullptr)
	{
	}

	CopySolution(const R& _r, const ADS& _ads) :
			r(new R(_r)), ads(new ADS(_ads))
	{
	}

	// move constructor (implemented via move constructor for R)
	CopySolution(R&& _r) :
			r(new R(std::move(_r))), ads(nullptr)
	{
	}

	// move constructor (implemented via move constructor for R and ADS)
	CopySolution(R&& _r, ADS&& _ads) :
			r(new R(std::move(_r))), ads(new ADS(std::move(_ads)))
	{
	}

	CopySolution(const CopySolution<R, ADS>& s) :
			r(new R(*s.r)), ads(s.ads ? new ADS(*s.ads) : nullptr)
	{
	}

	CopySolution(CopySolution<R, ADS> && s) :
			r(s.r), ads(s.ads)
	{
		s.r = nullptr;
		s.ads = nullptr;
	}

	CopySolution<R, ADS>& operator=(const CopySolution<R, ADS>& s)
	{
		if (&s == this) // auto ref check
			return *this;

		// TODO: keep as a #DEFINE option? I don't see any advantage...
		//(*r) = (*s.r);
		delete r;
		r = new R(*s.r);
		if (ads)
		{
			// TODO: keep as a #DEFINE option? I don't see any advantage...
			//(*ads) = (*s.ads);
			delete ads;
			ads = new ADS(*s.ads);
		}
		else
			ads = nullptr;

		return *this;
	}

	CopySolution<R, ADS>& operator=(CopySolution<R, ADS> && s) noexcept
	{
		// steal pointer from s
		r = s.r;
		// steal pointer from s
		ads = s.ads;
		// make sure s forgets about its r and ads (if it existed before)
		s.r = nullptr;
		s.ads = nullptr;

		return *this;
	}

	// destructor for Solution (must free R and ADS objects)
	virtual ~CopySolution()
	{
		// if r not null
		if (r)
			delete r;
		// if ads not null
		if (ads)
			delete ads;
	}

	// ==================
	// end canonical part
	// ==================

   // shouldn't clone here!

   // TODO: remove!!!
	CopySolution<R, ADS>& clone() const
	{
		// if ads not null
		if (ads)
			return *new CopySolution<R, ADS>(*r, *ads);
		else
			return *new CopySolution<R, ADS>(*r);
	}

	// returns true if ads is not null
	bool hasADS() const
	{
		return ads;
	}

	// =======
	// setters
	// =======

	// setR with copy constructor
	void setR(const R& _r)
	{
		// TODO: keep as a #DEFINE option? I don't see any advantage...
		//(*r) = _r;
		delete r;
		r = new R(_r);
	}

	// setR with pointer copy
	void setR(R* _r)
	{
		assert(_r);
		delete r;
		r = _r;
	}

	// setR with move semantics
	void setR(R&& _r)
	{
		// move content from rhs param _r
		(*r) = std::move(_r);
	}

	// setADS with copy constructor
	void setADS(const ADS& _ads)
	{
		// TODO: keep as a #DEFINE option? I don't see any advantage...
		//(*ads) = _ads;
		if (ads)
			delete ads;
		ads = new ADS(_ads);
	}

	// setADS with pointer copy
	void setADS(ADS* _ads)
	{
		if (ads)
			delete ads;
		ads = _ads;
	}

	// setADS with move semantics
	void setADS(ADS&& _ads)
	{
		// move content from rhs param _ads
		(*ads) = std::move(_ads);
	}

	// =======
	// getters
	// =======

	// get reference of r
	R& getR()
	{
		return *r;
	}

	// get const reference of r
	const R& getR() const
	{
		return *r;
	}

	// contract: assumes hasADS() with positive result
	ADS& getADS()
	{
		assert(hasADS());
		return *ads;
	}

	// contract: assumes hasADS() with positive result
	const ADS& getADS() const
	{
		assert(hasADS());
		return *ads;
	}

	// get ADS pointer
	ADS* getADSptr()
	{
		return ads;
	}

	// get ADS pointer (const)
	const ADS* getADSptr() const
	{
		return ads;
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
		ss << "CopySolution: " << *r;
		if (ads)
			ss << "ADS: " << ads;
		return ss.str();
	}

   friend std::ostream& operator<<(std::ostream& os, const CopySolution<R, ADS>& s)
   {
      os << s.toString();
      return os;
   }
};

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>>
struct _Testing
{
   S s;
};

}

#endif /* OPTFRAME_COPY_SOLUTION_HPP_ */
