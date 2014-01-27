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

#ifndef OPTFRAME_NSSEQ_HPP_
#define OPTFRAME_NSSEQ_HPP_

#include "NS.hpp"
#include "NSIterator.hpp"

using namespace std;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NSSeq: public NS<R, ADS, DS>
{
public:

	using NS<R, ADS, DS>::move; // prevents name hiding

	virtual ~NSSeq()
	{
	}

////protected:
	virtual Move<R, ADS, DS>& move(const R&, const ADS&) = 0;

public:
	NSIterator<R, ADS, DS>& getIterator(const Solution<R, ADS>& s)
	{
		return getIterator(s.getR(), s.getADS());
	}

////protected:
	virtual NSIterator<R, ADS, DS>& getIterator(const R& r, const ADS& ads) = 0;

public:
	static string idComponent()
	{
		stringstream ss;
		ss << NS<R, ADS, DS>::idComponent() << ":NSSeq";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (NS<R, ADS, DS>::compatible(s));
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NSSeqAction: public Action<R, ADS, DS>
{
public:

	virtual ~NSSeqAction()
	{
	}

	virtual string usage()
	{
		string u;
		u.append("OptFrame:NS:NSSeq idx  getIterator   OptFrame:Solution idx   [output_variable] => OptFrame:NSIterator");
		return u;
	}

	virtual bool handleComponent(string type)
	{
		return Component::compareBase(NSSeq<R, ADS, DS>::idComponent(), type);
	}

	virtual bool handleComponent(Component& component)
	{
		return component.compatible(NSSeq<R, ADS, DS>::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "getIterator");
	}

	virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& d)
	{
		if(!handleComponent(type))
		{
			cout << "NSSeqAction::doCast error: can't handle component type '" << type << " " << id << "'" << endl;
			return false;
		}

		Component* comp = hf.components[component].at(id);

		if(!comp)
		{
			cout << "NSSeqAction::doCast error: NULL component '" << component << " " << id << "'" << endl;
			return false;
		}

		if(!Component::compareBase(comp->id(), type))
		{
			cout << "NSSeqAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
			return false;
		}

		// remove old component from factory
		hf.components[component].at(id) = NULL;

		// cast object to lower type
		Component* final = NULL;

		if(type == NSSeq<R, ADS, DS>::idComponent())
		{
			final = (NSSeq<R, ADS, DS>*) comp;
		}
		else
		{
			cout << "NSSeqAction::doCast error: no cast for type '" << type << "'" << endl;
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

		NSSeq<R, ADS, DS>* nsseq;
		hf.assign(nsseq, scanner.nextInt(), scanner.next());

		if(!nsseq)
			return false;

		if(!scanner.hasNext())
			return false;

		string action = scanner.next();

		if(!handleAction(action))
			return false;

		if(action == "getIterator")
		{
			if(!scanner.hasNext())
				return false;

			Solution<R, ADS>* s;
			hf.assign(s, scanner.nextInt(), scanner.next());

			if(!s)
				return false;

			NSIterator<R, ADS, DS>& it = nsseq->getIterator(*s);

			if(!scanner.hasNext())
				return false;

			return Action<R, ADS, DS>::addAndRegister(scanner, it, hf, dictionary);
		}

		// no action found!
		return false;
	}

};

}

#endif /*OPTFRAME_NSSEQ_HPP_*/
