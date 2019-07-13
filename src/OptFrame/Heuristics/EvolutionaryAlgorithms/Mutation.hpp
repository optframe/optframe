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

#ifndef MUTATION_HPP_
#define MUTATION_HPP_

#include "../../Component.hpp"
#include "../../Solution.hpp"
#include "../../Evaluation.hpp"
#include "../../NS.hpp"

#include "EA.h"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class Mutation: public Component, public EA
{

public:

	virtual ~Mutation()
	{
	}

	virtual void mutate(Solution<R, ADS>& individual, Evaluation& e) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":" << EA::family() << ":Mutation";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicMutation: public Mutation<R, ADS>
{
protected:
	unsigned n;
	vector<NS<R, ADS>*> vNS;
	RandGen& rg;

public:

	BasicMutation(unsigned _n, vector<NS<R, ADS>*> _vNS, RandGen& _rg) :
			n(_n), vNS(_vNS), rg(_rg)
	{
	}

	virtual ~BasicMutation()
	{
	}

	virtual void mutate(Solution<R, ADS>& s, Evaluation& e)
	{
		for (unsigned i = 0; i < n; i++)
		{
			int x = rg.rand(vNS.size());
			Move<R, ADS>* mp = vNS[x]->validMove(s);
			if (!mp)
				cout << "Warning: no move in BasicMutation!" << endl;
			else
			{
				delete &mp->apply(e, s);
				delete mp;
			}
		}
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":" << EA::family() << ":Mutation";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicMutationBuilder: public ComponentBuilder<R, ADS>
{
public:
	virtual ~BasicMutationBuilder()
	{
	}

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		int n = scanner.nextInt();

		vector<NS<R, ADS>*> ns_list;
		hf.assignList(ns_list, scanner.nextInt(), scanner.next()); // reads backwards!

		return new BasicMutation<R, ADS>(n, ns_list, hf.getRandGen());
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair("OptFrame:int", "number of moves"));
		stringstream ss;
		ss << NS<R, ADS>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of neighborhood structures"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicMutation<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS>::idComponent() << "" << EA::family() << ":BasicMutation";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* MUTATION_HPP_ */
