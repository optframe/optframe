#ifndef OPTFRAME_NSSEQVECTOROROPTK_HPP_
#define OPTFRAME_NSSEQVECTOROROPTK_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class MoveOrOptk: public Move<vector<T> , M>
{
	typedef vector<T> Route;

private:
	int i; // origin
	int j; // destination
	int k; // number of elements

public:

	MoveOrOptk(int _i, int _j, int _k) :
		i(_i), j(_j), k(_k)
	{
	}

	virtual ~MoveOrOptk()
	{
	}

	bool canBeApplied(const Route& rep)
	{
		//return (i != j) && (i + k <= rep.size());
		return (i != j);
	}

	Move<Route, M>& apply(Route& rep)
	{
		vector<T> v_aux;
		v_aux.insert(v_aux.begin(), rep.begin() + i, rep.begin() + i + k - 1);
		rep.erase(rep.begin() + i, rep.begin() + i + k - 1);
		rep.insert(rep.begin() + j, v_aux.begin(), v_aux.end());

		return *new MoveOrOptk(j, i, k);
	}

	virtual bool operator==(const Move<Route, M>& _m) const
	{
		const MoveOrOptk& m1 = (const MoveOrOptk&) _m;
		return (m1.i == i) && (m1.j == j) && (m1.k == k);
	}

	void print()
	{
		cout << "MoveVectorOrOpt{K=" << k << "}";
		cout << "(" << i << ";" << j << ")" << endl;
	}
};

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

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class NSSeqVectorOrOptk: public NSSeq<vector<T> , M>
{
	typedef vector<T> Route;
	int k;

public:

	NSSeqVectorOrOptk(int _k) :
		k(_k)
	{
	}

	virtual ~NSSeqVectorOrOptk()
	{
	}

	Move<Route, M>& move(const Route& rep)
	{
		int n = rep.size();

		if (n - k <= 0)
			return *new MoveOrOptk<T, M> (0, 0, k);

		int i = rand() % (n - k + 1);

		int j = i;
		while (i == j)
			j = rand() % (n - k + 1);

		return *new MoveOrOptk<T, M> (i, j, k);
	}

	virtual NSIterator<Route, M>& getIterator(const Route& r)
	{
		return *new NSIteratorOrOptk<T, M> (r.size(), k);
	}

	virtual void print()
	{
		cout << "NSSeqVectorOrOpt{K=" << k << "}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQVECTOROROPTK_HPP_*/
