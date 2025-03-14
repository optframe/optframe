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

#include "../SeqMR/SeqMR.h"
#include <iostream>

class MyMapper : public SeqMR_Mapper<int, char, int, char, string>
{
public:
   MyMapper(MapReduce<int, char, int, char, string>* mr)
     : SeqMR_Mapper<int, char, int, char, string>(mr){};
   virtual std::vector<std::pair<int, char>> map(pair<int, char> a)
   {
      std::vector<std::pair<int, char>> m;
      m.push_back(a);
      return m;
   };
};

class MyReducer : public SeqMR_Reducer<int, char, int, char, string>
{
public:
   MyReducer(MapReduce<int, char, int, char, string>* mr)
     : SeqMR_Reducer<int, char, int, char, string>(mr){};
   virtual pair<int, string> reduce(pair<int, vector<char>> bs)
   {
      string reduced;
      for (std::vector<char>::iterator it = bs.second.begin(); it != bs.second.end(); ++it)
         reduced += *it;
      return pair<int, string>(bs.first, reduced);
   };
};

int
main()
{
   std::cout << "SeqMR test" << std::endl;

   std::vector<std::pair<int, char>> input;
   input.push_back(pair<int, char>(1, 'a'));
   input.push_back(pair<int, char>(2, 'b'));
   input.push_back(pair<int, char>(1, 'c'));

   SeqMR_MapReduce<int, char, int, char, string> mapReduce;
   MyMapper mapper(&mapReduce);
   MyReducer reducer(&mapReduce);

   std::vector<std::pair<int, string>> output = mapReduce.run(mapper, reducer, input);
   std::cout << output << std::endl;

   return 0;
}
