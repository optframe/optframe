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

#ifndef EtII_Delta_Move_Rotate_HPP_
#define EtII_Delta_Move_Rotate_HPP_

#include "./NSSeqRotate.h"

#include <cmath>

using namespace std;

namespace EtII
{

class DeltaMoveRotate: public MoveRotate
{

public:

	DeltaMoveRotate(int _nRot, int _x, int _y) :
		MoveRotate(_nRot, _x, _y)
	{

	}

	virtual ~DeltaMoveRotate()
	{
	}

	MoveCost* cost(const Evaluation<MemEtII>&, const RepEtII& rep, const OPTFRAME_DEFAULT_ADS& ads)
	{
		double f = 0;

		int f1 = 0;
		if (rep(x, y).left == rep(x, y - 1).right)
			f1++;
		if (rep(x, y).up == rep(x - 1, y).down)
			f1++;
		if (rep(x, y).right == rep(x, y + 1).left)
			f1++;
		if (rep(x, y).down == rep(x + 1, y).up)
			f1++;

		int f2 = 0;
		if (nRot == 1)
		{
			if (rep(x, y).left == rep(x - 1, y).down)
				f2++;
			if (rep(x, y).up ==  rep(x, y + 1).left)
				f2++;
			if (rep(x, y).right ==  rep(x + 1, y).up)
				f2++;
			if (rep(x, y).down == rep(x, y - 1).right)
				f2++;
		}

		if (nRot == 2)
		{
			if (rep(x, y).left == rep(x, y + 1).left)
				f2++;
			if (rep(x, y).up == rep(x + 1, y).up)
				f2++;
			if (rep(x, y).right == rep(x, y - 1).right)
				f2++;
			if (rep(x, y).down == rep(x - 1, y).down)
				f2++;
		}

		if (nRot == 3)
		{
			if (rep(x, y).left == rep(x + 1, y).up)
				f2++;
			if (rep(x, y).up ==rep(x, y - 1).right)
				f2++;
			if (rep(x, y).right == rep(x - 1, y).down)
				f2++;
			if (rep(x, y).down == rep(x, y + 1).left)
				f2++;
		}

		f += (f2 - f1);

		return new MoveCost (f, 0);
	}

	static string idComponent()
	{
		string idComp = DeltaMoveRotate::idComponent();
		idComp.append(":DeltaMoveRotate");
		return idComp;
	}

	virtual bool operator==(const DeltaMoveRotate& _m) const
	{
		const DeltaMoveRotate& m = (const DeltaMoveRotate&) _m;
		return (m.nRot == nRot) && (m.x == x) && (m.y == y);
	}

	virtual void print() const
	{
		cout << "DeltaMoveRotate( ";
		cout << nRot << " rotations on (" << x << "," << y << ")" << endl;
		cout << endl;
	}

	string id() const
	{
		return "OptFrame:Move:DeltaMoveRotate";
	}
};
}
#endif /*EtII_Delta_Move_Rotate_HPP_*/

