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

#ifndef OPTFRAME_ITERATORNSSEQTSPOR1OPT_HPP_
#define OPTFRAME_ITERATORNSSEQTSPOR1OPT_HPP_

#include <vector>

// Framework includes
#include "../../NSIterator.hpp"

#include "../Moves/MoveTSPOr1Opt.hpp"

using namespace std;

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY>
class IteratorNSSeqTSPOr1Opt : public NSIterator<vector<T>, DS>
{

private:
   typedef vector<T> Route;

   //MoveTSPOr1Opt<T, DS >* m;
   //vector<uptr<MoveTSPOr1Opt<T,DS >>> moves;
   uptr<Move<ESolutionHFM>> m;             // general move
   vector<uptr<Move<ESolutionHFM>>> moves; // general moves

   int index; //index of moves
   const Route& rep;

public:
   IteratorNSSeqTSPOr1Opt(const Route& _r)
     : rep(_r)
   {
      m = nullptr;
      index = 0;
   }

   virtual ~IteratorNSSeqTSPOr1Opt()
   {
   }

   void first()
   {

      for (int c = 0; c < rep.size(); c++) {
         for (int pos = 0; pos <= rep.size(); pos++) {
            if ((c != pos) && (c + 1 != pos)) {
               moves.push_back(uptr<Move<ESolutionHFM>>(new MoveTSPOr1Opt<T, DS>(c, pos)));
            }
         }
      }

      if (moves.size() > 0) {
         m = std::move(moves[index]); // stealing from vector... verify if this is correct! otherwise, must need clone() on Move
      } else
         m = nullptr;
   }

   void next()
   {
      index++;
      if (index < moves.size()) {
         m = std::move(moves[index]); // stealing from vector... verify if this is correct! otherwise, must need clone() on Move
      } else
         m = nullptr;
   }

   bool isDone()
   {
      return m == nullptr;
   }

   MoveTSPOr1Opt<T, DS>& current()
   {
      if (isDone()) {
         cout << "There isnt any current element!" << endl;
         cout << "TSPOr1Opt. Aborting." << endl;
         exit(1);
      }

      return *m;
   }
};
#endif /*OPTFRAME_ITERATORNSSEQTSPOR1OPT_HPP_*/
