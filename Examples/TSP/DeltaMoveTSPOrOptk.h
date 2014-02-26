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

#ifndef TSP_DELTA_MOVETSP_OROPTK_HPP_
#define TSP_DELTA_MOVETSP_OROPTK_HPP_

#include "../../OptFrame/Util/NeighborhoodStructures/Moves/MoveTSPOrOptk.hpp"
#include "Memory.h"
#include "Solution.h"
#include "ProblemInstance.h"

#include <cmath>

using namespace std;

namespace TSP
{

class DeltaMoveTSPOrOptk: public MoveTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, MemTSP >
{
	typedef MoveTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, MemTSP> super;

private:
	ProblemInstance* tsp;

public:

	DeltaMoveTSPOrOptk(int i, int j, int k, ProblemInstance* _tsp) :
		 super(i, j, k, _tsp), tsp(_tsp)
	{
		if(!tsp)
		{
			cout << "Error: tsp problem is NULL!" << endl;
			print();
			exit(1);
		}
	}

	virtual ~DeltaMoveTSPOrOptk()
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
		// before i and j
		int bi = i - 1;
		int bj = j - 1;

		if(bi < 0)
			bi = rep.size() - 1;
		if(bj < 0)
			bj = rep.size() - 1;

		// after j
		unsigned aj = j + 1;

		if(aj == rep.size())
			aj = 0;

		// last i+k
		unsigned lik = i + k - 1;

		// after i+k
		unsigned aik = i + k;

		if(aik == rep.size())
			aik = 0;


		double f = 0;

		if(((i == ((int)rep.size())-1) && (j == 0)) || ((i == 0 && (j == ((int)rep.size())-1)))) // extreme point
		{
			f = 0;
		}
		else
		{
			if(i < j)
			{
				f -= (*tsp->dist)(rep[bi], rep[i]);
				//cout << "-d(" << rep[bi] << "," << rep[i] << ") ";
				f -= (*tsp->dist)(rep[lik], rep[aik]);
				//cout << "-d(" << rep[lik] << "," << rep[aik] << ") ";
				f -= (*tsp->dist)(rep[j], rep[aj]);
				//cout << "-d(" << rep[j] << "," << rep[aj] << ") ";

				f += (*tsp->dist)(rep[bi], rep[aik]);
				//cout << "+d(" << rep[bi] << "," << rep[aik] << ") ";
				f += (*tsp->dist)(rep[j], rep[i]);
				//cout << "+d(" << rep[j] << "," << rep[i] << ") ";
				f += (*tsp->dist)(rep[lik], rep[aj]);
				//cout << "+d(" << rep[lik] << "," << rep[aj] << ") ";
			}
			else // i > j
			{
				f -= (*tsp->dist)(rep[bi], rep[i]);
				//cout << "-d(" << rep[bi] << "," << rep[i] << ") ";
				f -= (*tsp->dist)(rep[lik], rep[aik]);
				//cout << "-d(" << rep[lik] << "," << rep[aik] << ") ";
				f -= (*tsp->dist)(rep[bj], rep[j]);
				//cout << "-d(" << rep[bj] << "," << rep[j] << ") ";

				f += (*tsp->dist)(rep[bi], rep[aik]);
				//cout << "+d(" << rep[bi] << "," << rep[aik] << ") ";
				f += (*tsp->dist)(rep[bj], rep[i]);
				//cout << "+d(" << rep[bj] << "," << rep[i] << ") ";
				f += (*tsp->dist)(rep[lik], rep[j]);
				//cout << "+d(" << rep[lik] << "," << rep[j] << ") ";
			}
		}

		//cout << endl;

		return new MoveCost(f, 0);
	}

	static string idComponent()
	{
		string idComp = super::idComponent();
		idComp.append(":DeltaMoveTSPOrOptk");
		return idComp;
	}

	virtual bool operator==(const Move<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP >& _m) const
	{
		const DeltaMoveTSPOrOptk& m1 = (const DeltaMoveTSPOrOptk&) _m;
		return (m1.i == i) && (m1.j == j) && (m1.k == k);
	}

	virtual void print() const
	{
		cout << "DeltaMoveTSPOrOpt{K=" << k << "}";
		cout << "(" << i << ";" << j << ")" << endl;
	}
};

}

#endif /*TSP_DELTA_MOVETSP_OROPTK_HPP_*/
