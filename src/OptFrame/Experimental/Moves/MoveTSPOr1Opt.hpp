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

#ifndef OPTFRAME_MOVETSPOR1OPT_HPP_
#define OPTFRAME_MOVETSPOR1OPT_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

template<class T, class DS >
class MoveTSPOr1Opt: public Move<vector<T>, DS >
{

private:
	int c; // client
	int pos; // position

	typedef vector<T> Route;

public:

	MoveTSPOr1Opt(int _c, int _pos) :
		c(_c), pos(_pos)
	{
	}

	virtual ~MoveTSPOr1Opt()
	{
	}


	int getClient()
	{
		return c;
	}

	int getPosition()
	{
		return pos;
	}

	bool canBeApplied(const Route& rep)
	{
		bool all_positive = (c >= 0) && (pos >= 0);
		return all_positive && (c != pos) && (c + 1 != pos);
	}

	MoveTSPOr1Opt& apply(Route& rep)
	{
		T aux;
		if (c < pos)
		{
			for (int i = c; i < (pos - 1); i++)
			{
				aux = rep.at(i);
				rep.at(i) = rep.at(i + 1);
				rep.at(i + 1) = aux;
			}
			return *new MoveTSPOr1Opt(pos - 1, c);
		}
		else
		{
			for (int i = c; i > pos; i--)
			{
				aux = rep.at(i);
				rep.at(i) = rep.at(i - 1);
				rep.at(i - 1) = aux;
			}
			return *new MoveTSPOr1Opt(pos, c + 1);

		}

		return *new MoveTSPOr1Opt(-1, -1);
	}

	bool operator==(const Move<vector<T>, DS >& _m) const
	{
		const MoveTSPOr1Opt& m1 = (const MoveTSPOr1Opt&) _m;
		return (m1.c == c) && (m1.pos == pos);
	}

	void print() const
	{
		cout << "MoveTSPOr1Opt";
		cout << "( "<< c << " , " << pos << " )" << endl;
	}
};


#endif /*OPTFRAME_MOVETSPOR1OPT_HPP_*/
