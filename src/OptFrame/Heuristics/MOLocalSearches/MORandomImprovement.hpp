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

//Basic MORI does not considering valid move, parameter iterMax only.
template<XSolution S, XEvaluation XEv=Evaluation<>>
class MORandomImprovement: public MOLocalSearch<S, XEv>
{
private:
	MultiEvaluator<S>& mev;
	NS<S, XEv>& ns;

	// logs
	double sum_time;
	int num_calls;
	int iterMax;
public:

	MORandomImprovement(MultiEvaluator<S>& _mev, NS<S, XEv>& _ns, unsigned int _iterMax) :
			mev(_mev), ns(_ns), iterMax(_iterMax)
	{
		sum_time = 0.0;
		num_calls = 0;
	}

	virtual ~MORandomImprovement()
	{
	}

	virtual void moSearchFrom(Pareto<S, XEv>& p, S& s, paretoManager<S, XEv>& pManager, SOSC<>& stopCriteria) override
	{
		MultiEvaluation<> sMev(std::move(mev.evaluate(s)));

		moSearchFrom(p, s, sMev, pManager, stopCriteria);
	}

	virtual void moSearchFrom(Pareto<S, XEv>& p, S& s, MultiEvaluation<>& sMev, paretoManager<S, XEv>& pManager, SOSC<>& stopCriteria) override
	{
		num_calls++;
		Timer t;

		int iter = 0;

		while ((iter < iterMax) && ((t.now() - stopCriteria.timelimit) < 0))
		{
			Move<S, XEv>* move = ns.randomMove(s);
			if (move->canBeApplied(s))
			{
				//Move and mark sMev as outdated
				Move<S, XEv>* mov_rev = move->apply(s);

				//Call method to reevaluate sMev and try to include TODO
//				pManager->addSolutionWithMEVReevaluation(p, *s,*sMev);

				pManager.addSolution(p, s);
				delete mov_rev->apply(s);
				delete mov_rev;

				//			vector<MoveCost<>*> vMoveCost;
//			for (int ev = 0; ev < v_e.size(); ev++)
//			{
//				vMoveCost.push_back(&v_e[ev].moveCost(sMev[ev], move, s));
//			}
//			bool itsWorthAdding = pManager.checkDominance(pManager.getParetoInsideManager(), &sMev);
//			if (itsWorthAdding)

			}
			delete move;

			iter++;
		}

		sum_time += t.inMilliSecs();
	}
	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (MOLocalSearch<S, XEv>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << MOLocalSearch<S, XEv>::idComponent() << "MO-RI";
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
