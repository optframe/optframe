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

#include <iostream>

#include "Solution.hpp"
#include "Evaluator.hpp"
#include "MultiEvaluation.hpp"
#include "MultiDirection.hpp"
#include "Action.hpp"

using namespace std;
using namespace scannerpp;

namespace optframe
{

template<XSolution S, XEvaluation XEv = Evaluation<>>
class MultiEvaluator: public MultiDirection
{
protected:
	vector<Evaluator<S, XEv>*> sngEvaluators; // single evaluators
	bool allowCosts; // move.cost() is enabled or disabled for this Evaluator

public:

	MultiEvaluator(vector<Evaluator<S, XEv>*> _veval) :
			sngEvaluators(_veval), allowCosts(false)
	{
		for (unsigned i = 0; i < _veval.size(); i++)
			if (_veval[i])
				vDir.push_back(_veval[i]);
		nObjectives = vDir.size();
	}

	MultiEvaluator(bool _allowCosts = false) :
			allowCosts(_allowCosts)
	{
	}

	virtual void addEvaluator(Evaluator<S, XEv>& ev)
	{
		sngEvaluators.push_back(&ev);
	}

//	MultiEvaluator(MultiDirection& mDir, bool _allowCosts = false) :
//			MultiDirection(mDir), allowCosts(_allowCosts)
//	{
//	}
//
//	MultiEvaluator(vector<Direction*>& vDir, bool _allowCosts = false) :
//			MultiDirection(vDir), allowCosts(_allowCosts)
//	{
//	}

//	MultiEvaluator(MultiEvaluator<S, XEv>& _mev) :
//			sngEvaluators(*_mev.getEvaluators2()), allowCosts(false)
//	{
//		cout<<"sngEvaluators.size():"<<sngEvaluators.size()<<endl;
//		for (unsigned i = 0; i < sngEvaluators.size(); i++)
//			if (sngEvaluators[i])
//				vDir.push_back(sngEvaluators[i]);
//		nObjectives = vDir.size();
//	}

	virtual ~MultiEvaluator()
	{
	}

	unsigned size()
	{
		return sngEvaluators.size();
	}


	unsigned size() const
	{
		return sngEvaluators.size();
	}

	virtual bool betterThan(const Evaluation<>& ev1, const Evaluation<>& ev2, int index)
	{
		return sngEvaluators[index]->betterThan(ev1, ev2);
	}

	virtual bool equals(const Evaluation<>& ev1, const Evaluation<>& ev2, int index)
	{
		return sngEvaluators[index]->equals(ev1, ev2);
	}

    //changed to Meval without point TODO
	virtual MultiEvaluation<> evaluate(const S& s)
	{
		cout << "inside mother class" << endl;
		getchar();
		MultiEvaluation<> nev;
		for (unsigned i = 0; i < sngEvaluators.size(); i++)
		{
			Evaluation<> ev {sngEvaluators[i]->evaluate(s)};
			nev.addEvaluation(ev);
		}

		return nev;
	}


	void clear()
	{
		for (int e = 0; e < int(sngEvaluators.size()); e++)
			delete sngEvaluators[e];
	}

	virtual void reevaluateMEV(MultiEvaluation<>& mev, const S& s)
	{
		for (unsigned i = 0; i < sngEvaluators.size(); i++)
		{
			//Evaluation<> e { std::move(mev[i]) }; // TODO (IGOR): why move????
			//sngEvaluators[i]->reevaluate(e, s);
			//mev[i] = std::move(e);
         //
         Evaluation<>& e = mev[i]; 
			sngEvaluators[i]->reevaluate(e, s);
			//mev[i] = std::move(e);
		}
	}

//	bool getAllowCosts()
//	{
//		return allowCosts;
//	}

//	vector<Evaluator<S, XEv>*> getEvaluators2()
//	{
//		return sngEvaluators;
//	}

	//	// TODO: check
//	const vector<const Evaluator<S, XEv>*>* getEvaluatorsConstTest() const
//	{
//		if (sngEvaluators.size() > 0)
//			return new vector<const Evaluator<S, XEv>*>(sngEvaluators);
//		else
//			return nullptr;
//	}

//	Evaluator<S, XEv>& at(unsigned index)
//	{
//		return *sngEvaluators.at(index);
//	}
//
//	const Evaluator<S, XEv>& at(unsigned index) const
//	{
//		return *sngEvaluators.at(index);
//	}
//
//	Evaluator<S, XEv>& operator[](unsigned index)
//	{
//		return *sngEvaluators[index];
//	}
//
//	const Evaluator<S, XEv>& operator[](unsigned index) const
//	{
//		return *sngEvaluators[index];
//	}

//	void addEvaluator(const Evaluator<S, XEv>& ev)
//	{
//		sngEvaluators.push_back(&ev.clone());
//	}



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

template<XSolution S, XEvaluation XEv = Evaluation<>, X2ESolution<S, XEv> X2ES = MultiESolution<S, XEv>>
class MultiEvaluatorAction: public Action<S, XEv, X2ES>
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
		return Component::compareBase(MultiEvaluator<S, XEv>::idComponent(), type);
	}

	virtual bool handleComponent(Component& component)
	{
		return component.compatible(MultiEvaluator<S, XEv>::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "evaluate"); //|| (action == "betterThan") || (action == "betterOrEquals");
	}

	virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<S, XEv, X2ES>& hf, map<string, string>& d)
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
			cout << "EvaluatorAction::doCast error: nullptr component '" << component << " " << id << "'" << endl;
			return false;
		}

		if (!Component::compareBase(comp->id(), type))
		{
			cout << "EvaluatorAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
			return false;
		}

		// remove old component from factory
		hf.components[component].at(id) = nullptr;

		// cast object to lower type
		Component* final = nullptr;

		if (type == Evaluator<S, XEv>::idComponent())
		{
			final = (Evaluator<S, XEv>*) comp;
		}
		else
		{
			cout << "EvaluatorAction::doCast error: no cast for type '" << type << "'" << endl;
			return false;
		}

		// add new component
		Scanner scanner(variable);
		return ComponentAction<S, XEv>::addAndRegister(scanner, *final, hf, d);
	}

	virtual bool doAction(string content, HeuristicFactory<S, XEv, X2ES>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		cout << "MultiEvaluator::doAction: NOT IMPLEMENTED!" << endl;
		return false;

		//cout << "Evaluator::doAction '" << content << "'" << endl;

		Scanner scanner(content);

		if (!scanner.hasNext())
			return false;

		Evaluator<S, XEv>* ev;
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

			S* s;
			hf.assign(s, scanner.nextInt(), scanner.next());

			if (!s)
				return false;

			Evaluation<>& e = ev->evaluate(*s);

			return Action<S, XEv, X2ES>::addAndRegister(scanner, e, hf, dictionary);
		}

		// no action found!
		return false;
	}

};

}

#endif /*OPTFRAME_MULTI_EVALUATOR_HPP_*/
