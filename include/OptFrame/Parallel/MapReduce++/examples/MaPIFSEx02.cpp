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

#include "../MaPI/MaPI.h"
#include <sstream>

string
fmap(string st, string obj)
{
   printf("%s\n", (st + obj).c_str());
   return st + obj;
};

vector<string>*
freduce(std::vector<string>* mapped, string obj)
{
   vector<string>* reduced = new vector<string>;
   reduced->push_back(mapped->at(0) + "(reduzida) ");
   return reduced;
};

int
main(int argc, char** argv)
{
   string text(" mapeada");
   MaPI_FSMapReduce<string> mr;
   mr.init(argc, argv, fmap, text);

   vector<string> input;
   for (int i = 0; i < 4; i++) {
      stringstream s;
      s << "Entrada " << i;
      input.push_back(s.str());
   }

   for (int i = 0; i < input.size(); i++) {
      printf("%s\n", input.at(i).c_str());
   }

   vector<string>* output = mr.mapreduce(fmap, freduce, &input);

   printf("%s\n", output->at(0).c_str());

   mr.finalize();
}
