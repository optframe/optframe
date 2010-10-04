/* MapReduce Interface
Example: 
Author : 
Date   : 
*/

#include "../MapMP/MapMP.h"
#include <iostream>

class MyMapper : public Seq_Mapper<int,int,int,int,int> {
public:
	MyMapper(MapReduce<int,int,int,int,int> * mr):MapMP_Mapper<int,int,int,int,int>(mr){};
	virtual multimap<int,int> map( pair<int,int> a ) 
	{ 
		multimap<int,int> m; 
		cout << "\tMapping..\n"; 
		//...
		cout << "\tOk\n"; 
		return m; 
	};
};

class MyReducer : public Seq_Reducer<int,int,int,int,int> {
public:
	MyReducer(MapReduce<int,int,int,int,int> * mr):MapMP_Reducer<int,int,int,int,int>(mr){};
	virtual pair<int,int> reduce( pair<int, vector<int> > bs ) 
	{
		int reduced;
		cout << "\tReducing..\n"; 
		//...
		cout << "\tOk\n"; 
		return pair<int,int>(bs.first,reduced); 
	};
};

int main()
{
	cout << "MapMP test" << endl;

	multimap<int,int> input;

	Seq_MapReduce<int,int,int,int,int> mapReduce;
	MyMapper mapper(&mapReduce);
	MyReducer reducer(&mapReduce);

	map<int,int> output = mapReduce.run(mapper,reducer,input);

	return 0;
}
