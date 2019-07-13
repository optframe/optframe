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

#ifndef OPTFRAME_NSSEQTSP2OPT_HPP_
#define OPTFRAME_NSSEQTSP2OPT_HPP_

#include "../../Move.hpp"
#include "../../NSSeq.hpp"

#include "Moves/MoveTSP2Opt.hpp"
#include "NSIterators/IteratorTSP2Opt.hpp"

using namespace std;

//! \english NSSeq class for the classic Traveling Salesman Problem (TSP) neighborhood of 2-Opt. \endenglish \portuguese Classe NSSeq para a estrutura de vizinhança clássica de 2-Opt do Problema do Caixeiro Viajante. \endportuguese

/*!
 \english
 Working structure: vector<T>.

 Classical Problem: Traveling Salesman Problem

 The Neighborhood Structure 2-opt has proposed by Cross (1958)

 Cross G., A method for solving traveling salesman problems. Operations Research 6 (1958), pp. 791–812

 2-Opt is applied for any problem that representation is like a vector<T>, where T is the type of the vector.

 e.g: vector<T> where type of T is int

 s means Solution

 initial s: 1 2 3 4 5 6 7 8

 s' after apply MoveTSP2Opt(2,5) in s

 s': 1 2 5 4 3 6 7 8

 s" after apply MoveTSP2Opt(3,8) in s'

 s": 1 2 5 8 7 6 3 4
 \endenglish

 \portuguese
 Estrutura alvo: vector<T>.

 Problema clássico: Problema do Caixeiro Viajante (em inglês TSP - Traveling Salesman Problem)

 A estrutura de vizinhança 2-opt foi proposta por Cross (1958)

 Artigo referência: Cross G., A method for solving traveling salesman problems. Operations Research 6 (1958), pp. 791–812

 2-Opt é aplicável em todo tipo de problema no qual a representação é um vector<T>, onde T é o tipo do vector.

 Exemplo: vector<T> onde o tipo de T é int

 s significa solução

 inicial s: 1 2 3 4 5 6 7 8

 s' após aplicação do MoveTSP2Opt(2,5) em s

 s': 1 2 5 4 3 6 7 8

 s" após aplicação do MoveTSP2Opt(3,8) em s'

 s": 1 2 5 8 7 6 3 4
 \endportuguese
 */

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveTSP2Opt<T, ADS>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorTSP2Opt<T, ADS, MOVE, P> >
class NSSeqTSP2Opt: public NSSeq<vector<T>, ADS>
{
	typedef vector<T> Route;

private:
	P* p; // has to be the last

public:

	NSSeqTSP2Opt(P* _p = nullptr)
	:
			p(_p)
	{
	}

	virtual ~NSSeqTSP2Opt()
	{
	}

	Move<Route, ADS>& move(const Route& rep, const ADS&)
	{
		if (rep.size() < 2)
			return *new MOVE(-1, -1, p);

		int p1 = rand() % (rep.size() + 1);
		int p2 = rand() % (rep.size() + 1);

		do
		{
			p1 = rand() % (rep.size() + 1);
			p2 = rand() % (rep.size() + 1);
		}
		while ((abs(p1 - p2) < 2) || (p1 > p2));

		// create 2-opt(p1,p2) move
		return *new MOVE(p1, p2, p);
	}

	virtual NSIterator<Route, ADS>& getIterator(const Route& r, const ADS&)
	{
		return *new NSITERATOR(r, p);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << NSSeq<vector<T>, ADS>::idComponent() << ":NSSeqTSP2Opt";
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
		ss << "NSSeqTSP2Opt with move: " << MOVE::idComponent();

		return ss.str();
	}
};

#endif /*OPTFRAME_NSSEQTSP2OPT_HPP_*/
