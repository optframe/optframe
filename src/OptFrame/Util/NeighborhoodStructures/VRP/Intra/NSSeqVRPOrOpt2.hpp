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

#ifndef OPTFRAME_NSSEQVRP0ROPT2_HPP_
#define OPTFRAME_NSSEQVRP0ROPT2_HPP_

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

/*
template<class T, class ADS, XBaseSolution<vector<vector<T> >,ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class MoveVRPOrOpt2: public Move<XES, XEv>
{

	typedef vector<vector<T> > Routes;
*/
template<XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM>
class MoveVRPOrOpt2: public Move<XES, typename XES::second_type>
{
   using XEv = typename XES::second_type;
	typedef vector<vector<int> > Routes;
public:
   Routes& (*getRoutes)(const XES&); // function to get routes from type 'R'

private:
	P* problem;

protected:
	int r; // route id
	int c; // client
	int pos; // position

public:

	MoveVRPOrOpt2(Routes& (*_getRoutes)(const XES&), int _r, int _c, int _pos, P* _problem = nullptr) :
		getRoutes(_getRoutes), r(_r), c(_c), pos(_pos), problem(_problem)
	{
	}

	virtual ~MoveVRPOrOpt2()
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

	virtual bool canBeApplied(const XES& se) override
	{
      const Routes& rep = se.first.getR();
		bool all_positive = (r >= 0) && (c >= 0) && (pos >= 0);
		return all_positive && (c != pos) && (c + 1 != pos) && (c + 2 != pos);
	}

/*
	virtual void updateNeighStatus(ADS& ads)
	{
	}
*/

	virtual uptr<Move<XES>> apply(XES& se) override
	{
      Routes& rep = se.first.getR();
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
			return uptr<Move<XES>>(new MoveVRPOrOpt2<XES, P> (getRoutes, r, pos - 2, c));
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
			return uptr<Move<XES>>(new MoveVRPOrOpt2<XES, P> (getRoutes, r, pos, c + 2));

		}

		return uptr<Move<XES>>(new MoveVRPOrOpt2<XES, P> (getRoutes, -1, -1, -1));
	}

	bool operator==(const Move<XES>& _m) const
	{
		const MoveVRPOrOpt2<XES, P>& m1 = (const MoveVRPOrOpt2<XES, P>&) _m;
		return (m1.r == r) && (m1.c == c) && (m1.pos == pos);
	}

	void print() const
	{
		cout << "MoveVRPOrOpt2";
		cout << "( " << r << " , " << c << " , " << pos << " )" << endl;
	}
};

/*
template<class T, class ADS, XBaseSolution<vector<vector<T>>,ADS> S, class MOVE = MoveVRPOrOpt2<XES, P>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class NSIteratorVRPOrOpt2: public NSIterator<XES, XEv>
{
	typedef vector<vector<T> > Routes;
*/

template<XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM, class MOVE = MoveVRPOrOpt2<XES, P>>
class NSIteratorVRPOrOpt2: public NSIterator<XES, typename XES::second_type>
{
	typedef vector<vector<int> > Routes;

protected:

	uptr<Move<XES>> m;
	vector<uptr<Move<XES>>> moves;
	int index; //index of moves
	const Routes& rep;

	P* p; // has to be the last
public:

   Routes& (*getRoutes)(const XES&); // function to get routes from type 'R'

	NSIteratorVRPOrOpt2(Routes& (*getRoutes)(const XES&), const XES& se, P* _p = nullptr) :
		getRoutes(getRoutes), rep{getRoutes(se)}, p(_p)
	{
		m = nullptr;
		index = 0;
	}

	virtual ~NSIteratorVRPOrOpt2()
	{
	}

	virtual void first() override
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
							moves.push_back(uptr<Move<XES>>(new MOVE(getRoutes, r, c, pos, p)));
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

	virtual uptr<Move<XES>> current() override
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "VRPOrOpt1. Aborting." << endl;
			exit(1);
		}

      uptr<Move<XES>> m2 = std::move(m);

		return m2;
	}
};

/*
template<class T, class ADS, XBaseSolution<vector<vector<T>>,ADS> S, class MOVE = MoveVRPOrOpt2<XES, P>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPOrOpt2<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, XSearch<XES> XSH = std::pair<S, XEv>>
class NSSeqVRPOrOpt2: public NSSeq<XES, XEv, XSH>
{
	typedef vector<vector<T> > Routes;

private:
	P* p; // has to be the last
*/
template<XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM, class MOVE = MoveVRPOrOpt2<XES, P>, class NSITERATOR = NSIteratorVRPOrOpt2<XES, P, MOVE>>
class NSSeqVRPOrOpt2: public NSSeq<XES, typename XES::second_type, XES>
{
	typedef vector<vector<int> > Routes;

public:
   Routes& (*getRoutes)(const XES&); // function to get routes from type 'R'

private:
	P* p; // has to be the last (?)

public:

	NSSeqVRPOrOpt2(Routes& (*_getRoutes)(const XES&), P* _p = nullptr) :
		getRoutes(_getRoutes),  p(_p)
	{
	}

	virtual ~NSSeqVRPOrOpt2()
	{
	}

	uptr<Move<XES>> randomMove(const XES& se) override
	{
      const Routes& rep = se.first.getR();
		int r = rand() % rep.size();

		if (rep.at(r).size() < 3)
			return uptr<Move<XES>>(new MOVE(getRoutes, -1, -1, -1, p));

		int c = rand() % (rep.at(r).size() - 1);

		int pos;
		do
		{
			pos = rand() % (rep.at(r).size() + 1);
		} while ((c == pos) || (c + 1 == pos) || (c + 2 == pos));

		return uptr<Move<XES>>(new MOVE(getRoutes, r, c, pos, p));
	}

	virtual uptr<NSIterator<XES>> getIterator(const XES& se) override
	{
      XSolution& s = se.first;
		return uptr<NSIterator<XES>>(new NSITERATOR(getRoutes, se, p));
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqVRPOrOpt2 with move: " << MOVE::idComponent();
		return ss.str();
	}
};

#endif /*OPTFRAME_NSSEQVRP0ROPT2_HPP_*/
