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
Example: 
Author : 
Date   : 
*/

#include "../MapMP/MapMP.h"
#include <iostream>

class MyMapper : public Seq_Mapper<int, int, int, int, int>
{
public:
   MyMapper(MapReduce<int, int, int, int, int>* mr)
     : MapMP_Mapper<int, int, int, int, int>(mr){};
   virtual multimap<int, int> map(pair<int, int> a)
   {
      multimap<int, int> m;
      cout << "\tMapping..\n";
      //...
      cout << "\tOk\n";
      return m;
   };
};

class MyReducer : public Seq_Reducer<int, int, int, int, int>
{
public:
   MyReducer(MapReduce<int, int, int, int, int>* mr)
     : MapMP_Reducer<int, int, int, int, int>(mr){};
   virtual pair<int, int> reduce(pair<int, vector<int>> bs)
   {
      int reduced;
      cout << "\tReducing..\n";
      //...
      cout << "\tOk\n";
      return pair<int, int>(bs.first, reduced);
   };
};

int
main()
{
   cout << "MapMP test" << endl;

   multimap<int, int> input;

   Seq_MapReduce<int, int, int, int, int> mapReduce;
   MyMapper mapper(&mapReduce);
   MyReducer reducer(&mapReduce);

   map<int, int> output = mapReduce.run(mapper, reducer, input);

   return 0;
}
