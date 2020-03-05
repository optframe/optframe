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

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
template<class T, class ADS, XBaseSolution<vector<vector<T> >,ADS> S, XEvaluation XEv = Evaluation<>>
class MoveVRPOrOpt1: public Move<S, XEv>
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

	virtual bool canBeApplied(const S& s) override
	{
      const Routes& rep = s.getR();
		bool all_positive = (r >= 0) && (c >= 0) && (pos >= 0);
		return all_positive && (c != pos) && (c + 1 != pos);
	}

	virtual void updateNeighStatus(ADS& ads)
	{

	}

	virtual uptr<Move<S>> apply(S& s) override
	{
      Routes& rep = s.getR();
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
			return uptr<Move<S>>(new MoveVRPOrOpt1<T, ADS, S> (r, pos - 1, c));
		}
		else
		{
			aux = rep.at(r).at(c);
			for (i = c; i > pos; i--)
			{
				rep.at(r).at(i) = rep.at(r).at(i - 1);
			}
			rep.at(r).at(i) = aux;
			return uptr<Move<S>>(new MoveVRPOrOpt1<T, ADS, S> (r, pos, c + 1));

		}

		return uptr<Move<S>>(new MoveVRPOrOpt1<T, ADS, S> (-1, -1, -1));
		///exit(1);
	}

	virtual bool operator==(const Move<S>& _m) const
	{
		const MoveVRPOrOpt1<T, ADS, S>& m1 = (const MoveVRPOrOpt1<T, ADS, S>&) _m;
		return (m1.r == r) && (m1.c == c) && (m1.pos == pos);
	}

	void print() const
	{
		cout << "MoveVRPOrOpt1";
		cout << "( " << r << " , " << c << " , " << pos << " )" << endl;
	}
};

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRPOrOpt1<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM>
template<class T, class ADS, XBaseSolution<vector<vector<T>>,ADS> S, class MOVE = MoveVRPOrOpt1<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>>
class NSIteratorVRPOrOpt1: public NSIterator<S, XEv>
{
	typedef vector<vector<T> > Routes;

protected:

	uptr<Move<S>> m;
	vector<uptr<Move<S>>> moves;
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

	virtual void first() override
	{

		for (int r = 0; r < rep.size(); r++)
		{
			for (int c = 0; c < rep.at(r).size(); c++)
			{
				for (int pos = 0; pos <= rep.at(r).size(); pos++)
				{
					if ((c != pos) && (c + 1 != pos))
					{
						moves.push_back(uptr<Move<S>>(new MOVE(r, c, pos, p)));
					}
				}
			}
		}

		if (moves.size() > 0)
		{
			m = std::move(moves[index]); // stealing from vector... verify if this is correct! otherwise, must need clone() on Move
		}
		else
			m = nullptr;
	}

	virtual void next() override
	{
		index++;
		if (index < moves.size())
		{
			m = std::move(moves[index]); // stealing from vector... verify if this is correct! otherwise, must need clone() on Move
		}
		else
			m = nullptr;
	}

	virtual bool isDone() override
	{
		return m == nullptr;
	}

	virtual uptr<Move<S>> current() override
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "VRPOrOpt1. Aborting." << endl;
			exit(1);
		}

      uptr<Move<S>> m2 = std::move(m);
      m = nullptr;

		return m2;
	}
};

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRPOrOpt1<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPOrOpt1<T, ADS, MOVE, P> >
template<class T, class ADS, XBaseSolution<vector<vector<T>>,ADS> S, class MOVE = MoveVRPOrOpt1<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPOrOpt1<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, XSearch<XES> XSH = std::pair<S, XEv>>
class NSSeqVRPOrOpt1: public NSSeq<S, XEv, XSH>
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

	uptr<Move<S>> randomMove(const S& s) override
	{
      const Routes& rep = s.getR();
		int r = rand() % rep.size();

		if (rep.at(r).size() < 2)
			return uptr<Move<S>>(new MOVE(-1, -1, -1, p));

		int c = rand() % rep.at(r).size();

		int pos = rand() % (rep.at(r).size() + 1);

		return uptr<Move<S>>(new MOVE(r, c, pos, p));
	}

	virtual uptr<NSIterator<S>> getIterator(const S& s) override
	{
		return uptr<NSIterator<S>>(new NSITERATOR(s.getR(), s.getADS(), p));
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqVRPOrOpt1 with move: " << MOVE::idComponent();
		return ss.str();
	}
};

#endif /*OPTFRAME_NSSEQVRP0ROPT1_HPP_*/
