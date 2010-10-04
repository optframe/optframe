/* MapReduce Interface
Project: 
Author : 
Date   : 
*/

#include "../../$MRI/$MRI.h"
#include <iostream>

class MyMapper : public $MRI_Mapper<$keyA,$A,$keyB,$B,$C> {
public:
	MyMapper(MapReduce<$keyA,$A,$keyB,$B,$C> * mr):$MRI_Mapper<$keyA,$A,$keyB,$B,$C>(mr){};
	virtual multimap<$keyB,$B> map( pair<$keyA,$A> a ) 
	{ 
		multimap<$keyB,$B> m; 
		cout << "\tMapping..\n"; 
		//...
		cout << "\tOk\n"; 
		return m; 
	};
};

class MyReducer : public $MRI_Reducer<$keyA,$A,$keyB,$B,$C> {
public:
	MyReducer(MapReduce<$keyA,$A,$keyB,$B,$C> * mr):$MRI_Reducer<$keyA,$A,$keyB,$B,$C>(mr){};
	virtual pair<$keyB,$C> reduce( pair<$keyB, vector<$B> > bs ) 
	{
		$C reduced;
		cout << "\tReducing..\n"; 
		//...
		cout << "\tOk\n"; 
		return pair<$keyB,$C>(bs.first,reduced); 
	};
};

int main()
{
	cout << "$MRI test" << endl;

	multimap<$keyA,$A> input;

	$MRI_MapReduce<$keyA,$A,$keyB,$B,$C> mapReduce;
	MyMapper mapper(&mapReduce);
	MyReducer reducer(&mapReduce);

	map<$keyB,$C> output = mapReduce.run(mapper,reducer,input);

	return 0;
}
