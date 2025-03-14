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

string
fmap(string input, string shared)
{
   string mapped = input + shared;
   printf("%s\n", mapped.c_str());
   return mapped;
}

vector<string>*
freduce(std::vector<string>* mapped, string shared)
{
   vector<string>* reduced = new vector<string>;
   reduced->push_back(mapped->at(0) + " e reduzida");
   return reduced;
}

int
main(int argc, char** argv)
{
   MaPI_FSMapReduce<string> mr;
   mr.init(argc, argv, fmap, string(" mapeada"));

   vector<string> inputs;
   inputs.push_back("Entrada 1");
   inputs.push_back("Entrada 2");

   vector<string>* output = mr.mapreduce(fmap, freduce, &inputs);
   printf("%s\n", output->at(0).c_str());

   mr.finalize();
}
