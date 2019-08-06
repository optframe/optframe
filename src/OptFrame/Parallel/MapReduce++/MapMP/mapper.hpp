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

#ifndef MAPPER_HPP
#define MAPPER_HPP
#include <iterator>

///MapMP_Mapper.
template<class KeyA, class A, class KeyB, class B, class C>
class MapMP_Mapper : public Mapper<KeyA,A,KeyB,B,C>
{ 
public:
	///Constructor.
	MapMP_Mapper(MapReduce<KeyA,A,KeyB,B,C> * _mapReduce):mapReduce(_mapReduce){};
	///Iterator mapping execution (implemented by library).
#ifndef MRI_USE_MULTIMAP
	virtual vector< pair<KeyB,B> > run( vector< pair<KeyA,A> > & as )
	{
		vector< pair<KeyB,B> > bs;

		#pragma omp parallel for
		for (int i = 0 ; i < as.size() ; i++) 
		{
			vector< pair<KeyB,B> > bss = map( as[i] );
			#pragma omp critical
			for (int j = 0 ; j < bss.size() ; j++) bs.push_back( bss[j] );
		}

		return bs;
	};
#else
	virtual multimap<KeyB,B> run( vector< pair<KeyA,A> > & as )
	{
		multimap<KeyB,B> bs;

		#pragma omp parallel for
		for (int i = 0 ; i < as.size() ; i++)
		{
			vector< pair<KeyB,B> > bss = map( as[i] );
			#pragma omp critical
			for (int j = 0 ; j < bss.size() ; j++) bs.insert( bss[j] );
		}

		return bs;
	};
#endif
	///Map function (implemented by user).
	virtual vector< pair<KeyB,B> > map( pair<KeyA,A> ) = 0 ;
protected:
	MapReduce<KeyA,A,KeyB,B,C> * mapReduce;
};

///MapMP_StrMapper.
class MapMP_StrMapper : public MapMP_Mapper<string,string,string,string,string> 
{
public:
	///Constructor.
	MapMP_StrMapper(MapReduce<string,string,string,string,string> * _mapReduce)
		:MapMP_Mapper<string,string,string,string,string>(_mapReduce) {};	
};

#endif /* MAPPER_HPP */
