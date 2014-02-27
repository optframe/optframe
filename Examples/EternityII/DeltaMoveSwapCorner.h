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

#ifndef EtII_Delta_Move_SwapCorner_HPP_
#define EtII_Delta_Move_SwapCorner_HPP_

#include "./NSSeqSwapCorner.h"

#include <cmath>

using namespace std;

namespace EtII
{

class DeltaMoveSwapCorner: public MoveSwapCorner
{

public:

	DeltaMoveSwapCorner(int _x1, int _y1, int _x2, int _y2) :
		MoveSwapCorner(_x1, _y1, _x2, _y2)
	{

	}

	virtual ~DeltaMoveSwapCorner()
	{
	}

	MoveCost* cost(const Evaluation<MemEtII>&, const RepEtII& rep, const OPTFRAME_DEFAULT_ADS& ads)
	{
		//considering the puzzle huge than 2x2
		// corner0 -------------------------------- corner1
		// ------------------------------------------------
		// ------------------------------------------------
		// ------------------------------------------------
		// corner2 -------------------------------- corner3
		double f = 0;

		// Calculus for the slot (x1, y1)
		double f1 = 0;
		double f2 = 0;

		//find corners rotation
		int nRot1 = 0;
		int nRot2 = 0;

		if (x1 == x2)
		{
			if (x1 != 0)
			{
				if (y1 < y2)
				{
					nRot1 += 3;
					nRot2++;
				}
				else
				{
					nRot1++;
					nRot2 += 3;
				}

			}
			else
			{
				if (y1 < y2)
				{
					nRot1++;
					nRot2 += 3;
				}
				else
				{
					nRot1 += 3;
					nRot2++;
				}
			}

		}
		else
		{
			if (y1 == y2)
			{
				if (y1 == 0)
				{
					if (x1 < x2)
					{
						nRot1 += 3;
						nRot2++;
					}
					else
					{
						nRot1++;
						nRot2 += 3;
					}
				}
				else
				{

					if (x1 < x2)
					{
						nRot1++;
						nRot2 += 3;
					}
					else
					{
						nRot1 += 3;
						nRot2++;
					}
				}
			}
			else
			{
				nRot1 += 2;
				nRot2 += 2;
			}
		}

		Piece p1 = rep(x1, y1);
		Piece p2 = rep(x2, y2);

		for (int r = 0; r < nRot1; r++)
			p1.rotate();
		for (int r = 0; r < nRot2; r++)
			p2.rotate();

		if (y1 - 1 >= 0)
		{
			if (rep(x1, y1).left == rep(x1, y1 - 1).right)
				f1++;

			if (p2.left == rep(x1, y1 - 1).right)
				f2++;
		}

		if (x1 - 1 >= 0)
		{
			if (rep(x1, y1).up == rep(x1 - 1, y1).down)
				f1++;

			if (p2.up == rep(x1 - 1, y1).down)
				f2++;
		}

		if ((y1 + 1) < ((int)rep.getNumCols()))
		{
			if (rep(x1, y1).right == rep(x1, y1 + 1).left)
				f1++;

			if (p2.right == rep(x1, y1 + 1).left)
				f2++;
		}

		if ((x1 + 1) < ((int)rep.getNumRows()))
		{
			if (rep(x1, y1).down == rep(x1 + 1, y1).up)
				f1++;

			if (p2.down == rep(x1 + 1, y1).up)
				f2++;
		}

		// Calculus for the slot (x2, y2)
		double g1 = 0;
		double g2 = 0;

		if (y2 - 1 >= 0)
		{
			if (rep(x2, y2).left == rep(x2, y2 - 1).right)
				g1++;

			if (p1.left == rep(x2, y2 - 1).right)
				g2++;
		}

		if (x2 - 1 >= 0)
		{
			if (rep(x2, y2).up == rep(x2 - 1, y2).down)
				g1++;

			if (p1.up == rep(x2 - 1, y2).down)
				g2++;
		}

		if ((y2 + 1) < ((int)rep.getNumCols()))
		{
			if (rep(x2, y2).right == rep(x2, y2 + 1).left)
				g1++;

			if (p1.right == rep(x2, y2 + 1).left)
				g2++;
		}

		if ((x2 + 1) < ((int)rep.getNumRows()))
		{
			if (rep(x2, y2).down == rep(x2 + 1, y2).up)
				g1++;

			if (p1.down == rep(x2 + 1, y2).up)
				g2++;
		}

		f = (f2 - f1) + (g2 - g1);

		return new MoveCost (f, 0);
	}

	static string idComponent()
	{
		string idComp = DeltaMoveSwapCorner::idComponent();
		idComp.append(":DeltaMoveSwapCorner");
		return idComp;
	}

	virtual bool operator==(const DeltaMoveSwapCorner& _m) const
	{
		const DeltaMoveSwapCorner& m = (const DeltaMoveSwapCorner&) _m;
		return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2);
	}

	virtual void print() const
	{
		cout << "DeltaMoveSwapCorner: (" << x1 << "," << y1 << ") <=> (" << x2 << "," << y2 << ")" << endl;
	}

	string id() const
	{
		return "OptFrame:Move:MoveMoveSwapCorner";
	}
};
}
#endif /*EtII_Delta_Move_SwapCorner_HPP_*/

