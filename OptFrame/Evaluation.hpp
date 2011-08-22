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

typedef int OPTFRAME_DEFAULT_MEMORY;

#include <cstdlib>
#include <iostream>

using namespace std;

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

template<class M = OPTFRAME_DEFAULT_MEMORY>
class Evaluation
{
protected:
   double objFunction;
   double infMeasure;
   M& m;

public:
	Evaluation(double obj, double inf, M& mm):
		objFunction(obj),infMeasure(inf),m(*new M(mm)){};

	Evaluation(double obj, M& mm):
		m(*new M(mm))
	{
	   objFunction = obj;
	   infMeasure = 0;
	};

	Evaluation(const Evaluation<M>& e):
		objFunction(e.objFunction),infMeasure(e.infMeasure),m(*new M(e.m)){};

	virtual ~Evaluation() { delete &m; }

	const M& getM() const { return m; }
	M& getM() { return m; }
	double getObjFunction() const { return objFunction; }
	double getInfMeasure() const  { return infMeasure;  }

	void setM(const M& mm){ m = mm; }
	void setObjFunction(double obj){ objFunction = obj; }
	void setInfMeasure (double inf){ infMeasure = inf;  }

	virtual double evaluation() const { return objFunction + infMeasure; }
	virtual bool   isFeasible() const { return (infMeasure == 0); }

	virtual void print() const
	{
		cout << "Evaluation function value = " << evaluation();
		cout << (isFeasible()?" ":" (not feasible) ") << endl;

		// default - not printing memory
		// cout << m << endl;
	}

	virtual Evaluation& operator= (const Evaluation& e)
	{
		if(&e == this) // auto ref check
			return *this;

		m = e.m;
		objFunction = e.objFunction;
		infMeasure = e.infMeasure;

		return *this;
	}

	virtual Evaluation<M>& clone() const
	{
		return * new Evaluation<M>(*this);
	}
};

#endif /*OPTFRAME_EVALUATION_HPP_*/
