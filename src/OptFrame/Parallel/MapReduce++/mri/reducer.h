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

#ifndef REDUCER_H
#define REDUCER_H

///Reducer.
template<class KeyA, class A, class KeyB, class B, class C>
class Reducer
{
public:
   ///Iterator reducing execution (implemented by library).
#ifndef MRI_USE_MULTIMAP
   virtual vector<pair<KeyB, C>> run(vector<pair<KeyB, B>>&) = 0;
#else
   virtual vector<pair<KeyB, C>> run(multimap<KeyB, B>&) = 0;
#endif
   ///Reduce function (implemented by user).
   virtual pair<KeyB, C> reduce(pair<KeyB, vector<B>>) = 0;

private:
   MapReduce<KeyA, A, KeyB, B, C>* mapReduce;
};

///StrReducer.
class StrReducer : public Reducer<string, string, string, string, string>
{};

#endif /* REDUCER_H */
