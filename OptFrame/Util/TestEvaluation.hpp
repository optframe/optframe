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

#ifndef OPTFRAME_TESTEVALUATION_HPP_
#define OPTFRAME_TESTEVALUATION_HPP_

#include "../Evaluation.hpp"

#include <cstdlib>
#include <iostream>

using namespace std;

namespace optframe
{

template<class DS = OPTFRAME_DEFAULT_DS>
class TestEvaluation: public Evaluation<DS>
{
private:
	static const unsigned long long MAX_EV_IN_MEMORY_ERROR = 1000;
	static unsigned long long MAX_EV_IN_MEMORY_WARNING;

	static unsigned long long ev_objects;
	static unsigned long long ev_objects_nodecrement;

	unsigned long long ev_number;

public:

	TestEvaluation(double obj, double inf, DS& mm) :
		Evaluation<DS> (obj, inf, mm)
	{
		ev_objects++;
		ev_objects_nodecrement++;
		check();

		ev_number = ev_objects_nodecrement;
	}

	TestEvaluation(double obj, DS& mm) :
		Evaluation<DS> (obj, mm)
	{
		ev_objects++;
		ev_objects_nodecrement++;
		check();

		ev_number = ev_objects_nodecrement;
	}

	TestEvaluation(const TestEvaluation<DS>& e) :
		Evaluation<DS> (e)
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
			cout << "WARNING: " << TestEvaluation<DS>::ev_objects
					<< " Evaluation objects in memory!" << endl;
			TestEvaluation<DS>::MAX_EV_IN_MEMORY_WARNING++;
		}

		if (ev_objects >= MAX_EV_IN_MEMORY_ERROR)
		{
			cout << "ERROR: " << TestEvaluation<DS>::ev_objects
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

		cout << "Evaluation function value = " << Evaluation<DS>::evaluation();
		cout << (Evaluation<DS>::isFeasible() ? " " : " (not feasible) ")
				<< endl;

		// default - not printing memory
		//cout << Evaluation<DS>::m << endl;
	}

	TestEvaluation<DS>& operator=(const TestEvaluation<DS>& e)
	{
		if (&e == this) // auto ref check
			return *this;

		*this = Evaluation<DS>::operator=(e);

		// do not copy the 'ev_number'

		return *this;
	}

	Evaluation<DS>& operator=(const Evaluation<DS>& e)
	{
		return operator=((const TestEvaluation<DS>&) e);
	}

	Evaluation<DS>& clone() const
	{
		Evaluation<DS>* e = new TestEvaluation<DS> (*this);
		return (*e);
	}
};

template<class DS >
unsigned long long TestEvaluation<DS>::MAX_EV_IN_MEMORY_WARNING = 0.7
		* MAX_EV_IN_MEMORY_ERROR;

template<class DS >
unsigned long long TestEvaluation<DS>::ev_objects = 0;

template<class DS >
unsigned long long TestEvaluation<DS>::ev_objects_nodecrement = 0;

}

#endif /*OPTFRAME_TESTEVALUATION_HPP_*/
