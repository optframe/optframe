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

#ifndef OPTFRAME_EVALUATOR_HPP_
#define OPTFRAME_EVALUATOR_HPP_

#include "Solution.hpp"
#include "ADSManager.hpp"
#include "Evaluation.hpp"
#include "Move.hpp"
#include "MoveCost.hpp"

#include <iostream>

#include "Component.hpp"
#include "Action.hpp"

//#define OPTFRAME_EPSILON 0.0001

using namespace std;
using namespace scannerpp;

namespace optframe
{


//! \english The Evaluator class is responsible for the attribution of objective values for each Solution \endenglish \portuguese A classe Evaluator é responsável pela atribuição de valores objetivo para cada Solution \endportuguese

/*!
 \english
 The Evaluator class is responsible for the attribution of objective values for each Solution
 This is done by the method evaluate().
 \endenglish

 \portuguese
 A classe Evaluator é responsável pela atribuição de valores objetivo para cada Solution.
 Isto é feito através do método evaluate().
 \endportuguese
 */

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class Evaluator: public Component
{
protected:
	bool allowCosts; // move.cost() is enabled or disabled for this Evaluator

public:

	Evaluator(bool _allowCosts = true) :
			allowCosts(_allowCosts)
	{
	}

	virtual ~Evaluator()
	{
	}

	bool getAllowCosts()
	{
		return allowCosts;
	}

	Evaluation<DS>& evaluate(const Solution<R, ADS>& s)
	{
		return evaluate(s.getR(), s.getADS());
	}

//protected:
public: // because of MultiEvaluator... otherwise, make it 'friend'

	virtual Evaluation<DS>& evaluate(const R& r) = 0;

	virtual Evaluation<DS>& evaluate(const R& r, const ADS&)
	{
		return evaluate(r);
	}

public:
	void evaluate(Evaluation<DS>& e, const Solution<R, ADS>& s)
	{
		evaluate(e, s.getR(), s.getADS());
	}

//protected:
public: // because of MultiEvaluator... otherwise, make it 'friend'
	virtual void evaluate(Evaluation<DS>& e, const R& r, const ADS& ads)
	{
		Evaluation<DS>& e1 = evaluate(r, ads);
		e = e1;
		delete &e1;
	}

public:

	// Apply movement considering a previous evaluation => Faster.
	// Update evaluation 'e'
	Move<R, ADS, DS>& applyMove(Evaluation<DS>& e, Move<R, ADS, DS>& m, Solution<R, ADS>& s)
	{
		Move<R, ADS, DS>& rev = m.apply(e, s);
		evaluate(e, s);
		return rev;
	}

	// Apply movement without considering a previous evaluation => Slower.
	// Return new evaluation 'e'
	pair<Move<R, ADS, DS>&, Evaluation<DS>&>& applyMove(Move<R, ADS, DS>& m, Solution<R, ADS>& s)
	{
		Move<R, ADS, DS>& rev = m.apply(s);
		return *new pair<Move<R, ADS, DS>&, Evaluation<DS>&>(rev, evaluate(s));
	}

	// Movement cost based on reevaluation of 'e'
	MoveCost& moveCost(Evaluation<DS>& e, Move<R, ADS, DS>& m, Solution<R, ADS>& s)
	{
		MoveCost* p = NULL;
		if (allowCosts)
			p = m.cost(e, s.getR(), s.getADS());

		// do not update 's' => much faster (using updateDelta)
		if (p)
			return *p;
		else // need to update 's' together with reevaluation of 'e' => little faster (doesn't use updateDelta, but do reevaluation)
		{
			Move<R, ADS, DS>& rev = applyMove(e, m, s);
			pair<double, double> e_end = make_pair(e.getObjFunction(), e.getInfMeasure());

			vector<pair<double, double> > alternatives(e.getAlternativeCosts().size());

			for (unsigned i = 0; i < alternatives.size(); i++)
			{
				alternatives[i].first = e.getAlternativeCosts()[i].first;
				alternatives[i].second = e.getAlternativeCosts()[i].second;
			}

			Move<R, ADS, DS>& ini = applyMove(e, rev, s);
			pair<double, double> e_ini = make_pair(e.getObjFunction(), e.getInfMeasure());

			for (unsigned i = 0; i < alternatives.size(); i++)
			{
				alternatives[i].first -= e.getAlternativeCosts()[i].first;
				alternatives[i].second -= e.getAlternativeCosts()[i].second;
			}

			delete &rev;
			delete &ini;

			p = new MoveCost(e_end.first - e_ini.first, e_end.second - e_end.second);
			p->setAlternativeCosts(alternatives);

			return *p;
		}
	}

	// Movement cost based on complete evaluation
	// USE ONLY FOR VALIDATION OF CODE! OTHERWISE, USE moveCost(e, m, s)
	MoveCost& moveCost(Move<R, ADS, DS>& m, Solution<R, ADS>& s)
	{
		pair<Move<R, ADS, DS>&, Evaluation<DS>&>& rev = applyMove(m, s);

		pair<Move<R, ADS, DS>&, Evaluation<DS>&>& ini = applyMove(rev.first, s);

		// Difference: new - original

		double obj = rev.second.getObjFunction() - ini.second.getObjFunction();
		double inf = rev.second.getInfMeasure() - ini.second.getInfMeasure();

		vector<pair<double, double> > alternatives(rev.second.getAlternativeCosts().size());

		for (unsigned i = 0; i < alternatives.size(); i++)
		{
			alternatives[i].first = rev.second.getAlternativeCosts()[i].first - ini.second.getAlternativeCosts()[i].first;
			alternatives[i].second = rev.second.getAlternativeCosts()[i].second - ini.second.getAlternativeCosts()[i].second;
		}

		MoveCost* p = new MoveCost(obj, inf);
		p->setAlternativeCosts(alternatives);

		delete &rev.first;
		delete &rev.second;
		delete &ini.first;
		delete &ini.second;

		delete &rev;
		delete &ini;

		return *p;
	}


	// ============ betterThan ===========

	//! abstract method betterThan: true when a < b for minimization problems; and true when a > b for maximization problems.
	/*!
	 betterThan is the method in OptFrame used to guide the search methods into the solution space.
	 with betterThan the search methods are able to compare good and poor solutions, in one of the two directions: minimization and maximization.
	 It must be implemented by the final user in one of these ways:
	 - for minimization problems, returns a < b;
	 - for maximization problems, returns a > b.
	 */
	virtual bool betterThan(double a, double b) = 0;

	// true if 's1' is better than 's2'
	bool betterThan(const Solution<R, ADS>& s1, const Solution<R, ADS>& s2)
	{
		Evaluation<DS>& e1 = evaluate(s1);
		Evaluation<DS>& e2 = evaluate(s2);

		double f1 = e1.evaluation();
		double f2 = e2.evaluation();

		delete &e1;
		delete &e2;

		return betterThan(f1, f2);
	}

	// true if 'mc1' is better than 'mc2'
	virtual bool betterThan(const MoveCost& mc1, const MoveCost& mc2)
	{
		return betterThan(mc1.cost(), mc2.cost());
	}

	// true if 'e1' is better than 'e2'
	virtual bool betterThan(const Evaluation<DS>& e1, const Evaluation<DS>& e2)
	{
		return betterThan(e1.evaluation(), e2.evaluation());
	}

	virtual bool betterThan(const vector<pair<double, double> >& altCosts1, const vector<pair<double, double> >& altCosts2)
	{
		if (altCosts1.size() != altCosts2.size())
			return false;
		for (unsigned i = 0; i < altCosts1.size(); i++)
			if (!betterThan(altCosts1[i].first + altCosts1[i].second, altCosts2[i].first + altCosts2[i].second))
				return false;
		return true;
	}

	// ============ betterOrEquals ===========

	bool betterOrEquals(const Solution<R>& s1, const Solution<R>& s2)
	{
		Evaluation<DS>& e1 = evaluate(s1);
		Evaluation<DS>& e2 = evaluate(s2);

		double f1 = e1.evaluation();
		double f2 = e2.evaluation();

		delete &e1;
		delete &e2;

		return betterOrEquals(f1, f2);
	}

	bool betterOrEquals(const vector<pair<double, double> >& altCosts1, const vector<pair<double, double> >& altCosts2)
	{
		return betterThan(altCosts1, altCosts2) || equals(altCosts1, altCosts2);
	}

	bool betterOrEquals(const MoveCost& mc1, const MoveCost& mc2)
	{
		return betterThan(mc1, mc2) || equals(mc1, mc2);
	}

	bool betterOrEquals(const Evaluation<DS>& e1, const Evaluation<DS>& e2)
	{
		return betterThan(e1, e2) || equals(e1, e2);
	}

	bool betterOrEquals(double a, double b)
	{
		return betterThan(a, b) || equals(a, b);
	}

	// ============ equals ============

	virtual bool equals(const vector<pair<double, double> >& altCosts1, const vector<pair<double, double> >& altCosts2)
	{
		if (altCosts1.size() != altCosts2.size())
			return false;
		for (unsigned i = 0; i < altCosts1.size(); i++)
			if (!equals(altCosts1[i].first + altCosts1[i].second, altCosts2[i].first + altCosts2[i].second))
				return false;
		return true;
	}

	virtual bool equals(const MoveCost& mc1, const MoveCost& mc2)
	{
		return equals(mc1.cost(), mc2.cost());
	}

	virtual bool equals(const Evaluation<DS>& e1, const Evaluation<DS>& e2)
	{
		return equals(e1.evaluation(), e2.evaluation());
	}

	virtual bool equals(double a, double b)
	{
		return (abs(a - b) < OPTFRAME_EPSILON);
	}

	// ============= improvement =============

	virtual bool isImprovement(const MoveCost& mc, const Evaluation<DS>& e1, const Evaluation<DS>& e2)
	{
		double ec1 = mc.cost() + e1.evaluation();
		if (betterThan(ec1, e2.evaluation()))
			return true;
		else if (equals(ec1, e2.evaluation()))
		{
			if (e1.getAlternativeCosts().size() != e2.getAlternativeCosts().size())
			{
				cout << "Evaluator Error: |e1.alternatives|=" << e1.getAlternativeCosts().size() << " |e2.alternatives|=" << e2.getAlternativeCosts().size();
				cout << endl;
				exit(1);
				return false;
			}

			if (mc.getAlternativeCosts().size() != e1.getAlternativeCosts().size())
			{
				cout << "Evaluator Error: |mc.alternatives|=" << mc.getAlternativeCosts().size() << " |e1.alternatives|=" << e1.getAlternativeCosts().size();
				cout << endl;
				exit(1);
				return false;
			}

			vector<pair<double, double> > altCosts1(e1.getAlternativeCosts());
			for (unsigned i = 0; i < altCosts1.size(); i++)
			{
				altCosts1[i].first += mc.getAlternativeCosts()[i].first;
				altCosts1[i].second += mc.getAlternativeCosts()[i].second;
			}
			return betterThan(altCosts1, e2.getAlternativeCosts());
		}
		else
			return false;
	}

	virtual bool isImprovement(const MoveCost& mc)
	{
		return betterThan(mc.cost(), 0);
	}

	// ============= direction ==============

	bool isMinimization()
	{
		return betterThan(0, 1);
	}

	bool isMaximization()
	{
		return !isMinimization();
	}

	// ============= Component ===============

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":Evaluator";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class EvaluatorAction: public Action<R, ADS, DS>
{
public:

	virtual ~EvaluatorAction()
	{
	}

	virtual string usage()
	{
		return "OptFrame:Evaluator idx  evaluate   OptFrame:Solution idx  [output_variable] => OptFrame:Evaluation";
	}

	virtual bool handleComponent(string type)
	{
		return Component::compareBase(Evaluator<R, ADS, DS>::idComponent(), type);
	}

	virtual bool handleComponent(Component& component)
	{
		return component.compatible(Evaluator<R, ADS, DS>::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "evaluate") || (action == "betterThan") || (action == "betterOrEquals");
	}

	virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& d)
	{
		if(!handleComponent(type))
		{
			cout << "EvaluatorAction::doCast error: can't handle component type '" << type << " " << id << "'" << endl;
			return false;
		}

		Component* comp = hf.components[component].at(id);

		if(!comp)
		{
			cout << "EvaluatorAction::doCast error: NULL component '" << component << " " << id << "'" << endl;
			return false;
		}

		if(!Component::compareBase(comp->id(), type))
		{
			cout << "EvaluatorAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
			return false;
		}

		// remove old component from factory
		hf.components[component].at(id) = NULL;

		// cast object to lower type
		Component* final = NULL;

		if(type == Evaluator<R, ADS, DS>::idComponent())
		{
			final = (Evaluator<R, ADS, DS>*) comp;
		}
		else
		{
			cout << "EvaluatorAction::doCast error: no cast for type '" << type << "'" << endl;
			return false;
		}

		// add new component
		Scanner scanner(variable);
		return ComponentAction<R, ADS, DS>::addAndRegister(scanner, *final, hf, d);
	}

	virtual bool doAction(string content, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
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

#endif /*OPTFRAME_EVALUATOR_HPP_*/
