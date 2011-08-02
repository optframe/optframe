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

#ifndef OPTFRAME_NSSEQTSPSWAP_HPP_
#define OPTFRAME_NSSEQTSPSWAP_HPP_

#include "../../Move.hpp"
#include "../../NSSeq.hpp"

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

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveTSPSwap<T, M> >
class NSSeqTSPSwap: public NSSeq<vector<T> , M>
{
	typedef vector<T> Route;

private:

public:

	NSSeqTSPSwap()
	{
	}

	virtual ~NSSeqTSPSwap()
	{
	}

	Move<Route, M>& move(const Route& rep)
	{
		if (rep.size() < 2)
			return *new MOVE(-1, -1);

		int p1 = rand() % rep.size();

		int p2 = p1;

		while (p2 == p1)
			p2 = rand() % rep.size();

		return *new MOVE(p1, p2);
	}

	virtual NSIterator<Route, M>& getIterator(const Route& r)
	{
		return *new NSIteratorTSPSwap<T, M, MOVE> (r.size());
	}

	virtual void print()
	{
		cout << "NSSeqTSPSwap" << endl;
	}
};

#endif /*OPTFRAME_NSSEQTSPSWAP_HPP_*/
