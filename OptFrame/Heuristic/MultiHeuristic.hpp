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

#ifndef MULTIHEURISTIC_HPP_
#define MULTIHEURISTIC_HPP_

#include <math.h>
#include <vector>

#include "../Heuristic.hpp"
#include "../NS.hpp"
#include "../Evaluator.hpp"
#include "../Solution.hpp"

//#define MULTIHEURISTIC_DEBUG

#ifdef  METHOD_DEBUG
#define MULTIHEURISTIC_DEBUG
#endif


template<class R, class M>
class MultiHeuristic : public Heuristic<R,M>
{
public:
	using Heuristic<R, M>::exec; // prevents name hiding

	MultiHeuristic(Evaluator<R, M>& _ev, vector<Heuristic<R, M>*> _heuristics) :
		ev(_ev), heuristics(_heuristics)
	{
	}

	virtual void exec(Solution<R>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = ev.evaluate(s.getR());
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		cout << "MultiHeuristic exec("<<target_f<<","<<timelimit<<")" << endl;

		cout << "MultiHeuristic Initial Solution: " ; e.print();
		cout << heuristics.size() << endl;

		for (unsigned int h = 0; h < heuristics.size(); h++)
		{
			cout << "MultiHeuristic iter "<<h<<":"<<endl;
			(heuristics)[h]->exec(s,timelimit, target_f);
		}
	}

private:
    Evaluator<R,M>& ev;
    vector< Heuristic<R,M>* > heuristics;
};

#endif
