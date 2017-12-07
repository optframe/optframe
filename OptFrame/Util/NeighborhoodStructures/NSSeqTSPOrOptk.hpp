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

#ifndef OPTFRAME_NSSEQ_TSP_OROPTK_HPP_
#define OPTFRAME_NSSEQ_TSP_OROPTK_HPP_

// Framework includes
#include "../../NSSeq.hpp"

#include "Moves/MoveTSPOrOptk.hpp"
#include "NSIterators/IteratorTSPOrOptk.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveTSPOrOptk<T, ADS>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorTSPOrOptk<T, ADS, MOVE, P> >
class NSSeqTSPOrOptk: public NSSeq<vector<T>, ADS>
{
	typedef vector<T> Route;

private:
	int k;
	P* p; // has to be the last

public:

	NSSeqTSPOrOptk(int _k, P* _p = nullptr) :
			k(_k), p(_p)
	{
	}

	virtual ~NSSeqTSPOrOptk()
	{
	}

	using NSSeq<vector<T>, ADS>::move;
	using NSSeq<vector<T>, ADS>::getIterator;

	Move<Route, ADS>& move(const Route& rep, const ADS& ads)
	{
		int n = rep.size();

		if (n - k <= 0)
		{
			// THROW EXCEPTION!
			cerr << "CANNOT GENERATE MOVE OPTK FOR SOLUTION " << rep << endl;
			exit(1);
			//return *new MOVE(0, 0, k, p);
		}

		int i = rand() % (n - k + 1);

		int j = i;
		while (abs(i - j) < k)
			j = rand() % (n - k + 1);

		Move<Route, ADS>* m = new MOVE(i, j, k, p);
		if (!m->canBeApplied(rep, ads))
		{
			cout << "ERROR IN GENERATION!" << endl;
			m->print();
			exit(1);
		}
		return *m;
	}

	Move<Route, ADS>* validMove(const Route& r, const ADS& ads)
	{
		Move<Route, ADS>* m = &move(r, ads);
		if (m->canBeApplied(r, ads))
			return m;
		else
		{
			delete m;
			return nullptr;
		}
	}

	virtual NSIterator<Route, ADS>& getIterator(const Route& r, const ADS&)
	{
		return *new NSITERATOR(r.size(), k, p);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << NSSeq<vector<T>, ADS>::idComponent() << ":NSSeqTSPOrOptk";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (NSSeq<vector<T>, ADS>::compatible(s));
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqTSPOrOpt{K=" << k << "} with move: " << MOVE::idComponent();
		return ss.str();
	}
};

#endif /*OPTFRAME_NSSEQ_TSP_OROPTK_HPP_*/
