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

#ifndef OPTFRAME_CS_HPP_
#define OPTFRAME_CS_HPP_

#include "../../LocalSearch.hpp"
#include "../../NSEnum.hpp"
#include "../../Evaluator.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class CircularSearch: public LocalSearch<R, ADS>
{
private:
	Evaluator<R, ADS>& eval;
	NSEnum<R, ADS>& ns;

	int initial_w;

public:

	CircularSearch(Evaluator<R, ADS>& _eval, NSEnum<R, ADS>& _nsEnum) :
			eval(_eval), ns(_nsEnum)
	{
		initial_w = 0;
	}

	virtual ~CircularSearch()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation& e = eval.evaluate(s);
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation& e, double timelimit, double target_f)
	{
		int Wmax = ns.size();

		int w = initial_w % Wmax;

		do
		{
			Move<R, ADS>& m = ns.move(w);

			if (m.canBeApplied(s))
			{
				MoveCost& cost = eval.moveCost(e, m, s);

				if (eval.isImprovement(cost))
				{
					//double old_f = e.evaluation();

					Component::safe_delete(m.apply(e, s));
					eval.evaluate(e, s); // updates 'e'

					//cout << "CS improvement! w:" << w << " fo=" << e.evaluation() << " (antiga fo="<< old_f << ")" << endl << endl;

					initial_w = w + 1;

					delete &cost;
					delete &m;
					return;
				}

				delete &cost;
			}

			delete &m;

			w = (w + 1) % Wmax;
		}
		while (w != initial_w);
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS>::idComponent() << ":CS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class CircularSearchBuilder: public LocalSearchBuilder<R, ADS>
{
public:
	virtual ~CircularSearchBuilder()
	{
	}

	virtual LocalSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NSEnum<R, ADS>* nsenum;
		hf.assign(nsenum, scanner.nextInt(), scanner.next()); // reads backwards!

		return new CircularSearch<R, ADS>(*eval, *nsenum);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NSEnum<R, ADS>::idComponent(), "neighborhood structure"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == CircularSearch<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS>::idComponent() << ":CS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_CS_HPP_*/
