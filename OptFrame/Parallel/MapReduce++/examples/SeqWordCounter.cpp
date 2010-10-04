/* MapReduce Interface
Example: Sequential Word Counter with MapReduce Interface
Author : Sabir Ribas
Date   : 2009-02-09
*/

#include "../SeqMR/SeqMR.h"
#include "../util/Scanner++/Scanner.h"
#include <iostream>

class MyMapper : public SeqMR_Mapper<string,string,string,int,int> {
public:
	MyMapper(MapReduce<string,string,string,int,int> * mr):SeqMR_Mapper<string,string,string,int,int>(mr){};
	virtual vector< pair<string,int> > map( pair<string,string> a ) // Map function implementation
	{
		vector< pair<string,int> > mapped;
		Scanner scan ( a.second ); scan.useSeparators(string(" \n\r\t\\\'\"!@#^&*()+={[}]|:;,<>./?"));
		while (scan.hasNext()) mapped.push_back ( pair<string,int> ( scan.next() , 1 ) );
		return mapped;
	};
};

class MyReducer : public SeqMR_Reducer<string,string,string,int,int> {
public:
	MyReducer(MapReduce<string,string,string,int,int> * mr):SeqMR_Reducer<string,string,string,int,int>(mr){};
	virtual pair<string,int> reduce( pair<string, vector<int> > bs ) // Reduce function implementation
	{
		int reduced = 0;
		for( vector<int>::iterator it = bs.second.begin() ; it != bs.second.end() ; ++it) reduced += *it;
		return pair<string,int>(bs.first,reduced); 
	};
};

int main()
{
	cout << "SeqMR_WorldCounter test" << endl;

	vector< pair<string,string> > input;
	input.push_back(pair<string,string>("doc","content : this is the content of the text ."));

	SeqMR_MapReduce<string,string,string,int,int> mapReduce;
	MyMapper mapper(&mapReduce);
	MyReducer reducer(&mapReduce);

	vector< pair<string,int> > output = mapReduce.run(mapper,reducer,input);
	cout << output << endl;

	return 0;
}
