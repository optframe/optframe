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

#include "../../SeqMR/SeqMR.h"
#include "../../util/Timer.hpp"
#include <cstdlib>
#include <iostream>

class MyMapper : public SeqMR_Mapper<int, int, int, int, int>
{
public:
   MyMapper(MapReduce<int, int, int, int, int>* mr)
     : SeqMR_Mapper<int, int, int, int, int>(mr){};
   virtual vector<pair<int, int>> map(pair<int, int> a)
   {
      vector<pair<int, int>> m;
      //cout << "\tMapping..\n";
      m.push_back(make_pair(a.first, a.second));
      //cout << "\tOk\n";
      return m;
   };
};

class MyReducer : public SeqMR_Reducer<int, int, int, int, int>
{
public:
   MyReducer(MapReduce<int, int, int, int, int>* mr)
     : SeqMR_Reducer<int, int, int, int, int>(mr){};
   virtual pair<int, int> reduce(pair<int, vector<int>> bs)
   {
      int reduced = 0;
      //cout << "\tReducing..\n";
      for (int i = 0; i < bs.second.size(); i++)
         reduced += bs.second[i];
      //cout << "\tOk\n";
      return pair<int, int>(bs.first, reduced);
   };
};

int
main()
{
   cout << "SeqMR test" << endl;
   Timer timer;

   int n = 1000000;
   srand(n);

   vector<pair<int, int>> input;
   for (int i = 0; i < n; i++)
      input.push_back(make_pair(rand() % n, rand() % n));

   SeqMR_MapReduce<int, int, int, int, int> mapReduce;
   MyMapper mapper(&mapReduce);
   MyReducer reducer(&mapReduce);

   vector<pair<int, int>> output = mapReduce.run(mapper, reducer, input);

   return 0;
}
