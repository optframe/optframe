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

#ifndef DELTA_NSITERATORVRP2OPT_HPP_
#define DELTA_NSITERATORVRP2OPT_HPP_

#include <cmath>

using namespace std;

namespace HFMVRP
{

template<class MOVE = DeltaMoveVRP2Opt>
//class DeltaNSIteratorVRP2Opt: public NSIteratorVRP2Opt<int, AdsHFMVRP, SolutionHFMVRP, DeltaMoveVRP2Opt, ProblemInstance>
class DeltaNSIteratorVRP2Opt: public NSIteratorVRP2Opt<ESolutionHFMVRP, ProblemInstance, DeltaMoveVRP2Opt>
{
	//typedef NSIteratorVRP2Opt<int, AdsHFMVRP, SolutionHFMVRP, DeltaMoveVRP2Opt, ProblemInstance> super;
   typedef NSIteratorVRP2Opt<ESolutionHFMVRP, ProblemInstance, DeltaMoveVRP2Opt> super;

   typedef vector<vector<int>> Routes;

private:
	const AdsHFMVRP& ads; //TODO COULD BE A POINTER? WHAT IS THE BEST OPTION?

public:

   //const vector<vector<int>>& (*getRoutesX)(const ESolutionHFMVRP& s);

	//DeltaNSIteratorVRP2Opt(const RepHFMVRP& _rep, const AdsHFMVRP& _ads, ProblemInstance* _hfmvrp) :
	//	super(_rep, _ads, _hfmvrp), ads(_ads)
   DeltaNSIteratorVRP2Opt(vector<vector<int>>& (*_getRoutes)(const ESolutionHFMVRP& s), const ESolutionHFMVRP& se, ProblemInstance* _hfmvrp) :
   	super(_getRoutes, se, _hfmvrp), ads(se.first.getADS())
	{
		if (!_hfmvrp)
		{
			cout << "Error: hfmvrp problem is NULL!" << endl;
			exit(1);
		}
   
	}

	virtual ~DeltaNSIteratorVRP2Opt()
	{
	}

	void first() override
	{

		for (int r = 0; r < rep.size(); r++)
			if (!(ads.neighborhoodStatus.find(DeltaMoveVRP2Opt::idComponent())->second[r]))
			{
				int tam = rep.at(r).size() - 2;
				for (int p1 = 0; p1 < tam; p1++)
				{
					for (int p2 = p1 + 2; p2 < rep.at(r).size(); p2++)
					{
                  uptr<Move<ESolutionHFMVRP>> m1(new MOVE(r, p1, p2, p));
						moves.push_back(std::move(m1));
					}
				}
			}

		if (moves.size() > 0)
		{
			m = std::move(moves[index]); // stealing from vector! If not right, TODO implement clone() on Move class
		}
		else
			m = NULL;
	}

	void next() override
	{
		index++;
		if (index < moves.size())
		{
			m = std::move(moves[index]); // stealing from vector! If not right, TODO implement clone() on Move class
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
			cout << "DeltaNSSeqVRP2Opt. Aborting." << endl;
			exit(1);
		}

      uptr<Move<ESolutionHFMVRP>> m2(std::move(m));
      m = nullptr;
		return m2;
	}
};

}

#endif /*DELTA_NSITERATORVRP2OPT_HPP_*/
