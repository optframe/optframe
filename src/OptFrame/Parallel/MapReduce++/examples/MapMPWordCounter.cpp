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
Example: Word Counter with MapReduce Interface with MapMP
Author : Sabir Ribas
Date   : 2009-02-09
*/

#include "../MapMP/MapMP.h"
#include "../util/Scanner++/Scanner.h"
#include <iostream>
#include <time.h>

class MyMapper : public MapMP_Mapper<string,string,string,int,int> {
public:
	MyMapper(MapReduce<string,string,string,int,int> * mr):MapMP_Mapper<string,string,string,int,int>(mr){};
	virtual vector< pair<string,int> > map( pair<string,string> a ) // Map function implementation
	{
		vector< pair<string,int> > mapped;
		Scanner scan ( a.second ); scan.useSeparators(string(" \n\r\t\\\'\"!@#^&*()+={[}]|:;,<>./?"));
		while (scan.hasNext()) mapped.push_back ( pair<string,int> ( scan.next() , 1 ) );
		return mapped;
	};
};

class MyReducer : public MapMP_Reducer<string,string,string,int,int> {
public:
	MyReducer(MapReduce<string,string,string,int,int> * mr):MapMP_Reducer<string,string,string,int,int>(mr){};
	virtual pair<string,int> reduce( pair<string, vector<int> > bs ) // Reduce function implementation
	{
		int reduced = 0;
		for( vector<int>::iterator it = bs.second.begin() ; it != bs.second.end() ; ++it) reduced += *it;
		return pair<string,int>(bs.first,reduced); 
	};
};

int main()
{
	cout << "MapMP_WorldCounter test" << endl;

	// Reading text
	Scanner scan(new File("doc"));
	string text;
	while (scan.hasNext()) text += scan.next() + " ";

	// MapReduce input
	vector< pair<string,string> > input;
	input.push_back(pair<string,string>("doc",text));

	// MapReduce declaration and application
	MapMP_MapReduce<string,string,string,int,int> mapReduce;
	MyMapper mapper(&mapReduce);
	MyReducer reducer(&mapReduce);

	vector< pair<string,int> > output = mapReduce.run(mapper,reducer,input);

	// Output exibition
	cout << "MapReduce word counter result" << endl;
	multimap<int,string> sortedOutput;
	for (vector< pair<string,int> >::iterator it = output.begin() ; it != output.end() ; it++)
		sortedOutput.insert( pair<int,string>( (*it).second , (*it).first ) );
	for (multimap<int,string>::reverse_iterator it = sortedOutput.rbegin() ; it != sortedOutput.rend() ; it++) 
		cout << *it << endl;
	
	return 0;
}
