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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MultiEvaluator: public MultiDirection<DS>
{
protected:
	bool allowCosts; // move.cost() is enabled or disabled for this Evaluator
	vector<Evaluator<R, ADS, DS>*> sngEvaluators; // single evaluators

public:

	MultiEvaluator(bool _allowCosts = false) :
			allowCosts(_allowCosts)
	{
	}

	MultiEvaluator(MultiDirection<DS>& mDir, bool _allowCosts = false) :
			MultiDirection<DS>(mDir), allowCosts(_allowCosts)
	{
	}

	MultiEvaluator(vector<Direction<DS>*>& vDir, bool _allowCosts = false) :
			MultiDirection<DS>(vDir), allowCosts(_allowCosts)
	{
	}

	MultiEvaluator(vector<Evaluator<R, ADS, DS>*> _sngEvaluators) :
			MultiDirection<DS>(_sngEvaluators), sngEvaluators(_sngEvaluators), allowCosts(false)
	{
	}

	virtual ~MultiEvaluator()
	{
	}

	bool getAllowCosts()
	{
		return allowCosts;
	}

	vector<Evaluator<R, ADS, DS>*>* getEvaluators()
	{
		if(sngEvaluators.size()>0)
		{
			return new vector<Evaluator<R, ADS, DS>*>(sngEvaluators);
		}
		else
			return NULL;
	}

	// TODO: check
	const vector<const Evaluator<R, ADS, DS>*>* getEvaluators() const
	{
		if(sngEvaluators.size()>0)
			return new vector<const Evaluator<R, ADS, DS>*>(sngEvaluators);
		else
			return NULL;
	}

	MultiEvaluation<DS>& evaluate(const Solution<R, ADS>& s)
	{
		return evaluate(s.getR(), s.getADS());
	}


public: // protected: not possible because of GeneralizedMultiEvaluator

	// TODO: make virtual "= 0"
	virtual MultiEvaluation<DS>& evaluate(const R& r)
	{
		vector<Evaluation<DS>*> nev;
		for(unsigned i=0; i<sngEvaluators.size(); i++)
			nev.push_back(&sngEvaluators[i]->evaluate(r));
		return * new MultiEvaluation<DS>(nev);
	}

	virtual MultiEvaluation<DS>& evaluate(const R& r, const ADS&)
	{
		return evaluate(r);
	}

public:
	void evaluate(MultiEvaluation<DS>& mev, const Solution<R, ADS>& s)
	{
		evaluate(mev, s.getR(), s.getADS());
	}

protected:
	virtual void evaluate(MultiEvaluation<DS>& mev, const R& r, const ADS& ads)
	{
		MultiEvaluation<DS>& ve1 = evaluate(r, ads);

		for (unsigned i = 0; i < ve1.size(); i++)
		{
			mev.at(i) = ve1.at(i);
		}

		delete &ve1;
	}


	// ============= Component ===============
	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (MultiDirection<DS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << MultiDirection<DS>::idComponent() << ":MultiEvaluator";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MultiEvaluatorAction: public Action<R, ADS, DS>
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
		return Component::compareBase(MultiEvaluator<R, ADS, DS>::idComponent(), type);
	}

	virtual bool handleComponent(Component& component)
	{
		return component.compatible(MultiEvaluator<R, ADS, DS>::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "evaluate"); //|| (action == "betterThan") || (action == "betterOrEquals");
	}

	virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& d)
	{
		cout << "MultiEvaluator::doCast: NOT IMPLEMENTED!" << endl;
		return false;

		if (!handleComponent(type))
		{
			cout << "EvaluatorAction::doCast error: can't handle component type '" << type
					<< " " << id << "'" << endl;
			return false;
		}

		Component* comp = hf.components[component].at(id);

		if (!comp)
		{
			cout << "EvaluatorAction::doCast error: NULL component '" << component << " "
					<< id << "'" << endl;
			return false;
		}

		if (!Component::compareBase(comp->id(), type))
		{
			cout << "EvaluatorAction::doCast error: component '" << comp->id()
					<< " is not base of " << type << "'" << endl;
			return false;
		}

		// remove old component from factory
		hf.components[component].at(id) = NULL;

		// cast object to lower type
		Component* final = NULL;

		if (type == Evaluator<R, ADS, DS>::idComponent())
		{
			final = (Evaluator<R, ADS, DS>*) comp;
		}
		else
		{
			cout << "EvaluatorAction::doCast error: no cast for type '" << type << "'"
					<< endl;
			return false;
		}

		// add new component
		Scanner scanner(variable);
		return ComponentAction<R, ADS, DS>::addAndRegister(scanner, *final, hf, d);
	}

	virtual bool doAction(string content, HeuristicFactory<R, ADS, DS>& hf,	map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		cout << "MultiEvaluator::doAction: NOT IMPLEMENTED!" << endl;
		return false;

		//cout << "Evaluator::doAction '" << content << "'" << endl;

		Scanner scanner(content);

		if (!scanner.hasNext())
			return false;

		Evaluator<R, ADS, DS>* ev;
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

			Evaluation<DS>& e = ev->evaluate(*s);

			return Action<R, ADS, DS>::addAndRegister(scanner, e, hf, dictionary);
		}

		// no action found!
		return false;
	}

};

}

#endif /*OPTFRAME_MULTI_EVALUATOR_HPP_*/
