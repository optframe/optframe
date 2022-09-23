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

#ifndef MAPREDUCE_HPP
#define MAPREDUCE_HPP

#include <omp.h>

///MapMP_MapReduce.
template<class KeyA, class A, class KeyB, class B, class C>
class MapMP_MapReduce : public MapReduce<KeyA, A, KeyB, B, C>
{
public:
   MapMP_MapReduce()
     : numThreads(1){};

   ///MapReduce execution (implemented by library).
   virtual vector<pair<KeyB, C>> run(Mapper<KeyA, A, KeyB, B, C>& mapper, Reducer<KeyA, A, KeyB, B, C>& reducer, vector<pair<KeyA, A>>& as)
   {
      //cout << "Input  :\t" << as << endl;
      omp_set_num_threads(getNumThreads());
#ifndef MRI_USE_MULTIMAP
      vector<pair<KeyB, B>> bs = mapper.run(as);
#else
      multimap<KeyB, B> bs = mapper.run(as);
#endif
      //cout << "Mapped :\t" << bs << endl;
      omp_set_num_threads(getNumThreads());
      vector<pair<KeyB, C>> cs = reducer.run(bs);
      //cout << "Reduced:\t" << cs << endl;
      return cs;
   };

   void setNumThreads(int n) { numThreads = n; };
   int getNumThreads() { return numThreads; };

private:
   int numThreads;
};

///MapMP_StrMapReduce.
class MapMP_StrMapReduce : public MapMP_MapReduce<string, string, string, string, string>
{};

#endif /* MAPREDUCE_HPP */
