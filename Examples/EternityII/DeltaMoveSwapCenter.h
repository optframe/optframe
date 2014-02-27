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

#ifndef EtII_Delta_Move_SwapCenter_HPP_
#define EtII_Delta_Move_SwapCenter_HPP_

#include "./NSSeqSwapCenter.h"

#include <cmath>

using namespace std;

namespace EtII
{

class DeltaMoveSwapCenter: public MoveSwapCenter
{

public:

	DeltaMoveSwapCenter(int _x1, int _y1, int _x2, int _y2) :
		MoveSwapCenter(_x1, _y1, _x2, _y2)
	{

	}

	virtual ~DeltaMoveSwapCenter()
	{
	}

	MoveCost* cost(const Evaluation<MemEtII>&, const RepEtII& rep, const OPTFRAME_DEFAULT_ADS& ads)
	{
		// Calculus for the slot (x1, y1)
		double f1 = 0;
		if (rep(x1, y1).left == rep(x1, y1 - 1).right)
			f1++;
		if (rep(x1, y1).up == rep(x1 - 1, y1).down)
			f1++;
		if (rep(x1, y1).right == rep(x1, y1 + 1).left)
			f1++;
		if (rep(x1, y1).down == rep(x1 + 1, y1).up)
			f1++;

		int f2 = 0;
		int neighbor = 0;

		if ((x1 == x2) && (y1 - 1 == y2))
			neighbor++;
		if (rep(x2, y2).left == rep(x1, y1 - 1 + neighbor).right)
			f2++;
		neighbor = 0;

		if ((x1 - 1 == x2) && (y1 == y2))
			neighbor++;
		if (rep(x2, y2).up == rep(x1 - 1 + neighbor, y1).down)
			f2++;
		neighbor = 0;

		if ((x1 == x2) && (y1 + 1 == y2))
			neighbor++;
		if (rep(x2, y2).right == rep(x1, y1 + 1 - neighbor).left)
			f2++;
		neighbor = 0;

		if ((x1 + 1 == x2) && (y1 == y2))
			neighbor++;
		if (rep(x2, y2).down == rep(x1 + 1 - neighbor, y1).up)
			f2++;
		neighbor = 0;

		// Calculus for the slot (x2, y2)
		int g1 = 0;
		if ((rep(x2, y2).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
			g1++;
		if ((rep(x2, y2).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
			g1++;
		if ((rep(x2, y2).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
			g1++;
		if ((rep(x2, y2).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
			g1++;

		int g2 = 0;
		if ((rep(x1, y1).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
			g2++;
		if ((rep(x1, y1).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
			g2++;
		if ((rep(x1, y1).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
			g2++;
		if ((rep(x1, y1).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
			g2++;

		double f = (f2 - f1) + (g2 - g1);

		return new MoveCost (f, 0);
	}

	static string idComponent()
	{
		string idComp = DeltaMoveSwapCenter::idComponent();
		idComp.append(":DeltaMoveSwapCenter");
		return idComp;
	}

	virtual bool operator==(const DeltaMoveSwapCenter& _m) const
	{
		const DeltaMoveSwapCenter& m = (const DeltaMoveSwapCenter&) _m;
		return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2);
	}

	virtual void print() const
	{
		cout << "DeltaMoveSwapCenter: (" << x1 << "," << y1 << ") <=> (" << x2 << "," << y2 << ")" << endl;
	}

	string id() const
	{
		return "OptFrame:Move:DeltaMoveSwapCenter";
	}
};
}
#endif /*EtII_Delta_Move_SwapCenter_HPP_*/

