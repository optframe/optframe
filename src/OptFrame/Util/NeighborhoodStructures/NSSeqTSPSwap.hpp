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

#ifndef OPTFRAME_NSSEQTSPSWAP_HPP_
#define OPTFRAME_NSSEQTSPSWAP_HPP_

#include "../../Move.hpp"
#include "../../NSSeq.hpp"
#include "../../Solution.hpp"

#include "BaseSolutionTSP.hpp"

#include "Moves/MoveTSPSwap.hpp"
#include "NSIterators/IteratorTSPSwap.hpp"

using namespace std;

//! \english NSSeq class for the classic Traveling Salesman Problem (TSP) neighborhood of Swap. \endenglish \portuguese Classe NSSeq para a estrutura de vizinhança clássica de troca (Swap) do Problema do Caixeiro Viajante. \endportuguese

/*!
 \english
 Working structure: vector<T>.

 Classical Problem: Traveling Salesman Problem

 The Classic Swap Neighborhood Structure.

 Article:

 Swap is applied for any problem that representation is like a vector<T>, where T is the type of the vector.

 e.g: vector<T> where type of T is int

 s means Solution

 initial s: 1 2 3 4 5 6 7 8

 s' after apply MoveTSPSwap(2,5) in s

 s': 1 2 6 4 5 3 7 8
 \endenglish

 \portuguese
 Estrutura alvo: vector<T>.

 Problema clássico: Problema do Caixeiro Viajante (em inglês TSP - Traveling Salesman Problem)

 Estrutura de vizinhança clássica de troca.

 Artigo referência:

 Swap é aplicável em todo tipo de problema no qual a representação é um vector<T>, onde T é o tipo do vector.

 Exemplo: vector<T> onde o tipo de T é int

 s significa solução

 inicial s: 1 2 3 4 5 6 7 8

 s' após aplicação do MoveTSPSwap(2,5) em s

 s': 1 2 6 4 5 3 7 8
 \endportuguese
 */

namespace optframe
{

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, class MOVE = MoveTSPSwap<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorTSPSwap<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, XSearch<XES> XSH = std::pair<S, XEv>>
class NSSeqTSPSwap: public NSSeq<S, XEv, XSH>
{
	typedef vector<T> Route;

private:
	P* p; // has to be the last

public:

	NSSeqTSPSwap(P* _p = nullptr) :
			p(_p)
	{
	}

	virtual ~NSSeqTSPSwap()
	{
	}

	//using NSSeq<S, XEv, XSH>::move;
	//using NSSeq<S, XEv, XSH>::getIterator;

	uptr<Move<S, XEv>> randomMove(const S& s) override
	{
      const Route& rep = s.getR();
		if (rep.size() < 2)
			return uptr<Move<S, XEv>>(new MOVE(-1, -1, p));

		int p1 = rand() % rep.size();

		int p2 = p1;

		while (p2 == p1)
			p2 = rand() % rep.size();

		return uptr<Move<S, XEv>>(new MOVE(p1, p2, p));
	}

	virtual uptr<NSIterator<S, XEv>> getIterator(const S& s) override
	{
      const Route& r = s.getR();
		return uptr<NSIterator<S, XEv>>(new NSITERATOR(r.size(), p));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << NSSeq<S, XEv, XSH>::idComponent() << ":NSSeqTSPSwap";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (NSSeq<S, XEv, XSH>::compatible(s));
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqTSPSwap";
		return ss.str();
	}
};

} // namespace optframe

#endif /*OPTFRAME_NSSEQTSPSWAP_HPP_*/
