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

#include "../../SingleObjSearch.hpp"
#include "../../LocalSearch.hpp"

#include "GRASP.h"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class GRASP: public SingleObjSearch<R, ADS>
{
private:
	Evaluator<R, ADS>& evaluator;
	Constructive<R, ADS>& constructive;
	LocalSearch<R, ADS>& ls;
	unsigned int iterMax;

public:

	GRASP(Evaluator& _eval, Constructive<R, ADS>& _constructive, LocalSearch<R, ADS>& _ls, int _iterMax) :
		evaluator(_eval), constructive(_constructive), ls(_ls)
	{
	   iterMax = _iterMax;
	}

	virtual ~GRASP()
	{
	}

	pair<Solution<R, ADS>&, Evaluation<DS>&>* search(double timelimit = 100000000, double target_f = 0, const Solution<R, ADS>* _s = nullptr,  const Evaluation<DS>* _e = nullptr)
	{
		long tini = time(nullptr);

		unsigned int iter = 0;

		long tnow = time(nullptr);

		Solution<R, ADS>& s = constructive.generateSolution();
		Evaluation& e    = evaluator.evaluate(s);

		while ((iter < iterMax) && ((tnow - tini) < timelimit) && (evaluator.betterThan(target_f, e.evaluation())))
		{
			if(Component::verbose)
				cout << "GRASP::iter=" << iter << endl;

			Solution<R, ADS>& s1 = constructive.generateSolution();
			Evaluation<DS>& e1 = evaluator.evaluate(s1);

			ls.exec(s1,e1,timelimit, target_f);

			if(evaluator.betterThan(e1,e))
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

			tnow = time(nullptr);
			iter++;
		}

		return new pair<Solution<R, ADS>&, Evaluation<DS>&>(s, e);
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS, DS>::idComponent() << GRASPH::family() << "GRASP";
		return ss.str();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class GRASPBuilder : public GRASPH, public SingleObjSearchBuilder<R, ADS, DS>
{
public:
	virtual ~GRASPBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS, DS>* build(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		Evaluator<R, ADS, DS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		Constructive<R, ADS>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<R, ADS, DS>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<R, ADS, DS>* h = method.first;

		scanner = Scanner(method.second);

		if(!scanner.hasNext())
			return nullptr;

		int iterMax = scanner.nextInt();

		return new GRASP<R, ADS, DS>(*eval, *constructive, *h, iterMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(Constructive<R, ADS>::idComponent(), "constructive heuristic"));
		params.push_back(make_pair(LocalSearch<R, ADS, DS>::idComponent(), "local search"));
		params.push_back(make_pair("int", "max number of iterations"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == GRASPBuilder<R, ADS, DS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<R, ADS, DS>::idComponent() << GRASPH::family() << "GRASP";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_GRASP_HPP_*/
