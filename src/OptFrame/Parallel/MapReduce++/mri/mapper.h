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

#ifndef MAPPER_H
#define MAPPER_H

///Mapper.
template<class KeyA, class A, class KeyB, class B, class C>
class Mapper 
{ 
public: 
	///Iterator mapping execution (implemented by library).
#ifndef MRI_USE_MULTIMAP
	virtual vector< pair<KeyB,B> > run( vector< pair<KeyA,A> > & ) = 0 ;
#else
	virtual multimap<KeyB,B> run( vector< pair<KeyA,A> > & ) = 0 ;
#endif
	///Map function (implemented by user).
	virtual vector< pair<KeyB,B> > map( pair<KeyA,A> ) = 0 ;
private:
	MapReduce<KeyA,A,KeyB,B,C> * mapReduce;
};

///StrMapper.
class StrMapper : public Mapper<string,string,string,string,string> {};

#endif /* MAPPER_H */
