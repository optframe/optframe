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

#include "../SingleObjSearch.hpp"
#include "../LocalSearch.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class GRASP: public SingleObjSearch<R, ADS, M>
{
private:
	Evaluator<R, ADS, M>& evaluator;
	Constructive<R, ADS>& constructive;
	LocalSearch<R, ADS, M>& ls;
	unsigned int iterMax;

public:

	GRASP(Evaluator<R, ADS, M>& _eval, Constructive<R, ADS>& _constructive, LocalSearch<R, ADS, M>& _ls, int _iterMax) :
		evaluator(_eval), constructive(_constructive), ls(_ls)
	{
	   iterMax = _iterMax;
	}

	virtual ~GRASP()
	{
	}

	pair<Solution<R, ADS>&, Evaluation<M>&>* search(double timelimit = 100000000, double target_f = 0)
	{
		long tini = time(NULL);

		unsigned int iter = 0;

		long tnow = time(NULL);

		Solution<R, ADS>& s = constructive.generateSolution();
		Evaluation<M>& e    = evaluator.evaluate(s);

		while (iter < iterMax && ((tnow - tini) < timelimit))
		{
			Solution<R, ADS>& s1 = constructive.generateSolution();
			Evaluation<M>& e1 = evaluator.evaluate(s1);

			ls.exec(s1,e1,timelimit, target_f);

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

		return new pair<Solution<R, ADS>&, Evaluation<M>&>(s, e);
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS, M>::idComponent() << "grasp";
		return ss.str();

	}
};

#endif /*OPTFRAME_GRASP_HPP_*/
