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

#ifndef MAPREDUCE_HPP
#define MAPREDUCE_HPP

#include <omp.h>

///MapMP_MapReduce.
template<class KeyA, class A, class KeyB, class B, class C>
class MapMP_MapReduce : public MapReduce<KeyA,A,KeyB,B,C>
{
public:
	MapMP_MapReduce():numThreads(1){};

	///MapReduce execution (implemented by library).
	virtual vector< pair<KeyB,C> > run( Mapper<KeyA,A,KeyB,B,C> & mapper, Reducer<KeyA,A,KeyB,B,C> & reducer, vector< pair<KeyA,A> > & as)
	{	
		//cout << "Input  :\t" << as << endl;
		omp_set_num_threads(getNumThreads());
#ifndef MRI_USE_MULTIMAP
		vector< pair<KeyB,B> > bs = mapper.run( as );
#else
		multimap<KeyB,B> bs = mapper.run( as );
#endif
		//cout << "Mapped :\t" << bs << endl;
		omp_set_num_threads(getNumThreads());
		vector< pair<KeyB,C> > cs = reducer.run( bs );
		//cout << "Reduced:\t" << cs << endl;
		return cs;
	};

	void setNumThreads(int n) { numThreads = n; };
	int  getNumThreads()  { return numThreads; };
private:
	int numThreads;
};

///MapMP_StrMapReduce.
class MapMP_StrMapReduce : public MapMP_MapReduce<string,string,string,string,string> {};

#endif /* MAPREDUCE_HPP */
