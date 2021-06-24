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
Example: Sequential Word Counter with MapReduce Interface
Author : Sabir Ribas
Date   : 2009-02-09
*/

#include "../SeqMR/SeqMR.h"
#include "../util/Scanner++/Scanner.h"
#include <iostream>

class MyMapper : public SeqMR_Mapper<string, string, string, int, int>
{
public:
   MyMapper(MapReduce<string, string, string, int, int>* mr)
     : SeqMR_Mapper<string, string, string, int, int>(mr){};
   virtual vector<pair<string, int>> map(pair<string, string> a) // Map function implementation
   {
      vector<pair<string, int>> mapped;
      Scanner scan(a.second);
      scan.useSeparators(string(" \n\r\t\\\'\"!@#^&*()+={[}]|:;,<>./?"));
      while (scan.hasNext())
         mapped.push_back(pair<string, int>(scan.next(), 1));
      return mapped;
   };
};

class MyReducer : public SeqMR_Reducer<string, string, string, int, int>
{
public:
   MyReducer(MapReduce<string, string, string, int, int>* mr)
     : SeqMR_Reducer<string, string, string, int, int>(mr){};
   virtual pair<string, int> reduce(pair<string, vector<int>> bs) // Reduce function implementation
   {
      int reduced = 0;
      for (vector<int>::iterator it = bs.second.begin(); it != bs.second.end(); ++it)
         reduced += *it;
      return pair<string, int>(bs.first, reduced);
   };
};

int
main()
{
   cout << "SeqMR_WorldCounter test" << endl;

   vector<pair<string, string>> input;
   input.push_back(pair<string, string>("doc", "content : this is the content of the text ."));

   SeqMR_MapReduce<string, string, string, int, int> mapReduce;
   MyMapper mapper(&mapReduce);
   MyReducer reducer(&mapReduce);

   vector<pair<string, int>> output = mapReduce.run(mapper, reducer, input);
   cout << output << endl;

   return 0;
}
