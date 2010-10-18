/* MapReduce Interface
Example: Basic example using a sequential string-based Mapper and Reducer with MapReduce Interface
Author : Sabir Ribas
Date   : 2009-02-09
*/

#include "../MapMP/MapMP.h"
#include <iostream>

class MyMapper : public MapMP_StrMapper {
public:
	MyMapper(MapMP_StrMapReduce * mr):MapMP_StrMapper(mr){};
	virtual multimap<string,string> map( pair<string,string> a ) 
	{ 
		multimap<string,string> mapped; 
		mapped.insert(a); 
		cout << "\tMapping..\n"; 
		sleep(1); 
		cout << "\tOk\n"; 
		return mapped; 
	};
};

class MyReducer : public MapMP_StrReducer {
public:
	MyReducer(MapMP_StrMapReduce * mr):MapMP_StrReducer(mr){};
	virtual pair<string,string> reduce( pair<string, vector<string> > bs ) 
	{ 
		string reduced;
		for( vector<string>::iterator it = bs.second.begin() ; it != bs.second.end() ; ++it) reduced += *it;
		cout << "\tReducing..\n"; 
		sleep(1); 
		cout << "\tOk\n"; 
		return pair<string,string>(bs.first,reduced); 
	};
};

int main()
{
	cout << "StrMapMP test" << endl;

	multimap<string,string> input;
	input.insert(pair<string,string>("1","Map"));
	input.insert(pair<string,string>("1","Reduce"));
	input.insert(pair<string,string>("2","Interface"));

	MapMP_StrMapReduce mapReduce;
	MyMapper mapper(&mapReduce);
	MyReducer reducer(&mapReduce);

	map<string,string> output = mapReduce.run(mapper,reducer,input);
	
	return 0;
}
