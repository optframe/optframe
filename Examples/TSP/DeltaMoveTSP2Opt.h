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

#ifndef TSP_DELTA_MOVETSP2OPT_HPP_
#define TSP_DELTA_MOVETSP2OPT_HPP_

#include "../../OptFrame/Util/NeighborhoodStructures/Moves/MoveTSP2Opt.hpp"
#include "Memory.h"
#include "ProblemInstance.h"
#include "Solution.h"


#include <cmath>

using namespace std;
using namespace optframe;

namespace TSP
{

class DeltaMoveTSP2Opt: public MoveTSP2Opt<int, OPTFRAME_DEFAULT_ADS, MemTSP >
{
	typedef MoveTSP2Opt<int, OPTFRAME_DEFAULT_ADS, MemTSP> super;

private:
	ProblemInstance* tsp;

public:

	DeltaMoveTSP2Opt(int p1, int p2, ProblemInstance* _tsp) :
		 super(p1, p2, _tsp), tsp(_tsp)
	{
		if(!tsp)
		{
			cout << "Error: tsp problem is NULL!" << endl;
			print();
			exit(1);
		}
	}

	virtual ~DeltaMoveTSP2Opt()
	{
	}

	int myabs(int x)
	{
		return std::abs(x);
	}

	/*
	virtual bool canBeApplied(const RepTSP& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		bool all_positive = (p1 >= 0) && (p2 >= 0);
		return all_positive && (rep.size() >= 2);
	}
	*/

	/*
	virtual Move<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& apply(Route& rep, OPTFRAME_DEFAULT_ADS&)
	{
		reverse(rep.begin() + p1, rep.begin() + p2);

		// r1->r1, r2->r2, e1->i1, e2->i2, n1->n2, n2->n1, i1->e1, i2->e2
		return *new MoveTSP2Opt(p1, p2);
	}
	*/


	MoveCost* cost(const Evaluation<MemTSP>&, const RepTSP& rep, const OPTFRAME_DEFAULT_ADS& ads)
	{
		if(p1 >= p2)
		{
			cout << "DeltaMoveTSP2Opt FATAL ERROR!" << endl;
			exit(1);
		}
		unsigned k1 = p1;
		unsigned k2 = p2;

		if(k2 == rep.size())
			k2 = 0;

		// before k1 and k2
		unsigned bk1 = k1 - 1;
		unsigned bk2 = k2 - 1;

		if (k1 == 0)
			bk1 = rep.size() - 1;
		if (k2 == 0)
			bk2 = rep.size() - 1;

		double f = 0;

		if(myabs(k1-k2) >= 2)
		{
			f -= (*tsp->dist)(rep[bk1], rep[k1]);
			//cout << "-d(" << rep[bk1] << "," << rep[k1] << ") ";
			f -= (*tsp->dist)(rep[bk2], rep[k2]);
			//cout << "-d(" << rep[bk2] << "," << rep[k2] << ") ";

			f += (*tsp->dist)(rep[bk1], rep[bk2]);
			//cout << "+d(" << rep[bk1] << "," << rep[bk2] << ") ";
			f += (*tsp->dist)(rep[k1], rep[k2]);
			//cout << "+d(" << rep[k1] << "," << rep[k2] << ") ";
		}
		//cout << endl;

		return new MoveCost(f, 0);
	}

	static string idComponent()
	{
		string idComp = super::idComponent();
		idComp.append(":DeltaMoveTSP2Opt");
		return idComp;
	}

	virtual bool operator==(const Move<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP >& _m) const
	{
		const DeltaMoveTSP2Opt& m1 = (const DeltaMoveTSP2Opt&) _m;
		return ((m1.p1 == p1) && (m1.p2 == p2));
	}

	virtual void print() const
	{
		cout << "DeltaMoveTSP2Opt( ";
		cout << " edge " << p1 << " <=>  edge " << p2 << " )";
		cout << endl;
	}
};

}

#endif /*TSP_DELTA_MOVETSP2OPT_HPP_*/
