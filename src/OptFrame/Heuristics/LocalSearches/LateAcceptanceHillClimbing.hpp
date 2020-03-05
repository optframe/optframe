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

#ifndef LATE_ACCEPTANCE_HILL_CLIMBING_HPP_
#define LATE_ACCEPTANCE_HILL_CLIMBING_HPP_

#include "../../LocalSearch.hpp"
#include "../../NS.hpp"
#include "../../Evaluator.hpp"

#define BRAND_NEW

namespace optframe
{

template<XSolution S, XEvaluation XEv=Evaluation<>>
class LateAcceptanceHillClimbing: public LocalSearch<S, XEv>
{
private:
	Evaluator<S, XEv>& ev;
	vector<NS<S, XEv>*> lns;
	int L;       // size of list
	int iterMax; // max iterations without improvement

public:

	LateAcceptanceHillClimbing(Evaluator<S, XEv>& _ev, NS<S, XEv>& _ns, int _L, int _iterMax) :
			ev(_ev), L(_L), iterMax(_iterMax)
	{
		lns.push_back(&_ns);
	}

	LateAcceptanceHillClimbing(Evaluator<S, XEv>& _ev, vector<NS<S, XEv>*> _lns, int _L, int _iterMax) :
			ev(_ev), lns(_lns), L(_L), iterMax(_iterMax)
	{
	}

	virtual ~LateAcceptanceHillClimbing()
	{
	}

	// DEPRECATED
	//virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
	//{
	//	Evaluation<> e = std::move(ev.evaluate(s));
	//	exec(s, e, stopCriteria);
	//}

	virtual void searchFrom(pair<S, XEv>& se, const StopCriteria<XEv>& sosc) override
	{
      S& sStar = se.first;
      XEv& eStar = se.second;
      double timelimit = sosc.timelimit;
      XEv target_f(sosc.target_f);

		long tini = time(nullptr);

#ifdef BRAND_NEW
		vector<Evaluation<>* > eList;
		for(int i=0; i<L; i++)
			eList.push_back(&eStar.clone());
#else
		vector<double > eList(L, eStar.evaluation());
#endif

		int iter = 1;
		unsigned index = 0;

		S& s = sStar.clone();
		Evaluation<>& e = eStar.clone();

		long tnow = time(nullptr);

		while (ev.betterThan(target_f, eStar) && (iter <= iterMax) && ((tnow - tini) < timelimit))
		{
			// choose random neighborhood
			int ns_k = rand() % lns.size();

			uptr<Move<S, XEv>> move = lns[ns_k]->validRandomMove(s);
	
			if (!move)
			{
				cout << "Warning in LAHC: cannot find an appliable move for neighborhood ";
				lns[ns_k]->print();
            // TODO: return FAIL here
			}

			if (move && move->canBeApplied(s))
			{
            bool mayEstimate = false;
				///MoveCost<>& cost = *ev.moveCost(*move, se, mayEstimate);
            op<XEv> cost = ev.moveCost(*move, se, mayEstimate);

				// test for current index
#ifdef BRAND_NEW
				if (ev.isImprovement(*cost, e, *eList[index]))
#else
				if (ev.betterThan(cost.cost()+e.evaluation(), eList[index]))
#endif
				{
					move->applyUpdate(se);
					ev.reevaluate(se);

#ifdef BRAND_NEW
					delete eList[index];
					eList[index] = &e.clone();
#else
					eList[index] = e.evaluation();
#endif

					if (ev.betterThan(e, eStar))
					{
						sStar = s;
						eStar = e;

						cout << "LAHC: best solution in iter=" << iter << " => ";
						e.print();

						iter = 0;
					}
				}
			}

			iter++;

			index++;
			if (index == eList.size())
				index = 0;

			tnow = time(nullptr);
		}

        delete& e;
        delete& s;

        // free list

#ifdef BRAND_NEW
        for (unsigned i = 0; i < eList.size(); i++)
			delete eList[i];
#endif

        eList.clear();
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<S, XEv>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<S, XEv>::idComponent() << ":LAHC";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "LAHC: [ ";
		for (unsigned i = 0; i < lns.size(); i++)
		{
			ss << lns[i]->toString();
			if (i != lns.size() - 1)
				ss << ",";
		}
		ss << "]";

		return ss.str();
	}
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class LateAcceptanceHillClimbingBuilder: public LocalSearchBuilder<S, XEv, X2ES>
{
public:
	virtual ~LateAcceptanceHillClimbingBuilder()
	{
	}

	virtual LocalSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		Evaluator<S, XEv>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<NS<S, XEv>*> nslist;
		hf.assignList(nslist, scanner.nextInt(), scanner.next()); // reads backwards!

		int L = scanner.nextInt();

		int iterMax = scanner.nextInt();

		return new LateAcceptanceHillClimbing<S, XEv>(*eval, nslist, L, iterMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<S, XEv>::idComponent(), "evaluation function"));
		stringstream ss;
		ss << NS<S, XEv>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of NS"));
		params.push_back(make_pair("OptFrame:int", "list size L"));
		params.push_back(make_pair("OptFrame:int", "iterMax iterations without improvement"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == LateAcceptanceHillClimbing<S, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<S, XEv>::idComponent() << ":LAHC";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* LATE_ACCEPTANCE_HILL_CLIMBING_HPP_ */
