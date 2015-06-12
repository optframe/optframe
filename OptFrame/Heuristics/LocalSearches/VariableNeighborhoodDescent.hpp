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

#ifndef VARIABLENEIGHBORHOODDESCENT_HPP_
#define VARIABLENEIGHBORHOODDESCENT_HPP_

#include "../../LocalSearch.hpp"
#include "../../NSEnum.hpp"
#include "../../Evaluator.hpp"

#include "VND.h"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class VariableNeighborhoodDescent: public LocalSearch<R, ADS>
{
public:

	VariableNeighborhoodDescent(Evaluator<R, ADS>& _ev, vector<LocalSearch<R, ADS>*> _lsList) :
		ev(_ev), lsList(_lsList)
	{
	}

	virtual ~VariableNeighborhoodDescent()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation& e = ev.evaluate(s);

		exec(s, e, timelimit, target_f);

		delete &e;
	}


	virtual void exec(Solution<R, ADS>& s, Evaluation& e, double timelimit, double target_f)
	{
		long tini = time(NULL);

		int r = lsList.size();

		int k = 1;

		long tnow = time(NULL);
		while (ev.betterThan(target_f, e.evaluation()) && (k <= r) && ((tnow - tini) < timelimit))
		{
			Solution<R, ADS>* s0 = &s.clone();
			Evaluation* e0 = &e.clone();

			lsList[k - 1]->exec(*s0, *e0, timelimit, target_f);
			if (ev.betterThan(*s0, s))
			{
				s = *s0;
				e = *e0;
				delete s0;
				delete e0;
				k = 1;
			}
			else
			{
				delete s0;
				delete e0;
				k = k + 1;
			}
			ev.evaluate(e, s);

			tnow = time(NULL);
		}

	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS>::idComponent() << ":VND";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "VND: [ ";
		for(unsigned i=0; i<lsList.size(); i++)
		{
			ss << lsList[i]->toString();
			if(i != lsList.size()-1)
				ss << ",";
		}
		ss << "]";

		return ss.str();
	}

private:
	Evaluator<R, ADS>& ev;
	vector<LocalSearch<R, ADS>*> lsList;
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class VariableNeighborhoodDescentBuilder : public LocalSearchBuilder<R, ADS>
{
public:
	virtual ~VariableNeighborhoodDescentBuilder()
	{
	}

	virtual LocalSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<LocalSearch<R, ADS>*> hlist;
		hf.assignList(hlist, scanner.nextInt(), scanner.next()); // reads backwards!

		return new VariableNeighborhoodDescent<R, ADS>(*eval, hlist);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		stringstream ss;
		ss << LocalSearch<R, ADS>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of local searches"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == VariableNeighborhoodDescent<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS>::idComponent() << ":VND";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*VARIABLENEIGHBORHOODDESCENT_HPP_*/
