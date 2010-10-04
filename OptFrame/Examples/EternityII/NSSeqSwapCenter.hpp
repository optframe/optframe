#ifndef EtII_NSSEQSwapCenter_HPP_
#define EtII_NSSEQSwapCenter_HPP_

// Framework includes
#include "../../../OptFrame/NSSeq.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "Memory.h"
#include "Solution.h"

using namespace std;

class MoveSwapCenter: public Move<RepEtII, MemEtII>
{
private:
	int x1, y1, x2, y2;

public:

	using Move<RepEtII, MemEtII>::apply; // prevents name hiding
	using Move<RepEtII, MemEtII>::canBeApplied; // prevents name hiding

	MoveSwapCenter(int _x1, int _y1, int _x2, int _y2) :
		x1(_x1), y1(_y1), x2(_x2), y2(_y2)
	{
	}

	virtual ~MoveSwapCenter()
	{
	}

	bool canBeApplied(const RepEtII& rep)
	{
		return true;
	}

	Move<RepEtII, MemEtII>& apply(RepEtII& rep)
	{
		Piece p = rep(x1, y1);
		rep(x1, y1) = rep(x2, y2);
		rep(x2, y2) = p;

		return *new MoveSwapCenter(x2, y2, x1, y1);
	}

	Move<RepEtII, MemEtII>& apply(MemEtII& mem, RepEtII& rep)
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

		Move<RepEtII, MemEtII>& rev = apply(rep);

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

	virtual bool operator==(const Move<RepEtII, MemEtII>& _m) const
	{
		const MoveSwapCenter& m = (const MoveSwapCenter&) _m;
		return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2);
	}

	void print()
	{
		cout << "MoveSwapCenter: (" << x1 << "," << y1 << ") <=> (" << x2 << "," << y2 << ")" << endl;
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

	virtual Move<RepEtII, MemEtII>& current()
	{
		return *new MoveSwapCenter(x1 + 1, y1 + 1, x2 + 1, y2 + 1);
	}
};

class NSSeqSwapCenter: public NSSeq<RepEtII, MemEtII>
{
public:

	using NSSeq<RepEtII, MemEtII>::move; // prevents name hiding
	using NSSeq<RepEtII, MemEtII>::getIterator; // prevents name hiding

	NSSeqSwapCenter()
	{
	}

	virtual ~NSSeqSwapCenter()
	{
	}

	virtual Move<RepEtII, MemEtII>& move(const RepEtII& rep)
	{
		if ((rep.getRows() == 3) && (rep.getCols() == 3))
			return *new MoveSwapCenter(1, 1, 1, 1);

		// line 'x' and col 'y'
		int x1 = rand() % (rep.getRows() - 2) + 1;
		int y1 = rand() % (rep.getCols() - 2) + 1;

		int x2 = x1;
		int y2 = y1;
		while ((x2 == x1) && (y2 == y1))
		{
			x2 = rand() % (rep.getRows() - 2) + 1;
			y2 = rand() % (rep.getCols() - 2) + 1;
		}

		return *new MoveSwapCenter(x1, y1, x2, y2);
	}

	virtual NSIterator<RepEtII, MemEtII>& getIterator(const RepEtII& rep)
	{
		// return an iterator to the neighbors of 'rep'
		return *new NSIteratorSwapCenter(rep.getRows() - 2, rep.getCols() - 2);
	}

	virtual void print()
	{
		cout << "NSSeqSwapCenter" << endl;
	}
};

#endif /*EtII_NSSEQSwapCenter_HPP_*/

