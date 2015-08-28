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

#ifndef DELTA_NSITERATORVRPEXCHANGE_HPP_
#define DELTA_NSITERATORVRPEXCHANGE_HPP_

#include <cmath>

using namespace std;

namespace HFMVRP
{

template<class MOVE = DeltaMoveVRPExchange>
class DeltaNSIteratorVRPExchange: public NSIteratorVRPExchange<int, AdsHFMVRP, DeltaMoveVRPExchange, ProblemInstance>
{
	typedef NSIteratorVRPExchange<int, AdsHFMVRP, DeltaMoveVRPExchange, ProblemInstance> super;

private:
	const AdsHFMVRP& ads; //TODO COULD BE A POINTER? WHAT IS THE BEST OPTION?

public:

	DeltaNSIteratorVRPExchange(const RepHFMVRP& _rep, const AdsHFMVRP& _ads, ProblemInstance* _hfmvrp) :
		super(_rep, _ads, _hfmvrp), ads(_ads)
	{
		if (!_hfmvrp)
		{
			cout << "Error: hfmvrp problem is NULL!" << endl;
			exit(1);
		}

	}

	virtual ~DeltaNSIteratorVRPExchange()
	{
	}

	void first()
	{

		for (int r = 0; r < rep.size(); r++)
			if (!(ads.neighborhoodStatus.find(DeltaMoveVRPExchange::idComponent())->second[r]))
			{
				for (int c1 = 0; c1 < rep.at(r).size(); c1++)
				{
					for (int c2 = 0; c2 < rep.at(r).size(); c2++)
					{
						if (c1 != c2)
							moves.push_back(new MOVE(r, c1, c2, p));
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
			cout << "DeltaNSSeqVRPExchange. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

}

#endif /*DELTA_NSITERATORVRPEXCHANGE_HPP_*/
