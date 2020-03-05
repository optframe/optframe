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


template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XSH = std::pair<S, XEv>>
class NSEnum: public NSSeq<S, XEv, XSH>
{

protected:
	RandGen& rg;

public:

	NSEnum(RandGen& _rg):rg(_rg)
	{}

	virtual ~NSEnum()
	{
	}

	virtual uptr<Move<S, XEv>> randomMove(const S&)
	{
		unsigned int x = rg.rand(size());
		return indexMove(x);
	}

	virtual uptr<NSIterator<S, XEv>> getIterator(const S&)
	{
		return uptr<NSIterator<S, XEv>>(new NSEnumIterator<S, XEv> (*this));
	}

	virtual uptr<Move<S, XEv>> indexMove(unsigned int index) = 0;

	virtual unsigned int size() const = 0;

   static string idComponent()
   {
		stringstream ss;
		ss << NSSeq<S, XEv, XSH>::idComponent() << ":NSEnum";
		return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual bool compatible(string s)
   {
	   return ( s == idComponent() ) || ( NSSeq<S, XEv, XSH>::compatible(s) );
   }
};

}

#endif /*OPTFRAME_NSENUM_HPP_*/
