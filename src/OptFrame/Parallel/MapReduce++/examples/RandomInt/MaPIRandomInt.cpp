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

#include "../../MaPI/MaPI.h"
#include "../../util/Timer.hpp"
#include <iostream>
#include <cstdlib>

class MySerializer : public MaPI_Serializer<int,int,int,int,int>
{
public:

	virtual int		KeyA_fromString(string s)	{return atoi(s.c_str());};
	virtual string	KeyA_toString(int o)		{stringstream s; s << o; return s.str();};

	virtual int		A_fromString(string s)	{return atoi(s.c_str());};
	virtual string	A_toString(int o)		{stringstream s; s << o; return s.str();};

	virtual int		KeyB_fromString(string s)	{return atoi(s.c_str());};
	virtual string	KeyB_toString(int o)		{stringstream s; s << o; return s.str();};

	virtual int		B_fromString(string s)	{return atoi(s.c_str());};
	virtual string	B_toString(int o)		{stringstream s; s << o; return s.str();};

	virtual int		C_fromString(string s)	{return atoi(s.c_str());};
	virtual string	C_toString(int o)		{stringstream s; s << o; return s.str();};

};

class MyMapper : public MaPI_Mapper<int,int,int,int,int> {
public:
	MyMapper(MaPI_MapReduce<int,int,int,int,int> * mr, MaPI_Serializer<int,int,int,int,int> * s):MaPI_Mapper<int,int,int,int,int>(mr,s){};
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

class MyReducer : public MaPI_Reducer<int,int,int,int,int> {
public:
	MyReducer(MaPI_MapReduce<int,int,int,int,int> * mr, MaPI_Serializer<int,int,int,int,int> * s):MaPI_Reducer<int,int,int,int,int>(mr,s){};
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

int main(int argc, char** argv)
{
	cout << "MaPI test" << endl;

	Timer timer;

	MaPI_MapReduce<int,int,int,int,int> mapReduce;
	MySerializer serializer;	
	MyMapper mapper(&mapReduce,&serializer);
	MyReducer reducer(&mapReduce,&serializer);
	mapReduce.initServers(argc,argv);

	int n = 10;
	srand(n);

	vector< pair<int,int> > input;
	for (int i = 0 ; i < n ; i++) input.push_back(make_pair(rand()%n,rand()%n));

	vector< pair<int,int> > output = mapReduce.run(mapper,reducer,input);

	return 0;
}
