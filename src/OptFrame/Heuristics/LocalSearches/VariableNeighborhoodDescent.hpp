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

#ifndef OPTFRAME_VARIABLENEIGHBORHOODDESCENT_HPP_
#define OPTFRAME_VARIABLENEIGHBORHOODDESCENT_HPP_

#include "../../LocalSearch.hpp"
#include "../../NSEnum.hpp"
#include "../../Evaluator.hpp"
#include "../../RandGen.hpp"

#include "VND.h"

namespace optframe
{

//When RandGen is given as parameter it performs RVND
template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class VariableNeighborhoodDescent: public LocalSearch<R, ADS>
{
private:
	Evaluator<R, ADS>& ev;
	vector<LocalSearch<R, ADS>*> lsList;
	RandGen* rg;

public:

	VariableNeighborhoodDescent(Evaluator<R, ADS>& _ev, vector<LocalSearch<R, ADS>*> _lsList, RandGen* _rg = nullptr) :
			ev(_ev), lsList(_lsList), rg(_rg)
	{
	}

	virtual ~VariableNeighborhoodDescent()
	{
	}

	virtual void exec(Solution<R, ADS>& s, SOSC& stopCriteria)
	{
		Evaluation e = std::move(ev.evaluateSolution(s));

		exec(s, e, stopCriteria);
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation& e, SOSC& stopCriteria)
	{
		if (Component::information)
			cout << "VND::starts" << endl;

		Timer tNow;

		if (rg)
			rg->shuffle(lsList); // shuffle elements

		int r = lsList.size();

		int k = 1;

		Evaluation eCurrent(e);
		while (ev.betterThan(stopCriteria.target_f, e.evaluation()) && (k <= r) && (tNow.now() < stopCriteria.timelimit))
		{
			eCurrent = e;
			SOSC stopCriteriaNextLS = stopCriteria;
			stopCriteriaNextLS.updateTimeLimit(tNow.now());
			lsList[k - 1]->exec(s, e, stopCriteriaNextLS);

			if (ev.betterThan(e, eCurrent))
			{
				k = 1;
				if (Component::information)
					e.print();
			}
			else
			{
				k = k + 1;

				if (Component::information)
					cout << "VND::k=" << k << endl;
			}
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
		for (unsigned i = 0; i < lsList.size(); i++)
		{
			ss << lsList[i]->toString();
			if (i != lsList.size() - 1)
				ss << ",";
		}
		ss << "]";

		return ss.str();
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class VariableNeighborhoodDescentBuilder: public LocalSearchBuilder<R, ADS>
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

#endif /*OPTFRAME_VARIABLENEIGHBORHOODDESCENT_HPP_*/
