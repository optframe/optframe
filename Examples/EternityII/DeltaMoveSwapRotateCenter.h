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

#ifndef EtII_Delta_Move_Swap_Rotate_Center_HPP_
#define EtII_Delta_Move_Swap_Rotate_Center_HPP_

#include "NSSeqSwapRotateCenter.h"

#include <cmath>

using namespace std;

namespace EtII
{

class DeltaMoveSwapRotateCenter: public MoveSwapRotateCenter
{

public:

	DeltaMoveSwapRotateCenter(int _x1, int _y1, int _x2, int _y2) :
		MoveSwapRotateCenter(_x1, _y1, _x2, _y2)
	{

	}

	virtual ~DeltaMoveSwapRotateCenter()
	{
	}

	MoveCost* cost(const Evaluation<MemEtII>&, const RepEtII& rep, const OPTFRAME_DEFAULT_ADS& ads)
	{
		double f = 0;

		return new MoveCost (f, 0);
	}

	static string idComponent()
	{
		string idComp = DeltaMoveSwapRotateCenter::idComponent();
		idComp.append(":DeltaMoveSwapRotateCenter");
		return idComp;
	}

	virtual bool operator==(const DeltaMoveSwapRotateCenter& _m) const
	{
		const DeltaMoveSwapRotateCenter& m = (const DeltaMoveSwapRotateCenter&) _m;
		return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2) && (m.r1 == r1) && (m.r2 == r2);

	}

	virtual void print() const
	{
		cout << "DeltaMoveSwapRotateCenter: (" << x1 << "," << y1 << ") r=" << r1 << " <=> (" << x2 << "," << y2 << ") r=" << r2 << endl;
	}

	string id() const
	{
		return "OptFrame:Move:DeltaMoveSwapRotateCenter";
	}
};
}
#endif /*EtII_Delta_Move_Swap_Rotate_Center_HPP_*/

