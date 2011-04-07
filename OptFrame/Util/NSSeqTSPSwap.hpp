#ifndef OPTFRAME_NSSEQTSPSWAP_HPP_
#define OPTFRAME_NSSEQTSPSWAP_HPP_
/*
 Classical Problem: Traveling Salesman Problem

 The Classic Swap Neighborhood Structure.

 Article:

 Swap is applied for any problem that representation is like a vector<T>, where T is the type of the vector.

 e.g: vector<T> where type of T is int
 s means Solution

 initial s: 1 2 3 4 5 6 7 8

 s' after apply MoveTSPSwap(2,5) in s

 s': 1 2 6 4 5 3 7 8

 */

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class MoveTSPSwap: public Move<vector<T> , M>
{
	typedef vector<T> Route;

protected:
	int p1, p2; // position 1 and position 2, respectively

public:

	MoveTSPSwap(int _p1, int _p2) :
		p1(_p1), p2(_p2)
	{
	}

	virtual ~MoveTSPSwap()
	{
	}

	int get_p1()
	{
		return p1;
	}

	int get_p2()
	{
		return p2;
	}

	bool canBeApplied(const Route& rep)
	{
		bool all_positive = (p1 >= 0) && (p2 >= 0);
		bool size_ok = (p1 < rep.size()) && (p2 < rep.size());
		return all_positive && size_ok && (rep.size() >= 2);
	}

	Move<Route, M>& apply(Route& rep)
	{
		T t = rep[p1];
		rep[p1] = rep[p2];
		rep[p2] = t;

		return *new MoveTSPSwap(p1, p2);
	}

	virtual bool operator==(const Move<Route, M>& _m) const
	{
		const MoveTSPSwap& m1 = (const MoveTSPSwap&) _m;
		return ((m1.p1 == p1) && (m1.p2 == p2));
	}

	void print() const
	{
		cout << "MoveTSPSwap( " << p1 << " <=> " << p2 << " )" << endl;
	}
};

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveTSPSwap<T, M> >
class NSIteratorTSPSwap: public NSIterator<vector<T> , M>
{
	typedef vector<T> Route;

private:
	MOVE* m;
	int p1, p2; // position 1 and position 2, respectively
	int n;

public:

	NSIteratorTSPSwap(int _n) :
		n(_n), m(NULL)
	{
	}

	virtual ~NSIteratorTSPSwap()
	{
	}

	void first()
	{
		if (n >= 2)
		{
			p1 = 0;
			p2 = 1;
			m = new MoveTSPSwap<T, M> (p1, p2);
		}
		else
			m = NULL;
	}

	void next()
	{
		if (!((p1 == n - 2) && (p2 == n - 1)))
		{
			if (p2 < (n - 1))
				p2++;

			else
			{
				p1++;
				p2 = p1 + 1;
			}

			m = new MoveTSPSwap<T, M> (p1, p2);
		}
		else
			m = NULL;
	}

	bool isDone()
	{
		return (m == NULL);
	}

	Move<Route, M>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqTSPSwap. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveTSPSwap<T, M> >
class NSSeqTSPSwap: public NSSeq<vector<T> , M>
{
	typedef vector<T> Route;

private:

public:

	NSSeqTSPSwap()
	{
	}

	virtual ~NSSeqTSPSwap()
	{
	}

	Move<Route, M>& move(const Route& rep)
	{
		if (rep.size() < 2)
			return *new MOVE(-1, -1);

		int p1 = rand() % rep.size();

		int p2 = p1;

		while (p2 == p1)
			p2 = rand() % rep.size();

		return *new MOVE(p1, p2);
	}

	virtual NSIterator<Route, M>& getIterator(const Route& r)
	{
		return *new NSIteratorTSPSwap<T, M, MOVE> (r.size());
	}

	virtual void print()
	{
		cout << "NSSeqTSPSwap" << endl;
	}
};

#endif /*OPTFRAME_NSSEQTSPSWAP_HPP_*/
