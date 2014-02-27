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

#ifndef EtII_NSSEQSwapSide_HPP_
#define EtII_NSSEQSwapSide_HPP_

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

class MoveSwapSide: public Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>
{
protected:
	int x1, y1, x2, y2;

public:

	using Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>::apply; // prevents name hiding
	using Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>::canBeApplied; // prevents name hiding

	MoveSwapSide(int _x1, int _y1, int _x2, int _y2) :
		x1(_x1), y1(_y1), x2(_x2), y2(_y2)
	{
	}

	virtual ~MoveSwapSide()
	{
	}

	bool canBeApplied(const RepEtII& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		bool left_upper = ((x1 == 0) && (y1 == 0)) || ((x2 == 0) && (y2 == 0));
		bool right_upper = ((x1 == 0) && (y1 == (int)rep.getNumCols() - 1)) || ((x2 == 0) && (y2 == (int)rep.getNumCols() - 1));
		bool left_lower = ((x1 == (int)rep.getNumRows() - 1) && (y1 == 0)) || ((x2 == (int)rep.getNumRows() - 1) && (y2 == 0));
		bool right_lower = ((x1 == (int)rep.getNumRows() - 1) && (y1 == (int)rep.getNumCols() - 1)) || ((x2 == (int)rep.getNumRows() - 1) && (y2 == (int)rep.getNumCols() - 1));

		return !left_upper && !right_upper && !left_lower && !right_lower;
	}

	Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& apply(RepEtII& rep, OPTFRAME_DEFAULT_ADS&)
	{
		Piece p = rep(x1, y1);
		rep(x1, y1) = rep(x2, y2);
		rep(x2, y2) = p;

		if (x1 == 0)
			while (rep(x1, y1).up != 0)
				rep(x1, y1).rotate();

		if (x1 == (int)rep.getNumRows() - 1)
			while (rep(x1, y1).down != 0)
				rep(x1, y1).rotate();

		if (y1 == 0)
			while (rep(x1, y1).left != 0)
				rep(x1, y1).rotate();

		if (y1 == (int)rep.getNumCols() - 1)
			while (rep(x1, y1).right != 0)
				rep(x1, y1).rotate();

		// ==========

		if (x2 == 0)
			while (rep(x2, y2).up != 0)
				rep(x2, y2).rotate();

		if (x2 == (int)rep.getNumRows() - 1)
			while (rep(x2, y2).down != 0)
				rep(x2, y2).rotate();

		if (y2 == 0)
			while (rep(x2, y2).left != 0)
				rep(x2, y2).rotate();

		if (y2 == (int)rep.getNumCols() - 1)
			while (rep(x2, y2).right != 0)
				rep(x2, y2).rotate();

		// ==========

		return *new MoveSwapSide(x2, y2, x1, y1);
	}

	Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& apply(MemEtII& mem, RepEtII& rep, OPTFRAME_DEFAULT_ADS& ads)
	{
		int f = 0;
		if (((y1 - 1) >= 0) && (rep(x1, y1).left == rep(x1, y1 - 1).right))
			f++;
		if (((x1 - 1) >= 0) && (rep(x1, y1).up == rep(x1 - 1, y1).down))
			f++;
		if (((y1 + 1) < (int)rep.getNumCols()) && (rep(x1, y1).right == rep(x1, y1 + 1).left))
			f++;
		if (((x1 + 1) < (int)rep.getNumRows()) && (rep(x1, y1).down == rep(x1 + 1, y1).up))
			f++;

		int g = 0;
		if (((y2 - 1) >= 0) && (rep(x2, y2).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
			g++;
		if (((x2 - 1) >= 0) && (rep(x2, y2).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
			g++;
		if (((y2 + 1) < (int)rep.getNumCols()) && (rep(x2, y2).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
			g++;
		if (((x2 + 1) < (int)rep.getNumRows()) && (rep(x2, y2).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
			g++;

		Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& rev = apply(rep, ads);

		int f2 = 0;
		if (((y1 - 1) >= 0) && (rep(x1, y1).left == rep(x1, y1 - 1).right))
			f2++;
		if (((x1 - 1) >= 0) && (rep(x1, y1).up == rep(x1 - 1, y1).down))
			f2++;
		if (((y1 + 1) < (int)rep.getNumCols()) && (rep(x1, y1).right == rep(x1, y1 + 1).left))
			f2++;
		if (((x1 + 1) < (int)rep.getNumRows()) && (rep(x1, y1).down == rep(x1 + 1, y1).up))
			f2++;

		int g2 = 0;
		if (((y2 - 1) >= 0) && (rep(x2, y2).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
			g2++;
		if (((x2 - 1) >= 0) && (rep(x2, y2).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
			g2++;
		if (((y2 + 1) < (int)rep.getNumCols()) && (rep(x2, y2).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
			g2++;
		if (((x2 + 1) < (int)rep.getNumRows()) && (rep(x2, y2).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
			g2++;

		mem += (f2 - f);
		mem += (g2 - g);

		return rev;
	}

	virtual bool operator==(const Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& _m) const
	{
		const MoveSwapSide& m = (const MoveSwapSide&) _m;
		return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2);
	}

	void print() const
	{
		cout << "MoveSwapSide: (" << x1 << "," << y1 << ") <=> (" << x2 << "," << y2 << ")" << endl;
	}

	string id() const
	{
		return "OptFrame:Move:MoveSwapSide";
	}
};

class NSIteratorSwapSide: public NSIterator<RepEtII, MemEtII>
{
private:
	int x1, y1, x2, y2;
	int nRows, nCols;
public:
	NSIteratorSwapSide(int _nRows, int _nCols) :
		nRows(_nRows), nCols(_nCols)
	{
	}

	virtual ~NSIteratorSwapSide()
	{
	}

	virtual void first()
	{
		x1 = 0;
		y1 = 1;
		x2 = 0;
		y2 = 2;
	}

	virtual void next()
	{
		int inc = nCols - 1;
		if ((x2 == 0) || (x2 == nRows - 1))
			inc = 1;
		y2 += inc;

		if (y2 >= nCols)
		{
			y2 = 0;
			x2++;
			if (x2 >= nRows)
			{
				inc = nCols - 1;
				if ((x1 == 0) || (x1 == nRows - 1))
					inc = 1;
				y1 += inc;

				if (y1 >= nCols)
				{
					y1 = 0;
					x1++;

					inc = nCols - 1;
					if ((x1 == 0) || (x1 == nRows - 1))
						inc = 1;

					x2 = x1;
					y2 = y1 + inc;

					if (y2 >= nCols)
					{
						y2 = 0;
						x2++;
					}
				}
				else
				{
					inc = nCols - 1;
					if ((x1 == 0) || (x1 == nRows - 1))
						inc = 1;

					x2 = x1;
					y2 = y1 + inc;

					if (y2 >= nCols)
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
		return x2 >= nRows;
	}

	virtual Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& current()
	{
		return *new MoveSwapSide(x1, y1, x2, y2);
	}
};

template<class MOVE = MoveSwapSide>
class NSSeqSwapSide: public NSSeq<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>
{
private:
	RandGen& rg;
public:

	using NSSeq<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>::move; // prevents name hiding

	NSSeqSwapSide(RandGen& _rg): rg(_rg)
	{
	}

	virtual ~NSSeqSwapSide()
	{
	}

	virtual Move<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& move(const RepEtII& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		int x1, y1;

		if ((rg.rand(2)) == 0) // vert
		{
			x1 = rg.rand((rep.getNumRows() - 2)) + 1;
			y1 = rg.rand(2) * (rep.getNumCols() - 1);
		}
		else // horz
		{
			x1 = (rg.rand(2)) * (rep.getNumRows() - 1);
			y1 = rg.rand((rep.getNumCols() - 2)) + 1;
		}

		int x2 = x1;
		int y2 = y1;
		while ((x2 == x1) && (y2 == y1))
			if ((rg.rand(2)) == 0) // vert
			{
				x2 = rg.rand((rep.getNumRows() - 2)) + 1;
				y2 = (rg.rand(2)) * (rep.getNumCols() - 1);
			}
			else // horz
			{
				x2 = (rg.rand(2)) * (rep.getNumRows() - 1);
				y2 = rg.rand((rep.getNumCols() - 2)) + 1;
			}

		return *new MOVE(x1, y1, x2, y2);
	}

	virtual NSIterator<RepEtII, MemEtII>& getIterator(const RepEtII& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return *new NSIteratorSwapSide(rep.getNumRows(), rep.getNumCols());
	}

	virtual void print() const
	{
		cout << "NSSeqSwapSide" << endl;
	}

	string id() const
	{
		return "OptFrame:NS:NSSeqSwapSide";
	}
};

}

#endif /*EtII_NSSEQSwapSide_HPP_*/

