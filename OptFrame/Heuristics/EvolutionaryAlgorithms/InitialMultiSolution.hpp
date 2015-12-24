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

#ifndef OPTFRAME_INITIALPOPULATION_HPP_
#define OPTFRAME_INITIALPOPULATION_HPP_

#include "../../Component.hpp"
#include "../../Constructive.h"
#include "../../MultiSolution.hpp"

#include "EA.h"

using namespace std;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class InitialMultiSolution: public Component, public EA
{
public:

	virtual ~InitialMultiSolution()
	{
	}

	virtual MultiSolution<R, ADS>& generatePopulation(unsigned populationSize) = 0;

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":" << EA::family() << ":InitialPopulation";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicInitialMultiSolution: public InitialMultiSolution<R, ADS>
{
public:

	Constructive<R, ADS>& constructive;

	BasicInitialMultiSolution(Constructive<R, ADS>& _constructive) :
			constructive(_constructive)
	{
	}

	virtual ~BasicInitialMultiSolution()
	{
	}

	virtual MultiSolution<R, ADS>& generatePopulation(unsigned populationSize)
	{
		MultiSolution<R, ADS>* p = new MultiSolution<R, ADS>;
		for (unsigned i = 0; i < populationSize; i++)
			p->push_back(&constructive.generateSolution());
		return *p;
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (InitialMultiSolution<R, ADS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << InitialMultiSolution<R, ADS>::idComponent() << ":BasicInitialPopulation";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicInitialPopulationBuilder: public ComponentBuilder<R, ADS>
{
public:
	virtual ~BasicInitialPopulationBuilder()
	{
	}

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Constructive<R, ADS>* c;
		hf.assign(c, scanner.nextInt(), scanner.next()); // reads backwards!

		return new BasicInitialMultiSolution<R, ADS>(*c);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Constructive<R, ADS>::idComponent(), "constructive heuristic"));
		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicInitialMultiSolution<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS>::idComponent() << "" << EA::family() << ":BasicInitialPopulation";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};
}

#endif /*OPTFRAME_INITIALPOPULATION_HPP_*/
