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

#ifndef OPTFRAME_TESTSOLUTION_HPP_
#define OPTFRAME_TESTSOLUTION_HPP_

#include "../Solution.hpp"

#include <cstdlib>
#include <iostream>

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class TestSolution: public Solution<R, ADS>
{
private:
	static const unsigned long long MAX_SOL_IN_MEMORY_ERROR = 100000;
	static unsigned long long MAX_SOL_IN_MEMORY_WARNING;
	static int LAST_WARNING_RATIO;

	static unsigned long long testsolution_objects;
	static unsigned long long testsolution_objects_nodecrement;

	unsigned long long testsolution_number;

public:

	explicit TestSolution(R& _r) :
			Solution<R, ADS>(_r)
	{
		testsolution_objects++;
		testsolution_objects_nodecrement++;
		memcheck();

		testsolution_number = testsolution_objects_nodecrement;
	}

	explicit TestSolution(R&& _r) :
			Solution<R, ADS>(_r)
	{
		testsolution_objects++;
		testsolution_objects_nodecrement++;
		memcheck();

		testsolution_number = testsolution_objects_nodecrement;
	}

	TestSolution(R& _r, ADS& _ads) :
			Solution<R, ADS>(_r, _ads)
	{
		testsolution_objects++;
		testsolution_objects_nodecrement++;
		memcheck();

		testsolution_number = testsolution_objects_nodecrement;
	}

	TestSolution(const TestSolution<R, ADS>& s) :
			Solution<R, ADS>(s)
	{
		testsolution_objects++;
		testsolution_objects_nodecrement++;
		memcheck();

		testsolution_number = testsolution_objects_nodecrement;
	}

	virtual ~TestSolution()
	{
		testsolution_objects--;
	}

	void memcheck() // check number of TestSolution objects in memory
	{
		double dratio = ((double) testsolution_objects) / MAX_SOL_IN_MEMORY_ERROR;
		int ratio = (int) (dratio * 100);
		if ((testsolution_objects >= MAX_SOL_IN_MEMORY_WARNING) && (ratio % 10 == 0) && (ratio > LAST_WARNING_RATIO))
		{
			LAST_WARNING_RATIO = ratio;
			cout << "WARNING: " << TestSolution<R, ADS>::testsolution_objects << " TestSolution objects in memory!" << endl;
			TestSolution<R, ADS>::MAX_SOL_IN_MEMORY_WARNING++;
		}

		if (testsolution_objects >= MAX_SOL_IN_MEMORY_ERROR)
		{
			cout << "ERROR: " << TestSolution<R, ADS>::testsolution_objects << " TestSolution objects in memory!" << endl;
			cout << "MAX_SOL_IN_MEMORY_ERROR = " << MAX_SOL_IN_MEMORY_ERROR << endl;
			cout << "aborting...";
			exit(1);
		}
	}

	void print() const
	{
		cout << "TestSolution #" << testsolution_number << " (" << testsolution_objects << " in memory now): ";
		cout << (Solution<R, ADS>::r) << endl;
	}

	TestSolution<R, ADS>& operator=(const TestSolution<R, ADS>& s)
	{
		if (&s == this) // auto ref check
			return *this;

		*this = Solution<R, ADS>::operator=(s);

		// do not copy the 'testsolution_number'

		return *this;
	}

	Solution<R, ADS>& operator=(const Solution<R, ADS>& s)
	{
		cout << __PRETTY_FUNCTION__ << endl;
		return operator=((const TestSolution<R, ADS>&) s);
	}

	Solution<R, ADS>& clone() const
	{
		Solution<R, ADS>* s = new TestSolution<R, ADS>(*this);
		return (*s);
	}
};

template<class R, class ADS>
unsigned long long TestSolution<R, ADS>::MAX_SOL_IN_MEMORY_WARNING = 0.6 * MAX_SOL_IN_MEMORY_ERROR;

template<class R, class ADS>
int TestSolution<R, ADS>::LAST_WARNING_RATIO = 59; // SMALLER THAN 0.6

template<class R, class ADS>
unsigned long long TestSolution<R, ADS>::testsolution_objects = 0;

template<class R, class ADS>
unsigned long long TestSolution<R, ADS>::testsolution_objects_nodecrement = 0;

}

#endif /* OPTFRAME_TESTSOLUTION_HPP_ */
