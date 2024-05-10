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

#ifndef OPTFRAME_NSENUMVECTORSHIFT_HPP_
#define OPTFRAME_NSENUMVECTORSHIFT_HPP_

// Framework includes

#include "../../NSEnum.hpp"

#include "./Moves/MoveVectorShift.hpp"

using namespace std;

namespace optframe {

//============================================================================
//                  Shift Neighborhood Structure
//============================================================================

template<class T>
class NSEnumVectorShift : public NSEnum<vector<T>>
{
private:
   int n;

public:
   NSEnumVectorShift(int n)
   {
      this->n = n;
   }

   virtual Move<vector<T>>* kmove(int k)
   {
      if (k > size()) {
         cerr << "Neighborhood Shift Error! Move " << k << " doesnt exist! Valid Interval from 0 to " << (size() - 1) << "." << endl;
         exit(1);

         return nullptr;
      }

      return new MoveVectorShift<T>((k / n), (k % n));
   }

   int size()
   {
      return n * n;
   }

   virtual void print()
   {
      cout << "NSEnum Vector Shift (" << size() << ")" << endl;
   }
};

} // namespace optframe

#endif /*OPTFRAME_NSENUMVECTORSHIFT_HPP_*/
