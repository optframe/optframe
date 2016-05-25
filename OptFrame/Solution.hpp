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

#ifndef OPTFRAME_SOLUTION_HPP_
#define OPTFRAME_SOLUTION_HPP_

#include <cstdlib>
#include <iostream>

#include "Component.hpp"

using namespace std;

typedef int OPTFRAME_DEFAULT_ADS;

namespace optframe
{

//! \english The Solution class is a container class for the Representation structure R. \endenglish \portuguese A classe Solution é uma classe contêiner para a Representação R. \endportuguese

/*!
 \english
 In the constructor, a copy of R is stored inside the Solution class.
 The getR() method returns a reference to the stored Representation R.
 \endenglish

 \portuguese
 No construtor, uma cópia de R é armazenada dentro da classe Solution.
 O método getR() retorna uma referência à Representação R armazenada.
 \endportuguese
 */

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class Solution: public Component
{
protected:
	R r;     // representation
	ADS* ads; // auxiliary data structure

public:

	Solution(const R& _r) :
			r(_r), ads(nullptr)
	{
	}

	Solution(R&& _r) noexcept :
			r(std::move(_r)), ads(nullptr)
	{
	}


	Solution(const R& _r, const ADS& _ads) :
			r(_r), ads(new ADS(_ads))
	{
	}

	Solution(R&& _r, ADS&& _ads) noexcept :
			r(_r), ads(new ADS(std::move(_ads)))
	{
	}

	Solution(const Solution<R, ADS>& s) :
			r(s.r)
	{
		cout << __PRETTY_FUNCTION__ << endl;
		if (s.ads)
			ads = new ADS(*s.ads);
		else
			ads = nullptr;
	}

	//! move constructor
	/*!
	 Solution move constructor will steal the pointers from the object to itself
	 and set them to null in the object
	 */
	Solution(Solution<R, ADS> && s) noexcept :
			r(std::move(s.r)), ads(s.ads)
	{
		s.ads = nullptr;
	}

	// leave option to rewrite with clone()
	virtual Solution<R, ADS>& operator=(const Solution<R, ADS>& s)
	{
		if (&s == this) // auto ref check
			return *this;

		r = s.r;
		if (ads)
			(*ads) = (*s.ads);
		else
			ads = nullptr;

		return *this;
	}

	//! move operator
	/*!
	 Solution move operator will steal the pointers from the object to itself
	 and set them to null in the object
	 */
	virtual Solution<R, ADS>& operator=(Solution<R, ADS> && s) noexcept
	{
		// transform s in a rhs
		r = std::move(s.r);
		// steal pointer from s
		ads = s.ads;
		// make sure s forgets about its ads (if it existed before)
		s.ads = nullptr;

		return *this;
	}

	virtual ~Solution()
	{
		// if ads not null
		if (ads)
			delete ads;
	}

	// ==================
	// end canonical part
	// ==================

	virtual Solution<R, ADS>& clone() const
	{
		// if ads not null
		if (ads)
			return *new Solution<R, ADS>(r, *ads);
		else
			return *new Solution<R, ADS>(r);
	}

	// leave option to rewrite with clone()
	virtual void setR(const R& _r)
	{
		// shallow copy
		r = _r;
	}

	// leave option to rewrite with clone()
	virtual void setR(const R&& _r)
	{
		// move content from rhs param _r
		r = std::move(_r);
	}

	// leave option to rewrite with clone()
	virtual void setADS(const ADS& _ads)
	{
		// if ads not null, destroy it
		if (ads)
			delete ads;
		// store a new copy of _ads
		ads = new ADS(_ads);
	}

	bool hasADS() const
	{
		return ads != nullptr;
	}

	// get const reference of r
	const R& getR() const
	{
		return r;
	}

	// get ADS (depends on hasADS() positive result)
	const ADS& getADS() const
	{
		return *ads;
	}

	// get reference of r
	R& getR()
	{
		return r;
	}

	// get ADS (depends on hasADS() positive result)
	ADS& getADS()
	{
		return *ads;
	}

	// =================
	// begin Object part
	// =================

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":Solution";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "Solution: " << r;
		//ss << "ADS: "<< ads;
		return ss.str();
	}

};

}

#endif /* OPTFRAME_SOLUTION_HPP_ */
