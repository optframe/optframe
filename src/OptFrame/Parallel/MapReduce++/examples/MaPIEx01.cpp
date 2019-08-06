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

#include "../MaPI/MaPI.h"
#include <iostream>

class MySerializer : public MaPI_Serializer<int,char,int,char,string>
{
public:

	virtual int		KeyA_fromString(string s)	{return atoi(s.c_str());};
	virtual string	KeyA_toString(int o)		{stringstream s; s << o; return s.str();};

	virtual char	A_fromString(string s)		{return s[0];};
	virtual string	A_toString(char o)			{stringstream s; s << o; return s.str();};

	virtual int		KeyB_fromString(string s)	{return atoi(s.c_str());};
	virtual string	KeyB_toString(int o)		{stringstream s; s << o; return s.str();};

	virtual char	B_fromString(string s)		{return s[0];};
	virtual string	B_toString(char o)			{stringstream s; s << o; return s.str();};

	virtual string	C_fromString(string s)		{return s;};
	virtual string	C_toString(string o)		{return o;};

};

class MyMapper : public MaPI_Mapper<int,char,int,char,string> {
public:
	MyMapper(MaPI_MapReduce<int,char,int,char,string> * mr, MaPI_Serializer<int,char,int,char,string> * s)
		:MaPI_Mapper<int,char,int,char,string>(mr,s){};
	virtual vector< pair<int,char> > map( pair<int,char> a ) 
	{ 
		vector< pair<int,char> > m; 
		m.push_back(a); 
		cout << "\tMapping..\n"; 
		sleep(1); 
		cout << "\tOk\n"; 
		return m; 
	};
};

class MyReducer : public MaPI_Reducer<int,char,int,char,string> {
public:
	MyReducer(MaPI_MapReduce<int,char,int,char,string> * mr, MaPI_Serializer<int,char,int,char,string> * s)
		:MaPI_Reducer<int,char,int,char,string>(mr,s){};
	virtual pair<int,string> reduce( pair<int, vector<char> > bs ) 
	{
		string reduced;
		for( vector<char>::iterator it = bs.second.begin() ; it != bs.second.end() ; ++it) reduced += *it;
		cout << "\tReducing..\n"; 
		sleep(1); 
		cout << "\tOk\n"; 
		return pair<int,string>(bs.first,reduced); 
	};
};

int main(int argc,char** argv)
{
	cout << "MaPI test" << endl;

	MaPI_MapReduce<int,char,int,char,string> mapReduce;
	MySerializer serializer;
	MyMapper mapper(&mapReduce,&serializer);
	MyReducer reducer(&mapReduce,&serializer);
	mapReduce.initServers(argc,argv);

	vector< pair<int,char> > input;
	input.push_back(pair<int,char>(1,'a'));
	input.push_back(pair<int,char>(2,'b'));
	input.push_back(pair<int,char>(1,'c'));

	vector< pair<int,string> > output = mapReduce.run(mapper,reducer,input);
	cout << output << endl;

	return 0;
}
