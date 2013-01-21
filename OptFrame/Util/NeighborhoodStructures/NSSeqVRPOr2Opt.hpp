// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OPTFRAME_NSSEQVRP0R2OPT_HPP_
#define OPTFRAME_NSSEQVRP0R2OPT_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSSeq.hpp"

using namespace std;

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MoveVRP2OrOpt: public Move<vector<vector<T> > >
{

	typedef vector<vector<T> > Routes;

private:
	int r; // route id
	int c; // client
	int pos; // position

public:

	MoveVRP2OrOpt(int _r, int _c, int _pos) :
		r(_r), c(_c), pos(_pos)
	{
	}

	virtual ~MoveVRP2OrOpt()
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

	bool canBeApplied(const Routes& rep)
	{
		bool all_positive = (r >= 0) && (c >= 0) && (pos >= 0);
		return all_positive && (c != pos) && (c + 1 != pos) && (c + 2 != pos);
	}

	MoveVRP2OrOpt<T, ADS, DS >& apply(Routes& rep)
	{
		int aux;
		if (c < pos)
		{
			for (int i = c + 1; i < (pos - 1); i++)
			{
				aux = rep.at(r).at(i);
				rep.at(r).at(i) = rep.at(r).at(i + 1);
				rep.at(r).at(i + 1) = aux;
			}
			for (int i = c; i < (pos - 2); i++)
			{
				aux = rep.at(r).at(i);
				rep.at(r).at(i) = rep.at(r).at(i + 1);
				rep.at(r).at(i + 1) = aux;
			}
			return *new MoveVRP2OrOpt<T, ADS, DS >(r, pos - 2, c);
		}
		else
		{
			for (int i = c; i > pos; i--)
			{
				aux = rep.at(r).at(i);
				rep.at(r).at(i) = rep.at(r).at(i - 1);
				rep.at(r).at(i - 1) = aux;
			}
			for (int i = c + 1; i > pos + 1; i--)
			{
				aux = rep.at(r).at(i);
				rep.at(r).at(i) = rep.at(r).at(i - 1);
				rep.at(r).at(i - 1) = aux;
			}
			return *new MoveVRP2OrOpt<T, ADS, DS >(r, pos, c + 2);

		}

		return *new MoveVRP2OrOpt<T, ADS, DS >(-1, -1, -1);
	}

	bool operator==(const Move<Routes>& _m) const
	{
		const MoveVRP2OrOpt<T, ADS, DS >& m1 = (const MoveVRP2OrOpt<T, ADS, DS >&) _m;
		return (m1.r == r) && (m1.c == c) && (m1.pos == pos);
	}

	void print() const
	{
		cout << "MoveVRP2OrOpt";
		cout << "( " << r << " , " << c << " , " << pos << " )" << endl;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NSIteratorVRP2OrOpt: public NSIterator<vector<vector<T> > >
{

	typedef vector<vector<T> > Routes;

private:

	MoveVRP2OrOpt<T, ADS, DS >* m;
	vector<MoveVRP2OrOpt<T, ADS, DS >*> moves;
	int index; //index of moves
	const Routes& rep;

public:

	NSIteratorVRP2OrOpt(const Routes& _r) :
	rep(_r)
	{
		m = NULL;
		index = 0;
	}

	virtual ~NSIteratorVRP2OrOpt()
	{
	}

	void first()
	{

		for (int r = 0; r < rep.size(); r++)
		{
			if (rep.at(r).size() > 1)
				for (int c = 0; c < rep.at(r).size() - 1; c++)
				{
					for (int pos = 0; pos <= rep.at(r).size(); pos++)
					{
						if ((c != pos) && (c + 1 != pos) && (c + 2 != pos))
						{
							moves.push_back(new MoveVRP2OrOpt<T, ADS, DS >(r, c, pos));
						}
					}
				}
		}

		if (moves.size() > 0)
		{
			m = moves[index];
		}
		else
			m = NULL;
	}

	void next()
	{
		index++;
		if (index < moves.size())
		{
			m = moves[index];
		}
		else
			m = NULL;
	}

	bool isDone()
	{
		return m == NULL;
	}

	MoveVRP2OrOpt<T, ADS, DS >& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "VRP1OrOpt. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};


template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NSSeqVRPOr2Opt: public NSSeq<vector<vector<T> > >
{

	typedef vector<vector<T> > Routes;

public:

	NSSeqVRPOr2Opt()
	{
	}

	virtual ~NSSeqVRPOr2Opt()
	{
	}

	MoveVRP2OrOpt<T, ADS, DS >& move(const Routes& rep)
	{
		int r = rand() % rep.size();

		if (rep.at(r).size() < 3)
			return *new MoveVRP2OrOpt<T, ADS, DS >(-1, -1, -1);

		int c = rand() % (rep.at(r).size() - 1);

		int pos;
		do
		{
			pos = rand() % (rep.at(r).size() + 1);
		} while ((c == pos) || (c + 1 == pos) || (c + 2 == pos));

		return *new MoveVRP2OrOpt<T, ADS, DS >(r, c, pos);
	}

	virtual NSIteratorVRP2OrOpt<T, ADS, DS >& getIterator(const Routes& r)
	{
		return *new NSIteratorVRP2OrOpt<T, ADS, DS >(r);
	}

	virtual void print()
	{
		cout << "NSSeqVRPOr2Opt" << endl;
	}
};

#endif /*OPTFRAME_NSSEQVRP0R2OPT_HPP_*/
