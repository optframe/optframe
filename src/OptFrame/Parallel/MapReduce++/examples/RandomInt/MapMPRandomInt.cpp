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
Project: 
Author : 
Date   : 
*/

#include "../../MapMP/MapMP.h"
#include "../../util/Timer.hpp"
#include <iostream>
#include <cstdlib>

class MyMapper : public MapMP_Mapper<int,int,int,int,int> {
public:
	MyMapper(MapReduce<int,int,int,int,int> * mr):MapMP_Mapper<int,int,int,int,int>(mr){};
	virtual vector< pair<int,int> > map( pair<int,int> a ) 
	{ 
		vector< pair<int,int> > m; 
		//cout << "\tMapping..\n"; 
		m.push_back(make_pair(a.first,a.second));
		for (int i = 0 ; i < 100000000 ; i++);
		//cout << "\tOk\n"; 
		return m; 
	};
};

class MyReducer : public MapMP_Reducer<int,int,int,int,int> {
public:
	MyReducer(MapReduce<int,int,int,int,int> * mr):MapMP_Reducer<int,int,int,int,int>(mr){};
	virtual pair<int,int> reduce( pair<int, vector<int> > bs ) 
	{
		int reduced = 0;
		//cout << "\tReducing..\n"; 
		for (int i = 0 ; i < bs.second.size() ; i++) reduced += bs.second[i];
		for (int i = 0 ; i < 100000000 ; i++);
		//cout << "\tOk\n"; 
		return pair<int,int>(bs.first,reduced); 
	};
};

int main()
{
	cout << "MapMP test" << endl;
	Timer timer;

	int n = 10;
	srand(n);

	vector< pair<int,int> > input;
	for (int i = 0 ; i < n ; i++) input.push_back(make_pair(rand()%n,rand()%n));

	MapMP_MapReduce<int,int,int,int,int> mapReduce;
	mapReduce.setNumThreads(2);
	MyMapper mapper(&mapReduce);
	MyReducer reducer(&mapReduce);

	vector< pair<int,int> > output = mapReduce.run(mapper,reducer,input);

	return 0;
}
