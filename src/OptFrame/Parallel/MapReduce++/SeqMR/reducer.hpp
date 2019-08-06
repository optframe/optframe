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

#ifndef REDUCER_HPP
#define REDUCER_HPP

///SeqMR_Reducer.
template<class KeyA, class A, class KeyB, class B, class C>
class SeqMR_Reducer : public Reducer<KeyA,A,KeyB,B,C>
{ 
public:
	///Constructor.
	SeqMR_Reducer(MapReduce<KeyA,A,KeyB,B,C> * _mapReduce):mapReduce(_mapReduce){};
	///Iterator reducing execution (implemented by library).
#ifndef MRI_USE_MULTIMAP
	virtual vector< pair<KeyB,C> > run( vector< pair<KeyB,B> > & _mapped) {
		multimap<KeyB,B> mapped;
		for (int i = 0 ; i < _mapped.size() ; i++) mapped.insert(_mapped[i]);
#else
	virtual vector< pair<KeyB,C> > run( multimap<KeyB,B> & mapped) {
#endif
		vector< pair<KeyB,C> > reduced;

		typename std::multimap<KeyB,B>::iterator it = mapped.begin();
		KeyB lastKey = (*it).first;
		vector<B> toReduce;
		for( ; it != mapped.end() ; ++it)
		{
			if ((*it).first != lastKey) {
				pair<KeyB,C> r = reduce( pair<KeyB, vector<B> >( lastKey , toReduce ) ); toReduce.clear();
				reduced.push_back( r );
				lastKey = (*it).first;
			}
			toReduce.push_back( (*it).second );
		}

		pair<KeyB,C> r = reduce( pair<KeyB, vector<B> >( lastKey , toReduce ) ); toReduce.clear();
		reduced.push_back( r );

		return reduced;
	};
	///Reduce function (implemented by user).
	virtual pair<KeyB,C> reduce( pair<KeyB, vector<B> > ) = 0 ;
protected:
	MapReduce<KeyA,A,KeyB,B,C> * mapReduce;
};

///SeqMR_StrReducer.
class SeqMR_StrReducer : public SeqMR_Reducer<string,string,string,string,string> 
{
public:
	///Constructor.
	SeqMR_StrReducer(MapReduce<string,string,string,string,string> * _mapReduce)
		:SeqMR_Reducer<string,string,string,string,string>(_mapReduce){};	
};

#endif /* REDUCER_HPP */
