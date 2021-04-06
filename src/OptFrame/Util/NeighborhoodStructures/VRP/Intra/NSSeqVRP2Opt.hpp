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
//template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
//
//template<class T, class ADS, XBaseSolution<vector<vector<T> >,ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
//
//template<class R, class ADS, XBaseSolution<R,ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
template<XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM>
class MoveVRP2Opt: public Move<XES, typename XES::second_type>
{
   using XEv = typename XES::second_type;
	typedef vector<vector<int> > Routes;
public:
   Routes& (*getRoutes)(const XES&); // function to get routes from type 'R'

protected:
	int p1, p2; // position 1 and position 2, respectively
	int r; //route id

	P* problem;

public:

/*
	MoveVRP2Opt(int _r, int _p1, int _p2, P* _problem = nullptr) :
		r(_r), p1(_p1), p2(_p2), problem(_problem)
	{
	}
*/

	MoveVRP2Opt(Routes& (*_getRoutes)(const XES&), int _r, int _p1, int _p2, P* _problem = nullptr) :
		getRoutes(_getRoutes), r(_r), p1(_p1), p2(_p2), problem(_problem)
	{
	}

/*
      template<class ADS, XBaseSolution<Routes,ADS> S,
            typename = typename std::enable_if<
              std::is_same<S, CopySolution<Routes,ADS>>::value>::type>
	MoveVRP2Opt(int _r, int _p1, int _p2, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr) :
		getRoutes([](XES& se){return se.getR();}), r(_r), p1(_p1), p2(_p2), problem(_problem)
	{
	}
*/

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

	virtual bool canBeApplied(const XES& se) override
	{
      const Routes& rep = getRoutes(se); //se.first.getR();
		bool all_positive = (p1 >= 0) && (p2 >= 0) && (r >= 0);
		return all_positive && (rep.at(r).size() >= 2);
	}

	//virtual void updateNeighStatus(ADS& ads)
	//{
	//}

	virtual uptr<Move<XES>> apply(XES& se) override
	{
      Routes& rep = getRoutes(se); //se.first.getR();
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

		return uptr<Move<XES>>(new MoveVRP2Opt(getRoutes, r, p1, p2));
	}

	virtual bool operator==(const Move<XES>& _m) const
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

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRP2Opt<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM>
//
//template<class T, class ADS, XBaseSolution<vector<vector<T>>,ADS> S, class MOVE = MoveVRP2Opt<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
//
//template<class T, class ADS, XBaseSolution<vector<vector<T>>,ADS> S, class MOVE = MoveVRP2Opt<S>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
//
template<XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM, class MOVE = MoveVRP2Opt<XES, P>>
class NSIteratorVRP2Opt: public NSIterator<XES, typename XES::second_type>
{
	typedef vector<vector<int> > Routes;

protected:

   uptr<Move<XES>> m; // storing general move (do we need specific here?)
	int index;
   vector<uptr<Move<XES>>> moves; // storing general moves (do we need specific here?)
   //
   Routes& (*getRoutes)(const XES&); // function to get routes from type 'R'
   //
	const Routes& rep;

	P* p; // has to be the last

public:

	//NSIteratorVRP2Opt(const Routes& _r, const ADS& _ads, P* _p = nullptr) :
   NSIteratorVRP2Opt(Routes& (*getRoutes)(const XES&), const XES& se, P* _p = nullptr) :
      getRoutes(getRoutes), rep{getRoutes(se)}, p(_p)
		//rep(_r), p(_p)
	{
		index = 0;
		m = nullptr;
	}

	virtual ~NSIteratorVRP2Opt()
	{
	}

	virtual void first() override
	{
		for (int r = 0; r < rep.size(); r++)
		{
			int tam = rep.at(r).size() - 2;
			for (int p1 = 0; p1 < tam; p1++)
			{
				for (int p2 = p1 + 2; p2 < rep.at(r).size(); p2++)
				{
					moves.push_back(uptr<Move<XES>>(new MOVE(getRoutes, r, p1, p2, p)));
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
			cout << "NSSeqVRP2Opt. Aborting." << endl;
			exit(1);
		}

      uptr<Move<XES>> m2 = std::move(m);
      m = nullptr;

		return m2;
	}
};

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRP2Opt<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRP2Opt<T, ADS, MOVE, P> >
//
//template<class T, class ADS, XBaseSolution<vector<vector<T>>,ADS> S, class MOVE = MoveVRP2Opt<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRP2Opt<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, XSearch<XES> XSH = std::pair<S, XEv>>
//
//template<class T, class ADS, XBaseSolution<vector<vector<T>>,ADS> S, class MOVE = MoveVRP2Opt<S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRP2Opt<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, XSearch<XES> XSH = std::pair<S, XEv>>
template<XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM, class MOVE = MoveVRP2Opt<XES, P>, class NSITERATOR = NSIteratorVRP2Opt<XES, P, MOVE>>
class NSSeqVRP2Opt: public NSSeq<XES, typename XES::second_type, XES>
{
	typedef vector<vector<int> > Routes;

public:
   Routes& (*getRoutes)(const XES&); // function to get routes from type 'R'

private:
	P* p; // has to be the last (?)

public:

	NSSeqVRP2Opt(Routes& (*_getRoutes)(const XES&), P* _p = nullptr) :
		getRoutes(_getRoutes), p(_p)
	{
	}

	virtual ~NSSeqVRP2Opt()
	{
	}

	uptr<Move<XES>> randomMove(const XES& se) override
	{
      const Routes& rep = getRoutes(se); //se.first.getR();
		int r = rand() % rep.size();
		if (rep.at(r).size() < 3)
			return uptr<Move<XES>>(new MOVE(getRoutes, -1, -1, -1, p));

		int p1 = rand() % (rep.at(r).size() + 1);

		int p2;

		do
		{
			p2 = rand() % (rep.at(r).size() + 1);
		} while (abs(p1 - p2) < 2);

		// create 2-opt(p1,p2) move
		return uptr<Move<XES>>(new MOVE(getRoutes, r, p1, p2, p));
	}

	//virtual uptr<NSITERATOR> getIterator(const S& s) override
   virtual uptr<NSIterator<XES>> getIterator(const XES& se) override
	{
      XSolution& s = se.first;
		//return uptr<NSIterator<XES>>(new NSITERATOR(s.getR(), s.getADS(), p));
      return uptr<NSIterator<XES>>(new NSITERATOR(getRoutes, se, p));
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqVRP2Opt with move: " << MOVE::idComponent();
		return ss.str();
	}
};

#endif /*OPTFRAME_NSSEQVRP2OPT_HPP_*/
