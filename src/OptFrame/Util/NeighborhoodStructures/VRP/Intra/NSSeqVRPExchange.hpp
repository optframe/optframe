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

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
/*
template<class T, class ADS, XBaseSolution<vector<vector<T> >,ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class MoveVRPExchange: public Move<XES, XEv>
{

	typedef vector<vector<T> > Routes;
*/
template<XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM>
class MoveVRPExchange: public Move<XES, typename XES::second_type>
{
   using XEv = typename XES::second_type;
	typedef vector<vector<int> > Routes;
public:
   Routes& (*getRoutes)(const XES&); // function to get routes from type 'R'
private:
	P* problem;

protected:
	int c1, c2; // client 1 and client 2, respectively
	int r; //route


public:

	MoveVRPExchange(Routes& (*_getRoutes)(const XES&), int _r, int _c1, int _c2, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr) :
		getRoutes(_getRoutes), r(_r), c1(_c1), c2(_c2), problem(_problem)
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

	virtual bool canBeApplied(const XES& se) override
	{
      const Routes& rep = se.first.getR();
		bool all_positive = (c1 >= 0) && (c2 >= 0) && (r >= 0);
		return all_positive && (rep.at(r).size() >= 2);
	}

/*
	virtual void updateNeighStatus(ADS& ads)
	{
	}
*/

	virtual uptr<Move<XES>> apply(XES& se) override
	{
      Routes& rep = se.first.getR();
		int aux = rep.at(r).at(c1);
		rep.at(r).at(c1) = rep.at(r).at(c2);
		rep.at(r).at(c2) = aux;

		return uptr<Move<XES>>(new MoveVRPExchange(getRoutes, r, c1, c2));
	}

	virtual bool operator==(const Move<XES>& _m) const
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

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRPExchange<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM>
/*
template<class T, class ADS, XBaseSolution<vector<vector<T>>,ADS> S, class MOVE = MoveVRPExchange<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class NSIteratorVRPExchange: public NSIterator<XES, XEv>
{
	typedef vector<vector<T> > Routes;
*/
template<XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM, class MOVE = MoveVRPExchange<XES, P>>
class NSIteratorVRPExchange: public NSIterator<XES, typename XES::second_type>
{
	typedef vector<vector<int> > Routes;


protected:

	uptr<Move<XES>> m;
	int index;
	vector<uptr<Move<XES>>> moves;
	const Routes& rep;

	P* p; // has to be the last
public:

   Routes& (*getRoutes)(const XES&); // function to get routes from type 'R'

	NSIteratorVRPExchange(Routes& (*getRoutes)(const XES&), const XES& se, P* _p = nullptr) :
		getRoutes(getRoutes), rep{getRoutes(se)}, p(_p)
	{
		index = 0;
		m = nullptr;
	}

	virtual ~NSIteratorVRPExchange()
	{
	}

	virtual void first() override
	{
		for (int r = 0; r < rep.size(); r++)
		{
			for (int c1 = 0; c1 < rep.at(r).size(); c1++)
			{
				for (int c2 = 0; c2 < rep.at(r).size(); c2++)
				{
					if (c1 != c2)
						moves.push_back(uptr<Move<XES>>(new MOVE(getRoutes, r, c1, c2, p)));
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
			cout << "NSSeqVRPExchange. Aborting." << endl;
			exit(1);
		}

      uptr<Move<XES>> m2 = std::move(m);
      m = nullptr;

		return m2;
	}
};

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRPExchange<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPExchange<T, ADS, MOVE, P> >
/*
template<class T, class ADS, XBaseSolution<vector<vector<T>>,ADS> S, class MOVE = MoveVRPExchange<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPExchange<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, XSearch<XES> XSH = std::pair<S, XEv>>
class NSSeqVRPExchange: public NSSeq<XES, XEv, XSH>
{
	typedef vector<vector<T> > Routes;

private:
	P* p; // has to be the last
*/
template<XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM, class MOVE = MoveVRPExchange<XES, P>, class NSITERATOR = NSIteratorVRPExchange<XES, P, MOVE>>
class NSSeqVRPExchange: public NSSeq<XES, typename XES::second_type, XES>
{
	typedef vector<vector<int> > Routes;

public:
   Routes& (*getRoutes)(const XES&); // function to get routes from type 'R'

private:
	P* p; // has to be the last (?)

public:

	NSSeqVRPExchange(Routes& (*_getRoutes)(const XES&), P* _p = nullptr) :
		getRoutes(_getRoutes), p(_p)
	{
	}

	virtual ~NSSeqVRPExchange()
	{
	}

	uptr<Move<XES>> randomMove(const XES& se) override
	{
      const Routes& rep = se.first.getR();
		int r = rand() % rep.size();
		if (rep.at(r).size() < 2)
			return uptr<Move<XES>>(new MOVE(getRoutes, -1, -1, -1, p));

		int c1 = rand() % rep.at(r).size();

		int c2;

		do
		{
			c2 = rand() % rep.at(r).size();
		} while (c1 == c2);

		// create exchange(p1,p2) move
		return uptr<Move<XES>>(new MOVE(getRoutes, r, c1, c2, p));
	}

	virtual uptr<NSIterator<XES>> getIterator(const XES& se) override
	{
      XSolution& s = se.first;
		return uptr<NSIterator<XES>>(new NSITERATOR(getRoutes, se, p));
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqVRPExchange with move: " << MOVE::idComponent();
		return ss.str();
	}
};

#endif /*OPTFRAME_NSSEQVRPEXCHANGE_HPP_*/
