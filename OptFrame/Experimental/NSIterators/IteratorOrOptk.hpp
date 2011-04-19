#ifndef OPTFRAME_NSITERATOROROPTK_HPP_
#define OPTFRAME_NSITERATOROROPTK_HPP_

// Framework includes
#include "../../NSIterator.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class NSIteratorOrOptk: public NSIterator<vector<T> , M>
{
	typedef vector<T> Route;

private:
	int n, k;
	int i, j;

public:

	NSIteratorOrOptk(int _n, int _k) :
		n(_n), k(_k)
	{
	}

	virtual ~NSIteratorOrOptk()
	{
	}

	void first()
	{
		i = 0;
		j = 1;
	}

	void next()
	{
		j++;

		if (j == i)
			j++;

		if (j > n - k)
		{
			j = 0;
			i++;
		}
	}

	bool isDone()
	{
		return i > n - k;
	}

	Move<Route, M>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "OrOpt{K=" << k << "}. Aborting." << endl;
			exit(1);
		}

		return *new MoveOrOptk<T, M> (i, j, k);
	}
};

#endif /*OPTFRAME_NSITERATOROROPTK_HPP_*/
