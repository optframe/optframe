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

#ifndef RVND_HPP_
#define RVND_HPP_

#include <algorithm>

#include "../HTrajectory.hpp"
#include "../Evaluator.hpp"
#include "../RandGen.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class RVND: public HTrajectory<R, ADS, M>
{
public:
	using HTrajectory<R, ADS, M>::exec; // prevents name hiding

	RVND(Evaluator<R, ADS, M>& _ev, vector<HTrajectory<R, ADS, M>*> _neighbors, RandGen& _rg) :
		ev(_ev), neighbors(_neighbors), rg(_rg)
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = ev.evaluate(s.getR());
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit,
			double target_f)
	{

		long tini = time(NULL);

		rg.shuffle(neighbors); // shuffle elements

		int r = neighbors.size();

		int k = 1;

		long tnow = time(NULL);
		while (ev.betterThan(target_f, e.evaluation()) && (k <= r) && ((tnow - tini) < timelimit))
		{
			Solution<R, ADS>* s0 = &s.clone();
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

	virtual string id() const
   {
      return "OptFrame:VND:rvnd";
   }

private:
	Evaluator<R, ADS, M>& ev;
	vector<HTrajectory<R, ADS, M>*> neighbors;
	RandGen& rg;
};

#endif /*RVND_HPP_*/
