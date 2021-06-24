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
Example: Word Counter with MapReduce Interface with MaPI
Author : Sabir Ribas
Date   : 2009-02-09
*/

#include "../MaPI/MaPI.h"
#include "../util/Scanner++/Scanner.h"
#include <iostream>

class MySerializer : public MaPI_Serializer<string, string, string, int, int>
{
public:
   virtual string KeyA_fromString(string s) { return s; };
   virtual string KeyA_toString(string o) { return o; };

   virtual string A_fromString(string s) { return s; };
   virtual string A_toString(string o) { return o; };

   virtual string KeyB_fromString(string s) { return s; };
   virtual string KeyB_toString(string o) { return o; };

   virtual int B_fromString(string s) { return atoi(s.c_str()); };
   virtual string B_toString(int o)
   {
      stringstream s;
      s << o;
      return s.str();
   };

   virtual int C_fromString(string s) { return atoi(s.c_str()); };
   virtual string C_toString(int o)
   {
      stringstream s;
      s << o;
      return s.str();
   };
};

class MyMapper : public MaPI_Mapper<string, string, string, int, int>
{
public:
   MyMapper(MaPI_MapReduce<string, string, string, int, int>* mr, MaPI_Serializer<string, string, string, int, int>* s)
     : MaPI_Mapper<string, string, string, int, int>(mr, s){};
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

class MyReducer : public MaPI_Reducer<string, string, string, int, int>
{
public:
   MyReducer(MaPI_MapReduce<string, string, string, int, int>* mr, MaPI_Serializer<string, string, string, int, int>* s)
     : MaPI_Reducer<string, string, string, int, int>(mr, s){};
   virtual pair<string, int> reduce(pair<string, vector<int>> bs) // Reduce function implementation
   {
      int reduced = 0;
      for (vector<int>::iterator it = bs.second.begin(); it != bs.second.end(); ++it)
         reduced += *it;
      return pair<string, int>(bs.first, reduced);
   };
};

int
main(int argc, char** argv)
{
   cout << "MaPI_WorldCounter test" << endl;

   // MapReduce declaration
   MaPI_MapReduce<string, string, string, int, int> mapReduce;
   MySerializer serializer;
   MyMapper mapper(&mapReduce, &serializer);
   MyReducer reducer(&mapReduce, &serializer);
   mapReduce.initServers(argc, argv);

   // Reading text
   Scanner scan(new File("doc"));

   string text;
   while (scan.hasNext())
      text += scan.next() + " ";

   // MapReduce input
   vector<pair<string, string>> input;
   input.push_back(pair<string, string>("doc", text));

   // MapReduce application
   vector<pair<string, int>> output = mapReduce.run(mapper, reducer, input);

   // Output exibition
   cout << "MapReduce word counter result" << endl;
   multimap<int, string> sortedOutput;
   for (vector<pair<string, int>>::iterator it = output.begin(); it != output.end(); it++)
      sortedOutput.insert(pair<int, string>((*it).second, (*it).first));
   for (multimap<int, string>::reverse_iterator it = sortedOutput.rbegin(); it != sortedOutput.rend(); it++)
      cout << *it << endl;

   return 0;
}
