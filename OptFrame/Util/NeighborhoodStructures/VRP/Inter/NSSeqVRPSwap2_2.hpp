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


#ifndef OPTFRAME_NSSEQVRPSWAP2_2_HPP_
#define OPTFRAME_NSSEQVRPSWAP2_2_HPP_

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MoveVRPSwap2_2: public Move<vector<vector<T> >, ADS, DS >
{

	typedef vector<vector<T> > Routes;

protected:
	int r1, r2; // route 1 and route 2;
	int c1, c2; // position 1 and position 2 of the clients, respectively
	bool reverseR1, reverseR2; // true -> reverse movement of route R

public:

	MoveVRPSwap2_2(int _r1, int _r2, int _c1, int _c2, bool _reverseR1, bool _reverseR2, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr) :
		r1(_r1), r2(_r2), c1(_c1), c2(_c2), reverseR1(_reverseR1), reverseR2(_reverseR2), problem(_problem)
	{
	}

	virtual ~MoveVRPSwap2_2()
	{
	}

	bool get_reverseR1()
	{
		return reverseR1;
	}

	bool get_reverseR2()
	{
		return reverseR2;
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
		return all_positive && (rep.size() >= 2) && (rep.at(r1).size() >= 2) && (rep.at(r2).size() >= 2);
	}

	virtual Move<Routes, ADS, DS>& apply(Routes& rep, ADS&)
	{
		int aux;

		if (reverseR1)
		{
			aux = rep.at(r1).at(c1);
			rep.at(r1).at(c1) = rep.at(r1).at(c1 + 1);
			rep.at(r1).at(c1 + 1) = aux;
		}

		if (reverseR2)
		{
			aux = rep.at(r2).at(c2);
			rep.at(r2).at(c2) = rep.at(r2).at(c2 + 1);
			rep.at(r2).at(c2 + 1) = aux;
		}

		//swapping
		aux = rep.at(r1).at(c1);
		rep.at(r1).at(c1) = rep.at(r2).at(c2);
		rep.at(r2).at(c2) = aux;

		aux = rep.at(r1).at(c1 + 1);
		rep.at(r1).at(c1 + 1) = rep.at(r2).at(c2 + 1);
		rep.at(r2).at(c2 + 1) = aux;

		// p->p, r1->r2, r2->r1, c1->c2, c2->c1, reverse->reverse,
		return *new MoveVRPSwap2_2<T, ADS, DS > (r2, r1, c2, c1, reverseR1, reverseR2);
	}

	virtual bool operator==(const Move<Routes>& _m) const
	{
		const MoveVRPSwap2_2<T, ADS, DS >& m1 = (const MoveVRPSwap2_2<T, ADS, DS >&) _m;
		return ((m1.r1 == r1) && (m1.r2 == r2) && (m1.c1 == c1) && (m1.c2 == c2) && (m1.reverseR1 == reverseR1) && (m1.reverseR2 == reverseR2));
	}

	void print() const
	{
		cout << "MoveVRPSwap2_2( ";
		cout << r1 << ", " << r2 << ", " << c1 << ", " << c2 << ", " << reverseR1 << ", " << reverseR2 << " )";
		cout << endl;
	}
};


template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS, class MOVE = MoveVRPSwap2_2<T, ADS, DS>, class P = OPTFRAME_DEFAULT_PROBLEM>
class NSIteratorVRPSwap2_2: public NSIterator<vector<vector<T> > >
{
	typedef vector<vector<T> > Routes;

private:

	MOVE* m;
	vector<MOVE*> moves;
	int index; //index of moves
	const Routes& r;

	P* p; // has to be the last
public:

	NSIteratorVRPSwap2_2(const Routes& _r) :
		r(_r), p(_p)
	{
		m = nullptr;
		index = 0;
	}

	virtual ~NSIteratorVRPSwap2_2()
	{
		/*for (int i = 0; i < moves.size(); i++)
		 delete moves[i];*/
	}

	virtual void first()
	{

		for (int r1 = 0; r1 < r.size() - 1; r1++)
		{
			if (r.at(r1).size() >= 2) //verificando se há pelo menos dois clientes.
			{
				for (int r2 = r1 + 1; r2 < r.size(); r2++)
				{
					if (r.at(r2).size() >= 2) //verificando se há pelo menos dois clientes.
					{
						for (int c1 = 0; c1 < r.at(r1).size() - 1; c1++)
						{
							for (int c2 = 0; c2 < r.at(r2).size() - 1; c2++)
							{
								moves.push_back(new MOVE(r1, r2, c1, c2, false, false));//normal
								moves.push_back(new MOVE(r1, r2, c1, c2, true, false));//reverse route 1
								moves.push_back(new MOVE(r1, r2, c1, c2, false, true));//reverse route 2
								moves.push_back(new MOVE(r1, r2, c1, c2, true, true));//reverse booth
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
			cout << "NSSeqVRPSwap2_2. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};


template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS, class MOVE = MoveVRPSwap2_2<T, ADS, DS>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPSwap2_2<T, ADS, DS, MOVE, P > >
class NSSeqVRPSwap2_2: public NSSeq<vector<vector<T> > >
{
	typedef vector<vector<T> > Routes;

private:
	P* p; // has to be the last

public:

	NSSeqVRPSwap2_2(P* _p = nullptr) :
		p(_p)
	{
	}

	virtual ~NSSeqVRPSwap2_2()
	{
	}

	Move<Routes, ADS, DS>& move(const Routes& rep, const ADS&)
	{
		if (rep.size() < 2)
			return *new MOVE(-1, -1, -1, -1, false, false, p);

		int r1 = rand() % rep.size();

		if (rep.at(r1).size() < 2)
			return *new MOVE(-1, -1, -1, -1, false, false, p);

		int r2;

		do
		{
			r2 = rand() % rep.size();
		} while (r1 == r2);

		if (rep.at(r2).size() < 2)
			return *new MOVE(-1, -1, -1, -1, false, false, p);

		int c1 = rand() % (rep.at(r1).size() - 1);
		int c2 = rand() % (rep.at(r2).size() - 1);

		bool reverseR1 = rand() % 2;
		bool reverseR2 = rand() % 2;

		return *new MOVE(r1, r2, c1, c2, reverseR1, reverseR2, p);
	}

	virtual NSIteratorVRPSwap2_2<T, ADS, DS, MOVE, P >& getIterator(const Routes& r, const ADS&)
	{
		return *new NSITERATOR (r, p);
	}

	virtual void print()
	{
		cout << "NSSeqVRPSwap2_2 with move: " << MOVE::idComponent();
	}
};

#endif /*OPTFRAME_NSSEQSWAP2_2_HPP_*/
