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

#ifndef OPTFRAME_MULTI_OBJ_SEARCH_HPP_
#define OPTFRAME_MULTI_OBJ_SEARCH_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Solution.hpp"
//#include "Population.hpp"
#include "Evaluation.hpp"
#include "Direction.hpp"

#include "MultiEvaluator.hpp"

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "ParetoDominance.hpp"
#include "ParetoDominanceWeak.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class Pareto
{
private:
	vector<Solution<R, ADS>*> paretoSet;
	vector<vector<Evaluation*> > paretoFront;

public:

	Pareto()
	{
	}

	virtual ~Pareto()
	{
	}

	void push_back(Solution<R, ADS>& s, vector<Evaluation*>& v_e)
	{
		paretoSet.push_back(&s);
		paretoFront.push_back(v_e);
	}

	void push_back(Solution<R, ADS>* s, MultiEvaluation* mev)
	{
		paretoSet.push_back(s);
		vector<Evaluation*> v_e = mev->getCloneVector();
		paretoFront.push_back(v_e);
		delete mev;
	}

	unsigned size()
	{
		return paretoSet.size();
	}

	pair<Solution<R, ADS>&, vector<Evaluation*> > erase(unsigned index)
	{
		pair<Solution<R, ADS>&, vector<Evaluation*> > p(*paretoSet.at(index), paretoFront.at(index));
		paretoSet.erase(paretoSet.begin() + index);
		paretoSet.erase(paretoFront.begin() + index);
		return p;
	}

	pair<Solution<R, ADS>&, vector<Evaluation*> > at(unsigned index)
	{
		return make_pair(*paretoSet.at(index), paretoFront.at(index));
	}

	vector<Solution<R, ADS>*> getParetoSet()
	{
		return paretoSet;
	}

	vector<vector<Evaluation*> > getParetoFront()
	{
		return paretoFront;
	}

	static vector<MultiEvaluation*> filterDominated(vector<Direction*>& vdir, const vector<MultiEvaluation*>& candidates)
	{
		vector<MultiEvaluation*> nonDom;

		ParetoDominance<R, ADS> pDom(vdir);
		ParetoDominanceWeak<R, ADS> pDomWeak(vdir);

		for(unsigned i = 0; i < candidates.size(); i++)
			addSolution(pDom, pDomWeak, nonDom, candidates[i]);

		return nonDom;
	}



	// T. Lust et al (method addSolution)

	// class T must be handled by ParetoDominance operators (candidate: vector<double>, vector<Evaluation*>, MultiEvaluation*)

	template<class T>
	static bool addSolution(vector<Direction*>& vDir, vector<T*>& nonDom, T* candidate)
	{
		ParetoDominance<R, ADS> dom(vDir);
		ParetoDominanceWeak<R, ADS> domWeak(vDir);
		return addSolution(dom, domWeak, nonDom, candidate);
	}

	template<class T>
	static bool addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, vector<T*>& nonDom, T* candidate)
	{
		for(int ind = 0; ind < nonDom.size(); ind++)
		{
			if(domWeak.dominates(*nonDom.at(ind), *candidate))
				return false;

			if(dom.dominates(*candidate, *nonDom.at(ind)))
			{
				nonDom.erase(nonDom.begin() + ind);
				ind--;
			}
		}

		nonDom.push_back(candidate);
		return true;
	}

	static vector<pair<Solution<R>*, MultiEvaluation*> > filterDominated(vector<Direction*>& vdir, const vector<pair<Solution<R>*, MultiEvaluation*> >& candidates)
	{
		vector<pair<Solution<R>*, MultiEvaluation*> > nonDom;

		ParetoDominance<R, ADS> pDom(vdir);
		ParetoDominanceWeak<R, ADS> pDomWeak(vdir);

		for(unsigned i = 0; i < candidates.size(); i++)
			addSolution(pDom, pDomWeak, nonDom, candidates[i]);

		return nonDom;
	}

	static void addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, vector<pair<Solution<R>*, MultiEvaluation*> >& nonDom, pair<Solution<R>*, MultiEvaluation*> candidate)
	{
		for(int ind = 0; ind < nonDom.size(); ind++)
		{
			if(domWeak.dominates(*nonDom.at(ind).second, *candidate.second))
				return;

			if(dom.dominates(*candidate.second, *nonDom.at(ind).second))
			{
				nonDom.erase(nonDom.begin() + ind);
				ind--;
			}
		}

		nonDom.push_back(candidate);
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MultiObjSearch: public Component
{
public:

	MultiObjSearch()
	{
	}

	virtual ~MultiObjSearch()
	{
	}

	virtual Pareto<R, ADS>* search(double timelimit = 100000000, double target_f = 0, Pareto<R, ADS>* _pf = NULL) = 0;

	virtual string log()
	{
		return "Empty heuristic log.";
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << "MultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MultiObjSearchBuilder: public ComponentBuilder<R, ADS>
{
public:
	virtual ~MultiObjSearchBuilder()
	{
	}

	virtual MultiObjSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS>::idComponent() << "MultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* OPTFRAME_MULTI_OBJ_SEARCH_HPP_ */
