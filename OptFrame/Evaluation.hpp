#ifndef OPTFRAME_EVALUATION_HPP_
#define OPTFRAME_EVALUATION_HPP_

typedef int OPTFRAME_DEFAULT_MEMORY;

#include <cstdlib>
#include <iostream>

using namespace std;

template<class M = OPTFRAME_DEFAULT_MEMORY>
class Evaluation
{
protected:
	M& m;
	double objFunction;
	double infMeasure;

public:
	Evaluation(double obj, double inf, M& mm):
		objFunction(obj),infMeasure(inf),m(*new M(mm)){};

	Evaluation(double obj, M& mm):
		objFunction(obj),m(*new M(mm)){ infMeasure=0; };

	Evaluation(const Evaluation<M>& e):
		m(*new M(e.m)),objFunction(e.objFunction),infMeasure(e.infMeasure){};

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
