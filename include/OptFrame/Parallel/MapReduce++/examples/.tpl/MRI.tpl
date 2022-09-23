// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
// 
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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
