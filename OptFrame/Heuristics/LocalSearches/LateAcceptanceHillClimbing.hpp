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

#ifndef LATE_ACCEPTANCE_HILL_CLIMBING_HPP_
#define LATE_ACCEPTANCE_HILL_CLIMBING_HPP_

#include "../../LocalSearch.hpp"
#include "../../NS.hpp"
#include "../../Evaluator.hpp"

#define BRAND_NEW

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class LateAcceptanceHillClimbing: public LocalSearch<R, ADS, DS>
{
private:
	Evaluator<R, ADS, DS>& ev;
	vector<NS<R, ADS, DS>*> lns;
	int L;       // size of list
	int iterMax; // max iterations without improvement
	int limit;   // max number of moves without being appliable

public:

	LateAcceptanceHillClimbing(Evaluator<R, ADS, DS>& _ev, NS<R, ADS, DS>& _ns, int _L, int _iterMax, int _limit) :
			ev(_ev), L(_L), iterMax(_iterMax), limit(_limit)
	{
		lns.push_back(&_ns);
	}

	LateAcceptanceHillClimbing(Evaluator<R, ADS, DS>& _ev, vector<NS<R, ADS, DS>*> _lns, int _L, int _iterMax, int _limit) :
			ev(_ev), lns(_lns), L(_L), iterMax(_iterMax), limit(_limit)
	{
	}

	virtual ~LateAcceptanceHillClimbing()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation<DS>& e = ev.evaluate(s);

		exec(s, e, timelimit, target_f);

		delete &e;
	}

	virtual void exec(Solution<R, ADS>& sStar, Evaluation<DS>& eStar, double timelimit, double target_f)
	{
		long tini = time(NULL);

#ifdef BRAND_NEW
		vector<Evaluation<DS>* > eList;
		for(int i=0; i<L; i++)
			eList.push_back(&eStar.clone());
#else
		vector<double > eList(L, eStar.evaluation());
#endif

		int iter = 1;
		unsigned index = 0;

		Solution<R, ADS>& s = sStar.clone();
		Evaluation<DS>& e = eStar.clone();

		long tnow = time(NULL);

		while (ev.betterThan(target_f, eStar.evaluation()) && (iter <= iterMax) && ((tnow - tini) < timelimit))
		{
			// choose random neighborhood
			int ns_k = rand() % lns.size();

			Move<R, ADS, DS>* move = &lns[ns_k]->move(s);
			int move_tries = 1;
			while ((!move->canBeApplied(s)) && (move_tries <= limit))
			{
				delete move;
				move = &lns[ns_k]->move(s);
				move_tries++;
			}

			if (move_tries == limit)
			{
				cout << "Warning in LAHC: cannot find an appliable move (in " << limit << " tries) for neighborhood ";
				lns[ns_k]->print();
			}

			if (move->canBeApplied(s))
			{
				MoveCost& cost = ev.moveCost(e, *move, s);

				// test for current index
#ifdef BRAND_NEW
				if (ev.isImprovement(cost, e, *eList[index]))
#else
				if (ev.betterThan(cost.cost()+e.evaluation(), eList[index]))
#endif
				{
					delete &move->apply(e, s);
					ev.evaluate(e, s);

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

				delete& cost;
			}

            delete move;

			iter++;

			index++;
			if (index == eList.size())
				index = 0;

			tnow = time(NULL);
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
		return (s == idComponent()) || (LocalSearch<R, ADS, DS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS, DS>::idComponent() << "LAHC";
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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class LateAcceptanceHillClimbingBuilder: public LocalSearchBuilder<R, ADS, DS>
{
public:
	virtual ~LateAcceptanceHillClimbingBuilder()
	{
	}

	virtual LocalSearch<R, ADS, DS>* build(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		Evaluator<R, ADS, DS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<NS<R, ADS, DS>*> nslist;
		hf.assignList(nslist, scanner.nextInt(), scanner.next()); // reads backwards!

		int L = scanner.nextInt();

		int iterMax = scanner.nextInt();

		int limit = scanner.nextInt();

		return new LateAcceptanceHillClimbing<R, ADS, DS>(*eval, nslist, L, iterMax, limit);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));
		stringstream ss;
		ss << NS<R, ADS, DS>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of NS's"));
		params.push_back(make_pair("int", "L: list size"));
		params.push_back(make_pair("int", "iterMax: max iterations without improvement"));
		params.push_back(make_pair("int", "limit: max moves that cannot be applied"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == LateAcceptanceHillClimbing<R, ADS, DS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, DS>::idComponent() << "LAHC";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* LATE_ACCEPTANCE_HILL_CLIMBING_HPP_ */
