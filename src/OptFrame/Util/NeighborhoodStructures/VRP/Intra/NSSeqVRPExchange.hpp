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

#ifndef OPTFRAME_NSSEQVRPEXCHANGE_HPP_
#define OPTFRAME_NSSEQVRPEXCHANGE_HPP_

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
class MoveVRPExchange: public Move<vector<vector<T> > , ADS>
{

	typedef vector<vector<T> > Routes;
private:
	OPTFRAME_DEFAULT_PROBLEM* problem;

protected:
	int c1, c2; // client 1 and client 2, respectively
	int r; //route


public:

	MoveVRPExchange(int _r, int _c1, int _c2, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr) :
		r(_r), c1(_c1), c2(_c2), problem(_problem)
	{
	}

	virtual ~MoveVRPExchange()
	{
	}

	int get_c1()
	{
		return c1;
	}

	int get_c2()
	{
		return c2;
	}

	int get_r()
	{
		return r;
	}

	virtual bool canBeApplied(const Routes& rep, const ADS&)
	{
		bool all_positive = (c1 >= 0) && (c2 >= 0) && (r >= 0);
		return all_positive && (rep.at(r).size() >= 2);
	}

	virtual void updateNeighStatus(ADS& ads)
	{

	}

	virtual Move<Routes, ADS>* apply(Routes& rep, ADS&)
	{

		T aux = rep.at(r).at(c1);
		rep.at(r).at(c1) = rep.at(r).at(c2);
		rep.at(r).at(c2) = aux;

		return new MoveVRPExchange(r, c1, c2);
	}

	virtual bool operator==(const Move<Routes, ADS>& _m) const
	{
		const MoveVRPExchange& m1 = (const MoveVRPExchange&) _m;
		return ((m1.c1 == c1) && (m1.c2 == c2) && (m1.r == r));
	}

	void print() const
	{
		cout << "MoveVRPExchange( route: " << r << " : ";
		cout << " client " << c1 << " <=>  client" << c2 << " )";
		cout << endl;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRPExchange<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM>
class NSIteratorVRPExchange: public NSIterator<vector<vector<T> > , ADS>
{

	typedef vector<vector<T> > Routes;

protected:

	MOVE* m;
	int index;
	vector<MOVE*> moves;
	const Routes& rep;

	P* p; // has to be the last
public:

	NSIteratorVRPExchange(const Routes& _r, const ADS& _ads, P* _p = nullptr) :
		rep(_r), p(_p)
	{
		index = 0;
		m = nullptr;
	}

	virtual ~NSIteratorVRPExchange()
	{
	}

	virtual void first()
	{
		for (int r = 0; r < rep.size(); r++)
		{
			for (int c1 = 0; c1 < rep.at(r).size(); c1++)
			{
				for (int c2 = 0; c2 < rep.at(r).size(); c2++)
				{
					if (c1 != c2)
						moves.push_back(new MOVE(r, c1, c2, p));
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

	virtual Move<Routes, ADS>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqVRPExchange. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRPExchange<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPExchange<T, ADS, MOVE, P> >
class NSSeqVRPExchange: public NSSeq<vector<vector<T> > , ADS>
{
	typedef vector<vector<T> > Routes;

private:
	P* p; // has to be the last

public:

	NSSeqVRPExchange(P* _p = nullptr) :
		p(_p)
	{
	}

	virtual ~NSSeqVRPExchange()
	{
	}

	Move<Routes, ADS>& move(const Routes& rep, const ADS&)
	{
		int r = rand() % rep.size();
		if (rep.at(r).size() < 2)
			return *new MOVE(-1, -1, -1, p);

		int c1 = rand() % rep.at(r).size();

		int c2;

		do
		{
			c2 = rand() % rep.at(r).size();
		} while (c1 == c2);

		// create exchange(p1,p2) move
		return *new MOVE(r, c1, c2, p);
	}

	virtual NSITERATOR& getIterator(const Routes& r, const ADS& ads)
	{
		return *new NSITERATOR(r, ads, p);
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqVRPExchange with move: " << MOVE::idComponent();
		return ss.str();
	}
};

#endif /*OPTFRAME_NSSEQVRPEXCHANGE_HPP_*/
