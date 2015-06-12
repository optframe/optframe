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

#ifndef TESTMOVE_HPP_
#define TESTMOVE_HPP_

#include "../Move.hpp"

#include <cstdlib>
#include <iostream>

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class TestMove: public Move<R, ADS>
{
private:
	static const unsigned long long MAX_MOVE_IN_MEMORY_ERROR = 100000;
	static unsigned long long MAX_MOVE_IN_MEMORY_WARNING;

	static unsigned long long testmove_objects;
	static unsigned long long testmove_objects_nodecrement;

	unsigned long long testmove_number;

public:
	TestMove() :
			Move<R, ADS>()
	{
		testmove_objects++;
		testmove_objects_nodecrement++;
		check();

		testmove_number = testmove_objects_nodecrement;
	}

	virtual ~TestMove()
	{
		testmove_objects--;
	}

	void check() // check number of TestMove objects in memory
	{
		if (testmove_objects >= MAX_MOVE_IN_MEMORY_WARNING)
		{
			cout << "WARNING: " << TestMove<R, ADS>::testmove_objects << " TestMove objects in memory!" << endl;
			TestMove<R, ADS>::MAX_MOVE_IN_MEMORY_WARNING++;
		}

		if (testmove_objects >= MAX_MOVE_IN_MEMORY_ERROR)
		{
			cout << "ERROR: " << TestMove<R, ADS>::testmove_objects << " TestMove objects in memory!" << endl;
			cout << "MAX_MOVE_IN_MEMORY_ERROR = " << MAX_MOVE_IN_MEMORY_ERROR << endl;
			cout << "aborting...";
			exit(1);
		}
	}

	void print() const
	{
		cout << "TestMove #" << testmove_number << " (" << testmove_objects << " in memory now): \n";
	}

	TestMove<R, ADS>& operator=(const TestMove<R, ADS>& m)
	{
		if (&m == this) // auto ref check
			return *this;

		*this = Move<R, ADS>::operator=(m);

		// do not copy the 'testmove_number'

		return *this;
	}

	Move<R, ADS>& operator=(const Move<R, ADS>& m)
	{
		return operator=((const TestMove<R, ADS>&) m);
	}

	Move<R, ADS>& clone() const
	{
		Move<R, ADS>* m = new TestMove<R, ADS>(*this);
		return (*m);
	}
};

template<class R, class ADS>
unsigned long long TestMove<R, ADS>::MAX_MOVE_IN_MEMORY_WARNING = 0.7 * MAX_MOVE_IN_MEMORY_ERROR;

template<class R, class ADS>
unsigned long long TestMove<R, ADS>::testmove_objects = 0;

template<class R, class ADS>
unsigned long long TestMove<R, ADS>::testmove_objects_nodecrement = 0;

}

#endif /* TESTMOVE_HPP_ */
