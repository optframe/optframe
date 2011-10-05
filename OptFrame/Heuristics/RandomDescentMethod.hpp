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

#ifndef OPTFRAME_RDM_HPP_
#define OPTFRAME_RDM_HPP_

#include "../Heuristic.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class RandomDescentMethod: public Heuristic<R, ADS, M>
{
private:
   Evaluator<R, ADS, M>& evaluator;
	NS<R, ADS, M>& ns;
	unsigned int iterMax;

public:

	using Heuristic<R, ADS, M>::exec; // prevents name hiding

	RandomDescentMethod(Evaluator<R, ADS, M>& _eval, NS<R, ADS, M>& _ns, unsigned int _iterMax) :
		evaluator(_eval), ns(_ns), iterMax(_iterMax)
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = evaluator.evaluate(s.getR());
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		long tini = time(NULL);

		unsigned int iter = 0;

		long tnow = time(NULL);

		while (iter < iterMax && ((tnow - tini) < timelimit))
		{
			Move<R, ADS, M>& move = ns.move(s);

			double cost = 0;

			if (move.canBeApplied(e, s))
			{
				cost = evaluator.moveCost(e, move, s);
			}
			else
			{
				iter++;
				delete &move;
				tnow = time(NULL);
				continue;
			}

			iter++;

			if (evaluator.betterThan(cost, 0))
			{
				delete &move.apply(e, s);
				iter = 0;
			}

			delete &move;
			tnow = time(NULL);
		}
	}
};

#endif /*OPTFRAME_RDM_HPP_*/
