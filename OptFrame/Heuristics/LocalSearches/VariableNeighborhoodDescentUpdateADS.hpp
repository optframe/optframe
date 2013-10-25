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

#ifndef VARIABLENEIGHBORHOODDESCENTUPDATEADS_HPP_
#define VARIABLENEIGHBORHOODDESCENTUPDATEADS_HPP_

#include "../../LocalSearch.hpp"
#include "../../NSEnum.hpp"
#include "../../Evaluator.hpp"

#include "VND.h"

namespace optframe
{

template<class R, class ADS, class DS = OPTFRAME_DEFAULT_DS>
class VariableNeighborhoodDescentUpdateADS: public LocalSearch<R, ADS, DS>
{
private:
	Evaluator<R, ADS, DS>& ev;
	ADSManager<R, ADS>& adsMan;
	vector<LocalSearch<R, ADS, DS>*> lsList;
public:

	VariableNeighborhoodDescentUpdateADS(Evaluator<R, ADS, DS>& _ev, ADSManager<R, ADS>& _adsMan, vector<LocalSearch<R, ADS, DS>*> _lsList) :
		ev(_ev), adsMan(_adsMan), lsList(_lsList)
	{
	}

	virtual ~VariableNeighborhoodDescentUpdateADS()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation<DS>& e = ev.evaluate(s);

		exec(s, e, timelimit, target_f);

		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation<DS>& e, double timelimit, double target_f)
	{
		long tini = time(NULL);

		int r = lsList.size();

		int k = 1;

		long tnow = time(NULL);
		while (ev.betterThan(target_f, e.evaluation()) && (k <= r) && ((tnow - tini) < timelimit))
		{
			Solution<R, ADS>* s0 = &s.clone();
			Evaluation<DS>* e0 = &e.clone();

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
				//Find move ID
				string localSearchID = lsList[k - 1]->toString();
				unsigned found = localSearchID.find("OptFrame");
				string moveID = localSearchID.substr(found);
				adsMan.setNeighLocalOptimum(s, moveID);

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
		return (s == idComponent()) || (LocalSearch<R, ADS, DS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS, DS>::idComponent() << "VND";
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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class VariableNeighborhoodDescentUpdateADSBuilder: public LocalSearchBuilder<R, ADS, DS>
{
public:
	virtual ~VariableNeighborhoodDescentUpdateADSBuilder()
	{
	}

	virtual LocalSearch<R, ADS, DS>* build(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		Evaluator<R, ADS, DS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		ADSManager<R, ADS>* adsMan;
		hf.assign(adsMan, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<LocalSearch<R, ADS, DS>*> hlist;
		hf.assignList(hlist, scanner.nextInt(), scanner.next()); // reads backwards!

		return new VariableNeighborhoodDescentUpdateADS<R, ADS, DS> (*eval, *adsMan, hlist);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));

		params.push_back(make_pair(ADSManager<R, ADS>::idComponent(), "ADSManager function"));

		stringstream ss;
		ss << LocalSearch<R, ADS, DS>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of local searches"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == VariableNeighborhoodDescentUpdateADS<R, ADS, DS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, DS>::idComponent() << VND::family() << "VNDUpdateADS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*VARIABLENEIGHBORHOODDESCENTUPDATEADS_HPP_*/
