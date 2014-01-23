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

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ExtendedPareto
{
private:
	vector<Solution<R, ADS>*> paretoSet;
	vector<vector<MultiEvaluation<DS>*> > paretoFront;

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
	}

	void push_back(Solution<R, ADS>* s, vector<MultiEvaluation<DS>*>& v_e)
	{
		paretoSet.push_back(s);
		paretoFront.push_back(v_e);
	}

	void push_back(const Solution<R, ADS>& s, const vector<MultiEvaluation<DS>*>& v_e)
	{
		paretoSet.push_back(&s->clone());
		vector<MultiEvaluation<DS>*> ve;
		for(unsigned mev=0; mev<v_e.size(); mev++)
			ve.push_back(&v_e[mev]->clone());
		paretoFront.push_back(ve);
	}

	unsigned size()
	{
		return paretoSet.size();
	}

	pair<Solution<R, ADS>&, vector<MultiEvaluation<DS>*> > erase(unsigned index)
	{
		pair<Solution<R, ADS>&, vector<MultiEvaluation<DS>*> > p(*paretoSet.at(index), paretoFront.at(index));
		paretoSet.erase(paretoSet.begin() + index);
		paretoSet.erase(paretoFront.begin() + index);
		return p;
	}

	pair<Solution<R, ADS>&, vector<MultiEvaluation<DS>*> > at(unsigned index)
	{
		return make_pair(*paretoSet.at(index), paretoFront.at(index));
	}

	vector<Solution<R, ADS>*> getParetoSet()
	{
		return paretoSet;
	}

	vector<vector<Evaluation<DS>*> > getParetoFront()
	{
		return paretoFront;
	}

	void print() const
	{
		cout << "ExtendedPareto size=" << paretoFront.size();
		cout << endl;
	}

};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ExtendedMultiObjSearch: public Component
{
public:

	ExtendedMultiObjSearch()
	{
	}

	virtual ~ExtendedMultiObjSearch()
	{
	}

	virtual ExtendedPareto<R, ADS, DS>* search(double timelimit = 100000000, double target_f = 0, ExtendedPareto<R, ADS, DS>* _pf = NULL) = 0;

	virtual string log()
	{
		return "Empty heuristic log.";
	}

	virtual bool compatible(string s)
	{
		return ( s == idComponent() ) || ( Component::compatible(s) );
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


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ExtendedMultiObjSearchBuilder: public ComponentBuilder<R, ADS, DS>
{
public:
	virtual ~ExtendedMultiObjSearchBuilder()
	{
	}

	virtual ExtendedMultiObjSearch<R, ADS, DS>* build(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS, DS>::idComponent() << "ExtendedMultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* OPTFRAME_EXTENDED_MULTI_OBJ_SEARCH_HPP_ */
