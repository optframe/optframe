/* MapReduce Interface
Example: Basic example using a sequential string-based Mapper and Reducer with MapReduce Interface
Author : Sabir Ribas
Date   : 2009-02-09
*/

#include "../SeqMR/SeqMR.h"
#include <iostream>

class MyMapper : public SeqMR_StrMapper {
public:
	MyMapper(SeqMR_StrMapReduce * mr):SeqMR_StrMapper(mr){};
	virtual multimap<string,string> map( pair<string,string> a ) { multimap<string,string> mapped; mapped.insert(a); return mapped; };
};

class MyReducer : public SeqMR_StrReducer {
public:
	MyReducer(SeqMR_StrMapReduce * mr):SeqMR_StrReducer(mr){};
	virtual pair<string,string> reduce( pair<string, vector<string> > bs ) 
	{ 
		string reduced;
		for( vector<string>::iterator it = bs.second.begin() ; it != bs.second.end() ; ++it) reduced += *it;
		return pair<string,string>(bs.first,reduced); 
	};
};

int main()
{
	cout << "StrSeqMR test" << endl;

	multimap<string,string> input;
	input.insert(pair<string,string>("1","Map"));
	input.insert(pair<string,string>("1","Reduce"));
	input.insert(pair<string,string>("2","Interface"));

	SeqMR_StrMapReduce mapReduce;
	MyMapper mapper(&mapReduce);
	MyReducer reducer(&mapReduce);

	map<string,string> output = mapReduce.run(mapper,reducer,input);
	
	return 0;
}
