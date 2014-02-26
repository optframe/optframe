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

#ifndef OPTFRAME_NSITERATOR_HPP_
#define OPTFRAME_NSITERATOR_HPP_

#include "Move.hpp"

#include "Component.hpp"
#include "Action.hpp"

using namespace std;

namespace optframe
{

class IteratorOutOfBound
{
private:
	int id;
public:
	IteratorOutOfBound(int _id) :
			id(_id)
	{
	}

	int getId()
	{
		return id;
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NSIterator: public Component
{
public:
	virtual ~NSIterator()
	{
	}

	virtual void first() = 0;
	virtual void next() = 0;
	virtual bool isDone() = 0;
	virtual Move<R, ADS, DS>& current() = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":NSIterator";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NSIteratorAction: public Action<R, ADS, DS>
{
public:

	virtual ~NSIteratorAction()
	{
	}

	virtual string usage()
	{
		string u;
		u.append("OptFrame:NSIterator idx  first\n");
		u.append("OptFrame:NSIterator idx  next\n");
		u.append("OptFrame:NSIterator idx  isDone   [output_variable] => boolean\n");
		u.append("OptFrame:NSIterator idx  current  [output_variable] => OptFrame:Move");
		return u;
	}

	virtual bool handleComponent(string type)
	{
		return Component::compareBase(NSIterator<R, ADS, DS>::idComponent(), type);
	}

	virtual bool handleComponent(Component& component)
	{
		return component.compatible(NSIterator<R, ADS, DS>::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "first") || (action == "next") || (action == "isDone") || (action == "current");
	}

	virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& d)
	{
		if(!handleComponent(type))
		{
			cout << "NSIteratorAction::doCast error: can't handle component type '" << type << " " << id << "'" << endl;
			return false;
		}

		Component* comp = hf.components[component].at(id);

		if(!comp)
		{
			cout << "NSIteratorAction::doCast error: NULL component '" << component << " " << id << "'" << endl;
			return false;
		}

		if(!Component::compareBase(comp->id(), type))
		{
			cout << "NSIteratorAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
			return false;
		}

		// remove old component from factory
		hf.components[component].at(id) = NULL;

		// cast object to lower type
		Component* final = NULL;

		if(type == NSIterator<R, ADS, DS>::idComponent())
		{
			final = (NSIterator<R, ADS, DS>*) comp;
		}
		else
		{
			cout << "NSIteratorAction::doCast error: no cast for type '" << type << "'" << endl;
			return false;
		}

		// add new component
		Scanner scanner(variable);
		return ComponentAction<R, ADS, DS>::addAndRegister(scanner, *final, hf, d);
	}

	virtual bool doAction(string content, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		//cout << "NSIterator::doAction '" << content << "'" << endl;

		Scanner scanner(content);

		if(!scanner.hasNext())
			return false;

		NSIterator<R, ADS, DS>* it;
		hf.assign(it, scanner.nextInt(), scanner.next());

		if(!it)
			return false;

		if(!scanner.hasNext())
			return false;

		string action = scanner.next();

		if(!handleAction(action))
			return false;

		if(action == "first")
		{
			it->first();
			return true;
		}

		if(action == "next")
		{
			it->next();
			return true;
		}

		if(action == "isDone")
		{
			if(!scanner.hasNext())
				return false;

			string var = scanner.next();

			dictionary[var] = Action<R, ADS, DS>::formatBool(it->isDone());

			return true;
		}

		if(action == "current")
		{
			if(!scanner.hasNext())
				return false;

			Move<R, ADS, DS>& m = it->current();

			return Action<R, ADS, DS>::addAndRegister(scanner, m, hf, dictionary);
		}

		// no action found!
		return false;
	}

};

}

#endif //OPTFRAME_NSITERATOR_HPP_
