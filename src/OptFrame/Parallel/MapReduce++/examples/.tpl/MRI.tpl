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
