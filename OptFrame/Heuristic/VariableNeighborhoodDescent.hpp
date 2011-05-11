// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
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

#ifndef VARIABLENEIGHBORHOODDESCENT_HPP_
#define VARIABLENEIGHBORHOODDESCENT_HPP_

#include "../Heuristic.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class VariableNeighborhoodDescent: public Heuristic<R, M>
{
public:
	using Heuristic<R, M>::exec; // prevents name hiding

	VariableNeighborhoodDescent(Evaluator<R, M>& _ev, vector<Heuristic<R, M>*> _neighbors) :
		ev(_ev), neighbors(_neighbors)
	{
	}
	;

	virtual void exec(Solution<R>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = ev.evaluate(s.getR());

		exec(s, e, timelimit, target_f);

		delete &e;
	}
	;

	virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{

		long tini = time(NULL);

		int r = neighbors.size();

		int k = 1;

		long tnow = time(NULL);
		while (ev.betterThan(target_f, e.evaluation()) && (k <= r) && ((tnow - tini) < timelimit))
		{
			Solution<R>* s0 = &s.clone();
			Evaluation<M>* e0 = &e.clone();

			neighbors[k - 1]->exec(*s0, *e0, timelimit, target_f);
			if (ev.betterThan(*s0, s))
			{
				s = *s0;
				e = *e0;
				delete s0;
				delete e0;
				k = 1;
			}
			else
			{
				delete s0;
				delete e0;
				k = k + 1;
			}
			ev.evaluate(e, s);

			tnow = time(NULL);
		}

	}
	;

private:
	vector<Heuristic<R, M>*> neighbors;
	Evaluator<R, M>& ev;
};

#endif /*VARIABLENEIGHBORHOODDESCENT_HPP_*/
