#ifndef OPTFRAME_TESTEVALUATION_HPP_
#define OPTFRAME_TESTEVALUATION_HPP_

#include "../Evaluation.hpp"

#include <cstdlib>
#include <iostream>

using namespace std;

template<class M = OPTFRAME_DEFAULT_MEMORY>
class TestEvaluation: public Evaluation<M>
{
private:
	static const unsigned long long MAX_EV_IN_MEMORY_ERROR = 1000;
	static unsigned long long MAX_EV_IN_MEMORY_WARNING;

	static unsigned long long ev_objects;
	static unsigned long long ev_objects_nodecrement;

	unsigned long long ev_number;

public:

	TestEvaluation(double obj, double inf, M& mm) :
		Evaluation<M> (obj, inf, mm)
	{
		ev_objects++;
		ev_objects_nodecrement++;
		check();

		ev_number = ev_objects_nodecrement;
	}

	TestEvaluation(double obj, M& mm) :
		Evaluation<M> (obj, mm)
	{
		ev_objects++;
		ev_objects_nodecrement++;
		check();

		ev_number = ev_objects_nodecrement;
	}

	TestEvaluation(const TestEvaluation<M>& e) :
		Evaluation<M> (e)
	{
		ev_objects++;
		ev_objects_nodecrement++;
		check();

		ev_number = ev_objects_nodecrement;
	}

	virtual ~TestEvaluation()
	{
		ev_objects--;
	}

	void check() // check number of TestSolution objects in memory
	{

		if (ev_objects >= MAX_EV_IN_MEMORY_WARNING)
		{
			cout << "WARNING: " << TestEvaluation<M>::ev_objects
					<< " Evaluation objects in memory!" << endl;
			TestEvaluation<M>::MAX_EV_IN_MEMORY_WARNING++;
		}

		if (ev_objects >= MAX_EV_IN_MEMORY_ERROR)
		{
			cout << "ERROR: " << TestEvaluation<M>::ev_objects
					<< " Evaluation objects in memory!" << endl;
			cout << "MAX_EV_IN_MEMORY_ERROR = " << MAX_EV_IN_MEMORY_ERROR
					<< endl;
			cout << "aborting...";
			exit(1);
		}
	}

	void print() const
	{
		cout << "TestEvaluation #" << ev_number << " (" << ev_objects
				<< " in memory now) - ";

		cout << "Evaluation function value = " << Evaluation<M>::evaluation();
		cout << (Evaluation<M>::isFeasible() ? " " : " (not feasible) ")
				<< endl;

		// default - not printing memory
		//cout << Evaluation<M>::m << endl;
	}

	TestEvaluation<M>& operator=(const TestEvaluation<M>& e)
	{
		if (&e == this) // auto ref check
			return *this;

		*this = Evaluation<M>::operator=(e);

		// do not copy the 'ev_number'

		return *this;
	}

	Evaluation<M>& operator=(const Evaluation<M>& e)
	{
		return operator=((const TestEvaluation<M>&) e);
	}

	Evaluation<M>& clone() const
	{
		Evaluation<M>* e = new TestEvaluation<M> (*this);
		return (*e);
	}
};

template<class M>
unsigned long long TestEvaluation<M>::MAX_EV_IN_MEMORY_WARNING = 0.7
		* MAX_EV_IN_MEMORY_ERROR;

template<class M>
unsigned long long TestEvaluation<M>::ev_objects = 0;

template<class M>
unsigned long long TestEvaluation<M>::ev_objects_nodecrement = 0;

#endif /*OPTFRAME_TESTEVALUATION_HPP_*/
