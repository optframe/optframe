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

#ifndef TSP_REPRESENTATION_H_
#define TSP_REPRESENTATION_H_

#include <OptFrame/printable/printable.hpp>
#include <vector>

#include <OptFrame/Util/Matrix.hpp>

#include <vector>

using namespace std;

// Solution Representation
typedef vector<int> RepTSP;

#ifdef MaPI

class TSPSerializer : public MyMaPISerializer<RepTSP, MemTSP>
{
public:
   virtual RepTSP KeyA_fromString(string s)
   {
      //printf("\nINIT================\n");
      RepTSP r;
      //cout << "# "<< s << endl;
      //int i;
      //cin >> i;
      Scanner scan(s);
      scan.useSeparators("()[], ");

      scan.next(); // vector
      scan.next(); // int size

      while (scan.hasNext())
         r.push_back(scan.nextInt());

      //cout << "##" << r << endl;

      //printf("\nFIM================\n");
      return r;
   };
   virtual string KeyA_toString(RepTSP o)
   {
      stringstream s;
      s << o;
      return s.str();
   };

   virtual RankAndStop A_fromString(string s)
   {
      RankAndStop idd;
      Scanner scan(s);
      scan.useSeparators("()[], \t\r\npair");
      idd.first = scan.nextInt();
      idd.second.first = scan.nextDouble();
      idd.second.second = scan.nextDouble();
      return idd;
   };
   virtual string A_toString(RankAndStop o)
   {
      stringstream s;
      s << o;
      return s.str();
   };

   virtual int KeyB_fromString(string s)
   {
      return atoi(s.c_str());
   };
   virtual string KeyB_toString(int o)
   {
      stringstream s;
      s << o;
      return s.str();
   };

   virtual pair<RepTSP, double> B_fromString(string s) // TODO
   {
      //printf("\nINIT================\n%s\n",s.c_str());
      //cout << "Initializing!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;

      RepTSP r;
      double c;

      //cout << endl << "# "<< s << endl;

      Scanner scan(s);
      scan.useSeparators("()[], \t\n");

      scan.next(); // pair
      scan.next(); // vector

      //		cout << scan.next(); // pair
      //		cout << scan.next(); // -1

      //		cout << scan.next(); // pair
      //		cout << scan.next(); // vector

      int size = scan.nextInt(); // int size

      //cout << size << "!" << endl;

      for (int i = 0; i < size && scan.hasNext(); i++)
         r.push_back(scan.nextInt());

      //int i;				cin >> i;

      //cout << "rest" << scan.rest() << endl;

      c = scan.nextDouble();

      //cout << c << endl;

      //cout << "##" << r << " c=" << c << endl;
      //printf("\nFIM================\n");
      return make_pair(r, c);
   };
   virtual string B_toString(pair<RepTSP, double> o)
   {
      stringstream s;
      s << o;
      return s.str();
   };

   virtual RepTSP C_fromString(string s)
   {
      RepTSP r;
      //cout << "# "<< s << endl;
      //int i;
      //cin >> i;
      Scanner scan(s);
      scan.useSeparators("()[], ");

      scan.next(); // vector
      scan.next(); // int size

      while (scan.hasNext())
         r.push_back(scan.nextInt());

      //cout << "##" << r << endl;

      return r;
   };
   virtual string C_toString(RepTSP o)
   {
      stringstream s;
      s << o;
      return s.str();
   };
};

#endif

#endif /*TSP_REPRESENTATION_H_*/
