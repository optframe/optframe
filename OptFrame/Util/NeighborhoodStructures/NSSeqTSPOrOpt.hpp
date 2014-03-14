// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OPTFRAME_NSSEQVECTOROROPT_HPP_
#define OPTFRAME_NSSEQVECTOROROPT_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSSeq.hpp"

#include "NSSeqTSPOrOptk.hpp"
#include "NSSeqUnionAdapter.hpp"

using namespace std;

// definition: OrOpt is OrOpt{1..3}

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NSSeqTSPOrOpt: public NSSeq<vector<T> , ADS, DS >
{
	typedef vector<T> Route;

	NSSeqTSPOrOptk<T, ADS, DS >* OrOpt1;
	NSSeqTSPOrOptk<T, ADS, DS >* OrOpt2;
	NSSeqTSPOrOptk<T, ADS, DS >* OrOpt3;

	NSSeqUnionAdapter<vector<T>, ADS, DS >* OrOpt1_2_3;

public:

	NSSeqTSPOrOpt()
	{
      OrOpt1 = new NSSeqTSPOrOptk<T, ADS, DS > (1);
      OrOpt2 = new NSSeqTSPOrOptk<T, ADS, DS > (2);
      OrOpt3 = new NSSeqTSPOrOptk<T, ADS, DS > (3);

      OrOpt1_2_3 = new NSSeqUnionAdapter<vector<T> , ADS, DS > (*OrOpt1, *OrOpt2);
      OrOpt1_2_3->add_ns(*OrOpt3);
	}

	virtual ~NSSeqTSPOrOpt()
	{
		delete OrOpt1;
		delete OrOpt2;
		delete OrOpt3;

		delete OrOpt1_2_3;
	}

	Move<Route, ADS, DS >& move(const Route& rep, const ADS& ads)
	{
		return OrOpt1_2_3->move(rep, ads);
	}

	virtual Move<Route, ADS, DS>* validMove(const Route& r, const ADS& ads)
	{
		Move<Route, ADS, DS>* m = &move(r, ads);
		if(m->canBeApplied(r, ads))
			return m;
		else
		{
			delete m;
			return NULL;
		}
	}

	virtual NSIterator<Route, ADS, DS >& getIterator(const Route& rep, const ADS& ads)
	{
		return OrOpt1_2_3->getIterator(rep, ads);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << NSSeq<vector<T>, ADS, DS >::idComponent() << ":NSSeqTSPOrOpt";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual bool compatible(string s)
	{
		return ( s == idComponent() ) || ( NSSeq<vector<T>, ADS, DS >::compatible(s) );
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqVectorOrOpt{1..3}";
		return ss.str();
	}
};

#endif /*OPTFRAME_NSSEQVECTOROROPT_HPP_*/
