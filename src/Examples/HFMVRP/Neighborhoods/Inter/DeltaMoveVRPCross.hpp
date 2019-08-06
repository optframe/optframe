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

#ifndef DELTA_MOVEVRPCROSS_HPP_
#define DELTA_MOVEVRPCROSS_HPP_

#include "../../../../OptFrame/Util/NeighborhoodStructures/VRP/Inter/NSSeqVRPCross.hpp"

#include <cmath>

using namespace std;

namespace HFMVRP
{

class DeltaMoveVRPCross: public MoveVRPCross<int, AdsHFMVRP, OPTFRAME_DEFAULT_DS>
{
	typedef MoveVRPCross<int, AdsHFMVRP, OPTFRAME_DEFAULT_DS> super;

private:
	ProblemInstance* hfmvrp;

public:

	DeltaMoveVRPCross(int _r1, int _r2, int _p1, int _p2, ProblemInstance* _hfmvrp) :
		super(_r1, _r2, _p1, _p2), hfmvrp(_hfmvrp)
	{
		if (!_hfmvrp)
		{
			cout << "Error: hfmvrp problem is NULL!" << endl;
			print();
			exit(1);
		}
	}

	virtual ~DeltaMoveVRPCross()
	{
	}

	int myabs(int x)
	{
		return std::abs(x);
	}

	/*
	 virtual bool canBeApplied(const RepHFMVRP& rep, const AdsHFMVRP&)
	 {
	 bool all_positive = (p1 >= 1) && p1 < (rep[r].size() - 1) && (p2 >= 1) && p2 < (rep[r].size() - 1);
	 bool minSize = (rep[r].size() >= 3);



	 return all_positive && minSize;
	 }

	 /*
	 virtual Move<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& apply(Route& rep, OPTFRAME_DEFAULT_ADS&)
	 {
	 reverse(rep.begin() + p1, rep.begin() + p2);

	 // r1->r1, r2->r2, e1->i1, e2->i2, n1->n2, n2->n1, i1->e1, i2->e2
	 return *new MoveTSP2Opt(p1, p2);
	 }
	 */

	pair<double, double>* cost(const Evaluation<>&, const RepHFMVRP& rep, const AdsHFMVRP& ads)
	{
		double f = 0;

		return new pair<double, double> (f, 0);
	}

	static string idComponent()
	{
		string idComp = super::idComponent();
		idComp.append(":DeltaMoveVRP2Opt");
		return idComp;
	}

	virtual bool operator==(const Move<RepHFMVRP, AdsHFMVRP>& _m) const
	{
		const DeltaMoveVRPCross& m1 = (const DeltaMoveVRPCross&) _m;
		return ((m1.r1 == r1) && (m1.r2 == r2) && (m1.p1 == p1) && (m1.p2 == p2));
	}

	virtual void print() const
	{
		cout << "DeltaMoveVRPCross( ";
		cout << r1 << ", " << r2 << ", " << p1 << ", " << p2 << " )";
		cout << endl;
	}
};

}

#endif /*DELTA_MOVEVRPCROSS_HPP_*/
