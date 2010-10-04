/* MapReduce Interface
Example: Basic example using a sequential Mapper and Reducer with MapReduce Interface
Author : Sabir Ribas
Date   : 2009-02-09
*/

#include "../SeqMR/SeqMR.h"
#include <iostream>

class MyMapper : public SeqMR_Mapper<int,char,int,char,string> {
public:
	MyMapper(MapReduce<int,char,int,char,string> * mr):SeqMR_Mapper<int,char,int,char,string>(mr){};
	virtual vector< pair<int,char> > map( pair<int,char> a ) { vector< pair<int,char> > m; m.push_back(a); return m; };
};

class MyReducer : public SeqMR_Reducer<int,char,int,char,string> {
public:
	MyReducer(MapReduce<int,char,int,char,string> * mr):SeqMR_Reducer<int,char,int,char,string>(mr){};
	virtual pair<int,string> reduce( pair<int, vector<char> > bs ) 
	{
		string reduced;
		for( vector<char>::iterator it = bs.second.begin() ; it != bs.second.end() ; ++it) reduced += *it;
		return pair<int,string>(bs.first,reduced); 
	};
};

int main()
{
	cout << "SeqMR test" << endl;

	vector< pair<int,char> > input;
	input.push_back(pair<int,char>(1,'a'));
	input.push_back(pair<int,char>(2,'b'));
	input.push_back(pair<int,char>(1,'c'));

	SeqMR_MapReduce<int,char,int,char,string> mapReduce;
	MyMapper mapper(&mapReduce);
	MyReducer reducer(&mapReduce);

	vector< pair<int,string> > output = mapReduce.run(mapper,reducer,input);
	cout << output << endl;

	return 0;
}
