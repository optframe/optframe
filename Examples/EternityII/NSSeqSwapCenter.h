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

#ifndef EtII_NSSEQSwapCenter_HPP_
#define EtII_NSSEQSwapCenter_HPP_

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

class MoveSwapCenter: public Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>
{
protected:
	int x1, y1, x2, y2;
public:

	using Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>::apply; // prevents name hiding
	using Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>::canBeApplied; // prevents name hiding

	MoveSwapCenter(int _x1, int _y1, int _x2, int _y2) :
		x1(_x1), y1(_y1), x2(_x2), y2(_y2)
	{
	}

	virtual ~MoveSwapCenter()
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

		return *new MoveSwapCenter(x2, y2, x1, y1);
	}

	Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& apply(MemEtII& mem, RepEtII& rep, OPTFRAME_DEFAULT_ADS& ads)
	{
		int f = 0;
		if (rep(x1, y1).left == rep(x1, y1 - 1).right)
			f++;
		if (rep(x1, y1).up == rep(x1 - 1, y1).down)
			f++;
		if (rep(x1, y1).right == rep(x1, y1 + 1).left)
			f++;
		if (rep(x1, y1).down == rep(x1 + 1, y1).up)
			f++;

		int g = 0;
		if ((rep(x2, y2).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
			g++;
		if ((rep(x2, y2).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
			g++;
		if ((rep(x2, y2).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
			g++;
		if ((rep(x2, y2).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
			g++;

		Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& rev = apply(rep, ads);

		int f2 = 0;
		if (rep(x1, y1).left == rep(x1, y1 - 1).right)
			f2++;
		if (rep(x1, y1).up == rep(x1 - 1, y1).down)
			f2++;
		if (rep(x1, y1).right == rep(x1, y1 + 1).left)
			f2++;
		if (rep(x1, y1).down == rep(x1 + 1, y1).up)
			f2++;

		int g2 = 0;
		if ((rep(x2, y2).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
			g2++;
		if ((rep(x2, y2).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
			g2++;
		if ((rep(x2, y2).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
			g2++;
		if ((rep(x2, y2).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
			g2++;

		mem += (f2 - f);
		mem += (g2 - g);

		return rev;
	}

	virtual bool operator==(const Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& _m) const
	{
		const MoveSwapCenter& m = (const MoveSwapCenter&) _m;
		return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2);
	}

	void print() const
	{
		cout << "MoveSwapCenter: (" << x1 << "," << y1 << ") <=> (" << x2 << "," << y2 << ")" << endl;
	}

	string id() const
	{
		return "OptFrame:Move:MoveSwapCenter";
	}
};

class NSIteratorSwapCenter: public NSIterator<RepEtII, MemEtII>
{
private:
	int nIntraRows, nIntraCols;
	int x1, y1, x2, y2;

public:
	NSIteratorSwapCenter(int _nIntraRows, int _nIntraCols) :
		nIntraRows(_nIntraRows), nIntraCols(_nIntraCols)
	{
	}

	virtual ~NSIteratorSwapCenter()
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
		if (y2 >= nIntraCols)
		{
			y2 = 0;
			x2++;
			if (x2 >= nIntraRows)
			{

				y1++;
				if (y1 >= nIntraCols)
				{
					y1 = 0;
					x1++;

					x2 = x1;
					y2 = y1 + 1;
					if (y2 >= nIntraCols)
					{
						y2 = 0;
						x2++;
					}
				}
				else
				{
					x2 = x1;
					y2 = y1 + 1;
					if (y2 >= nIntraCols)
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
		return (x1 >= nIntraRows) || (y1 >= nIntraCols) || (x2 >= nIntraRows) || (y2 >= nIntraCols);
	}

	virtual Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& current()
	{
		return *new MoveSwapCenter(x1 + 1, y1 + 1, x2 + 1, y2 + 1);
	}
};

template<class MOVE = MoveSwapCenter>
class NSSeqSwapCenter: public NSSeq<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>
{
private:
	RandGen& rg;
public:

	using NSSeq<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>::move; // prevents name hiding
	using NSSeq<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>::getIterator; // prevents name hiding

	NSSeqSwapCenter(RandGen& _rg) :
		rg(_rg)
	{
	}

	virtual ~NSSeqSwapCenter()
	{
	}

	virtual Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& move(const RepEtII& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		if ((rep.getNumRows() == 3) && (rep.getNumCols() == 3))
			return *new MoveSwapCenter(1, 1, 1, 1);

		// line 'x' and col 'y'
		int x1 = rg.rand((rep.getNumRows() - 2)) + 1;
		int y1 = rg.rand((rep.getNumCols() - 2)) + 1;

		int x2 = x1;
		int y2 = y1;
		while ((x2 == x1) && (y2 == y1))
		{
			x2 = rg.rand((rep.getNumRows() - 2)) + 1;
			y2 = rg.rand((rep.getNumCols() - 2)) + 1;
		}

		return *new MOVE(x1, y1, x2, y2);
	}

	virtual NSIterator<RepEtII, MemEtII>& getIterator(const RepEtII& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		// return an iterator to the neighbors of 'rep'
		return *new NSIteratorSwapCenter(rep.getNumRows() - 2, rep.getNumCols() - 2);
	}

	virtual void print() const
	{
		cout << "NSSeqSwapCenter" << endl;
	}

	string id() const
	{
		return "OptFrame:NS:NSSeqSwapCenter";
	}
};

}

#endif /*EtII_NSSEQSwapCenter_HPP_*/

