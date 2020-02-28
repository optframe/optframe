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

namespace optframe
{

// this is 'final', because I don't see any reason for someone to inherit here...
// someone could just create a better class, and pass when XESolution is needed.
// anyway, if necessary, feel free to remove this 'final', I'd love to see an use case.
template<XRepresentation R, class ADS = OPTFRAME_DEFAULT_ADS, XEvaluation XEv = Evaluation<>>
class ESolution final : public Component
{
//protected:
//	R* r {nullptr};     // representation
//	ADS* ads {nullptr}; // auxiliary data structure
//   XEv e;

public:
   // desired compatibility with pair<S, XEv>
   Solution<R, ADS> first;
   XEv second;

public:

	ESolution(R* _r, ADS* _ads = nullptr, XEv _e = XEv()) :
			first(_r, _ads), second(_e)
	{
	}

	// copy constructor (implemented via copy constructor for R)
	// TODO: in the future, this could be made using 'R.clone()' operation in #DEFINE option.
	ESolution(const R& _r, ADS* _ads = nullptr, XEv _e = XEv()) :
			first(_r, _ads), second(_e)
	{
	}

	// move constructor (implemented via move constructor for R)
	ESolution(R&& _r) :
			first(_r) // TODO: avoid so many constructors....
	{
	}

	//! copy constructor
	/*!
	 Solution copy constructor will use copy constructor for R and ADS
	 TODO: in the future, this could be made using 'R.clone()' operation in #DEFINE option.
	 */
	ESolution(const ESolution<R, ADS, XEv>& s) :
			first(s.first), second(s.second)
	{
	}

	//! move constructor
	/*!
	 Solution move constructor will steal the pointers from the object to itself
	 and set them to null in the object
	 */
	ESolution(ESolution<R, ADS, XEv> && s) :
			first(s.first), second(s.second)
	{
	}

	// assignment operator (implemented via copy constructors for R and ADS)
	// TODO: in the future, this could be made using 'R.clone()' operation in #DEFINE option.
	ESolution<R, ADS, XEv>& operator=(const ESolution<R, ADS, XEv>& s)
	{
		if (&s == this) // auto ref check
			return *this;

		first = s.first;
      second = s.second;

		return *this;
	}

	//! move operator
	/*!
	 Solution move operator will steal the pointers from the object to itself
	 and set them to null in the object
	 */
	ESolution<R, ADS, XEv>& operator=(ESolution<R, ADS, XEv> && s) noexcept
	{
		first = s.first;
      second = s.second;

		return *this;
	}

	// destructor for Solution (must free R and ADS objects)
	virtual ~ESolution()
	{
      // nothing to destroy
	}

   optframe::totally_ordered evaluation()
   {
      return second.evaluation();
   }

	// ==================
	// end canonical part
	// ==================

	ESolution<R, ADS, XEv>& clone() const
	{
      return * new ESolution<R, ADS, XEv>(*this);
	}

	// returns true if ads is not null
	bool hasADS() const
	{
		return first.hasADS();
	}

	// =======
	// setters
	// =======

	// setR with copy constructor
	void setR(const R& _r)
	{
		first.setR(_r);
	}

	// setR with pointer copy
	void setR(R* _r)
	{
		first.setR(_r);
	}

	// setR with move semantics
	void setR(R&& _r)
	{
		first.setR(_r);
	}

	// setADS with pointer copy
	void setADS(ADS* _ads)
	{
		first.setADS(_ads);
	}


	// =======
	// getters
	// =======

	// get reference of r
	R& getR()
	{
		first.getR();
	}

	// get const reference of r
	const R& getR() const
	{
		first.getR();
	}

   /*
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
   */

	// get ADS pointer
	ADS* getADSptr()
	{
		first.getADSptr();
	}

	// get ADS pointer (const)
	const ADS* getADSptr() const
	{
		first.getADSptr();
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
		ss << "ESolution: " << first.getR();
		if (first.hasADS())
			ss << "ADS: " << first.getADSptr();
		return ss.str();
	}

   friend ostream& operator<<(ostream& os, const ESolution<R, ADS, XEv>& se)
   {
      os << se.toString();
      return os;
   }

};

} // namespace optframe

// compilation tests for concepts
#include "ESolution.ctest.hpp"

#endif /* OPTFRAME_ESOLUTION_HPP_ */
