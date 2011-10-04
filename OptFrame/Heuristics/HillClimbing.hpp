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

#ifndef OPTFRAME_HILLCLIMBING_HPP_
#define OPTFRAME_HILLCLIMBING_HPP_

#include "../Heuristic.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

template<class R, class M>
class HillClimbing: public Heuristic<R, M>
{
private:
	Evaluator<R, M>& evaluator;
	Heuristic<R, M>& heuristic;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	HillClimbing(Evaluator<R, M>& _ev, Heuristic<R, M>& _h) :
		evaluator(_ev), heuristic(_h)
	{
	}

	virtual void exec(Solution<R>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = evaluator.evaluate(s.getR());

		exec(s, e, timelimit, target_f);

		delete &e;
	}

	virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit,
			double target_f)
	{
		long tini = time(NULL);

		Solution<R>* s0 = &s.clone();
		Evaluation<M>* e0 = &e.clone();

		heuristic.exec(s, e, timelimit, target_f);

		long tnow = time(NULL);

		while ((evaluator.betterThan(s, *s0)) && ((tnow - tini) < timelimit))
		{
			delete s0;
			s0 = &s.clone();
			delete e0;
			e0 = &e.clone();

			heuristic.exec(s, e, timelimit, target_f);

			tnow = time(NULL);
		}

		s = *s0;
		e = *e0;

		delete s0;
		delete e0;
	}
};

#endif /*OPTFRAME_HILLCLIMBING_HPP_*/
