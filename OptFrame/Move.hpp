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

#ifndef OPTFRAME_MOVE_HPP_
#define OPTFRAME_MOVE_HPP_

#include "Solution.hpp"
#include "Evaluation.hpp"
#include "MoveCost.hpp"

#include "Component.hpp"

#include "Action.hpp"

using namespace std;

typedef void OPTFRAME_DEFAULT_PROBLEM;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class Move : public Component
{
public:

	virtual ~Move()
	{
	}

	bool canBeApplied(const Solution<R, ADS>& s)
	{
		return canBeApplied(s.getR(), s.getADS());
	}

//protected: // TODO: protect...
	virtual bool canBeApplied(const R&, const ADS&) = 0;

public:
	Move<R, ADS, DS>& apply(Solution<R, ADS>& s)
	{
		return apply(s.getR(), s.getADS());
	}

	Move<R, ADS, DS>& apply(Evaluation<DS>& e, Solution<R, ADS>& s)
	{
		return apply(e.getDS(), s.getR(), s.getADS());
	}

////protected:
	virtual Move<R, ADS, DS>& apply(R& r, ADS& ads) = 0;

	virtual Move<R, ADS, DS>& apply(DS& ds, R& r, ADS& ads)
	{
		Move<R, ADS, DS>& rev = apply(r, ads);
		updateNeighStatus(ads);
		return rev;
	}

	// TODO: coming in one of the next versions..
	//virtual pair<Move<R, ADS, DS>&, MoveCost*> apply(const Evaluation<DS>& e, R& r, ADS& ads) = 0;

public:

	// TODO: deprecated
	virtual void updateNeighStatus(ADS& ads)
	{
	}


	virtual MoveCost* cost(const Evaluation<DS>& e, const R& r, const ADS& ads)
	{
		return NULL;
	}

	virtual bool operator==(const Move<R, ADS, DS>& m) const = 0;

	bool operator!=(const Move<R, ADS, DS>& m) const
	{
		return !(*this == m);
	}

   static string idComponent()
   {
		stringstream ss;
		ss << Component::idComponent() << ":Move";
		return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

	virtual void print() const = 0;
};



template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MoveAction: public Action<R, ADS, DS>
{
public:

	virtual ~MoveAction()
	{
	}

	virtual string usage()
	{
		string u;
		u.append("OptFrame:Move idx  canBeApplied   OptFrame:Solution idx  [output_variable] => boolean\n");
		u.append("OptFrame:Move idx  apply   OptFrame:Solution idx  [output_variable] => OptFrame:Move\n");
		u.append("OptFrame:Move idx  apply_e  OptFrame:Evaluation idx   OptFrame:Solution idx  [output_variable] => OptFrame:Move");
		return u;
	}

	virtual bool handleComponent(string type)
	{
		return Component::compareBase(Move<R, ADS, DS>::idComponent(), type);
	}

	virtual bool handleComponent(Component& component)
	{
		return component.compatible(Move<R, ADS, DS>::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "canBeApplied") || (action == "apply") || (action == "apply_e");
	}

	virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& d)
	{
		if(!handleComponent(type))
		{
			cout << "MoveAction::doCast error: can't handle component type '" << type << " " << id << "'" << endl;
			return false;
		}

		Component* comp = hf.components[component].at(id);

		if(!comp)
		{
			cout << "MoveAction::doCast error: NULL component '" << component << " " << id << "'" << endl;
			return false;
		}

		if(!Component::compareBase(comp->id(), type))
		{
			cout << "MoveAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
			return false;
		}

		// remove old component from factory
		hf.components[component].at(id) = NULL;

		// cast object to lower type
		Component* final = NULL;

		if(type == Move<R, ADS, DS>::idComponent())
		{
			final = (Move<R, ADS, DS>*) comp;
		}
		else
		{
			cout << "MoveAction::doCast error: no cast for type '" << type << "'" << endl;
			return false;
		}

		// add new component
		Scanner scanner(variable);
		return ComponentAction<R, ADS, DS>::addAndRegister(scanner, *final, hf, d);
	}

	virtual bool doAction(string content, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		//cout << "Move::doAction '" << content << "'" << endl;

		Scanner scanner(content);

		if (!scanner.hasNext())
			return false;

		Move<R, ADS, DS>* m;
		hf.assign(m, scanner.nextInt(), scanner.next());

		if (!m)
			return false;

		if (!scanner.hasNext())
			return false;

		string action = scanner.next();

		if (!handleAction(action))
			return false;

		if (action == "canBeApplied")
		{
			if (!scanner.hasNext())
				return false;

			Solution<R, ADS>* s;
			hf.assign(s, scanner.nextInt(), scanner.next());

			if (!s)
				return false;

			bool b = m->canBeApplied(*s);

			if (!scanner.hasNext())
				return false;

			string var = scanner.next();

			dictionary[var] = Action<R, ADS, DS>::formatBool(b);

			return true;
		}

		if (action == "apply")
		{
			if (!scanner.hasNext())
				return false;

			Solution<R, ADS>* s;
			hf.assign(s, scanner.nextInt(), scanner.next());

			if (!s)
				return false;

			Move<R, ADS, DS>& mrev = m->apply(*s);

			return Action<R, ADS, DS>::addAndRegister(scanner, mrev, hf, dictionary);
		}

		if (action == "apply_e")
		{
			if (!scanner.hasNext())
				return false;

			Evaluation<DS>* e;
			hf.assign(e, scanner.nextInt(), scanner.next());

			if (!e)
				return false;

			// ------------------

			if (!scanner.hasNext())
				return false;

			Solution<R, ADS>* s;
			hf.assign(s, scanner.nextInt(), scanner.next());

			if (!s)
				return false;

			Move<R, ADS, DS>& mrev = m->apply(*e, *s);

			return Action<R, ADS, DS>::addAndRegister(scanner, mrev, hf, dictionary);
		}


		// no action found!
		return false;
	}

};

}


#endif /*OPTFRAME_MOVE_HPP_*/
