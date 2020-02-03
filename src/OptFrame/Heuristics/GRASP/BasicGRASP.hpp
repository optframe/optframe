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

#ifndef OPTFRAME_BASIC_GRASP_HPP_
#define OPTFRAME_BASIC_GRASP_HPP_

#include "../../SingleObjSearch.hpp"
#include "../../LocalSearch.hpp"
#include "../../Timer.hpp"

#include "GRASPFamily.h"
#include "GRConstructive.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>>
class BasicGRASP: public SingleObjSearch<R, ADS, S>, public GRASP
{
private:
	Evaluator<R, ADS, S>& evaluator;
	GRConstructive<S>& constructive;
	LocalSearch<R, ADS, S>& ls;
	float alpha;
	unsigned int iterMax;

public:

	BasicGRASP(Evaluator<R, ADS, S>& _eval, GRConstructive<S>& _constructive, LocalSearch<R, ADS, S>& _ls, float _alpha, int _iterMax) :
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

	pair<S, Evaluation>* search(SOSC& stopCriteria, const S* _s = nullptr, const Evaluation* _e = nullptr) override
	{
		double timelimit = stopCriteria.timelimit;
		double target_f = stopCriteria.target_f;

		Timer tNow;

		unsigned int iter = 0;

		S* s = constructive.generateGRSolution(alpha, timelimit);
		Evaluation e = evaluator.evaluateSolution(*s);

		if (Component::information)
			e.print();

		while ((iter < iterMax) && (tNow.now() < timelimit) && (evaluator.betterThan(target_f, e.evaluation())))
		{
			if (Component::verbose)
				cout << "GRASP::iter=" << iter << endl;

			S* s1 = constructive.generateGRSolution(alpha,timelimit - tNow.now());

			Evaluation e1 = evaluator.evaluateSolution(*s1);

			SOSC stopCriteriaLS = stopCriteria.newStopCriteriaWithTL(tNow.now());
			ls.exec(*s1, e1, stopCriteriaLS);

			if (evaluator.betterThan(e1, e))
			{
				(*s) = std::move(*s1);
				delete s1;
				e = std::move(e1);
				if (Component::information)
				{
					cout << "GRASP iter " << iter << ": ";
					e.print();
				}
			}

			iter++;
		}
		S sFinal = std::move(*s);
		delete s;

		return new pair<S, Evaluation>(sFinal, e);
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS, S>::idComponent() << ":" << GRASP::family() << ":BasicGRASP";
		return ss.str();

	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>>
class BasicGRASPBuilder: public GRASP, public SingleObjSearchBuilder<R, ADS, S>
{
public:
	virtual ~BasicGRASPBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS, S>* build(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
	{
		Evaluator<R, ADS, S>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		GRConstructive<S>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<R, ADS, S>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<R, ADS, S>* h = method.first;

		scanner = Scanner(method.second);

		if (!scanner.hasNext())
			return nullptr;

		float alpha = scanner.nextFloat();

		if (!scanner.hasNext())
			return nullptr;

		int iterMax = scanner.nextInt();

		return new BasicGRASP<R, ADS, S>(*eval, *constructive, *h, alpha, iterMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, S>::idComponent(), "evaluation function"));
		params.push_back(make_pair(GRConstructive<S>::idComponent(), "greedy randomized constructive heuristic"));
		params.push_back(make_pair(LocalSearch<R, ADS, S>::idComponent(), "local search"));
		params.push_back(make_pair("OptFrame:float", "alpha parameter [0,1]"));
		params.push_back(make_pair("OptFrame:int", "max number of iterations"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicGRASPBuilder<R, ADS, S>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<R, ADS, S>::idComponent() << ":" << GRASP::family() << ":BasicGRASP";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_BASIC_GRASP_HPP_*/
