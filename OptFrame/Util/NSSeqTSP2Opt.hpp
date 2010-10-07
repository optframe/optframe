#ifndef OPTFRAME_NSSEQTSP2OPT_HPP_
#define OPTFRAME_NSSEQTSP2OPT_HPP_
/*
 Classical Problem: Traveling Salesman Problem

 The Neighborhood Structure 2-opt has proposed by Cross (1958)
 Croes G., A method for solving traveling salesman problems. Operations Research 6 (1958), pp. 791–812

 2-Opt is aplied for any problem that representation is like a vector<T>, where T is the type of the vector.

 e.g: vector<T> where type of T is int
 s means Solution

 initial s: 1 2 3 4 5 6 7 8

 s' after apply MoveTSP2Opt(2,5) in s

 s': 1 2 5 4 3 6 7 8

 s" after apply MoveTSP2Opt(3,8) in s'

 s": 1 2 5 8 7 6 3 4

 */

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class MoveTSP2Opt: public Move<vector<T> , M>
{
	typedef vector<T> Route;

protected:
	int p1, p2; // position 1 and position 2, respectively

public:

	MoveTSP2Opt(int _p1, int _p2) :
		p1(_p1), p2(_p2)
	{
	}

	virtual ~MoveTSP2Opt()
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
		return all_positive && (rep.size() >= 2);
	}

	Move<Route, M>& apply(Route& rep)
	{

		reverse(rep.begin() + p1, rep.begin() + p2);

		// r1->r1, r2->r2, e1->i1, e2->i2, n1->n2, n2->n1, i1->e1, i2->e2
		return *new MoveTSP2Opt(p1, p2);
	}

	virtual bool operator==(const Move<Route, M>& _m) const
	{
		const MoveTSP2Opt& m1 = (const MoveTSP2Opt&) _m;
		return ((m1.p1 == p1) && (m1.p2 == p2));
	}

	void print()
	{
		cout << "MoveTSP2Opt( ";
		cout << " edge " << p1 << " <=>  edge " << p2 << " )";
		cout << endl;
	}
};

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveTSP2Opt<T, M> >
class NSIteratorTSP2Opt: public NSIterator<vector<T> , M>
{
	typedef vector<T> Route;

private:

	MOVE* m;
	int p1, p2; // position 1 and position 2, respectively
	const Route& r;

public:

	NSIteratorTSP2Opt(const Route& _r) :
		r(_r)
	{
		m = NULL;
	}

	virtual ~NSIteratorTSP2Opt()
	{
	}

	void first()
	{
		if (r.size() >= 2)
		{
			p1 = 0;
			p2 = 2;
			m = new MoveTSP2Opt<T, M> (p1, p2);
		}
		else
			m = NULL;
	}

	void next()
	{
		if (!((p1 == r.size() - 2) && (p2 == r.size())))
		{
			if (p2 != r.size())
			{
				p2++;
			}
			else
			{
				p1++;
				p2 = p1 + 2;
			}

			m = new MoveTSP2Opt<T, M> (p1, p2);
		}
		else
			m = NULL;
	}

	bool isDone()
	{
		return m == NULL;
	}

	Move<Route, M>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqRouteShift. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveTSP2Opt<T, M> >
class NSSeqTSP2Opt: public NSSeq<vector<T> , M>
{
	typedef vector<T> Route;

private:

public:

	NSSeqTSP2Opt()
	{
	}

	virtual ~NSSeqTSP2Opt()
	{
	}

	Move<Route, M>& move(const Route& rep)
	{
		if (rep.size() < 2)
			return *new MOVE(-1, -1);

		int p1 = rand() % (rep.size() + 1);

		int p2;

		do
		{
			p1 = rand() % (rep.size() + 1);
		} while (abs(p1 - p2) < 2);

		// create 2-opt(p1,p2) move
		return *new MOVE(p1, p2);
	}

	virtual NSIterator<Route, M>& getIterator(const Route& r)
	{
		return *new NSIteratorTSP2Opt<T, M, MOVE> (r);
	}

	virtual void print()
	{
		cout << "NSSeqTSP2Opt" << endl;
	}
};

#endif /*OPTFRAME_NSSEQTSP2OPT_HPP_*/
