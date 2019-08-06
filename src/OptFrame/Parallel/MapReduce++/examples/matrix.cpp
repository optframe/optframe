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
Example: 
Author : 
Date   : 
*/

#include "../MapMP/MapMP.h"
#include <iostream>

class MyMapper : public Seq_Mapper<int,int,int,int,int> {
public:
	MyMapper(MapReduce<int,int,int,int,int> * mr):MapMP_Mapper<int,int,int,int,int>(mr){};
	virtual multimap<int,int> map( pair<int,int> a ) 
	{ 
		multimap<int,int> m; 
		cout << "\tMapping..\n"; 
		//...
		cout << "\tOk\n"; 
		return m; 
	};
};

class MyReducer : public Seq_Reducer<int,int,int,int,int> {
public:
	MyReducer(MapReduce<int,int,int,int,int> * mr):MapMP_Reducer<int,int,int,int,int>(mr){};
	virtual pair<int,int> reduce( pair<int, vector<int> > bs ) 
	{
		int reduced;
		cout << "\tReducing..\n"; 
		//...
		cout << "\tOk\n"; 
		return pair<int,int>(bs.first,reduced); 
	};
};

int main()
{
	cout << "MapMP test" << endl;

	multimap<int,int> input;

	Seq_MapReduce<int,int,int,int,int> mapReduce;
	MyMapper mapper(&mapReduce);
	MyReducer reducer(&mapReduce);

	map<int,int> output = mapReduce.run(mapper,reducer,input);

	return 0;
}
