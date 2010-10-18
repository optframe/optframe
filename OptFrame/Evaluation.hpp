#ifndef EVALUATION_HPP_
#define EVALUATION_HPP_

#include <iostream>

using namespace std;

template<class M>
class Evaluation
{
private:
	M* memory;
	double objFunction;
	double infMeasure;

public:

	Evaluation()
	{
		objFunction = 0;
		infMeasure = 0;
		memory = NULL;
	}

	Evaluation(double objFunction, double infMeasure, M* memory);
	Evaluation(double objFunction, M* memory);
	Evaluation(double objFunction, double infMeasure);

	virtual ~Evaluation()
	{
		if(memory)
			delete memory;
	}

	virtual double evaluation();

	virtual bool isFeasible();
	virtual void print();
	Evaluation<M>* clone();


	M* getMemory(){return memory;};
	double getObjFunction(){return objFunction;};
	double getInfMeasure(){return infMeasure;};

	void setMemory(M* m){memory = m;};
	void setObjFunction(double o){objFunction = o;};
	void setInfMeasure(double i){infMeasure = i;};
};


template <class M>
Evaluation<M>::Evaluation(double objFunction, double infMeasure, M* memory)
{
	this->objFunction = objFunction;
	this->infMeasure = infMeasure;
	this->memory = memory;
}

template <class M>
Evaluation<M>::Evaluation(double objFunction, M* memory)
{
	this->objFunction = objFunction;
	this->infMeasure = 0;
	this->memory = memory;
}

template <class M>
Evaluation<M>::Evaluation(double objFunction, double infMeasure)
{
	this->objFunction = objFunction;
	this->infMeasure = infMeasure;
	this->memory = NULL;
}

template <class M>
double Evaluation<M>::evaluation()
{
	return objFunction + infMeasure;
}

template <class M>
bool Evaluation<M>::isFeasible()
{
	return (infMeasure == 0);
}

template <class M>
void Evaluation<M>::print()
{
	cout << "Evaluation function value = " << evaluation();
	cout << (isFeasible()?" ":" (not feasible) ") << endl;

	/*
	if(memory)
		cout << *(memory) << endl;
	else
		cout << "No Memory!" << endl;
	*/
}


template<class M>
Evaluation<M>* Evaluation<M>::clone()
{
    M* nova = NULL;

    if(memory)
		nova = new M(*memory);

	return new Evaluation<M>(objFunction, infMeasure,nova);
}

#endif /*EVALUATION_HPP_*/
