// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

#ifndef OPTFRAME_MORI_HPP_
#define OPTFRAME_MORI_HPP_

#include "../../MOLocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Evaluator.hpp"
#include "../../MultiObjSearch.hpp"

#include "../../Timer.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class MORandomImprovement: public MOLocalSearch<R, ADS>
{
private:
	MultiEvaluator<R, ADS>& v_e;
	NS<R, ADS>& ns;

	// logs
	double sum_time;
	int num_calls;
	unsigned int iterMax;
public:

	MORandomImprovement(MultiEvaluator<R, ADS>& _v_e, NS<R, ADS>& _ns, unsigned int _iterMax) :
			v_e(_v_e), ns(_ns), iterMax(_iterMax)
	{
		sum_time = 0.0;
		num_calls = 0;
	}

	virtual ~MORandomImprovement()
	{
	}

	virtual void exec(Solution<R, ADS>& s, paretoManager<R, ADS>& pManager, double timelimit, double target_f)
	{
		MultiEvaluation& sMev = v_e.evaluate(s);

		exec(s, sMev, pManager, timelimit, target_f);

		sMev.clear();
		delete &sMev;
	}

	virtual void exec(Solution<R, ADS>& s, MultiEvaluation& sMev, paretoManager<R, ADS>& pManager, double timelimit, double target_f)
	{

		num_calls++;
		Timer t;

		int iter = 0;
		while ((iter < iterMax) && ((t.now() - timelimit) < 0))
		{
			Move<R, ADS>& move = ns.move(s);
			Move<R, ADS>* mov_rev = move.apply(s);

			v_e.evaluate(sMev, s);

			bool added = pManager.addSolution(&s, &sMev);

			delete mov_rev->apply(s);
			delete mov_rev;
			delete &move;

			v_e.evaluate(sMev, s);
			iter++;
		}

		sum_time += t.inMilliSecs();
	}
	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (MOLocalSearch<R, ADS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS>::idComponent() << "MO-RI";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual void print() const
	{
		cout << toString() << endl;
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "MORI: " << ns.toString();
		return ss.str();
	}

	virtual string log() const
	{
		stringstream ss;
		ss << sum_time;
		return ss.str();
	}

};

}

#endif /*OPTFRAME_MORI_HPP_*/
