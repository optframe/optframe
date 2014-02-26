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

#ifndef OPTFRAME_EVALUATION_HPP_
#define OPTFRAME_EVALUATION_HPP_

// TODO: Delta Structure (DS) will be abandoned soon...
// TODO: use ADS instead.
typedef int OPTFRAME_DEFAULT_DS;

#include <cstdlib>
#include <iostream>
#include <cmath>

#include "Component.hpp"
#include "Action.hpp"

#define OPTFRAME_EPSILON 0.0001

using namespace std;

namespace optframe
{


// TODO: use enum?
typedef bool Status; // 'unknown' = false, 'local optimum' = true

//! \english The Evaluation class is a container class for the objective function value and the Memory structure M. \endenglish \portuguese A classe Evaluation é uma classe contêiner para o valor da função objetivo e a estrutura de Memória M. \endportuguese

/*!
 \english
 It is also possible to carry an infeasibility measure.
 The evaluation() method returns the sum of objFunction and infMeasure.
 \endenglish

 \portuguese
 Também é possível carregar uma medida de inviabilidade infMeasure.
 O método evaluation() retorna a soma da função objetivo objFunction e a infMeasure.
 \endportuguese
 */

template<class DS = OPTFRAME_DEFAULT_DS>
class Evaluation: public Component
{
protected:
	double objFunction;
	double infMeasure;
	DS* ds; // delta structure

	map<string, Status> localStatus; // mapping 'move.id()' to 'NeighborhoodStatus'
	Status globalOptimum;            // for exact methods only

	vector<pair<double, double> > alternatives; // for lexicographic approaches

public:
	Evaluation(double obj, double inf, DS& _ds) :
			objFunction(obj), infMeasure(inf), ds(new DS(_ds))
	{
		globalOptimum = false;
	}


	Evaluation(double obj, DS& _ds) :
			ds(new DS(_ds))
	{
		objFunction = obj;
		infMeasure = 0;

		globalOptimum = false;
	}


	explicit Evaluation(double obj) :
			ds(new DS)
	{
		objFunction = obj;
		infMeasure = 0;

		globalOptimum = false;
	}

	Evaluation(const Evaluation<DS>& e) :
			objFunction(e.objFunction), infMeasure(e.infMeasure), ds(new DS(*e.ds)), globalOptimum(e.globalOptimum), alternatives(e.alternatives)
	{
	}


	virtual ~Evaluation()
	{
		delete ds;
	}

	const DS& getDS() const
	{
		return *ds;
	}

	DS& getDS()
	{
		return *ds;
	}

	double getObjFunction() const
	{
		return objFunction;
	}

	double getInfMeasure() const
	{
		return infMeasure;
	}

	const vector<pair<double, double> >& getAlternativeCosts() const
	{
		return alternatives;
	}

	// leave option to rewrite with clone()
	virtual void setDS(const DS& _ds)
	{
		delete ds;
		ds = new DS(_ds);
	}

	void setObjFunction(double obj)
	{
		objFunction = obj;
	}

	void setInfMeasure(double inf)
	{
		infMeasure = inf;
	}

	void addAlternativeCost(const pair<double, double>& alternativeCost)
	{
		alternatives.push_back(alternativeCost);
	}

	void setAlternativeCosts(const vector<pair<double, double> >& alternativeCosts)
	{
		alternatives = alternativeCosts;
	}

	// -----------------
	// for local optimum
	// -----------------

	Status getLocalOptimumStatus(string moveId)
	{
		return localStatus[moveId];
	}

	void setLocalOptimumStatus(string moveId, Status status)
	{
		localStatus[moveId] = status;
	}

	// ------------------
	// for global optimum
	// ------------------

	Status getGlobalOptimumStatus()
	{
		return globalOptimum;
	}

	void setGlobalOptimumStatus(Status status)
	{
		globalOptimum = status;
	}

	// evaluation = objFunction + infMeasure
	double evaluation() const
	{
		return objFunction + infMeasure;
	}

	// leave option to rewrite tolerance
	virtual bool isFeasible() const
	{
		return (abs(infMeasure) < 0.0001);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":Evaluation";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}


	virtual void print() const
	{
		cout << toString() << endl;
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << fixed; // disable scientific notation
		ss << "Evaluation function value = " << evaluation();
		ss << (isFeasible() ? " " : " (not feasible) ");
		if(alternatives.size() > 0)
		{
			ss << " alternative costs: ";
			for(unsigned i = 0; i < alternatives.size(); i++)
				ss << "(" << alternatives[i].first << ";" << alternatives[i].second << ") ";
		}
		// ss << endl;

		// default - not printing ememory
		// ss << m << endl;

		return ss.str();
	}

	virtual Evaluation& operator=(const Evaluation& e)
	{
		if (&e == this) // auto ref check
			return *this;

		(*ds) = (*e.ds);
		objFunction = e.objFunction;
		infMeasure = e.infMeasure;
		alternatives = e.alternatives;

		return *this;
	}

	virtual Evaluation<DS>& clone() const
	{
		return *new Evaluation<DS>(*this);
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class EvaluationAction: public Action<R, ADS, DS>
{
public:

	virtual ~EvaluationAction()
	{
	}

	virtual string usage()
	{
		return "OptFrame:Evaluation idx  evaluation  output_variable";
	}

	virtual bool handleComponent(string type)
	{
		return Component::compareBase(Evaluation<DS>::idComponent(), type);
	}

	virtual bool handleComponent(Component& component)
	{
		return component.compatible(Evaluation<DS>::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "evaluation");
	}

	virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& d)
	{
		if(!handleComponent(type))
		{
			cout << "EvaluationAction::doCast error: can't handle component type '" << type << " " << id << "'" << endl;
			return false;
		}

		Component* comp = hf.components[component].at(id);

		if(!comp)
		{
			cout << "EvaluationAction::doCast error: NULL component '" << component << " " << id << "'" << endl;
			return false;
		}

		if(!Component::compareBase(comp->id(), type))
		{
			cout << "EvaluationAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
			return false;
		}

		// remove old component from factory
		hf.components[component].at(id) = NULL;

		// cast object to lower type
		Component* final = NULL;

		if(type == Evaluation<DS>::idComponent())
		{
			final = (Evaluation<DS>*) comp;
		}
		else
		{
			cout << "EvaluationAction::doCast error: no cast for type '" << type << "'" << endl;
			return false;
		}

		// add new component
		Scanner scanner(variable);
		return ComponentAction<R, ADS, DS>::addAndRegister(scanner, *final, hf, d);
	}

	virtual bool doAction(string content, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		//cout << "Evaluation::doAction '" << content << "'" << endl;

		Scanner scanner(content);

		if (!scanner.hasNext())
			return false;

		Evaluation<DS>* e;
		hf.assign(e, scanner.nextInt(), scanner.next());

		if (!e)
			return false;

		if (!scanner.hasNext())
			return false;

		string action = scanner.next();

		if (!handleAction(action))
			return false;

		if (action == "evaluation")
		{
			if (!scanner.hasNext())
				return false;

			string var = scanner.next();

			dictionary[var] = Action<R, ADS, DS>::formatDouble(e->evaluation());

			return true;
		}
		else
			return false;
	}

};

}

#endif /*OPTFRAME_EVALUATION_HPP_*/
