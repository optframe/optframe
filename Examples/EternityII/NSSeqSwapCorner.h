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

#ifndef EtII_NSSEQSwapCorner_HPP_
#define EtII_NSSEQSwapCorner_HPP_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"
#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.h"
#include "Memory.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

namespace EtII
{

class MoveSwapCorner: public Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>
{
protected:
	int x1, y1, x2, y2;

public:

	using Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>::apply; // prevents name hiding
	using Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>::canBeApplied; // prevents name hiding

	MoveSwapCorner(int _x1, int _y1, int _x2, int _y2) :
		x1(_x1), y1(_y1), x2(_x2), y2(_y2)
	{
	}

	virtual ~MoveSwapCorner()
	{
	}

	bool canBeApplied(const RepEtII& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return true;
	}

	Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& apply(RepEtII& rep, OPTFRAME_DEFAULT_ADS&)
	{
		Piece p = rep(x1, y1);
		rep(x1, y1) = rep(x2, y2);
		rep(x2, y2) = p;

		while ((rep(0, 0).left != 0) || (rep(0, 0).up != 0))
			rep(0, 0).rotate();

		while ((rep(0, rep.getNumCols() - 1).right != 0) || (rep(0, rep.getNumCols() - 1).up != 0))
			rep(0, rep.getNumCols() - 1).rotate();

		while ((rep(rep.getNumRows() - 1, 0).left != 0) || (rep(rep.getNumRows() - 1, 0).down != 0))
			rep(rep.getNumRows() - 1, 0).rotate();

		while ((rep(rep.getNumRows() - 1, rep.getNumCols() - 1).right != 0) || (rep(rep.getNumRows() - 1, rep.getNumCols() - 1).down != 0))
			rep(rep.getNumRows() - 1, rep.getNumCols() - 1).rotate();

		return *new MoveSwapCorner(x2, y2, x1, y1);
	}

	Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& apply(MemEtII& mem, RepEtII& rep, OPTFRAME_DEFAULT_ADS& ads)
	{
		int f = 0;

		if (rep(0, 0).right == rep(0, 1).left)
			f++;
		if (rep(0, 0).down == rep(1, 0).up)
			f++;

		if (rep(0, rep.getNumCols() - 1).left == rep(0, rep.getNumCols() - 2).right)
			f++;
		if (rep(0, rep.getNumCols() - 1).down == rep(1, rep.getNumCols() - 1).up)
			f++;

		if (rep(rep.getNumRows() - 1, 0).right == rep(rep.getNumRows() - 1, 1).left)
			f++;
		if (rep(rep.getNumRows() - 1, 0).up == rep(rep.getNumRows() - 2, 0).down)
			f++;

		if (rep(rep.getNumRows() - 1, rep.getNumCols() - 1).left == rep(rep.getNumRows() - 1, rep.getNumCols() - 2).right)
			f++;
		if (rep(rep.getNumRows() - 1, rep.getNumCols() - 1).up == rep(rep.getNumRows() - 2, rep.getNumCols() - 1).down)
			f++;

		Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& rev = apply(rep, ads);

		int f2 = 0;

		if (rep(0, 0).right == rep(0, 1).left)
			f2++;
		if (rep(0, 0).down == rep(1, 0).up)
			f2++;

		if (rep(0, rep.getNumCols() - 1).left == rep(0, rep.getNumCols() - 2).right)
			f2++;
		if (rep(0, rep.getNumCols() - 1).down == rep(1, rep.getNumCols() - 1).up)
			f2++;

		if (rep(rep.getNumRows() - 1, 0).right == rep(rep.getNumRows() - 1, 1).left)
			f2++;
		if (rep(rep.getNumRows() - 1, 0).up == rep(rep.getNumRows() - 2, 0).down)
			f2++;

		if (rep(rep.getNumRows() - 1, rep.getNumCols() - 1).left == rep(rep.getNumRows() - 1, rep.getNumCols() - 2).right)
			f2++;
		if (rep(rep.getNumRows() - 1, rep.getNumCols() - 1).up == rep(rep.getNumRows() - 2, rep.getNumCols() - 1).down)
			f2++;

		mem += (f2 - f);

		return rev;
	}

	virtual bool operator==(const Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& _m) const
	{
		const MoveSwapCorner& m = (const MoveSwapCorner&) _m;
		return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2);
	}

	void print() const
	{
		cout << "MoveSwapCorner: (" << x1 << "," << y1 << ") <=> (" << x2 << "," << y2 << ")" << endl;
	}

	string id() const
	{
		return "OptFrame:Move:MoveSwapCorner";
	}
};

class NSIteratorSwapCorner: public NSIterator<RepEtII, MemEtII>
{
private:
	int x1, y1, x2, y2;
	int nRows, nCols;

public:
	NSIteratorSwapCorner(int _nRows, int _nCols) :
		nRows(_nRows), nCols(_nCols)
	{
	}

	virtual ~NSIteratorSwapCorner()
	{
	}

	virtual void first()
	{
		x1 = 0;
		y1 = 0;
		x2 = 0;
		y2 = 1;
	}

	virtual void next()
	{
		y2++;
		if (y2 > 1)
		{
			y2 = 0;
			x2++;
			if (x2 > 1)
			{
				y1++;
				if (y1 > 1)
				{
					y1 = 0;
					x1++;

					x2 = x1;
					y2 = y1 + 1;
					if (y2 > 1)
					{
						y2 = 0;
						x2++;
					}
				}
				else
				{
					x2 = x1;
					y2 = y1 + 1;
					if (y2 > 1)
					{
						y2 = 0;
						x2++;
					}
				}
			}
		}
	}

	virtual bool isDone()
	{
		return x2 > 1;
	}

	virtual Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& current()
	{
		return *new MoveSwapCorner(x1 * (nRows - 1), y1 * (nCols - 1), x2 * (nRows - 1), y2 * (nCols - 1));
	}
};

template<class MOVE = MoveSwapCorner>
class NSSeqSwapCorner: public NSSeq<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>
{
private:
	RandGen& rg;

public:

	using NSSeq<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>::move; // prevents name hiding

	NSSeqSwapCorner(RandGen& _rg):rg(_rg)
	{
	}

	virtual ~NSSeqSwapCorner()
	{
	}

	virtual Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& move(const RepEtII& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		int x1 = (rg.rand(2)) * (rep.getNumRows() - 1);
		int y1 = (rg.rand(2)) * (rep.getNumCols() - 1);

		int x2 = x1;
		int y2 = y1;

		while ((x2 == x1) && (y2 == y1))
		{
			x2 = (rg.rand(2)) * (rep.getNumRows() - 1);
			y2 = (rg.rand(2)) * (rep.getNumCols() - 1);
		}

		return *new MOVE(x1, y1, x2, y2);
	}

	virtual NSIterator<RepEtII, MemEtII>& getIterator(const RepEtII& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		// return an iterator to the neighbors of 'rep'
		return *new NSIteratorSwapCorner(rep.getNumRows(), rep.getNumCols());
	}

	virtual void print() const
	{
		cout << "NSSeqSwapCorner" << endl;
	}

	string id() const
	{
		return "OptFrame:NS:NSSeqSwapCorner";
	}
};

}

#endif /*EtII_NSSEQSwapCorner_HPP_*/

