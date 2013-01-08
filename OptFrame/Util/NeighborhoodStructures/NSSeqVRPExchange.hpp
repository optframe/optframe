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


#ifndef OPTFRAME_NSSEQVRPEXCHANGE_HPP_
#define OPTFRAME_NSSEQVRPEXCHANGE_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSSeq.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class MoveVRPExchange: public Move<vector<vector<T> > >
{

	typedef vector<vector<T> > Routes;

protected:
	int c1, c2; // client 1 and client 2, respectively
	int r; //route

public:

	MoveVRPExchange(int _r, int _c1, int _c2) :
		r(_r), c1(_c1), c2(_c2)
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

	bool canBeApplied(const Routes& rep)
	{
		bool all_positive = (c1 >= 0) && (c2 >= 0) && (r >= 0);
		return all_positive && (rep.at(r).size() >= 2);
	}

	MoveVRPExchange<T, ADS, M>& apply(Routes& rep)
	{

		int aux = rep.at(r).at(c1);
		rep.at(r).at(c1) = rep.at(r).at(c2);
		rep.at(r).at(c2) = aux;

		return *new MoveVRPExchange<T, ADS, M>(r, c1, c2);
	}

	virtual bool operator==(const Move<Routes>& _m) const
	{
		const MoveVRPExchange<T, ADS, M>& m1 = (const MoveVRPExchange<T, ADS, M>&) _m;
		return ((m1.c1 == c1) && (m1.c2 == c2) && (m1.r == r));
	}

	void print() const
	{
		cout << "MoveVRPExchange( route: " << r << " : ";
		cout << " client " << c1 << " <=>  client" << c2 << " )";
		cout << endl;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class NSIteratorVRPExchange: public NSIterator<vector<vector<T> > >
{

	typedef vector<vector<T> > Routes;

private:

	MoveVRPExchange<T, ADS, M>* m;
	int index;
	vector<MoveVRPExchange<T, ADS, M>*> moves;
	const Routes& rep;

public:

	NSIteratorVRPExchange(const Routes& _r) :
		rep(_r)
	{
		index = 0;
		m = NULL;
	}

	virtual ~NSIteratorVRPExchange()
	{
	}

	void first()
	{
		for (int r = 0; r < rep.size(); r++)
		{
			for (int c1 = 0; c1 < rep.at(r).size(); c1++)
			{
				for (int c2 = 0; c2 < rep.at(r).size(); c2++)
				{
					if (c1 != c2)
						moves.push_back(new MoveVRPExchange<T, ADS, M>( r, c1, c2));
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

	MoveVRPExchange<T, ADS, M>& current()
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

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class NSSeqVRPExchange: public NSSeq<vector<vector<T> > >
{

	typedef vector<vector<T> > Routes;

private:

public:

	NSSeqVRPExchange()
	{
	}

	virtual ~NSSeqVRPExchange()
	{
	}

	MoveVRPExchange<T, ADS, M>& move(const Routes& rep)
	{
		int r = rand() % rep.size();
		if (rep.at(r).size() < 2)
			return *new MoveVRPExchange<T, ADS, M>(-1, -1, -1);

		int c1 = rand() % rep.at(r).size();

		int c2;

		do
		{
			c2 = rand() % rep.at(r).size();
		} while (c1 == c2);

		// create exchange(p1,p2) move
		return *new MoveVRPExchange<T, ADS, M>(r, c1, c2);
	}

	virtual NSIteratorVRPExchange<T, ADS, M>& getIterator(const Routes& r)
	{
		return *new NSIteratorVRPExchange<T, ADS, M>(r);
	}

	virtual void print()
	{
		cout << "NSSeqVRPExchange" << endl;
	}
};

#endif /*OPTFRAME_NSSEQVRPEXCHANGE_HPP_*/
