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

#ifndef OPTFRAME_SINGLE_OBJ_SEARCH_HPP_
#define OPTFRAME_SINGLE_OBJ_SEARCH_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Solution.hpp"
#include "Evaluation.hpp"
#include "Action.hpp"

#include "Component.hpp"
#include "ComponentBuilder.h"

namespace optframe
{

// Single Objective Stopping Criteria
// Must include GENERAL stopping criteria
// specific stopping criteria for metaheuristics can be included in their constructors
class SOSC: public Component
{
public:
	// maximum timelimit (seconds)
	double timelimit;
	// target objective function
	double target_f;

	SOSC(double _timelimit = 100000000.0, double _target_f = 0.0) :
			timelimit(_timelimit), target_f(_target_f)
	{
	}

	virtual ~SOSC()
	{
	}

	void updateTimeLimit(double subtrTL)
	{
		timelimit -= subtrTL;
	}

	SOSC newStopCriteriaWithTL(double subtrTL)
	{
		SOSC newStopCriteria = (*this);
		newStopCriteria.timelimit -= subtrTL;
		return newStopCriteria;
	}


	virtual string id() const
	{
		return "SOSC";
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class SingleObjSearch: public Component
{
	typedef vector<Evaluation*> FitnessValues;
	typedef const vector<const Evaluation*> ConstFitnessValues;

public:

	SingleObjSearch()
	{
	}

	virtual ~SingleObjSearch()
	{
	}

	// search method try to find a feasible solution within timelimit, if there is no such solution it returns nullptr.
	virtual pair<Solution<R, ADS>, Evaluation>* search(SOSC& stopCriteria, const Solution<R, ADS>* _s = nullptr, const Evaluation* _e = nullptr) = 0;

	virtual string log() const
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
		ss << Component::idComponent() << ":SingleObjSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class SingleObjSearchBuilder: public ComponentBuilder<R, ADS>
{
public:
	virtual ~SingleObjSearchBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS>::idComponent() << "SingleObjSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class SingleObjSearchAction: public Action<R, ADS>
{
public:

	virtual ~SingleObjSearchAction()
	{
	}

	virtual string usage()
	{
		return "OptFrame:SingleObjSearch idx   search    timelimit  target_f  OptFrame:Solution idx|-1   OptFrame:Evaluation idx|-1   [output_variable] => OptFrame:Solution|nullptr";
	}

	virtual bool handleComponent(string type)
	{
		return Component::compareBase(SingleObjSearch<R, ADS>::idComponent(), type);
	}

	virtual bool handleComponent(Component& component)
	{
		return component.compatible(SingleObjSearch<R, ADS>::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "search");
	}

	virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<R, ADS>& hf, map<string, string>& d)
	{
		if (!handleComponent(type))
		{
			cout << "SingleObjSearchAction::doCast error: can't handle component type '" << type << " " << id << "'" << endl;
			return false;
		}

		Component* comp = hf.components[component].at(id);

		if (!comp)
		{
			cout << "SingleObjSearchAction::doCast error: nullptr component '" << component << " " << id << "'" << endl;
			return false;
		}

		if (!Component::compareBase(comp->id(), type))
		{
			cout << "SingleObjSearchAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
			return false;
		}

		// remove old component from factory
		hf.components[component].at(id) = nullptr;

		// cast object to lower type
		Component* final = nullptr;

		if (type == SingleObjSearch<R, ADS>::idComponent())
		{
			final = (SingleObjSearch<R, ADS>*) comp;
		}
		else
		{
			cout << "SingleObjSearchAction::doCast error: no cast for type '" << type << "'" << endl;
			return false;
		}

		// add new component
		Scanner scanner(variable);
		return ComponentAction<R, ADS>::addAndRegister(scanner, *final, hf, d);
	}

	virtual bool doAction(string content, HeuristicFactory<R, ADS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		//cout << "LocalSearch::doAction '" << content << "'" << endl;

		Scanner scanner(content);

		if (!scanner.hasNext())
			return false;

		SingleObjSearch<R, ADS>* sios;
		hf.assign(sios, scanner.nextInt(), scanner.next());

		if (!sios)
			return false;

		if (!scanner.hasNext())
			return false;

		string action = scanner.next();

		if (!handleAction(action))
			return false;

		if (action == "search")
		{
			if (!scanner.hasNext())
				return false;

			double timelimit = scanner.nextDouble();

			if (!scanner.hasNext())
				return false;

			double target_f = scanner.nextDouble();

			if (!scanner.hasNext())
				return false;

			Solution<R, ADS>* s;
			hf.assign(s, scanner.nextInt(), scanner.next());

			if (!scanner.hasNext())
				return false;

			Evaluation* e;
			hf.assign(e, scanner.nextInt(), scanner.next());

			pair<Solution<R, ADS>, Evaluation>* p = sios->search(SOSC(timelimit, target_f), s, e);

			if (!p)
				return true;

			// TODO: use Move Semantics
			Solution<R, ADS>* s2 = new Solution<R, ADS>(p->first);

			delete p;

			return Action<R, ADS>::addAndRegister(scanner, *s2, hf, dictionary);
		}

		// no action found!
		return false;
	}

};

}

#endif /* OPTFRAME_SINGLE_OBJ_SEARCH_HPP_ */
