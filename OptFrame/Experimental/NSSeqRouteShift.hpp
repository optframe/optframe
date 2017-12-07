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

#ifndef OPTFRAME_NSSEQROUTESHIFT_HPP_
#define OPTFRAME_NSSEQROUTESHIFT_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY>
class MoveRouteShift: public Move<vector<vector<T> > , DS >
{
	typedef vector<vector<T> > Routes;

protected:
	int r1, r2; // route 1 and route 2, respectively
	int e1, e2; // elements from route 1 and 2, respectively
	int n1, n2; // number of elements shifted from route 1 and 2, respectively
	int i1, i2; // place to be inserted on route 1 and 2, respectively

	vector<T> v1, v2; // auxiliar: elements to be shifted

public:

	MoveRouteShift(int _r1, int _r2, int _e1, int _e2, int _n1, int _n2, int _i1, int _i2) :
		r1(_r1), r2(_r2), e1(_e1), e2(_e2), n1(_n1), n2(_n2), i1(_i1), i2(_i2)
	{
		for (int e = e1; e < e1 + n1; e++)
			v1.push_back(e);
		for (int e = e2; e < e2 + n2; e++)
			v2.push_back(e);
	}

	virtual ~MoveRouteShift()
	{
	}

	int get_r1()
	{
		return r1;
	}

	int get_r2()
	{
		return r2;
	}

	int get_e1()
	{
		return e1;
	}

	int get_e2()
	{
		return e2;
	}

	int get_n1()
	{
		return n1;
	}

	int get_n2()
	{
		return n2;
	}

	int get_i1()
	{
		return i1;
	}

	int get_i2()
	{
		return i2;
	}

	bool canBeApplied(const Routes& rep)
	{
		bool all_positive = (e1 >= 0) && (e2 >= 0) && (i1 >= 0) && (i2 >= 0);
		return all_positive && (r1 < rep.size()) && (r2 < rep.size()) && ((rep.at(r1).size() - n1) >= 0) && ((rep.at(r2).size() - n2) >= 0);
	}

	Move<Routes, DS >& apply(Routes& rep)
	{
		vector<T> v1;
		vector<T> v2;

		for (int e = 0; e < n1; e++)
			v1.push_back(rep[r1][e1 + e]);
		for (int e = 0; e < n2; e++)
			v2.push_back(rep[r2][e2 + e]);

		if (n1 > 0)
			rep[r1].erase(rep[r1].begin() + e1, rep[r1].begin() + e1 + n1);
		if (n2 > 0)
			rep[r2].erase(rep[r2].begin() + e2, rep[r2].begin() + e2 + n2);

		if (n1 > 0) // there is something to insert in r2
			rep[r2].insert(rep[r2].begin() + i2, v1.begin(), v1.end());
		if (n2 > 0) // there is something to insert in r1
			rep[r1].insert(rep[r1].begin() + i1, v2.begin(), v2.end());

		// r1->r1, r2->r2, e1->i1, e2->i2, n1->n2, n2->n1, i1->e1, i2->e2
		return *new MoveRouteShift(r1, r2, i1, i2, n2, n1, e1, e2);
	}

	virtual bool operator==(const Move<Routes, DS >& _m) const
	{
		const MoveRouteShift& m1 = (const MoveRouteShift&) _m;

		bool empty_v1 = (v1.size() == 0) && (m1.v1.size() == 0);
		bool empty_v2 = (v2.size() == 0) && (m1.v2.size() == 0);

		//
		//if (empty_v1 && empty_v2)
		//	return true;
		//else
		if (empty_v1)
			return (m1.r1 == r1) && (m1.r2 == r2) && (m1.i1 == i1) && (m1.v2 == v2);
		else if (empty_v2)
			return (m1.r1 == r1) && (m1.r2 == r2) && (m1.i2 == i2) && (m1.v1 == v1);
		else
			return (m1.r1 == r1) && (m1.r2 == r2) && (m1.i1 == i1) && (m1.i2 == i2) && (m1.e1 == e1) && (m1.e2 == e2);
	}

	void print() const
	{
		cout << "MoveRouteShift( ";
		cout << "route " << r1 << " [ ";
		for (int e = 0; e < n1; e++)
			cout << (e1 + e) << " ";
		cout << "] to pos " << i2 << " <=> ";
		cout << "route " << r2 << " [ ";
		for (int e = 0; e < n2; e++)
			cout << (e2 + e) << " ";
		cout << "] to pos " << i1 << " )";
		cout << endl;
	}
};

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY, class MOVE = MoveRouteShift<T, DS > >
class NSIteratorRoutesShift: public NSIterator<vector<vector<T> > , DS >
{
	typedef vector<vector<T> > Routes;
	const Routes& routes;

private:
	int n1, n2; // number of elements shifted between routes
	vector<MOVE*> moves;

