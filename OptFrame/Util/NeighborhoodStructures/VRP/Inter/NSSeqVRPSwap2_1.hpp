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


#ifndef OPTFRAME_NSSEQVRPSWAP2_1_HPP_
#define OPTFRAME_NSSEQVRPSWAP2_1_HPP_

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MoveVRPSwap2_1: public Move<vector<vector<T> >, ADS, DS >
{

	typedef vector<vector<T> > Routes;

protected:
	int r1, r2; // route 1 and route 2;
	int c1, c2; // position 1 and position 2 of the clients, respectively
	bool reverse; // true -> reverse moviment

public:

	MoveVRPSwap2_1(int _r1, int _r2, int _c1, int _c2, bool _reverse, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr) :
		r1(_r1), r2(_r2), c1(_c1), c2(_c2), reverse(_reverse), problem(_problem)
	{
	}

	virtual ~MoveVRPSwap2_1()
	{
	}

	bool get_reverse()
	{
		return reverse;
	}

	int get_r1()
	{
		return r1;
	}

	int get_r2()
	{
		return r2;
	}

	int get_c1()
	{
		return c1;
	}

	int get_c2()
	{
		return c2;
	}

	virtual bool canBeApplied(const Routes& rep, const ADS&)
	{
		bool all_positive = (r1 >= 0) && (r2 >= 0) && (c1 >= 0) && (c2 >= 0);
		return all_positive && (rep.size() >= 2) && (rep.at(r1).size() >= 2) && (rep.at(r2).size() > 0);
	}

	virtual Move<Routes, ADS, DS>& apply(Routes& rep, ADS&)
	{
		int aux;

		if (reverse)
		{
			aux = rep.at(r1).at(c1);
			rep.at(r1).at(c1) = rep.at(r1).at(c1 + 1);
			rep.at(r1).at(c1 + 1) = aux;
		}

		//swapping
		aux = rep.at(r1).at(c1);
		rep.at(r1).at(c1) = rep.at(r2).at(c2);
		rep.at(r2).at(c2) = aux;

		rep.at(r2).insert(rep.at(r2).begin() + c2 + 1, rep.at(r1).at(c1 + 1));
		rep.at(r1).erase(rep.at(r1).begin() + c1 + 1);

		// p->p, r1->r2, r2->r1, c1->c2, c2->c1, reverse->reverse,
		return *new MoveVRPSwap2_1<T, ADS, DS > (r2, r1, c2, c1, reverse);
	}

	virtual bool operator==(const Move<Routes>& _m) const
	{
		const MoveVRPSwap2_1<T, ADS, DS >& m1 = (const MoveVRPSwap2_1<T, ADS, DS >&) _m;
		return ((m1.r1 == r1) && (m1.r2 == r2) && (m1.c1 == c1) && (m1.c2 == c2) && (m1.reverse == reverse));
	}

	void print() const
	{
		cout << "MoveVRPSwap2_1( ";
		cout << r1 << ", " << r2 << ", " << c1 << ", " << c2 << ", " << reverse << " )";
		cout << endl;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS, class MOVE = MoveVRPSwap2_1<T, ADS, DS>, class P = OPTFRAME_DEFAULT_PROBLEM>
class NSIteratorVRPSwap2_1: public NSIterator<vector<vector<T> > >
{
	typedef vector<vector<T> > Routes;

private:
	MOVE* m;
	vector<MOVE*> moves;
	int index; //index of moves
	const Routes& r;

	P* p; // has to be the last
public:

	NSIteratorVRPSwap2_1(const Routes& _r) :
		r(_r), p(_p)
	{
		m = nullptr;
		index = 0;
	}

	virtual ~NSIteratorVRPSwap2_1()
	{
		/*for (int i = 0; i < moves.size(); i++)
		 delete moves[i];*/
	}

	virtual void first()
	{

		for (int r1 = 0; r1 < r.size(); r1++)
		{
			if (r.at(r1).size() >= 2)
			{
				for (int r2 = 0; r2 < r.size(); r2++)
				{
					if (r1 != r2)
					{
						for (int c1 = 0; c1 < ((int) r.at(r1).size()) - 1; c1++)
						{
							for (int c2 = 0; c2 < r.at(r2).size(); c2++)
							{
								moves.push_back(new MOVE(r1, r2, c1, c2, false,p));//normal
								moves.push_back(new MOVE(r1, r2, c1, c2, true,p));//reverse
							}
						}
					}
				}
			}
		}
		if (moves.size() > 0)
		{
			m = moves[index];
		}
		else
			m = nullptr;
	}

	virtual void next()
	{
		index++;
		if (index < moves.size())
		{
			m = moves[index];
		}
		else
			m = nullptr;
	}

	virtual bool isDone()
	{
		return m == nullptr;
	}

	virtual Move<T, ADS, DS>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqVRPSwap2_1. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};


template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS, class MOVE = MoveVRPSwap2_1<T, ADS, DS>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPSwap2_1<T, ADS, DS, MOVE, P > >
class NSSeqVRPSwap2_1: public NSSeq<vector<vector<T> > >
{
	typedef vector<vector<T> > Routes;

private:
	P* p; // has to be the last

public:

	NSSeqVRPSwap2_1(P* _p = nullptr) :
		p(_p)
	{
	}

	virtual ~NSSeqVRPSwap2_1()
	{
	}

	Move<Routes, ADS, DS>& move(const Routes& rep, const ADS&)
	{
		if (rep.size() < 2)
			return *new MOVE(-1, -1, -1, -1, false, p);

		int r1 = rand() % rep.size();

		if (rep.at(r1).size() < 2)
			return *new MOVE(-1, -1, -1, -1, false, p);

		int r2;

		do
		{
			r2 = rand() % rep.size();
		} while (r1 == r2);

		if (rep.at(r2).size() < 1)
			return *new MOVE(-1, -1, -1, -1, false, p);

		int c1 = rand() % (rep.at(r1).size() - 1);
		int c2 = rand() % rep.at(r2).size();

		bool reverse = rand() % 2;

		return *new MOVE(r1, r2, c1, c2, reverse, p);
	}

	virtual NSIteratorVRPSwap2_1<T, ADS, DS, MOVE, P >& getIterator(const Routes& r, const ADS&)
	{
		return *new NSITERATOR (r, p);
	}

	virtual void print()
	{
		cout << "NSSeqVRPSwap2_1 with move: " << MOVE::idComponent();
	}
};

#endif /*OPTFRAME_NSSEQVRPSWAP2_1_HPP_*/
