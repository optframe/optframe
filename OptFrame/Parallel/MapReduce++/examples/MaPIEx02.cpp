/* MapReduce Interface
Example: Basic example using a sequential string-based Mapper and Reducer with MapReduce Interface
Author : Sabir Ribas
Date   : 2009-02-09
*/

#include "../MaPI/MaPI.h"
#include <iostream>

#include "../MaPI/MaPI.h"
#include <iostream>

class MyMapper : public MaPI_StrMapper {
public:
	MyMapper(MaPI_StrMapReduce * mr, MaPI_StrSerializer * s):MaPI_StrMapper(mr,s){};
	virtual vector< pair<string,string> > map( pair<string,string> a ) 
	{ 
		vector< pair<string,string> > mapped; 
		mapped.push_back(a); 
		cout << "\tMapping..\n"; 
		sleep(1); 
		cout << "\tOk\n"; 
		return mapped; 
	};
};

class MyReducer : public MaPI_StrReducer {
public:
	MyReducer(MaPI_StrMapReduce * mr, MaPI_StrSerializer * s):MaPI_StrReducer(mr,s){};
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

int main(int argc,char** argv)
{
	cout << "StrMaPI test" << endl;

	MaPI_StrMapReduce mapReduce;
	MaPI_StrSerializer serializer;
	MyMapper mapper(&mapReduce,&serializer);
	MyReducer reducer(&mapReduce,&serializer);
	mapReduce.initServers(argc,argv);

	vector< pair<string,string> > input;
	input.push_back(pair<string,string>("1","Map"));
	input.push_back(pair<string,string>("1","Reduce"));
	input.push_back(pair<string,string>("2","Interface"));

	vector< pair<string,string> > output = mapReduce.run(mapper,reducer,input);
	cout << output << endl;

	return 0;
}
