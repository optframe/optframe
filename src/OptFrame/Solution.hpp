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

// for debug purposes
#include <assert.h>

// basic elements of an OptFrame Component
#include "Component.hpp"

namespace optframe
{

// the default ADS type is 'int'
// adopting 'void' type would cause troubles in constructor/copy/move operations
// if not used, it can be ignored with few impacts (hoping compiler will help us!)
typedef int OPTFRAME_DEFAULT_ADS;
typedef OPTFRAME_DEFAULT_ADS OptFrameADS; // more beautiful :)

//! \english The Solution class is a container class for the Representation structure (R) and Advanced Data Structure (ADS). \endenglish \portuguese A classe Solution é uma classe contêiner para a Representação (R) e Estrutura de Dados Avançada (ADS). \endportuguese

/*!
 \english
 In the constructor, a copy of R (and ADS, optionally) is stored inside the Solution class.
 The getR() method returns a reference to the stored Representation (R).
 The getADS() method returns a reference to the stored Advanced Data Structure (ADS).
 Solution container expects that R and ADS: (i) implement copy constructor; (ii) implement operator<<; (iii) implement move constructor; (iv) implement move assignment.
 Perhaps (iii) and (iv) can be relaxed with #DEFINE options.
 \endenglish

 \portuguese
 No construtor, uma cópia de R (e ADS, opcionalmente) é armazenada dentro da classe Solution.
 O método getR() retorna uma referência à Representação (R) armazenada.
 O método getADS() retorna uma referência à Estrutura de Dados Avançada (ADS) armazenada.
 O contâiner Solution espera que R e ADS: (i) implementem o construtor de cópia; (ii) implementem o operator<<; (iii) implementem o move constructor; (iv) implementem o move assignment.
 Talvez (iii) e (iv) sejam relaxadas no futuro através de opções de #DEFINE.
 \endportuguese
 */

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class Solution final : public Component
{
protected:
	R* r;     // representation
	ADS* ads; // auxiliary data structure

public:

	Solution(R* _r, ADS* _ads = nullptr) :
			r(_r), ads(_ads)
	{
		assert(r);
	}

	// copy constructor (implemented via copy constructor for R)
	// TODO: in the future, this could be made using 'R.clone()' operation in #DEFINE option.
	Solution(const R& _r) :
			r(new R(_r)), ads(nullptr)
	{
	}

	// copy constructor (implemented via copy constructor for R and ADS)
	// TODO: in the future, this could be made using 'R.clone()' operation in #DEFINE option.
	Solution(const R& _r, const ADS& _ads) :
			r(new R(_r)), ads(new ADS(_ads))
	{
	}

	// move constructor (implemented via move constructor for R)
	Solution(R&& _r) :
			r(new R(std::move(_r))), ads(nullptr)
	{
	}

	// move constructor (implemented via move constructor for R and ADS)
	Solution(R&& _r, ADS&& _ads) :
			r(new R(std::move(_r))), ads(new ADS(std::move(_ads)))
	{
	}

	//! copy constructor
	/*!
	 Solution copy constructor will use copy constructor for R and ADS
	 TODO: in the future, this could be made using 'R.clone()' operation in #DEFINE option.
	 */
	Solution(const Solution<R, ADS>& s) :
			r(new R(*s.r)), ads(s.ads ? new ADS(*s.ads) : nullptr)
	{
	}

	//! move constructor
	/*!
	 Solution move constructor will steal the pointers from the object to itself
	 and set them to null in the object
	 */
	Solution(Solution<R, ADS> && s) :
			r(s.r), ads(s.ads)
	{
		s.r = nullptr;
		s.ads = nullptr;
	}

	// assignment operator (implemented via copy constructors for R and ADS)
	// TODO: in the future, this could be made using 'R.clone()' operation in #DEFINE option.
	Solution<R, ADS>& operator=(const Solution<R, ADS>& s)
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

	//! move operator
	/*!
	 Solution move operator will steal the pointers from the object to itself
	 and set them to null in the object
	 */
	Solution<R, ADS>& operator=(Solution<R, ADS> && s) noexcept
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
	virtual ~Solution()
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

	Solution<R, ADS>& clone() const
	{
		// if ads not null
		if (ads)
			return *new Solution<R, ADS>(*r, *ads);
		else
			return *new Solution<R, ADS>(*r);
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
		ss << Component::idComponent() << ":Solution";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual string toString() const
	{
		std::stringstream ss;
		ss << "Solution: " << *r;
		if (ads)
			ss << "ADS: " << ads;
		return ss.str();
	}

};

}

#endif /* OPTFRAME_SOLUTION_HPP_ */
