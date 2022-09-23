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
Example: Basic example using a sequential Mapper and Reducer with MapReduce Interface
Author : Sabir Ribas
Date   : 2009-02-09
*/

#include "../MapMP/MapMP.h"
#include <iostream>

class MyMapper : public MapMP_Mapper<int, char, int, char, string>
{
public:
   MyMapper(MapReduce<int, char, int, char, string>* mr)
     : MapMP_Mapper<int, char, int, char, string>(mr){};
   virtual vector<pair<int, char>> map(pair<int, char> a)
   {
      vector<pair<int, char>> m;
      m.push_back(a);
      cout << "\tMapping..\n";
      sleep(1);
      cout << "\tOk\n";
      return m;
   };
};

class MyReducer : public MapMP_Reducer<int, char, int, char, string>
{
public:
   MyReducer(MapReduce<int, char, int, char, string>* mr)
     : MapMP_Reducer<int, char, int, char, string>(mr){};
   virtual pair<int, string> reduce(pair<int, vector<char>> bs)
   {
      string reduced;
      for (vector<char>::iterator it = bs.second.begin(); it != bs.second.end(); ++it)
         reduced += *it;
      cout << "\tReducing..\n";
      sleep(1);
      cout << "\tOk\n";
      return pair<int, string>(bs.first, reduced);
   };
};

int
main()
{
   cout << "MapMP test" << endl;

   vector<pair<int, char>> input;
   input.push_back(pair<int, char>(1, 'a'));
   input.push_back(pair<int, char>(2, 'b'));
   input.push_back(pair<int, char>(1, 'c'));

   MapMP_MapReduce<int, char, int, char, string> mapReduce;
   mapReduce.setNumThreads(2);
   MyMapper mapper(&mapReduce);
   MyReducer reducer(&mapReduce);

   vector<pair<int, string>> output = mapReduce.run(mapper, reducer, input);
   cout << output << endl;

   return 0;
}
