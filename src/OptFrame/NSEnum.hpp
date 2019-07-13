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

#ifndef OPTFRAME_NSENUM_HPP_
#define OPTFRAME_NSENUM_HPP_

#include "NSSeq.hpp"
#include "RandGen.hpp"

#include "NSEnumIterator.hpp"

using namespace std;

namespace optframe
{


template<class R, class ADS> // already defined: ADS = OPTFRAME_DEFAULT_ADS , M = OPTFRAME_DEFAULT_EMEMORY
class NSEnum: public NSSeq<R, ADS>
{

protected:
	RandGen& rg;

public:

	NSEnum(RandGen& _rg):rg(_rg)
	{}

	virtual ~NSEnum()
	{
	}

	virtual Move<R, ADS>* randomMove(const R&, const ADS*)
	{
		unsigned int x = rg.rand(size());
		return indexMove(x);
	}

	virtual NSIterator<R, ADS>* getIterator(const R&, const ADS*)
	{
		return new NSEnumIterator<R, ADS> (*this);
	}

	virtual Move<R, ADS>* indexMove(unsigned int index) = 0;

	virtual unsigned int size() const = 0;

   static string idComponent()
   {
		stringstream ss;
		ss << NSSeq<R, ADS>::idComponent() << ":NSEnum";
		return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual bool compatible(string s)
   {
	   return ( s == idComponent() ) || ( NSSeq<R, ADS>::compatible(s) );
   }
};

}

#endif /*OPTFRAME_NSENUM_HPP_*/
