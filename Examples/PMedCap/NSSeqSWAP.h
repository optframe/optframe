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

#ifndef PCAP_NSSEQSWAP_HPP_
#define PCAP_NSSEQSWAP_HPP_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"

#include "../../OptFrame/Util/TestMove.hpp"

#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.h"
#include "Memory.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

class MoveSWAP: public Move<RepPCAP>
//class MoveSWAP: public TestMove<RepPCAP>
{
public:

	using Move<RepPCAP>::apply; // prevents name hiding
	using Move<RepPCAP>::canBeApplied; // prevents name hiding

	int x;
	int med;
	MoveSWAP(int _x, int _med)
	{
		x = _x;
		med = _med;

	}

	virtual ~MoveSWAP()
	{
	}

	bool canBeApplied(const RepPCAP& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return true;
	}

	Move<RepPCAP>& apply(RepPCAP& rep, OPTFRAME_DEFAULT_ADS&)
	{
		int aux = rep.second[x];
		rep.second[x] = med;
		return *new MoveSWAP(x, aux);
	}

	virtual bool operator==(const Move<RepPCAP>& _m) const
	{
		const MoveSWAP& m = (const MoveSWAP&) _m;
		return x == m.x;
	}

	void print() const
	{
		cout << "MoveSwapMediana: (" << x << "," << med << ")" << endl;
	}
};

class NSIteratorSWAP: public NSIterator<RepPCAP>
{
public:
	PCAPProblemInstance& p;
	int x;
	int mediana;
	NSIteratorSWAP(PCAPProblemInstance& _p) :
		p(_p)
	{
	}

	virtual ~NSIteratorSWAP()
	{
	}

	virtual void first()
	{
		x = 0;
		mediana = 0;
	}

	virtual void next()
	{
		mediana++;
		if (mediana == (p.nMedianas - 1))
		{
			mediana = 0;
			x++;
		}
	}

	virtual bool isDone()
	{
		return x == p.nCidades;
	}

	virtual Move<RepPCAP>& current()
	{
		return *new MoveSWAP(x, mediana);
	}

};

class NSSeqSWAP: public NSSeq<RepPCAP>
{
public:

	using NSSeq<RepPCAP>::move; // prevents name hiding
	PCAPProblemInstance& p;
	RandGen& rg;

	NSSeqSWAP(PCAPProblemInstance& _p, RandGen& _rg) :
		p(_p), rg(_rg)
	{
	}

	virtual ~NSSeqSWAP()
	{
	}

	virtual Move<RepPCAP>& move(const RepPCAP& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		int cidade = rg.rand(rep.second.size());
		int mediana = rg.rand(rep.first.size());
		return *new MoveSWAP(cidade, mediana); // return a random move
	}

	virtual NSIterator<RepPCAP>& getIterator(const RepPCAP& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return *new NSIteratorSWAP(p); // return an iterator to the neighbors of 'rep'
	}

	virtual void print() const
	{
	}
};

#endif /*PCAP_NSSEQSWAP_HPP_*/

