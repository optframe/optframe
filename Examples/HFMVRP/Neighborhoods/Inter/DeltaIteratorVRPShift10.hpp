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

#ifndef DELTA_NSITERATORVRPSHIFT10_HPP_
#define DELTA_NSITERATORVRPSHIFT10_HPP_

#include "../../../../OptFrame/Util/NeighborhoodStructures/VRP/Inter/NSSeqVRPShift10.hpp"

#include <cmath>

using namespace std;

namespace HFMVRP
{

template<class MOVE = DeltaMoveVRPShift10>
class DeltaNSIteratorVRPShift10: public NSIteratorVRPShift10<int, AdsHFMVRP , DeltaMoveVRPShift10, ProblemInstance>
{
	typedef NSIteratorVRPShift10<int, AdsHFMVRP, DeltaMoveVRPShift10, ProblemInstance> super;

private:
	const AdsHFMVRP& ads; //TODO COULD BE A POINTER? WHAT IS THE BEST OPTION?

public:

	DeltaNSIteratorVRPShift10(const RepHFMVRP& _rep, const AdsHFMVRP& _ads, ProblemInstance* _hfmvrp) :
		super(_rep, _ads, _hfmvrp), ads(_ads)
	{
		if (!_hfmvrp)
		{
			cout << "Error: hfmvrp problem is NULL!" << endl;
			exit(1);
		}
	}

	virtual ~DeltaNSIteratorVRPShift10()
	{
	}

	void first()
	{

		for (int r1 = 0; r1 < r.size(); r1++)
			for (int r2 = 0; r2 < r.size(); r2++)
				if (! (ads.neighborhoodStatus.find(DeltaMoveVRPShift10::idComponent())->second[r1] && ads.neighborhoodStatus.find(DeltaMoveVRPShift10::idComponent())->second[r2]) )
					if ((ads.minDemand[r1] + ads.sumDemand[r2] <= p->vehiclesCap[r2]) && (r1 != r2))
					{
						for (int cli = 1; cli < (r.at(r1).size() - 1); cli++)
							if (p->demands[r.at(r1).at(cli)] + ads.sumDemand[r2] <= p->vehiclesCap[r2])
							{
								for (int pos = 1; pos < (r.at(r2).size() - 1); pos++)
									moves.push_back(new DeltaMoveVRPShift10(r1, r2, cli, pos, p));
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
			cout << "DeltaNSIteratorVRPShift10. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

}

#endif /*DELTA_NSITERATORVRPSHIFT10_HPP_*/
