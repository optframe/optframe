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

#ifndef OPTFRAME_MOVEVECTORSHIFT_HPP_
#define OPTFRAME_MOVEVECTORSHIFT_HPP_

// Framework includes
#include "../../../Move.hpp"

using namespace std;
using namespace optframe;

//============================================================================
//                           Shift Move
//============================================================================

template<class T>
class MoveVectorShift : public Move<vector<T> >
{
private:
	int t1, t2;
	int n;

public:

	MoveVectorShift(int t1, int t2)
	{
		this->t1 = t1;
		this->t2 = t2;
	}

	bool canBeApplied(Solution<vector<T> >* s)
	{
		return t1 != t2;
	}

	Move<vector<T> >* apply(Solution<vector<T> >* s)
	{
		vector<T>* rep = s->getRepresentation();

		T aux;
		if(t1 < t2)
			for(int i = t1; i < t2; i++)
			{
				aux = rep->at(i);
				(*rep)[i] = (*rep)[i+1];
				(*rep)[i+1] = aux;
			}
		else
			for(int i = t1; i > t2; i--)
			{
				aux = rep->at(i);
				(*rep)[i] = (*rep)[i-1];
				(*rep)[i-1] = aux;
			}

		MoveVectorShift<T> * ms = new MoveVectorShift<T>(t2,t1);

		return ms;
	}

	void print() const
	{
		cout << "Move Vector Shift("<< t1 << " -> " << t2 <<")"<<endl;
	}
};

#endif /*OPTFRAME_MOVEVECTORSHIFT_HPP_*/
