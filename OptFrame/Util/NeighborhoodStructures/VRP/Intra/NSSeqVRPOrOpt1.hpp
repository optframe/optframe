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

#ifndef OPTFRAME_NSSEQVRP0ROPT1_HPP_
#define OPTFRAME_NSSEQVRP0ROPT1_HPP_

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
class MoveVRPOrOpt1: public Move<vector<vector<T> > , ADS>
{

	typedef vector<vector<T> > Routes;

private:
	OPTFRAME_DEFAULT_PROBLEM* problem;

protected:
	int r; // route id
	int c; // client
	int pos; // position


public:

	MoveVRPOrOpt1(int _r, int _c, int _pos, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr) :
		r(_r), c(_c), pos(_pos), problem(_problem)
	{
	}

	virtual ~MoveVRPOrOpt1()
	{
	}

	int getRoute()
	{
		return r;
	}

	int getClient()
	{
		return c;
	}

	int getPosition()
	{
		return pos;
	}

	virtual bool canBeApplied(const Routes& rep, const ADS&)
	{
		bool all_positive = (r >= 0) && (c >= 0) && (pos >= 0);
		return all_positive && (c != pos) && (c + 1 != pos);
	}

	virtual void updateNeighStatus(ADS& ads)
	{

	}

	virtual Move<Routes, ADS>* apply(Routes& rep, ADS&)
	{
		int aux;
		int i;
		if (c < pos)
		{
			aux = rep.at(r).at(c);
			for (i = c; i < (pos - 1); i++)
			{
				//aux = rep.at(r).at(i);
				rep.at(r).at(i) = rep.at(r).at(i + 1);
				//rep.at(r).at(i + 1) = aux;
			}
			rep.at(r).at(i) = aux;
			return new MoveVRPOrOpt1<T, ADS> (r, pos - 1, c);
		}
		else
		{
			aux = rep.at(r).at(c);
			for (i = c; i > pos; i--)
			{
				rep.at(r).at(i) = rep.at(r).at(i - 1);
			}
			rep.at(r).at(i) = aux;
			return new MoveVRPOrOpt1<T, ADS> (r, pos, c + 1);

		}

		return new MoveVRPOrOpt1<T, ADS> (-1, -1, -1);
		///exit(1);
	}

	virtual bool operator==(const Move<Routes, ADS>& _m) const
	{
		const MoveVRPOrOpt1<T, ADS>& m1 = (const MoveVRPOrOpt1<T, ADS>&) _m;
		return (m1.r == r) && (m1.c == c) && (m1.pos == pos);
	}

	void print() const
	{
		cout << "MoveVRPOrOpt1";
		cout << "( " << r << " , " << c << " , " << pos << " )" << endl;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRPOrOpt1<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM>
class NSIteratorVRPOrOpt1: public NSIterator<vector<vector<T> > , ADS>
{
	typedef vector<vector<T> > Routes;

protected:

	MOVE* m;
	vector<MOVE*> moves;
	int index; //index of moves
	const Routes& rep;

	P* p; // has to be the last
public:

	NSIteratorVRPOrOpt1(const Routes& _r, const ADS& _ads, P* _p = nullptr) :
		rep(_r), p(_p)
	{
		m = nullptr;
		index = 0;
	}

	virtual ~NSIteratorVRPOrOpt1()
	{
	}

	virtual void first()
	{

		for (int r = 0; r < rep.size(); r++)
		{
			for (int c = 0; c < rep.at(r).size(); c++)
			{
				for (int pos = 0; pos <= rep.at(r).size(); pos++)
				{
					if ((c != pos) && (c + 1 != pos))
					{
						moves.push_back(new MOVE(r, c, pos, p));
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

	virtual Move<Routes, ADS>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "VRPOrOpt1. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRPOrOpt1<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPOrOpt1<T, ADS, MOVE, P> >
class NSSeqVRPOrOpt1: public NSSeq<vector<vector<T> > , ADS>
{
	typedef vector<vector<T> > Routes;

private:
	P* p; // has to be the last

public:

	NSSeqVRPOrOpt1(P* _p = nullptr) :
		p(_p)
	{
	}

	virtual ~NSSeqVRPOrOpt1()
	{
	}

	Move<Routes, ADS>& move(const Routes& rep, const ADS&)
	{
		int r = rand() % rep.size();

		if (rep.at(r).size() < 2)
			return *new MOVE(-1, -1, -1, p);

		int c = rand() % rep.at(r).size();

		int pos = rand() % (rep.at(r).size() + 1);

		return *new MOVE(r, c, pos, p);
	}

	virtual NSITERATOR& getIterator(const Routes& r, const ADS& ads)
	{
		return *new NSITERATOR(r, ads, p);
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqVRPOrOpt1 with move: " << MOVE::idComponent();
		return ss.str();
	}
};

#endif /*OPTFRAME_NSSEQVRP0ROPT1_HPP_*/
