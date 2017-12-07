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

#ifndef OPTFRAME_NSENUMVECTORSHIFT_HPP_
#define OPTFRAME_NSENUMVECTORSHIFT_HPP_

// Framework includes

#include "../../NSEnum.hpp"

#include "./Moves/MoveVectorShift.hpp"

using namespace std;

//============================================================================
//                  Shift Neighborhood Structure
//============================================================================


template<class T >
class NSEnumVectorShift: public NSEnum< vector<T> >
{
private:
	int n;

public:	

	NSEnumVectorShift(int n)
	{
		this->n = n;
	}


	virtual Move<vector<T> >* kmove(int k)
	{
		if(k>size())
		{
			cerr << "Neighborhood Shift Error! Move " << k << " doesnt exist! Valid Interval from 0 to " << (size()-1) << "." << endl;
			exit(1);

			return nullptr;
		}

		return new MoveVectorShift<T >((k/n), (k%n) );
	}


	int size()
	{
		return n*n;
	}

	virtual void print()
	{
		cout << "NSEnum Vector Shift ("<< size() << ")" << endl;
	}

};

#endif /*OPTFRAME_NSENUMVECTORSHIFT_HPP_*/
