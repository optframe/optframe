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

#ifndef OPTFRAME_NSENUMVVShiftkIntra_HPP_
#define OPTFRAME_NSENUMVVShiftkIntra_HPP_

// Framework includes
#include "../NSEnum.hpp"

#include "../NSEnumIterator.hpp"
#include "./Moves/MoveVVShiftkIntra.hpp"

using namespace std;

//============================================================================
//                  Shift Neighborhood Structure
//============================================================================

template<class T, class DS>
class NSEnumVVShiftkIntra : public NSEnum<vector<vector<T>>, DS>
{
protected:
   int k;
   vector<vector<pair<int, pair<int, int>>>*> moves;
   std::vector<std::pair<int, int>> moveindex;

public:
   using NSEnum<vector<vector<T>>, DS>::move; // prevents name hiding

   NSEnumVVShiftkIntra(int k)
   {
      this->k = k;
   }

   /*virtual void init(Solution<vector<vector<int> > >* s)
	{
		init(s->getR());
	}

	virtual void init(std::vector<vector<int> >& rep)
	{
		delete moves;
		moves = NSVector<int>::ShiftkIntra_appliableMoves(rep,k);
	}*/

   virtual NSIterator<vector<vector<T>>, DS>& getIterator(const vector<vector<T>>& rep)
   {
      for (int i = 0; i < moves.size(); i++)
         delete moves[i];
      moves.clear();
      moveindex.clear();

      for (int i = 0; i < rep.size(); i++) {
         moves.push_back(NSVector<int>::shiftk_appliableMoves(rep[i], k));
         for (int j = 0; j < moves.back()->size(); j++)
            moveindex.push_back(std::make_pair(i, j));
      }

      return *new NSEnumIterator<vector<vector<T>>, DS>(*this);
   }

   virtual Move<vector<vector<T>>, DS>& move(unsigned int _k)
   {
      if (_k > size()) {
         cerr << "Neighborhood Shift Error! Move " << _k << " does not exist! Valid Interval from 0 to " << (size() - 1) << "." << std::endl;
         exit(1);

         //return nullptr;
      }

      int i = moveindex[_k].first;
      int j = moveindex[_k].second;

      int k = moves[i]->at(j).first;
      int v = i;
      int p1 = moves[i]->at(j).second.first;
      int p2 = moves[i]->at(j).second.second;

      return *new MoveVVShiftkIntra<T, DS>(k, v, p1, p2);
   }

   virtual Move<vector<vector<T>>, DS>& move(const vector<vector<T>>& rep)
   {
      //cout << "*";
      int v;
      do
         v = rand() % rep.size();
      while (rep[v].size() < k);

      int p1 = rand() % (rep[v].size() - k + 1);

      int p2 = rand() % (rep[v].size() + 1);

      return *new MoveVVShiftkIntra<T, DS>(k, v, p1, p2);
   };

   virtual unsigned int size()
   {
      return moves.size();
   }

   virtual void print()
   {
      std::cout << "NSEnum Vector Vector ShiftkIntra (" << size() << ")" << std::endl;
   }
};

#endif /*OPTFRAME_NSENUMVVShiftkIntra_HPP_*/
