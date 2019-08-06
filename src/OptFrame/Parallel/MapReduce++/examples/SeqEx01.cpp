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

/* MapReduce Interface
Example: Basic example using a sequential Mapper and Reducer with MapReduce Interface
Author : Sabir Ribas
Date   : 2009-02-09
*/

#include "../SeqMR/SeqMR.h"
#include <iostream>

class MyMapper : public SeqMR_Mapper<int,char,int,char,string> {
public:
	MyMapper(MapReduce<int,char,int,char,string> * mr):SeqMR_Mapper<int,char,int,char,string>(mr){};
	virtual vector< pair<int,char> > map( pair<int,char> a ) { vector< pair<int,char> > m; m.push_back(a); return m; };
};

class MyReducer : public SeqMR_Reducer<int,char,int,char,string> {
public:
	MyReducer(MapReduce<int,char,int,char,string> * mr):SeqMR_Reducer<int,char,int,char,string>(mr){};
	virtual pair<int,string> reduce( pair<int, vector<char> > bs ) 
	{
		string reduced;
		for( vector<char>::iterator it = bs.second.begin() ; it != bs.second.end() ; ++it) reduced += *it;
		return pair<int,string>(bs.first,reduced); 
	};
};

int main()
{
	cout << "SeqMR test" << endl;

	vector< pair<int,char> > input;
	input.push_back(pair<int,char>(1,'a'));
	input.push_back(pair<int,char>(2,'b'));
	input.push_back(pair<int,char>(1,'c'));

	SeqMR_MapReduce<int,char,int,char,string> mapReduce;
	MyMapper mapper(&mapReduce);
	MyReducer reducer(&mapReduce);

	vector< pair<int,string> > output = mapReduce.run(mapper,reducer,input);
	cout << output << endl;

	return 0;
}
