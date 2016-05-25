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

#ifndef OPTFRAME_TESTEVALUATION_HPP_
#define OPTFRAME_TESTEVALUATION_HPP_

#include "../Evaluation.hpp"

#include <cstdlib>
#include <iostream>

using namespace std;

/*

namespace optframe
{

class TestEvaluation: public Evaluation
{
private:
	static const unsigned long long MAX_EV_IN_MEMORY_ERROR = 1000;
	static unsigned long long MAX_EV_IN_MEMORY_WARNING;

	static unsigned long long ev_objects;
	static unsigned long long ev_objects_nodecrement;

	unsigned long long ev_number;

public:

	TestEvaluation(double obj, double inf) :
			Evaluation(obj, inf)
	{
		ev_objects++;
		ev_objects_nodecrement++;
		check();

		ev_number = ev_objects_nodecrement;
	}

	TestEvaluation(double obj) :
			Evaluation(obj)
	{
		ev_objects++;
		ev_objects_nodecrement++;
		check();

		ev_number = ev_objects_nodecrement;
	}

	TestEvaluation(const TestEvaluation& e) :
			Evaluation(e)
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
			cout << "WARNING: " << TestEvaluation::ev_objects
					<< " Evaluation objects in memory!" << endl;
			TestEvaluation::MAX_EV_IN_MEMORY_WARNING++;
		}

		if (ev_objects >= MAX_EV_IN_MEMORY_ERROR)
		{
			cout << "ERROR: " << TestEvaluation::ev_objects
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

		cout << "Evaluation function value = " << Evaluation::evaluation();
		cout << (Evaluation::isFeasible() ? " " : " (not feasible) ")
				<< endl;

		// default - not printing memory
		//cout << Evaluation::m << endl;
	}

	TestEvaluation& operator=(const TestEvaluation& e)
	{
		if (&e == this) // auto ref check
			return *this;

		*this = Evaluation::operator=(e);

		// do not copy the 'ev_number'

		return *this;
	}

	Evaluation& operator=(const Evaluation& e)
	{
		return operator=((const TestEvaluation&) e);
	}

	Evaluation& clone() const
	{
		Evaluation* e = new TestEvaluation(*this);
		return (*e);
	}
};

unsigned long long TestEvaluation::MAX_EV_IN_MEMORY_WARNING = 0.7 * MAX_EV_IN_MEMORY_ERROR;

unsigned long long TestEvaluation::ev_objects = 0;

unsigned long long TestEvaluation::ev_objects_nodecrement = 0;

}

*/

#endif /*OPTFRAME_TESTEVALUATION_HPP_*/
