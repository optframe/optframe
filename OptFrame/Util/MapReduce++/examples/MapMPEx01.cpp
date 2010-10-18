/* MapReduce Interface
Example: Basic example using a sequential Mapper and Reducer with MapReduce Interface
Author : Sabir Ribas
Date   : 2009-02-09
*/

#include "../MapMP/MapMP.h"
#include <iostream>

class MyMapper : public MapMP_Mapper<int,char,int,char,string> {
public:
	MyMapper(MapReduce<int,char,int,char,string> * mr):MapMP_Mapper<int,char,int,char,string>(mr){};
	virtual multimap<int,char> map( pair<int,char> a ) 
	{ 
		multimap<int,char> m; 
		m.insert(a); 
		cout << "\tMapping..\n"; 
		sleep(1); 
		cout << "\tOk\n"; 
		return m; 
	};
};

class MyReducer : public MapMP_Reducer<int,char,int,char,string> {
public:
	MyReducer(MapReduce<int,char,int,char,string> * mr):MapMP_Reducer<int,char,int,char,string>(mr){};
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

int main()
{
	cout << "MapMP test" << endl;

	multimap<int,char> input;
	input.insert(pair<int,char>(1,'a'));
	input.insert(pair<int,char>(2,'b'));
	input.insert(pair<int,char>(1,'c'));

	MapMP_MapReduce<int,char,int,char,string> mapReduce;
	MyMapper mapper(&mapReduce);
	MyReducer reducer(&mapReduce);

	map<int,string> output = mapReduce.run(mapper,reducer,input);

	return 0;
}
