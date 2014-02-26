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

#ifndef OPTFRAME_NS_HPP_
#define OPTFRAME_NS_HPP_

#include "Move.hpp"
#include "Solution.hpp"

#include "Component.hpp"
#include "Action.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NS: public Component
{
public:

	virtual ~NS()
	{
	}

public:
	Move<R, ADS, DS>& move(const Solution<R, ADS>& s)
	{
		return move(s.getR(), s.getADS());
	}

	Move<R, ADS, DS>* validMove(const Solution<R, ADS>& s)
	{
		return validMove(s.getR(), s.getADS());
	}

////protected:
	virtual Move<R, ADS, DS>& move(const R&, const ADS&) = 0;

	virtual Move<R, ADS, DS>* validMove(const R& r, const ADS& ads)
	{
		Move<R, ADS, DS>* moveValid = &(this->move(r, ads));
		if(moveValid->canBeApplied(r, ads))
			return moveValid;
		else
		{
			delete moveValid;
			return NULL;
		}
	}

public:
	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":NS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NSAction: public Action<R, ADS, DS>
{
public:

	virtual ~NSAction()
	{
	}

	virtual string usage()
	{
		string u;
		u.append("OptFrame:NS idx  move   OptFrame:Solution idx  [output_variable] => OptFrame:Move\n");
		u.append("OptFrame:NS idx  validMove   OptFrame:Solution idx  [output_variable] => OptFrame:Move|NULL");
		return u;
	}

	virtual bool handleComponent(string type)
	{
		return Component::compareBase(NS<R, ADS, DS>::idComponent(), type);
	}

	virtual bool handleComponent(Component& component)
	{
		return component.compatible(NS<R, ADS, DS>::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "move") || (action == "validMove");
	}

	virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& d)
	{
		if(!handleComponent(type))
		{
			cout << "NSAction::doCast error: can't handle component type '" << type << " " << id << "'" << endl;
			return false;
		}

		Component* comp = hf.components[component].at(id);

		if(!comp)
		{
			cout << "NSAction::doCast error: NULL component '" << component << " " << id << "'" << endl;
			return false;
		}

		if(!Component::compareBase(comp->id(), type))
		{
			cout << "NSAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
			return false;
		}

		// remove old component from factory
		hf.components[component].at(id) = NULL;

		// cast object to lower type
		Component* final = NULL;

		if(type == NS<R, ADS, DS>::idComponent())
		{
			final = (NS<R, ADS, DS>*) comp;
		}
		else
		{
			cout << "NSAction::doCast error: no cast for type '" << type << "'" << endl;
			return false;
		}

		// add new component
		Scanner scanner(variable);
		return ComponentAction<R, ADS, DS>::addAndRegister(scanner, *final, hf, d);
	}

	virtual bool doAction(string content, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		//cout << "NS::doAction '" << content << "'" << endl;

		Scanner scanner(content);

		if(!scanner.hasNext())
			return false;

		NS<R, ADS, DS>* ns;
		hf.assign(ns, scanner.nextInt(), scanner.next());

		if(!ns)
			return false;

		if(!scanner.hasNext())
			return false;

		string action = scanner.next();

		if(!handleAction(action))
			return false;

		if(action == "move")
		{
			if(!scanner.hasNext())
				return false;

			Solution<R, ADS>* s;
			hf.assign(s, scanner.nextInt(), scanner.next());

			if(!s)
				return false;

			Move<R, ADS, DS>& m = ns->move(*s);

			if(!scanner.hasNext())
				return false;

			return Action<R, ADS, DS>::addAndRegister(scanner, m, hf, dictionary);
		}

		if(action == "validMove")
		{
			if(!scanner.hasNext())
				return false;

			Solution<R, ADS>* s;
			hf.assign(s, scanner.nextInt(), scanner.next());

			if(!s)
				return false;

			Move<R, ADS, DS>* m = ns->validMove(*s);

			if(!scanner.hasNext())
				return false;

			if(m)
				return Action<R, ADS, DS>::addAndRegister(scanner, *m, hf, dictionary);
			else
				return true; // not adding any component!
		}

		// no action found!
		return false;
	}

};

}

#endif /*OPTFRAME_NS_HPP_*/
