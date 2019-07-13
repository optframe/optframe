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
Example: Word Counter with MapReduce Interface with MaPI
Author : Sabir Ribas
Date   : 2009-02-09
*/

#include "../MaPI/MaPI.h"
#include "../util/Scanner++/Scanner.h"
#include <iostream>

class MySerializer : public MaPI_Serializer<string,string,string,int,int>
{
public:

	virtual string	KeyA_fromString(string s)	{return s;};
	virtual string	KeyA_toString(string o)		{return o;};

	virtual string	A_fromString(string s)		{return s;};
	virtual string	A_toString(string o)		{return o;};

	virtual string	KeyB_fromString(string s)	{return s;};
	virtual string	KeyB_toString(string o)		{return o;};

	virtual int	B_fromString(string s)			{return atoi(s.c_str());};
	virtual string	B_toString(int o)			{stringstream s; s << o; return s.str();};

	virtual int	C_fromString(string s)			{return atoi(s.c_str());};
	virtual string	C_toString(int o)			{stringstream s; s << o; return s.str();};

};

class MyMapper : public MaPI_Mapper<string,string,string,int,int> {
public:
	MyMapper(MaPI_MapReduce<string,string,string,int,int> * mr, MaPI_Serializer<string,string,string,int,int> * s)
		:MaPI_Mapper<string,string,string,int,int>(mr,s){};
	virtual vector< pair<string,int> > map( pair<string,string> a ) // Map function implementation
	{
		vector< pair<string,int> > mapped;
		Scanner scan ( a.second ); scan.useSeparators(string(" \n\r\t\\\'\"!@#^&*()+={[}]|:;,<>./?"));
		while (scan.hasNext()) mapped.push_back ( pair<string,int> ( scan.next() , 1 ) );
		return mapped;
	};
};

class MyReducer : public MaPI_Reducer<string,string,string,int,int> {
public:
	MyReducer(MaPI_MapReduce<string,string,string,int,int> * mr, MaPI_Serializer<string,string,string,int,int> * s)
		:MaPI_Reducer<string,string,string,int,int>(mr,s){};
	virtual pair<string,int> reduce( pair<string, vector<int> > bs ) // Reduce function implementation
	{
		int reduced = 0;
		for( vector<int>::iterator it = bs.second.begin() ; it != bs.second.end() ; ++it) reduced += *it;
		return pair<string,int>(bs.first,reduced); 
	};
};

int main(int argc,char** argv)
{
	cout << "MaPI_WorldCounter test" << endl;

	// MapReduce declaration
	MaPI_MapReduce<string,string,string,int,int> mapReduce;
	MySerializer serializer;
	MyMapper mapper(&mapReduce,&serializer);
	MyReducer reducer(&mapReduce,&serializer);
	mapReduce.initServers(argc,argv);

	// Reading text
	Scanner scan(new File("doc"));

	string text;
	while (scan.hasNext()) text += scan.next() + " ";

	// MapReduce input
	vector< pair<string,string> > input;
	input.push_back(pair<string,string>("doc",text));

	// MapReduce application
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
