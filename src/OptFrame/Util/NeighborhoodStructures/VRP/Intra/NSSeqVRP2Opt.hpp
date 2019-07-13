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

#ifndef OPTFRAME_NSSEQVRP2OPT_HPP_
#define OPTFRAME_NSSEQVRP2OPT_HPP_

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

// Working structure: vector<T>
template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
class MoveVRP2Opt: public Move<vector<vector<T> > , ADS>
{
	typedef vector<vector<T> > Routes;

protected:
	int p1, p2; // position 1 and position 2, respectively
	int r; //route id

	OPTFRAME_DEFAULT_PROBLEM* problem;

public:

	MoveVRP2Opt(int _r, int _p1, int _p2, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr) :
		r(_r), p1(_p1), p2(_p2), problem(_problem)
	{
	}

	virtual ~MoveVRP2Opt()
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

	int get_r()
	{
		return r;
	}

	virtual bool canBeApplied(const Routes& rep, const ADS&)
	{
		bool all_positive = (p1 >= 0) && (p2 >= 0) && (r >= 0);
		return all_positive && (rep.at(r).size() >= 2);
	}

	virtual void updateNeighStatus(ADS& ads)
	{

	}

	virtual Move<Routes, ADS>* apply(Routes& rep, ADS&)
	{
		int small, bigger;
		if (p1 <= p2)
		{
			small = p1;
			bigger = p2;
		}
		else
		{
			small = p2;
			bigger = p1;
		}

		reverse(rep.at(r).begin() + small, rep.at(r).begin() + bigger);

		return new MoveVRP2Opt(r, p1, p2);
	}

	virtual bool operator==(const Move<Routes, ADS>& _m) const
	{
		const MoveVRP2Opt& m1 = (const MoveVRP2Opt&) _m;
		return ((m1.p1 == p1) && (m1.p2 == p2) && (m1.r == r));
	}

	void print() const
	{
		cout << "MoveVRP2Opt( route: " << r << " : ";
		cout << " edge " << p1 << " <=>  edge " << p2 << " )";
		cout << endl;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRP2Opt<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM>
class NSIteratorVRP2Opt: public NSIterator<vector<vector<T> > , ADS>
{
	typedef vector<vector<T> > Routes;

protected:

	MOVE* m;
	int index;
	vector<MOVE*> moves;
	const Routes& rep;

	P* p; // has to be the last

public:

	NSIteratorVRP2Opt(const Routes& _r, const ADS& _ads, P* _p = nullptr) :
		rep(_r), p(_p)
	{
		index = 0;
		m = nullptr;
	}

	virtual ~NSIteratorVRP2Opt()
	{
	}

	virtual void first()
	{
		for (int r = 0; r < rep.size(); r++)
		{
			int tam = rep.at(r).size() - 2;
			for (int p1 = 0; p1 < tam; p1++)
			{
				for (int p2 = p1 + 2; p2 < rep.at(r).size(); p2++)
				{
					moves.push_back(new MOVE(r, p1, p2, p));
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
			cout << "NSSeqVRP2Opt. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRP2Opt<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRP2Opt<T, ADS, MOVE, P> >
class NSSeqVRP2Opt: public NSSeq<vector<vector<T> > , ADS>
{
	typedef vector<vector<T> > Routes;

private:
	P* p; // has to be the last

public:

	NSSeqVRP2Opt(P* _p = nullptr) :
		p(_p)
	{
	}

	virtual ~NSSeqVRP2Opt()
	{
	}

	Move<Routes, ADS>& move(const Routes& rep, const ADS&)
	{
		int r = rand() % rep.size();
		if (rep.at(r).size() < 3)
			return *new MOVE(-1, -1, -1, p);

		int p1 = rand() % (rep.at(r).size() + 1);

		int p2;

		do
		{
			p2 = rand() % (rep.at(r).size() + 1);
		} while (abs(p1 - p2) < 2);

		// create 2-opt(p1,p2) move
		return *new MOVE(r, p1, p2, p);
	}

	virtual NSITERATOR& getIterator(const Routes& r, const ADS& ads)
	{
		return *new NSITERATOR(r, ads, p);
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqVRP2Opt with move: " << MOVE::idComponent();
		return ss.str();
	}
};

#endif /*OPTFRAME_NSSEQVRP2OPT_HPP_*/
