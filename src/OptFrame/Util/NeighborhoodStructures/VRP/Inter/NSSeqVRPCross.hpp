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

#ifndef OPTFRAME_NSSEQPERIODICCROSS_HPP_
#define OPTFRAME_NSSEQPERIODICCROSS_HPP_

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MoveVRPCross: public Move<vector<vector<T> >, ADS, DS >
{

	typedef vector<vector<T> > Routes;

protected:
	int r1, r2; // route 1 and route 2;
	int p1, p2; // position 1 and position 2 of cross in r1 end r2, respectively

	OPTFRAME_DEFAULT_PROBLEM* problem;

public:

	MoveVRPCross(int _r1, int _r2, int _p1, int _p2, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr) :
		r1(_r1), r2(_r2), p1(_p1), p2(_p2), problem(_problem)
	{
	}

	virtual ~MoveVRPCross()
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

	int get_p1()
	{
		return p1;
	}

	int get_p2()
	{
		return p2;
	}

	virtual bool canBeApplied(const Routes& rep, const ADS&)
	{
		bool all_positive = (r1 >= 0) && (r2 >= 0) && (p1 >= 0) && (p2 >= 0);
		return all_positive && (rep.size() >= 2) && (rep.at(r1).size() >= 0) && (rep.at(r2).size() >= 0);
	}

	virtual void updateNeighStatus(ADS& ads);

	virtual Move<Routes, ADS, DS>& apply(Routes& rep, ADS&)
	{
		vector<int> cross_r1, cross_r2;

		//copiando as partes que serão trocadas
		cross_r1.assign(rep.at(r1).begin() + p1, rep.at(r1).end());
		cross_r2.assign(rep.at(r2).begin() + p2, rep.at(r2).end());

		//removendo as partes copiadas
		rep.at(r1).erase(rep.at(r1).begin() + p1, rep.at(r1).end());
		rep.at(r2).erase(rep.at(r2).begin() + p2, rep.at(r2).end());

		//inserindo as novas partes

		rep.at(r1).insert(rep.at(r1).end(), cross_r2.begin(), cross_r2.end());
		rep.at(r2).insert(rep.at(r2).end(), cross_r1.begin(), cross_r1.end());

		// p->p, r1->r2, r2->r1, c1->c2, c2->c1, reverse->reverse,
		return *new MoveVRPCross(r1, r2, p1, p2);
	}

	virtual bool operator==(const Move<Routes, ADS, DS>& _m) const
	{
		const MoveVRPCross& m1 = (const MoveVRPCross&) _m;
		return ((m1.r1 == r1) && (m1.r2 == r2) && (m1.p1 == p1) && (m1.p2 == p2));
	}

	void print() const
	{
		cout << "MoveVRPCross( ";
		cout << r1 << ", " << r2 << ", " << p1 << ", " << p2 << " )";
		cout << endl;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS, class MOVE = MoveVRPCross<T, ADS, DS>, class P = OPTFRAME_DEFAULT_PROBLEM>
class NSIteratorVRPCross: public NSIterator<vector<vector<T> > >
{

	typedef vector<vector<T> > Routes;

private:

	MoveVRPCross<T, ADS, DS >* m;
	vector<MoveVRPCross<T, ADS, DS >*> moves;
	int index; //index of moves
	const Routes& r;

	P* p; // has to be the last

public:

	NSIteratorVRPCross(const Routes& _r, P* _p = nullptr) :
		r(_r), p(_p)
	{
		m = nullptr;
		index = 0;
	}

	virtual ~NSIteratorVRPCross()
	{
		/*for (int i = 0; i < moves.size(); i++)
		 delete moves[i];*/
	}

	virtual void first()
	{

		for (int r1 = 0; r1 < r.size() - 1; r1++)
		{
			for (int r2 = r1 + 1; r2 < r.size(); r2++)
			{
				for (int p1 = 0; p1 <= r.at(r1).size(); p1++)
				{
					for (int p2 = 0; p2 <= r.at(r2).size(); p2++)
					{
						moves.push_back(new MOVE(r1, r2, p1, p2, p));
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

	virtual Move<Routes, ADS, DS>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqVRPCross. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS, class MOVE = MoveVRPCross<T, ADS, DS>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPCross<T, ADS, DS, MOVE, P > >
class NSSeqVRPCross: public NSSeq<vector<vector<T> > >
{

	typedef vector<vector<T> > Routes;

private:
	P* p; // has to be the last

public:

	NSSeqVRPCross(P* _p = nullptr) :
		p(_p)
	{
	}

	virtual ~NSSeqVRPCross()
	{
	}

	Move<Routes, ADS, DS>& move(const Routes& rep, const ADS&)
	{
		if (rep.size() < 2)
			return *new MOVE(-1, -1, -1, -1, p);

		int r1 = rand() % rep.size();

		int r2;

		do
		{
			r2 = rand() % rep.size();
		} while (r1 == r2);

		if ((rep.at(r2).size() == 0) && (rep.at(r1).size() == 0))
			return *new MOVE(-1, -1, -1, -1, p);

		int p1 = rand() % (rep.at(r1).size() + 1);
		int p2 = rand() % (rep.at(r2).size() + 1);

		return *new MOVE(r1, r2, p1, p2, p);
	}

	virtual NSIteratorVRPCross<T, ADS, DS, MOVE, P >& getIterator(const Routes& r, const ADS&)
	{
		return *new NSITERATOR (r, p);
	}

	virtual void print()
	{
		cout << "NSSeqVRPCross with move: " << MOVE::idComponent();
	}
};

#endif /*OPTFRAME_NSSEQPERIODICCROSS_HPP_*/
