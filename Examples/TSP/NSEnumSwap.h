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

#ifndef TSP_NSENUMSwap_HPP_
#define TSP_NSENUMSwap_HPP_

// Framework includes
#include "../../OptFrame/Move.hpp"
#include "../../OptFrame/NSEnum.hpp"

// Own includes
#include "ProblemInstance.h"
#include "Memory.h"
#include "Solution.h"
#include "../../OptFrame/RandGen.hpp"

//#define MOV_Swap_DEBUG

using namespace std;

//============================================================================
//                           Swap MOVE
//============================================================================

namespace TSP
{

class MoveSwap: public Move<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>
{
private:
	int c1, c2;
	ProblemInstance& tsp;

public:

	MoveSwap(int c1, int c2, ProblemInstance& _tsp) :
		tsp(_tsp)
	{
		this->c1 = c1;
		this->c2 = c2;

		// Put the rest of your code here
	}

	bool canBeApplied(const RepTSP& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		// If there are some move "MoveSwap" that can't be applied, implement this method

		return true;
	}

	Move<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& apply(RepTSP& rep, OPTFRAME_DEFAULT_ADS&)
	{
		// Specify how the move "MoveSwap" will be applied

		int aux = rep.at(c1);
		rep[c1] = rep[c2];
		rep[c2] = aux;

		// return the reverse move
		return *new MoveSwap(c2, c1, tsp);
	}


	Move<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& apply(MemTSP& mem, RepTSP& rep, OPTFRAME_DEFAULT_ADS& ads)
	{
		int k1, k2;

		if (c1 < c2)
		{
			k1 = c1;
			k2 = c2;
		}
		else
		{
			k1 = c2;
			k2 = c1;
		}

		// before k2 and k1
		int bk1 = k1 - 1;
		int bk2 = k2 - 1;
		// after k2 and k1
		int ak1 = k1 + 1;
		int ak2 = k2 + 1;

		if (k1 == 0)
			bk1 = rep.size() - 1;
		if (k2 == ((int)rep.size()) - 1)
			ak2 = 0;

		double f = 0;

		if (k2 - k1 == 1) // special case, cities are near
		{
			f -= (*tsp.dist)(rep[bk1], rep[k1]);
			f -= (*tsp.dist)(rep[k1], rep[k2]);
			f -= (*tsp.dist)(rep[k2], rep[ak2]);
		}
		else
		{
			f -= (*tsp.dist)(rep[bk1], rep[k1]);
			f -= (*tsp.dist)(rep[k1], rep[ak1]);
			f -= (*tsp.dist)(rep[bk2], rep[k2]);
			f -= (*tsp.dist)(rep[k2], rep[ak2]);
		}

		Move<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& rev = apply(rep, ads);

		if (k2 - k1 == 1) // special case, cities are near
		{
			f += (*tsp.dist)(rep[bk1], rep[k1]);
			f += (*tsp.dist)(rep[k1], rep[k2]);
			f += (*tsp.dist)(rep[k2], rep[ak2]);
		}
		else
		{
			f += (*tsp.dist)(rep[bk1], rep[k1]);
			f += (*tsp.dist)(rep[k1], rep[ak1]);
			f += (*tsp.dist)(rep[bk2], rep[k2]);
			f += (*tsp.dist)(rep[k2], rep[ak2]);
		}

		mem.first = true;
		mem.second += f;

		return rev;
	}


