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

#ifndef OPTFRAME_CONSTRUCTIVE_H_
#define OPTFRAME_CONSTRUCTIVE_H_

#include "Solution.hpp"
#include "Evaluation.hpp" // because of the action

namespace optframe
{


template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class Constructive : public Component
{
public:
	virtual ~Constructive()
	{
	}

	virtual Solution<R, ADS>& generateSolution() = 0;

    virtual bool compatible(string s)
    {
    	return ( s == idComponent() ) || (Component::compatible(s));
    }

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":Constructive";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ConstructiveAction: public Action<R, ADS, DS>
{
public:

	virtual ~ConstructiveAction()
	{
	}

	virtual string usage()
	{
		return "OptFrame:Constructive idx  generateSolution  output_variable => OptFrame:Solution idx";
	}

	virtual bool handleComponent(string type)
	{
		return Component::compareBase(Constructive<R, ADS>::idComponent(), type);
	}

	virtual bool handleComponent(Component& component)
	{
		return component.compatible(Constructive<R, ADS>::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "generateSolution");
	}

	virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& d)
	{
		if(!handleComponent(type))
		{
			cout << "ConstructiveAction::doCast error: can't handle component type '" << type << " " << id << "'" << endl;
			return false;
		}

		Component* comp = hf.components[component].at(id);

		if(!comp)
		{
			cout << "ConstructiveAction::doCast error: NULL component '" << component << " " << id << "'" << endl;
			return false;
		}

		if(!Component::compareBase(comp->id(), type))
		{
			cout << "ConstructiveAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
			return false;
		}

		// remove old component from factory
		hf.components[component].at(id) = NULL;

		// cast object to lower type
		Component* final = NULL;

		if(type == Constructive<R, ADS>::idComponent())
		{
			final = (Constructive<R, ADS>*) comp;
		}
		else
		{
			cout << "ConstructiveAction::doCast error: no cast for type '" << type << "'" << endl;
			return false;
		}

		// add new component
		Scanner scanner(variable);
		return ComponentAction<R, ADS, DS>::addAndRegister(scanner, *final, hf, d);

	}

	virtual bool doAction(string content, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		//cout << "Constructive::doAction '" << content << "'" << endl;

		Scanner scanner(content);

		if (!scanner.hasNext())
			return false;

		Constructive<R, ADS>* c;
		hf.assign(c, scanner.nextInt(), scanner.next());

		if (!c)
			return false;

		if (!scanner.hasNext())
			return false;

		string action = scanner.next();

		if (!handleAction(action))
			return false;

		if (action == "generateSolution")
		{
			if (!scanner.hasNext())
				return false;

			Solution<R, ADS>& s = c->generateSolution();

			return Action<R, ADS, DS>::addAndRegister(scanner, s, hf, dictionary);
		}

		// no action found!
		return false;
	}

};

}

#endif /*OPTFRAME_CONSTRUCTIVE_H_*/
