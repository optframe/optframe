#ifndef OPTFRAME_TESTSOLUTION_HPP_
#define OPTFRAME_TESTSOLUTION_HPP_

#include "../Solution.hpp"

#include <cstdlib>
#include <iostream>

template<class R>
class TestSolution : public Solution<R>
{
private:
	static const unsigned long long MAX_SOL_IN_MEMORY_ERROR = 1000;
	static unsigned long long MAX_SOL_IN_MEMORY_WARNING;

	static unsigned long long testsolution_objects;
	static unsigned long long testsolution_objects_nodecrement;

	unsigned long long testsolution_number;

public:
	TestSolution(R& rr):Solution<R>(rr)
	{
		testsolution_objects++;
		testsolution_objects_nodecrement++;
		check();

		testsolution_number = testsolution_objects_nodecrement;
	}

	TestSolution(const TestSolution<R>& s):Solution<R>(s)
	{
		testsolution_objects++;
		testsolution_objects_nodecrement++;
		check();

		testsolution_number = testsolution_objects_nodecrement;
	}

	virtual ~TestSolution() { testsolution_objects--; }

	void check() // check number of TestSolution objects in memory
	{
		if(testsolution_objects >= MAX_SOL_IN_MEMORY_WARNING)
		{
			cout << "WARNING: " << TestSolution<R>::testsolution_objects << " TestSolution objects in memory!" << endl;
			TestSolution<R>::MAX_SOL_IN_MEMORY_WARNING++;
		}

		if(testsolution_objects >= MAX_SOL_IN_MEMORY_ERROR)
		{
			cout << "ERROR: " << TestSolution<R>::testsolution_objects << " TestSolution objects in memory!" << endl;
			cout << "MAX_SOL_IN_MEMORY_ERROR = "<< MAX_SOL_IN_MEMORY_ERROR << endl;
			cout << "aborting...";
			exit(1);
		}
	}

	void print() const
	{
		cout << "TestSolution #"<<testsolution_number<<" ("<<testsolution_objects<<" in memory now): ";
		cout << Solution<R>::r << endl;
	}

	TestSolution<R>& operator= (const TestSolution<R>& s)
	{
		if(&s == this) // auto ref check
			return *this;

		*this = Solution<R>::operator=(s);

		// do not copy the 'testsolution_number'

		return *this;
	}

	Solution<R>& operator=(const Solution<R>& s)
	{
		return operator=((const TestSolution<R>&)s);
	}

	Solution<R>& clone() const
	{
		Solution<R>* s = new TestSolution<R>(*this);
		return (*s);
	}
};

template<class R>
unsigned long long TestSolution<R>::MAX_SOL_IN_MEMORY_WARNING = 0.7*MAX_SOL_IN_MEMORY_ERROR;

template<class R>
unsigned long long TestSolution<R>::testsolution_objects = 0;

template<class R>
unsigned long long TestSolution<R>::testsolution_objects_nodecrement = 0;

#endif /* OPTFRAME_TESTSOLUTION_HPP_ */