	MoveCost* cost(const Evaluation<MemTSP>& e, const RepTSP& rep, const OPTFRAME_DEFAULT_ADS& ads)
	{
		int k1, k2;

		if (c1 < c2)
		{
			k1 = c1;
			k2 = c2;
		}
		else
		{
			k1 = c2;
			k2 = c1;
		}

		//cout << "Swap k1=" << k1 << "(" << rep[k1] << ")" << " k2=" << k2 << "(" << rep[k2] << ")" << endl;

		// before k2 and k1
		int bk1 = k1 - 1;
		int bk2 = k2 - 1;
		// after k2 and k1
		int ak1 = k1 + 1;
		int ak2 = k2 + 1;

		if (k1 == 0)
			bk1 = ((int)rep.size()) - 1;
		if (k2 == ((int)rep.size()) - 1)
			ak2 = 0;

		double f = 0;

		if(k2 - k1 == 1) // special case, cities are near
		{
			f -= (*tsp.dist)(rep[bk1], rep[k1]);
			f -= (*tsp.dist)(rep[k1], rep[k2]);
			f -= (*tsp.dist)(rep[k2], rep[ak2]);

			f += (*tsp.dist)(rep[bk1], rep[k2]);
			f += (*tsp.dist)(rep[k2], rep[k1]);
			f += (*tsp.dist)(rep[k1], rep[ak2]);
		}
		else if((k1 == 0) && (k2 == ((int)rep.size()) - 1)) // special case, extreme points
		{
			f -= (*tsp.dist)(rep[bk2], rep[k2]);
			f -= (*tsp.dist)(rep[k2], rep[k1]);
			f -= (*tsp.dist)(rep[k1], rep[ak1]);

			f += (*tsp.dist)(rep[bk2], rep[k1]);
			f += (*tsp.dist)(rep[k1], rep[k2]);
			f += (*tsp.dist)(rep[k2], rep[ak1]);
		}
		else
		{
			f -= (*tsp.dist)(rep[bk1], rep[k1]);
			f -= (*tsp.dist)(rep[k1], rep[ak1]);
			f -= (*tsp.dist)(rep[bk2], rep[k2]);
			f -= (*tsp.dist)(rep[k2], rep[ak2]);

			f += (*tsp.dist)(rep[bk1], rep[k2]);
			f += (*tsp.dist)(rep[k2], rep[ak1]);
			f += (*tsp.dist)(rep[bk2], rep[k1]);
			f += (*tsp.dist)(rep[k1], rep[ak2]);
		}

		return new MoveCost(f, 0);
	}


	void print() const
	{
		cout << "MoveSwap between " << c1 << " and " << c2 << endl;
	}

	virtual bool operator==(const Move<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& _m) const
	{
		const MoveSwap& m = (const MoveSwap&) _m; // You can only compare if types are equal

		if ((c1 == m.c1 && c2 == m.c2) || (c1 == m.c2 && c2 == m.c1))
			return true;
		else
			return false;
	}

};

//============================================================================
//                  Swap Neighborhood Structure
//============================================================================


class NSEnumSwap: public NSEnum<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>
{
private:
	ProblemInstance* pI;
	int n;

	// Your private vars

public:

	using NSEnum<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>::move; // prevents name hiding

	NSEnumSwap(ProblemInstance* pI, RandGen& _rg) :
		NSEnum<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP> (_rg)
	{
		this->pI = pI;
		this->n = pI->n;
	}

	virtual Move<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& move(unsigned int k)
	{
		return busca(k, 1, 2 * n);
	}

	unsigned int size() const
	{
		return n * (n - 1) / 2;
	}

	virtual void print() const
	{
		cout << "\nNSEnum Swap (" << size() << ")\n";
	}

	// Auxiliar methods


	int corresp(int d)
	{
		return d - ((d - (n - 1)) - 1) * 2;
	}

	int numElem(int d)
	{
		if (d <= n)
			return (d / 2);
		else
			return numElem(corresp(d));
	}

	int comeca(int d)
	{

		if (d <= n)
		{
			int z = (d / 2);

			int ant = z * (z - 1);

			// Se impar, soma mais 'z'
			if (d % 2 == 1)
				ant += z;

			return ant;
		}
		else
		{
			return 2 * (comeca(n)) - comeca(corresp(d) + 1) + numElem(n);
		}

	}

	int termina(int d)
	{
		return comeca(d) + numElem(d) - 1;
	}

	Move<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& busca(int k, int a, int b)
	{
		int d = (a + b) / 2;

		//cout << "busca "<<k<<" na diagonal "<<d<<"entre ["<<a<<","<<b<<"]"<<endl;

		int c = comeca(d);
		int t = termina(d);

		//cout <<"comeca em "<<c<<" e termina em "<<t<<endl;

		//int p;
		//cin >>p;

		if (k < c)
		{
			//cout << "k<c"<<endl;
			return busca(k, a, d);
		}

		if (k > t)
		{
			//cout << "k>t"<<endl;

			if (a == d)
				d++;

			return busca(k, d, b);
		}

		if (d <= n)
		{

			for (int i = 0; i < numElem(d); i++)
				if (k == c + i)
					return *new MoveSwap(i, d - i - 1, *pI);
		}
		else
		{

			for (int i = 0; i < numElem(d); i++)
				if (k == c + i)
				{
					int j = d - n;
					return *new MoveSwap(i + j, d - i - 1 - j, *pI);
				}

		}

		cout << "Error!" << endl;
		return *new MoveSwap(0, 0, *pI);
	}

};

} // end namespace TSP

#endif /*TSP_NSENUMSwap_HPP_*/

