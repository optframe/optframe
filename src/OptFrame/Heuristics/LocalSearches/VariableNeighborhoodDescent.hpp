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
template<XESolution XES, XEvaluation XEv = Evaluation<>>
class VariableNeighborhoodDescent: public LocalSearch<XES, XEv>
{
private:
	sref<GeneralEvaluator<XES, XEv>> ev;
	vsref<LocalSearch<XES, XEv>> lsList;
	sptr<RandGen> rg;

public:

	VariableNeighborhoodDescent(sref<GeneralEvaluator<XES, XEv>> _ev, vsref<LocalSearch<XES, XEv>> _lsList, sptr<RandGen> _rg = nullptr) :
			ev(_ev), lsList(_lsList), rg(_rg)
	{
	}

	virtual ~VariableNeighborhoodDescent()
	{
	}

   // DEPRECATED
	//virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
	//{
	//	Evaluation<> e = std::move(ev.evaluate(s));
	//	exec(s, e, stopCriteria);
	//}

	virtual SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& stopCriteria) override
	{
      //S& s = se.first;
      //XEv& e = se.second;

		if (Component::information)
			cout << "VND::starts" << endl;

		Timer tNow;

		if (rg)
			rg->shuffle(lsList); // shuffle elements

		int r = lsList.size();

		int k = 1;

      XES current(se); // full backup! TODO: remove this copy
		//Evaluation<> eCurrent(e);
      //'target_f' will crash if not provided... removing
		while ((k <= r) && !stopCriteria.shouldStop())// && (ev.betterThan(stopCriteria.target_f, e))
		{
			//eCurrent = e; // backup
         current = se; // TODO: remove this copy

			StopCriteria<XEv> stopCriteriaNextLS = stopCriteria;
			stopCriteriaNextLS.updateTimeLimit(tNow.now());
			lsList[k - 1]->searchFrom(se, stopCriteriaNextLS);

			//if (ev.betterThan(e, eCurrent))
         //if (ev.betterThan(se, current))
         //if (se.second.betterStrict(current.second))
         std::cout << "VND will compare(" << se.second.outdated << ";" << current.second.outdated << ")" << std::endl;
         if (ev->betterStrict(se.second, current.second))
			{
            // improvement
				k = 1;
				if (Component::information)
					//e.print();
               se.second.print();
			}
			else
			{
				k = k + 1;

				if (Component::information)
					cout << "VND::k=" << k << endl;
			}
		}
      return SearchStatus::NO_REPORT;
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<XES, XEv>::idComponent() << ":VND";
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
         auto& x = const_cast<sref<LocalSearch<XES, XEv>>&>(lsList[i]);
			//ss << lsList[i]->toString();
         ss << x->toString();
			if (i != lsList.size() - 1)
				ss << ",";
		}
		ss << "]";

		return ss.str();
	}

};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class VariableNeighborhoodDescentBuilder: public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
	virtual ~VariableNeighborhoodDescentBuilder()
	{
	}

	virtual LocalSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		sptr<GeneralEvaluator<XES, XEv>> eval;
		hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

		vsptr<LocalSearch<XES, XEv>> _hlist;
		hf.assignList(_hlist, *scanner.nextInt(), scanner.next()); // reads backwards!
      vsref<LocalSearch<XES, XEv>> hlist;
      for(auto x : _hlist)
         hlist.push_back(x);

		return new VariableNeighborhoodDescent<XES, XEv>(eval, hlist);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
		stringstream ss;
		ss << LocalSearch<XES, XEv>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of local searches"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == VariableNeighborhoodDescent<XES, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<S, XEv>::idComponent() << ":VND";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_VARIABLENEIGHBORHOODDESCENT_HPP_*/
