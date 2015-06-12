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

#ifndef OPTFRAME_GRASP_HPP_
#define OPTFRAME_GRASP_HPP_

#include "../../SingleObjSearch.hpp"
#include "../../LocalSearch.hpp"

#include "GRASPFamily.h"
#include "GRConstructive.h"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicGRASP: public SingleObjSearch<R, ADS>, public GRASP
{
private:
	Evaluator<R, ADS>& evaluator;
	GRConstructive<R, ADS>& constructive;
	LocalSearch<R, ADS>& ls;
	float alpha;
	unsigned int iterMax;

public:

	BasicGRASP(Evaluator<R, ADS>& _eval, GRConstructive<R, ADS>& _constructive, LocalSearch<R, ADS>& _ls, float _alpha, int _iterMax) :
			evaluator(_eval), constructive(_constructive), ls(_ls)
	{
		if (_iterMax <= 0)
			_iterMax = 1;
		if (_alpha < 0 || _alpha > 1)
			_alpha = 1;
		alpha = _alpha;
		iterMax = _iterMax;
	}

	virtual ~BasicGRASP()
	{
	}

	pair<Solution<R, ADS>&, Evaluation&>* search(double timelimit = 100000000, double target_f = 0, const Solution<R, ADS>* _s = NULL, const Evaluation* _e = NULL)
	{
		long tini = time(NULL);

		unsigned int iter = 0;

		long tnow = time(NULL);

		Solution<R, ADS>& s = constructive.generateSolution(alpha);
		Evaluation& e = evaluator.evaluate(s);

		if (Component::information)
			e.print();

		while ((iter < iterMax) && ((tnow - tini) < timelimit) && (evaluator.betterThan(target_f, e.evaluation())))
		{
			if (Component::verbose)
				cout << "GRASP::iter=" << iter << endl;

			Solution<R, ADS>& s1 = constructive.generateSolution(alpha);
			Evaluation& e1 = evaluator.evaluate(s1);

			ls.exec(s1, e1, timelimit, target_f);

			if (evaluator.betterThan(e1, e))
			{
				s = s1;
				e = e1;
				if (Component::information)
				{
					cout << "GRASP iter " << iter << ": ";
					e.print();
				}
			}

			delete &s1;
			delete &e1;

			tnow = time(NULL);
			iter++;
		}

		return new pair<Solution<R, ADS>&, Evaluation&>(s, e);
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS>::idComponent() << ":" << GRASP::family() << ":BasicGRASP";
		return ss.str();

	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class BasicGRASPBuilder: public GRASP, public SingleObjSearchBuilder<R, ADS>
{
public:
	virtual ~BasicGRASPBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		GRConstructive<R, ADS>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<R, ADS>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<R, ADS>* h = method.first;

		scanner = Scanner(method.second);

		if (!scanner.hasNext())
			return NULL;

		float alpha = scanner.nextFloat();

		if (!scanner.hasNext())
			return NULL;

		int iterMax = scanner.nextInt();

		return new BasicGRASP<R, ADS>(*eval, *constructive, *h, alpha, iterMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(GRConstructive<R, ADS>::idComponent(), "greedy randomized constructive heuristic"));
		params.push_back(make_pair(LocalSearch<R, ADS>::idComponent(), "local search"));
		params.push_back(make_pair("OptFrame:float", "alpha parameter [0,1]"));
		params.push_back(make_pair("OptFrame:int", "max number of iterations"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicGRASPBuilder<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<R, ADS>::idComponent() << ":" << GRASP::family() << ":BasicGRASP";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_GRASP_HPP_*/
