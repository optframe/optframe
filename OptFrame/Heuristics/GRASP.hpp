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

#ifndef OPTFRAME_GRASP_HPP_
#define OPTFRAME_GRASP_HPP_

#include "../HTrajectory.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class GRASP: public HTrajectory<R, ADS, M>
{
private:
	Evaluator<R, ADS, M>& evaluator;
   InitialSolution<R, ADS>& initsol;
	HTrajectory<R, ADS, M>& h;
	unsigned int iterMax;

public:

	using HTrajectory<R, ADS, M>::exec; // prevents name hiding

	GRASP(Evaluator<R, ADS, M>& _eval, InitialSolution<R, ADS>& _initsol, HTrajectory<R, ADS, M>& _h, int _iterMax) :
		evaluator(_eval), initsol(_initsol), h(_h)
	{
	   iterMax = _iterMax;
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
			Solution<R, ADS>& s1 = initsol.generateSolution();
			Evaluation<M>& e1 = evaluator.evaluate(s1);

			h.exec(s1,e1,timelimit, target_f);

			if(evaluator.betterThan(e1,e))
			{
				s = s1;
				e = e1;
				cout << "GRASP iter "<<iter<<": ";
				e.print();
			}

			delete &s1;
			delete &e1;

			tnow = time(NULL);
			iter++;
		}
	}

	virtual string id() const
   {
      return "OptFrame:GRASP:grasp";
   }
};

#endif /*OPTFRAME_GRASP_HPP_*/
