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

#ifndef OPTFRAME_EXTENDED_MULTI_OBJ_SEARCH_HPP_
#define OPTFRAME_EXTENDED_MULTI_OBJ_SEARCH_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Solution.hpp"
#include "Population.hpp"
#include "Evaluation.hpp"

#include "MultiEvaluator.hpp"

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "MultiObjSearch.hpp"

namespace optframe
{

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS>
class ExtendedPareto
{
private:
	vector<Solution<R>*> paretoSet;
	vector<vector<MultiEvaluation*> > paretoFront;
	vector<Population<X, ADS>*> decodedSolutions;

public:

	ExtendedPareto()
	{
	}

	virtual ~ExtendedPareto()
	{
		for(unsigned i = 0; i < paretoSet.size(); i++)
			delete paretoSet[i];
		paretoSet.clear();

		for(unsigned i = 0; i < paretoFront.size(); i++)
		{
			for(unsigned j = 0; j < paretoFront[i].size(); j++)
				delete paretoFront[i][j];
			paretoFront[i].clear();
		}
		paretoFront.clear();

		for(unsigned i = 0; i < decodedSolutions.size(); i++)
			delete decodedSolutions[i];
		decodedSolutions.clear();
	}

	Pareto<X>* getPareto()
	{
		return nullptr;
	}

	void push_back(Solution<R>* s, vector<MultiEvaluation*>& v_e, Population<X, ADS>* v_x)
	{
		paretoSet.push_back(s);
		paretoFront.push_back(v_e);
		decodedSolutions.push_back(v_x);
	}

	void push_back(const Solution<R, ADS>& s, const vector<MultiEvaluation*>& v_e, const Population<X, ADS>& v_x)
	{
		paretoSet.push_back(&s->clone());
		vector<MultiEvaluation*> ve;
		for(unsigned mev = 0; mev < v_e.size(); mev++)
			ve.push_back(&v_e[mev]->clone());
		paretoFront.push_back(ve);
		decodedSolutions.push_back(&v_x.clone());
	}

	unsigned size()
	{
		return paretoSet.size();
	}

	pair<Solution<R>*, pair<vector<MultiEvaluation*>, vector<Population<X, ADS>*> > > erase(unsigned index)
	{
		vector<MultiEvaluation*> vme = paretoFront.at(index);
		Population<X, ADS>* pop = decodedSolutions.at(index);
		pair<vector<MultiEvaluation*>, Population<X, ADS>*> p1 = make_pair(vme, pop);
		pair<Solution<R>*, pair<vector<MultiEvaluation*>, Population<X, ADS>*> > p;
		p = make_pair(paretoSet.at(index), p1);
		paretoSet.erase(paretoSet.begin() + index);
		paretoSet.erase(paretoFront.begin() + index);
		decodedSolutions.erase(decodedSolutions.begin() + index);

		return p;
	}

	pair<Solution<R>*, pair<vector<MultiEvaluation*>, Population<X, ADS>*> > at(unsigned index)
	{
		vector<MultiEvaluation*> vme = paretoFront.at(index);
		Population<X, ADS>* pop = decodedSolutions.at(index);
		pair<vector<MultiEvaluation*>, Population<X, ADS>*> p1 = make_pair(vme, pop);
		return make_pair(paretoSet.at(index), p1);
	}

	vector<Solution<R, ADS>*> getParetoSet()
	{
		return paretoSet;
	}

	vector<vector<Evaluation*> > getParetoFront()
	{
		return paretoFront;
	}

	void print() const
	{
		cout << "ExtendedPareto size=" << paretoFront.size();
		cout << endl;
	}

};

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS>
class ExtendedMultiObjSearch: public Component
{
public:

	ExtendedMultiObjSearch()
	{
	}

	virtual ~ExtendedMultiObjSearch()
	{
	}

	virtual ExtendedPareto<R, X, ADS>* search(double timelimit = 100000000, double target_f = 0, ExtendedPareto<R, X, ADS>* _pf = nullptr) = 0;

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
		ss << Component::idComponent() << "ExtendedMultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS>
class ExtendedMultiObjSearchBuilder: public ComponentBuilder<R, ADS>
{
public:
	virtual ~ExtendedMultiObjSearchBuilder()
	{
	}

	virtual ExtendedMultiObjSearch<R, X, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS>::idComponent() << "ExtendedMultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* OPTFRAME_EXTENDED_MULTI_OBJ_SEARCH_HPP_ */
