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

typedef int OPTFRAME_DEFAULT_DS;

#include <cstdlib>
#include <iostream>
#include <cmath>

#include "OptFrameComponent.hpp"
#include "Action.hpp"

using namespace std;

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
class Evaluation : public OptFrameComponent
{
protected:
	double objFunction;
	double infMeasure;
	DS* ds; // delta structure

	map<string, Status> localStatus; // mapping 'move.id()' to 'NeighborhoodStatus'
	Status globalOptimum;            // for exact methods only

public:
	Evaluation(double obj, double inf, DS& _ds):
		objFunction(obj),infMeasure(inf),ds(new DS(_ds))
	{
	};

	Evaluation(double obj, DS& _ds):
		ds(new DS(_ds))
	{
		objFunction = obj;
		infMeasure = 0;
	};

	Evaluation(double obj) :
		ds(new OPTFRAME_DEFAULT_DS)
	{
		objFunction = obj;
		infMeasure = 0;
	}

	Evaluation(const Evaluation<DS>& e):
		objFunction(e.objFunction), infMeasure(e.infMeasure), ds(new DS(*e.ds))
	{
	};

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

	void setInfMeasure (double inf)
	{
		infMeasure = inf;
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
		return (abs(infMeasure)<0.0001);
	}

	static string idComponent()
	{
		return "OptFrame:Evaluation";
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual void print() const
	{
		cout << fixed; // disable scientific notation
		cout << "Evaluation function value = " << evaluation();
		cout << (isFeasible()?" ":" (not feasible) ") << endl;

		// default - not printing ememory
		// cout << m << endl;
	}

	virtual Evaluation& operator= (const Evaluation& e)
	{
		if(&e == this) // auto ref check
			return *this;

		(*ds) = (*e.ds);
		objFunction = e.objFunction;
		infMeasure = e.infMeasure;

		return *this;
	}

	virtual Evaluation<DS>& clone() const
	{
		return * new Evaluation<DS>(*this);
	}
};


namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class EvaluationAction: public Action<R, ADS, DS>
{
public:

	virtual ~EvaluationAction()
	{
	}

	virtual string usage()
	{
		return "OptFrame:Evaluation idx  evaluation  output_variable\nOptFrame:Evaluation idx  print\n";
	}

	virtual bool handleComponent(string component)
	{
		return (component == Evaluation<DS>::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "evaluation") || (action == "print");
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

			stringstream ss;
			ss << fixed;
			ss << e->evaluation();

			dictionary[var] = ss.str();

			return true;
		}

		if (action == "print")
		{
			e->print();

			return true;
		}

		// no action found!
		return false;
	}

};

}


#endif /*OPTFRAME_EVALUATION_HPP_*/
