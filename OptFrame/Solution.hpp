#ifndef OPTFRAME_SOLUTION_HPP_
#define OPTFRAME_SOLUTION_HPP_

#include <cstdlib>
#include <iostream>

using namespace std;

template<class R>
class Solution
{
protected:
	R& r;

public:
	Solution(R& rr):r(rr){};
	Solution(const Solution<R>& s):r(*new R(s.r)){}

	virtual ~Solution() { delete &r; }

	void setR(const R& rr){ r = rr; }

	const R& getR() const {	return r; }
	R& getR() { return r; }

	virtual void print() const
	{
		cout << "Solution: "<< r << endl;
	}

	virtual Solution<R>& operator= (const Solution<R>& s)
	{
		if(&s == this) // auto ref check
			return *this;

		r = s.r;

		return *this;
	}

	virtual Solution<R>& clone() const
	{
		return * new Solution<R>(*this);
	}

};

#endif /* OPTFRAME_SOLUTION_HPP_ */
