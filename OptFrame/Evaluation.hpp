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

// GlobalOptimumStatus
enum GOS
{
    gos_yes, gos_no, gos_unknown
};

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

//#include "Util/PackTypes.hpp"
//#define EVALUATION_TYPE PackTypes

#ifndef EVALUATION_TYPE
#define EVALUATION_TYPE double
#endif

typedef EVALUATION_TYPE evtype;

// note: for multi-objective problems with distinct objective space types
// such as (int, evtype, long long) you can use PackTypes in Utils or overload
// manually each of the numeric operators +, -, *

class Evaluation: public Component
{
protected:
	// pair<evtype, evtype>: OST (Objective Space Type)
	evtype objFunction;
	evtype infMeasure;

	// map<string, bool> localStatus; // mapping 'move.id()' to 'NeighborhoodStatus' TODO: REMOVE!
	GOS gos;            // for exact methods only

	vector<pair<evtype, evtype> > alternatives; // for lexicographic approaches

public:
	Evaluation(const evtype& obj, const evtype& inf) :
			objFunction(obj), infMeasure(inf)
	{
		gos = gos_unknown;
	}

	Evaluation(const evtype& obj) :
		objFunction(obj)
	{
		infMeasure = 0;

		gos = gos_unknown;
	}


	Evaluation(const Evaluation& e) :
			objFunction(e.objFunction), infMeasure(e.infMeasure), gos(e.gos), alternatives(e.alternatives)
	{
	}


	virtual ~Evaluation()
	{
	}


	virtual Evaluation& operator=(const Evaluation& e)
	{
		if (&e == this) // auto ref check
			return *this;

		objFunction = e.objFunction;
		infMeasure = e.infMeasure;
		alternatives = e.alternatives;

		gos = e.gos;

		return *this;
	}

	virtual Evaluation& clone() const
	{
		return *new Evaluation(*this);
	}

	// ======================================

	// end canonical part
	// begin Evaluation methods

	evtype getObjFunction() const
	{
		return objFunction;
	}

	evtype getInfMeasure() const
	{
		return infMeasure;
	}

	const vector<pair<evtype, evtype> >& getAlternativeCosts() const
	{
		return alternatives;
	}

	void setObjFunction(evtype obj)
	{
		objFunction = obj;
	}

	void setInfMeasure(evtype inf)
	{
		infMeasure = inf;
	}

	void addAlternativeCost(const pair<evtype, evtype>& alternativeCost)
	{
		alternatives.push_back(alternativeCost);
	}

	void setAlternativeCosts(const vector<pair<evtype, evtype> >& alternativeCosts)
	{
		alternatives = alternativeCosts;
	}

	// -----------------
	// for local optimum
	// -----------------

	// TODO: remove! LOS management is now on NSSeq and NSSeqIterators
	/*
	bool getLocalOptimumStatus(string moveId)
	{
		return localStatus[moveId];
	}

	void setLocalOptimumStatus(string moveId, bool status)
	{
		localStatus[moveId] = status;
	}
	*/

	// ------------------
	// for global optimum
	// ------------------

	GOS getGlobalOptimumStatus()
	{
		return gos;
	}

	void setGlobalOptimumStatus(GOS status)
	{
		gos = status;
	}

	// evaluation = objFunction + infMeasure
	evtype evaluation() const
	{
		return objFunction + infMeasure;
	}

	// leave option to rewrite tolerance
	virtual bool isFeasible() const
	{
		return (::abs(infMeasure) < 0.0001);
	}

	// ======================
	// Object specific part
	// ======================

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
};

}

#endif /*OPTFRAME_EVALUATION_HPP_*/
