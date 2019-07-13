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

#ifndef DELTA_NSITERATORVRPSWAP11_HPP_
#define DELTA_NSITERATORVRPSWAP11_HPP_

#include "../../../../OptFrame/Util/NeighborhoodStructures/VRP/Inter/NSSeqVRPSwap1_1.hpp"

#include <cmath>

using namespace std;

namespace HFMVRP
{

template<class MOVE = DeltaMoveVRPSwap1_1>
class DeltaNSIteratorVRPSwap1_1: public NSIteratorVRPSwap1_1<int, AdsHFMVRP, DeltaMoveVRPSwap1_1, ProblemInstance>
{
	typedef NSIteratorVRPSwap1_1<int, AdsHFMVRP, DeltaMoveVRPSwap1_1, ProblemInstance> super;

private:
	const AdsHFMVRP& ads; //TODO COULD BE A POINTER? WHAT IS THE BEST OPTION?

public:

	DeltaNSIteratorVRPSwap1_1(const RepHFMVRP& _rep, const AdsHFMVRP& _ads, ProblemInstance* _hfmvrp) :
		super(_rep, _ads, _hfmvrp), ads(_ads)
	{
		if (!_hfmvrp)
		{
			cout << "Error: hfmvrp problem is NULL!" << endl;
			exit(1);
		}

	}

	virtual ~DeltaNSIteratorVRPSwap1_1()
	{
	}

	void first()
	{

		for (int r1 = 0; r1 < r.size() - 1; r1++)
			for (int r2 = r1 + 1; r2 < r.size(); r2++)
				if (! (ads.neighborhoodStatus.find(DeltaMoveVRPSwap1_1::idComponent())->second[r1] && ads.neighborhoodStatus.find(DeltaMoveVRPSwap1_1::idComponent())->second[r2]) )
				{
					bool teste1 = (ads.minDemand[r1] - ads.maxDemand[r2] + ads.sumDemand[r2] <= p->vehiclesCap[r2]);
					bool teste2 = (ads.minDemand[r2] - ads.maxDemand[r1] + ads.sumDemand[r1] <= p->vehiclesCap[r1]);

					/*
					 if (teste1 != teste2)
					 {
					 cout << "Bug SWAP 11!!!!" << endl;
					 cout << r[r1] << endl;
					 cout << r[r2] << endl;
					 cout << "ads.minDemand[r1] = " << ads.minDemand[r1] << endl;
					 cout << "ads.maxDemand[r1] = " << ads.maxDemand[r1] << endl;
					 cout << "ads.sumDemand[r1] = " << ads.sumDemand[r1] << endl;
					 cout << "ads.minDemand[r2] = " << ads.minDemand[r2] << endl;
					 cout << "ads.maxDemand[r2] = " << ads.maxDemand[r2] << endl;
					 cout << "ads.sumDemand[r2] = " << ads.sumDemand[r2] << endl;
					 cout << "ads.cumulativeDemand" << ads.cumulativeDemand << endl;
					 cout << "ads.maxDemand" << ads.maxDemand << endl;
					 cout << "ads.minDemand" << ads.minDemand << endl;
					 cout << "ads.maxPairDemand" << ads.maxPairDemand << endl;
					 cout << "ads.minPairDemand" << ads.minPairDemand << endl;
					 cout << "ads.sumDemand" << ads.sumDemand << endl;

					 getchar();
					 }
					 */
					if (ads.minDemand[r1] - ads.maxDemand[r2] + ads.sumDemand[r2] <= p->vehiclesCap[r2]) //TODO ERRO - Falta verificar a insercao do cliente r2 na rota r1
					{
						for (int c1 = 1; c1 < (r.at(r1).size() - 1); c1++)
							if (p->demands[r.at(r1).at(c1)] - ads.maxDemand[r2] + ads.sumDemand[r2] <= p->vehiclesCap[r2])
							{
								for (int c2 = 1; c2 < (r.at(r2).size() - 1); c2++)
									moves.push_back(new DeltaMoveVRPSwap1_1(r1, r2, c1, c2, p));
							}

					}
				}

		if (moves.size() > 0)
		{
			m = moves[index];
		}
		else
			m = NULL;

	}

	void next()
	{
		index++;
		if (index < moves.size())
		{
			m = moves[index];
		}
		else
			m = NULL;

	}

	bool isDone()
	{
		return m == NULL;
	}

	Move<RepHFMVRP, AdsHFMVRP>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "DeltaNSSeqVRPSwap1_1. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

}

#endif /*DELTA_NSITERATORVRPSWAP11_HPP_*/
