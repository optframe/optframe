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

#ifndef OPTFRAME_ESOLUTION_HPP_
#define OPTFRAME_ESOLUTION_HPP_

// -----------------------------------------------------
// ESolution: an implementation of concept XESolution
// -----------------------------------------------------
// It basically groups XSolution with XEvaluation, but
// also provides access as pair<S, XEv> for compatibility

#include <cstdlib>
#include <iostream>

// for debug purposes
#include <assert.h>

// basic elements of an OptFrame Component
#include "../Component.hpp"

#include "../BaseConcepts.hpp"
#include "../Solution.hpp"
#include "../Evaluation.hpp"


namespace optframe
{

// forward declaration for ESolution class
template<XRepresentation R, class ADS, XEvaluation XEv>
class ESolution; 

// ostream operator<< forward declaration for ESolution
template<XRepresentation R, class ADS, XEvaluation XEv>
ostream& operator<<(ostream& os, const ESolution<R, ADS, XEv>& se);

// this is 'final', because I don't see any reason for someone to inherit here...
template<XRepresentation R, class ADS = OPTFRAME_DEFAULT_ADS, XEvaluation XEv = Evaluation<>>
class ESolution final : public Component, public IESolution<ESolution<R, ADS, XEv>, XEv> // CRTP!!!
{
   using super = IESolution<ESolution<R, ADS, XEv>, XEv>;
protected:
	R* r {nullptr};     // representation
	ADS* ads {nullptr}; // auxiliary data structure
   XEv e;

public:

   // pointer-based initialization (I will keep memory for me!!)
	ESolution(R* _r, ADS* _ads = nullptr, XEv _e = XEv()) :
			super(*this, e), r(_r), ads(_ads), e(_e)
	{
	}

	// copy constructor (implemented via copy constructor for R and ADS)
	ESolution(const R& _r, const ADS& _ads = nullptr, XEv _e = XEv()) :
			super(*this, e), r(new R(_r)), ads(new ADS(_ads)), e(_e)
	{
	}

	// move constructor (implemented via move constructor for R)
	ESolution(R&& _r) :
			super(*this, e), r(_r)  // TODO: avoid so many constructors....
	{
	}

	//! copy constructor
	ESolution(const ESolution<R, ADS, XEv>& s) :
			//super(s.first, s.second)
         ESolution(s.r, s.ads, s.e)
	{
	}

	//! move constructor
	ESolution(ESolution<R, ADS, XEv> && s) :
			//super::first(s.first), super::second(s.second)
         ESolution(s.r, s.ads, s.e)
	{
      s.r = nullptr; // stolen
      s.ads = nullptr; // stolen
	}


	// assignment operator (implemented via copy constructors for R and ADS)
	// TODO: in the future, this could be made using 'R.clone()' operation in #DEFINE option.
	ESolution<R, ADS, XEv>& operator=(const ESolution<R, ADS, XEv>& s)
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
      this->e = s.e; // copy

		return *this;
	}

	//! move operator
	/*!
	 Solution move operator will steal the pointers from the object to itself
	 and set them to null in the object
	 */
	ESolution<R, ADS>& operator=(ESolution<R, ADS> && s) noexcept
	{
		// steal pointer from s
		r = s.r;
		// steal pointer from s
		ads = s.ads;
      // steal evaluation
      this->e = s.e;
		// make sure s forgets about its r and ads (if it existed before)
		s.r = nullptr; // stolen
		s.ads = nullptr; // stolen

		return *this;
	}

   // not fully needed....
   optframe::objval evaluation()
   {
      return this->second.evaluation();
   }

	// ==================
	// end canonical part
	// ==================

	// destructor for ESolution (must free R and ADS objects)
	virtual ~ESolution()
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

	ESolution<R, ADS, XEv>& clone() const
	{
		// if ads not null
		if (ads)
			return *new ESolution<R, ADS, XEv>(*r, *ads);
		else
			return *new ESolution<R, ADS, XEv>(*r);
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
		ss << Component::idComponent() << ":ESolution";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual string toString() const
	{
		std::stringstream ss;
		ss << "ESolution: " << this->first.getR();
		if (this->first.hasADS())
			ss << "ADS: " << this->first.getADSptr();
      ss << "evaluation(ESolution)=" << this->second;   
		return ss.str();
	}

   friend ostream& operator<<(ostream& os, const ESolution<R, ADS, XEv>& se)
   {
      os << se.toString();
      return os;
   }

};

} // namespace optframe

// basic compilation test
static_assert(XESolution<ESolution<IsSolution<int>, Evaluation<double>>>);

// compilation tests for concepts
#include "ESolution.ctest.hpp"

#endif /* OPTFRAME_ESOLUTION_HPP_ */
