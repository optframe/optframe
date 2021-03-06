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

#ifndef DELTA_NSITERATORVRPOROPT2_HPP_
#define DELTA_NSITERATORVRPOROPT2_HPP_

#include <cmath>

using namespace std;

namespace HFMVRP
{

template<class MOVE = DeltaMoveVRPOrOpt2>
class DeltaNSIteratorVRPOrOpt2: public NSIteratorVRPOrOpt2<ESolutionHFMVRP, ProblemInstance, DeltaMoveVRPOrOpt2>//<int, AdsHFMVRP, SolutionHFMVRP, DeltaMoveVRPOrOpt2, ProblemInstance>
{
	typedef NSIteratorVRPOrOpt2<ESolutionHFMVRP, ProblemInstance, DeltaMoveVRPOrOpt2> super;//<int, AdsHFMVRP, SolutionHFMVRP, DeltaMoveVRPOrOpt2, ProblemInstance> super;

private:
	const AdsHFMVRP& ads; //TODO COULD BE A POINTER? WHAT IS THE BEST OPTION?

public:

	DeltaNSIteratorVRPOrOpt2(vector<vector<int>>& (*_getRoutes)(const ESolutionHFMVRP& s), const ESolutionHFMVRP& se, ProblemInstance* _hfmvrp) :
   	super(_getRoutes, se, _hfmvrp), ads(se.first.getADS())
	{
		if (!_hfmvrp)
		{
			cout << "Error: hfmvrp problem is NULL!" << endl;
			exit(1);
		}

	}

	virtual ~DeltaNSIteratorVRPOrOpt2()
	{
	}

	void first() override
	{

		for (int r = 0; r < rep.size(); r++)
			if (!(ads.neighborhoodStatus.find(DeltaMoveVRPOrOpt2::idComponent())->second[r]))
			{
				if (rep.at(r).size() > 1)
					for (int c = 0; c < rep.at(r).size() - 1; c++)
					{
						for (int pos = 0; pos <= rep.at(r).size(); pos++)
						{
							if ((c != pos) && (c + 1 != pos) && (c + 2 != pos))
							{
								moves.push_back(uptr<Move<ESolutionHFMVRP>>(new MOVE(getRoutes, r, c, pos, p)));
							}
						}
					}
			}

		if (moves.size() > 0)
		{
			m = std::move(moves[index]); // stealing from vector...
		}
		else
			m = NULL;

	}

	void next() override
	{
		index++;
		if (index < moves.size())
		{
			m = std::move(moves[index]); // stealing from vector...
		}
		else
			m = NULL;

	}

	bool isDone() override
	{
		return m == NULL;
	}

	uptr<Move<ESolutionHFMVRP>> current() override
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "DeltaNSSeqVRPOrOpt2. Aborting." << endl;
			exit(1);
		}

      uptr<Move<ESolutionHFMVRP>> m2 = std::move(m);
      m = nullptr;

		return m2;
	}
};

}

#endif /*DELTA_NSITERATORVRPOROPT2_HPP_*/
