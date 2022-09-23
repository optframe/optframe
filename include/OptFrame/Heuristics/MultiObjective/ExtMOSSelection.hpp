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

#ifndef OPTFRAME_EXTENDED_MOS_SELECTION_HPP_
#define OPTFRAME_EXTENDED_MOS_SELECTION_HPP_

#include "MOSIndividual.hpp"

#include "MOSExtIndividual.hpp"

#include "../../Component.hpp"

namespace optframe {

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ExtMOSSelection : Component
{
public:
   ExtMOSSelection()
   {
   }

   virtual ~ExtMOSSelection()
   {
   }

   // select individuals and FREE unused individuals, keep archive updated
   virtual void extSelect(unsigned target_size, vector<MOSIndividual<R>*>& ps, vector<MOSExtIndividual<R, X>*>& p, vector<MOSExtIndividual<R, X>*>& archive) = 0;

   virtual void print() const
   {
      cout << "ExtMOSSelection" << endl;
   }
};

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NSGAIIExtSelection : public ExtMOSSelection<R, X, ADS, DS>
{
public:
   NSGAIIExtSelection()
   {
   }

   virtual ~NSGAIIExtSelection()
   {
   }

   static bool compare(MOSExtIndividual<R, X>* ind1, MOSExtIndividual<R, X>* ind2)
   {
      return ind1->betterThan(*ind2);
   }

   virtual void extSelect(unsigned target_size, vector<MOSIndividual<R>*>& ps, vector<MOSExtIndividual<R, X>*>& Pop, vector<MOSExtIndividual<R, X>*>& archive)
   {
      int f = 0;
      unsigned count = 0;
      vector<vector<MOSExtIndividual<R, X>*>> F;
      while (count != Pop.size()) {
         vector<MOSExtIndividual<R, X>*> front;
         for (unsigned i = 0; i < Pop.size(); i++)
            if (Pop.at(i)->fitness == f)
               front.push_back(Pop.at(i));
         F.push_back(front);
         count += front.size();
         f++;
      }

      Pop.clear();

      unsigned i = 0;
      while ((Pop.size() + F[i].size()) <= target_size) {
         // Pt+1 = Pt+1 U Fi
         Pop.insert(Pop.end(), F[i].begin(), F[i].end());
         F[i].clear();

         i = i + 1;
      }

      if (Pop.size() < target_size) {
         sort(F[i].begin(), F[i].end(), compare);

         unsigned missing = target_size - Pop.size();
         for (unsigned j = 0; j < missing; j++) {
            Pop.push_back(F[i][j]);
            F[i][j] = nullptr;
         }
      }

      for (i = 0; i < F.size(); i++)
         for (unsigned j = 0; j < F[i].size(); j++)
            if (F[i][j])
               delete F[i][j];

      archive = Pop; // no other archiving
   }

   virtual void free(vector<MOSExtIndividual<R, X>*>& Pop, vector<MOSExtIndividual<R, X>*>& archive)
   {
      archive = Pop;
      Pop.clear();
   }
};

}

#endif /*OPTFRAME_EXTENDED_MOS_SELECTION_HPP_*/
