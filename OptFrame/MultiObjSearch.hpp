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

#include "OptFrameComponent.hpp"
#include "ComponentBuilder.h"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class MultiObjSearch: public OptFrameComponent
{
public:

	typedef vector<pair<Solution<R, ADS>&, vector<Evaluation<M>*> > > ParetoFront;

	MultiObjSearch()
	{
	}

	virtual ~MultiObjSearch()
	{
	}

	virtual ParetoFront* search(double timelimit = 100000000, double target_f = 0) = 0;

	virtual string log()
	{
		return "Empty heuristic log.";
	}

	virtual bool compatible(string s)
	{
		return ( s == idComponent() ) || ( OptFrameComponent::compatible(s) );
	}

	static string idComponent()
	{
		stringstream ss;
		ss << OptFrameComponent::idComponent() << "MultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class MultiObjSearchBuilder: public ComponentBuilder<R, ADS, M>
{
public:
	virtual ~MultiObjSearchBuilder()
	{
	}

	virtual MultiObjSearch<R, ADS, M>* build(Scanner& scanner, HeuristicFactory<R, ADS, M>& hf, string family = "") = 0;

	virtual OptFrameComponent* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS, M>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS, M>::idComponent() << "MultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


#endif /* OPTFRAME_MULTI_OBJ_SEARCH_HPP_ */
