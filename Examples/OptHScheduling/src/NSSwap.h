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

#ifndef OptHS_NSSwap_HPP_
#define OptHS_NSSwap_HPP_

// Framework includes
#include <OptFrame/NSSeq.hpp>

//#include "../../OptFrame/Util/TestMove.hpp"
#include <OptFrame/RandGen.hpp>

// Own includes
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

namespace OptHS
{

class MoveSwap: public Move<ESolutionOptHS>
{
public:

	int w1;
	int w2;

	MoveSwap(int _w1, int _w2) :
			w1(_w1), w2(_w2)
	{
	}

	virtual ~MoveSwap()
	{
	}

	bool canBeApplied(const ESolutionOptHS& s) override
	{
		return w1 != w2;
	}

	uptr<Move<ESolutionOptHS>> apply(ESolutionOptHS& se) override
	{
      RepOptHS& rep = se.first.getR();
		pair<char, char> aux = rep[w1];
		rep[w1] = rep[w2];
		rep[w2] = aux;
		return uptr<Move<ESolutionOptHS>>(new MoveSwap(w2,  w1));
	}

	virtual bool operator==(const Move<ESolutionOptHS>& _m) const
	{
		const MoveSwap& m = (const MoveSwap&) _m;
		return (w1 == m.w1) && (w2 == m.w2);
	}

	void print() const
	{
		cout << "MoveSwap: (" << w1 << "," << w2 << ")" << endl;
	}
};

class NSSwap: public NS<ESolutionOptHS>
{
public:

	ProblemInstance& p;
	RandGen& rg;

	NSSwap(ProblemInstance& _p, RandGen& _rg) :
			p(_p), rg(_rg)
	{
	}

	virtual ~NSSwap()
	{
	}

	virtual uptr<Move<ESolutionOptHS>> randomMove(const ESolutionOptHS& se) override
	{
      const RepOptHS& rep = se.first.getR();
		int w1 = rg.rand(rep.size());
		int w2 = w1;
		while(w2==w1)
			w2 = rg.rand(rep.size());

		return uptr<Move<ESolutionOptHS>>(new MoveSwap(w1, w2));
	}

	virtual void print() const
	{
		cout << "NSSwap" << endl;
	}
};

}

#endif /*OptHS_NSSwap_HPP_*/

