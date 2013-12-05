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

#ifndef OPTFRAME_MULTI_OBJ_SEARCH_HPP_
#define OPTFRAME_MULTI_OBJ_SEARCH_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Solution.hpp"
#include "Population.hpp"
#include "Evaluation.hpp"

#include "Component.hpp"
#include "ComponentBuilder.h"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class Pareto
{
private:
	vector<Solution<R, ADS>*> paretoSet;
	vector<vector<Evaluation<DS>*> > paretoFront;

public:

	Pareto()
	{
	}

	virtual ~Pareto()
	{
	}

	void push_back(Solution<R, ADS>& s, vector<Evaluation<DS>*>& v_e)
	{
		paretoSet.push_back(&s);
		paretoFront.push_back(v_e);
	}

	unsigned size()
	{
		return paretoSet.size();
	}

	pair<Solution<R, ADS>&, vector<Evaluation<DS>*> > erase(unsigned index)
	{
		pair<Solution<R, ADS>&, vector<Evaluation<DS>*> > p(*paretoSet.at(index), paretoFront.at(index));
		paretoSet.erase(paretoSet.begin() + index);
		paretoSet.erase(paretoFront.begin() + index);
		return p;
	}

	pair<Solution<R, ADS>&, vector<Evaluation<DS>*> > at(unsigned index)
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

	virtual Pareto<R, ADS, DS>* search(double timelimit = 100000000, double target_f = 0, Pareto<R, ADS, DS>* _pf = NULL) = 0;

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
		ss << Component::idComponent() << "MultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MultiObjSearchBuilder: public ComponentBuilder<R, ADS, DS>
{
public:
	virtual ~MultiObjSearchBuilder()
	{
	}

	virtual MultiObjSearch<R, ADS, DS>* build(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS, DS>::idComponent() << "MultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* OPTFRAME_MULTI_OBJ_SEARCH_HPP_ */
