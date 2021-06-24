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

#include "../MaPI/MaPI.h"
#include <iostream>

class MySerializer : public MaPI_Serializer<int, char, int, char, string>
{
public:
   virtual int KeyA_fromString(string s) { return atoi(s.c_str()); };
   virtual string KeyA_toString(int o)
   {
      stringstream s;
      s << o;
      return s.str();
   };

   virtual char A_fromString(string s) { return s[0]; };
   virtual string A_toString(char o)
   {
      stringstream s;
      s << o;
      return s.str();
   };

   virtual int KeyB_fromString(string s) { return atoi(s.c_str()); };
   virtual string KeyB_toString(int o)
   {
      stringstream s;
      s << o;
      return s.str();
   };

   virtual char B_fromString(string s) { return s[0]; };
   virtual string B_toString(char o)
   {
      stringstream s;
      s << o;
      return s.str();
   };

   virtual string C_fromString(string s) { return s; };
   virtual string C_toString(string o) { return o; };
};

class MyMapper : public MaPI_Mapper<int, char, int, char, string>
{
public:
   MyMapper(MaPI_MapReduce<int, char, int, char, string>* mr, MaPI_Serializer<int, char, int, char, string>* s)
     : MaPI_Mapper<int, char, int, char, string>(mr, s){};
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

class MyReducer : public MaPI_Reducer<int, char, int, char, string>
{
public:
   MyReducer(MaPI_MapReduce<int, char, int, char, string>* mr, MaPI_Serializer<int, char, int, char, string>* s)
     : MaPI_Reducer<int, char, int, char, string>(mr, s){};
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
main(int argc, char** argv)
{
   cout << "MaPI test" << endl;

   MaPI_MapReduce<int, char, int, char, string> mapReduce;
   MySerializer serializer;
   MyMapper mapper(&mapReduce, &serializer);
   MyReducer reducer(&mapReduce, &serializer);
   mapReduce.initServers(argc, argv);

   vector<pair<int, char>> input;
   input.push_back(pair<int, char>(1, 'a'));
   input.push_back(pair<int, char>(2, 'b'));
   input.push_back(pair<int, char>(1, 'c'));

   vector<pair<int, string>> output = mapReduce.run(mapper, reducer, input);
   cout << output << endl;

   return 0;
}
