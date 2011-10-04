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

#ifndef OPTFRAME_CS_HPP_
#define OPTFRAME_CS_HPP_

#include "../Heuristic.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

template<class R, class M>
class CircularSearch: public Heuristic<R, M>
{
private:
   Evaluator<R, M>& eval;
   NSEnum<R, M>& ns;

   int initial_w;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	CircularSearch(Evaluator<R, M>& _eval, NSEnum<R, M>& _nsEnum) :
		eval(_eval), ns(_nsEnum)
	{
           initial_w = 0;
	}

	virtual void exec(Solution<R>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = eval.evaluate(s.getR());
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
                int Wmax = ns.size();

                int w = initial_w % Wmax;

                do
                {
                    Move<R,M>& m = ns.move(w);

                    if(m.canBeApplied(e, s))
                    {
                      double cost = eval.moveCost(e, m, s);

                      if(eval.betterThan(cost, 0))
                      {
                         //double old_f = e.evaluation();

                         m.apply(e, s);
                         eval.evaluate(e, s); // updates 'e'

                         //cout << "CS improvement! w:" << w << " fo=" << e.evaluation() << " (antiga fo="<< old_f << ")" << endl << endl;

                         initial_w = w+1;

                         delete &m;
                         return;
                      }
                    }

                    delete &m;
                 
                    w = (w+1) % Wmax;
                 }
                 while(w != initial_w);
	}
};

#endif /*OPTFRAME_CS_HPP_*/
