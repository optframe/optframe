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

#ifndef VARIABLENEIGHBORHOODDESCENT_HPP_
#define VARIABLENEIGHBORHOODDESCENT_HPP_

#include "../LocalSearch.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class VariableNeighborhoodDescent: public LocalSearch<R, ADS, M>
{
public:

	VariableNeighborhoodDescent(Evaluator<R, ADS, M>& _ev, vector<LocalSearch<R, ADS, M>*> _lsList) :
		ev(_ev), lsList(_lsList)
	{
	}

	virtual ~VariableNeighborhoodDescent()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = ev.evaluate(s.getR());

		exec(s, e, timelimit, target_f);

		delete &e;
	}


	virtual void exec(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit, double target_f)
	{

		long tini = time(NULL);

		int r = lsList.size();

		int k = 1;

		long tnow = time(NULL);
		while (ev.betterThan(target_f, e.evaluation()) && (k <= r) && ((tnow - tini) < timelimit))
		{
			Solution<R, ADS>* s0 = &s.clone();
			Evaluation<M>* e0 = &e.clone();

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
		return (s == idComponent()) || (LocalSearch<R, ADS, M>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS, M>::idComponent() << "VND";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

private:
	Evaluator<R, ADS, M>& ev;
	vector<LocalSearch<R, ADS, M>*> lsList;
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class VariableNeighborhoodDescentBuilder : public LocalSearchBuilder<R, ADS, M>
{
public:
	virtual ~VariableNeighborhoodDescentBuilder()
	{
	}

	virtual LocalSearch<R, ADS, M>* build(Scanner& scanner, HeuristicFactory<R, ADS, M>& hf, string family = "")
	{
		Evaluator<R, ADS, M>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<LocalSearch<R, ADS, M>*> hlist;
		hf.assignList(hlist, scanner.nextInt(), scanner.next()); // reads backwards!

		return new VariableNeighborhoodDescent<R, ADS, M>(*eval, hlist);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, M>::idComponent(), "evaluation function"));
		stringstream ss;
		ss << "[]" << LocalSearch<R, ADS, M>::idComponent();
		params.push_back(make_pair(ss.str(), "list of local searches"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == VariableNeighborhoodDescent<R, ADS, M>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, M>::idComponent() << "VND";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


#endif /*VARIABLENEIGHBORHOODDESCENT_HPP_*/