	MOVE* m;

public:

	NSIteratorRoutesShift(const Routes& r, int _n1, int _n2) :
		routes(r), n1(_n1), n2(_n2)
	{
		m = nullptr;
	}

	virtual ~NSIteratorRoutesShift()
	{
		if (moves.size() > 0)
		{
			for (unsigned int i = 0; i < moves.size(); i++)
				delete moves[i];
			moves.clear();
		}
	}

	void first()
	{
		if (moves.size() > 0)
		{
			for (unsigned int i = 0; i < moves.size(); i++)
				delete moves[i];
			moves.clear();
		}

		if (n2 > 0)
			for (int r1 = 0; r1 < routes.size(); r1++)
				for (int r2 = (r1 == 0) ? 1 : 0; r2 < routes.size(); r2 = (r2 + 1 == r1) ? r2 += 2 : r2 += 1)
					for (int e1 = 0; e1 <= (int) routes[r1].size() - n1; e1++)
						for (int e2 = 0; e2 <= (int) routes[r2].size() - n2; e2++)
							for (int i1 = 0; i1 <= (int) routes[r1].size() - n1; i1++)
								for (int i2 = 0; i2 <= (int) routes[r2].size() - n2; i2++)
									moves.push_back(new MOVE(r1, r2, e1, e2, n1, n2, i1, i2));
		else
			for (int r1 = 0; r1 < routes.size(); r1++)
				for (int r2 = (r1 == 0) ? 1 : 0; r2 < routes.size(); r2 = (r2 + 1 == r1) ? r2 += 2 : r2 += 1)
					for (int e1 = 0; e1 <= (int) routes[r1].size() - n1; e1++)
						for (int i2 = 0; i2 <= (int) routes[r2].size(); i2++)
							moves.push_back(new MOVE(r1, r2, e1, 0, n1, n2, 0, i2));

		if (moves.size() > 0)
		{
			m = moves[0];
			moves.erase(moves.begin() + 0);
		}
		else
			m = nullptr;
	}

	void next()
	{
		if (moves.size() > 0)
		{
			m = moves[0];
			moves.erase(moves.begin() + 0);
		}
		else
			m = nullptr;
	}

	bool isDone()
	{
		return m == nullptr;
	}

	Move<Routes, DS >& current()
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

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY, class MOVE = MoveRouteShift<T, DS > >
class NSSeqRouteShift: public NSSeq<vector<vector<T> > , DS >
{
	typedef vector<vector<T> > Routes;

private:
	int n1, n2; // number of elements shifted between routes

public:

	NSSeqRouteShift(int _n1, int _n2)
	{
		if (_n1 > _n2)
		{
			n1 = _n1;
			n2 = _n2;
		}
		else
		{
			n1 = _n2;
			n2 = _n1;
		}

		if (n1 <= 0)
		{
			n1 = 1;
			cout << "Warning: invalid NSSeqShift(" << _n1 << "," << _n2 << ")!" << endl;
		}

		if (n2 < 0)
		{
			n2 = 0;
			cout << "Warning: invalid NSSeqShift(" << _n1 << "," << _n2 << ")!" << endl;
		}

	}

	virtual ~NSSeqRouteShift()
	{
	}

	Move<Routes, DS >& move(const Routes& rep)
	{
		if (rep.size() < 2)
			return *new MOVE(0, 0, -1, -1, 0, 0, -1, -1);

		// select different routes
		int r1 = rand() % rep.size();
		int r2 = r1;
		while (r1 == r2)
			r2 = rand() % rep.size();

		int e1 = -1; // select elements to be shifted from route 1
		int i1 = -1; // select place to be insert elements in route 1

		if (n1 <= rep[r1].size())
		{
			e1 = rand() % (rep[r1].size() - (n1 - 1));
			i1 = rand() % (rep[r1].size() - n1 + 1);
		}

		int e2 = -1; // select elements to be shifted from route 2
		int i2 = -1; // select place to be insert elements in route 2
		if (n2 > 0)
			if (n2 <= rep[r2].size())
			{
				e2 = rand() % (rep[r2].size() - (n2 - 1));
				i2 = rand() % (rep[r2].size() - n2 + 1);
			}

		if (n2 == 0)
		{
			e2 = 0;
			i2 = rand() % (rep[r2].size() + 1);
		}

		// create shift(n1,n2) move
		return *new MOVE(r1, r2, e1, e2, n1, n2, i1, i2);
	}

	virtual NSIterator<Routes, DS >& getIterator(const Routes& r)
	{
		return *new NSIteratorRoutesShift<T, DS, MOVE> (r, n1, n2);
	}

	virtual void print()
	{
		cout << "NSSeqRouteShift" << endl;
	}
};

#endif /*OPTFRAME_NSSEQROUTESHIFT_HPP_*/
