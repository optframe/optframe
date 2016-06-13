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

#ifndef OPTFRAME_MULTI_EVALUATOR_HPP_
#define OPTFRAME_MULTI_EVALUATOR_HPP_

#include "Solution.hpp"
#include "Evaluator.hpp"
#include "MultiEvaluation.hpp"

#include <iostream>

#include "MultiDirection.hpp"
#include "Action.hpp"

using namespace std;
using namespace scannerpp;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class MultiEvaluator: public MultiDirection
{
protected:
	bool allowCosts; // move.cost() is enabled or disabled for this Evaluator
	vector<Evaluator<R, ADS>*> sngEvaluators; // single evaluators

public:

	MultiEvaluator(bool _allowCosts = false) :
			allowCosts(_allowCosts)
	{
	}

	MultiEvaluator(MultiDirection& mDir, bool _allowCosts = false) :
			MultiDirection(mDir), allowCosts(_allowCosts)
	{
	}

	MultiEvaluator(vector<Direction*>& vDir, bool _allowCosts = false) :
			MultiDirection(vDir), allowCosts(_allowCosts)
	{
	}

	MultiEvaluator(vector<Evaluator<R, ADS>*> _vDir) :
			sngEvaluators(_vDir), allowCosts(false)
	{
		for (unsigned i = 0; i < _vDir.size(); i++)
			if (_vDir[i])
				vDir.push_back(_vDir[i]);
		nObjectives = vDir.size();
	}

	unsigned size()
	{
		return sngEvaluators.size();
	}
	virtual ~MultiEvaluator()
	{
	}

	bool getAllowCosts()
	{
		return allowCosts;
	}

	vector<Evaluator<R, ADS>*>* getEvaluators()
	{
		if (sngEvaluators.size() > 0)
		{
			return new vector<Evaluator<R, ADS>*>(sngEvaluators);
		}
		else
			return NULL;
	}

	Evaluator<R, ADS>& at(unsigned index)
	{
		return *sngEvaluators.at(index);
	}

	const Evaluator<R, ADS>& at(unsigned index) const
	{
		return *sngEvaluators.at(index);
	}

	Evaluator<R, ADS>& operator[](unsigned index)
	{
		return *sngEvaluators[index];
	}

	const Evaluator<R, ADS>& operator[](unsigned index) const
	{
		return *sngEvaluators[index];
	}

	// TODO: check
	const vector<const Evaluator<R, ADS>*>* getEvaluators() const
	{
		if (sngEvaluators.size() > 0)
			return new vector<const Evaluator<R, ADS>*>(sngEvaluators);
		else
			return NULL;
	}

	MultiEvaluation& evaluate(const Solution<R, ADS>& s)
	{
		return evaluate(s.getR(), s.getADS());
	}

public:
	// protected: not possible because of GeneralizedMultiEvaluator

	// TODO: make virtual "= 0"
	virtual MultiEvaluation& evaluate(const R& r)
	{
		MultiEvaluation* nev = new MultiEvaluation;

		for (unsigned i = 0; i < sngEvaluators.size(); i++)
			nev->addEvaluation(sngEvaluators[i]->evaluate(r));

		return *nev;
	}

	virtual MultiEvaluation& evaluate(const R& r, const ADS& ads)
	{
		MultiEvaluation* nev = new MultiEvaluation;

		for (unsigned i = 0; i < sngEvaluators.size(); i++)
			nev->addEvaluation(sngEvaluators[i]->evaluate(r, ads));

		return *nev;
	}

	void evaluate(MultiEvaluation& mev, const Solution<R, ADS>& s)
	{
		evaluate(mev, s.getR(), s.getADS());
	}

	virtual void evaluate(MultiEvaluation& mev, const R& r, const ADS& ads)
	{
		for (unsigned i = 0; i < sngEvaluators.size(); i++)
		{
			sngEvaluators[i]->evaluate(mev[i], r, ads);
		}

	}

protected:

	// ============= Component ===============
	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (MultiDirection::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << MultiDirection::idComponent() << ":MultiEvaluator";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MultiEvaluatorAction: public Action<R, ADS>
{
public:

	virtual ~MultiEvaluatorAction()
	{
	}

	virtual string usage()
	{
		return ":MultiEvaluator idx  evaluate   :Solution idx  [output_variable] => OptFrame:Evaluation";
	}

	virtual bool handleComponent(string type)
	{
		return Component::compareBase(MultiEvaluator<R, ADS>::idComponent(), type);
	}

	virtual bool handleComponent(Component& component)
	{
		return component.compatible(MultiEvaluator<R, ADS>::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "evaluate"); //|| (action == "betterThan") || (action == "betterOrEquals");
	}

	virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<R, ADS>& hf, map<string, string>& d)
	{
		cout << "MultiEvaluator::doCast: NOT IMPLEMENTED!" << endl;
		return false;

		if (!handleComponent(type))
		{
			cout << "EvaluatorAction::doCast error: can't handle component type '" << type << " " << id << "'" << endl;
			return false;
		}

		Component* comp = hf.components[component].at(id);

		if (!comp)
		{
			cout << "EvaluatorAction::doCast error: NULL component '" << component << " " << id << "'" << endl;
			return false;
		}

		if (!Component::compareBase(comp->id(), type))
		{
			cout << "EvaluatorAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
			return false;
		}

		// remove old component from factory
		hf.components[component].at(id) = NULL;

		// cast object to lower type
		Component* final = NULL;

		if (type == Evaluator<R, ADS>::idComponent())
		{
			final = (Evaluator<R, ADS>*) comp;
		}
		else
		{
			cout << "EvaluatorAction::doCast error: no cast for type '" << type << "'" << endl;
			return false;
		}

		// add new component
		Scanner scanner(variable);
		return ComponentAction<R, ADS>::addAndRegister(scanner, *final, hf, d);
	}

	virtual bool doAction(string content, HeuristicFactory<R, ADS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		cout << "MultiEvaluator::doAction: NOT IMPLEMENTED!" << endl;
		return false;

		//cout << "Evaluator::doAction '" << content << "'" << endl;

		Scanner scanner(content);

		if (!scanner.hasNext())
			return false;

		Evaluator<R, ADS>* ev;
		hf.assign(ev, scanner.nextInt(), scanner.next());

		if (!ev)
			return false;

		if (!scanner.hasNext())
			return false;

		string action = scanner.next();

		if (!handleAction(action))
			return false;

		if (action == "evaluate")
		{
			if (!scanner.hasNext())
				return false;

			Solution<R, ADS>* s;
			hf.assign(s, scanner.nextInt(), scanner.next());

			if (!s)
				return false;

			Evaluation& e = ev->evaluate(*s);

			return Action<R, ADS>::addAndRegister(scanner, e, hf, dictionary);
		}

		// no action found!
		return false;
	}

};

}

#endif /*OPTFRAME_MULTI_EVALUATOR_HPP_*/
