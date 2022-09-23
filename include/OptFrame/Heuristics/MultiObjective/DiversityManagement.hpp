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

#ifndef OPTFRAME_DIVERSITY_MANAGEMENT_HPP_
#define OPTFRAME_DIVERSITY_MANAGEMENT_HPP_

#include "../../Component.hpp"
#include "MOSIndividual.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class DiversityManagement : public Component
{
public:
   DiversityManagement()
   {
   }

   virtual ~DiversityManagement()
   {
   }

   // assign diversity to individual 's' according to population 'P'
   virtual void assignDiversityIndividual(MOSIndividual<R, ADS, DS>& s, const vector<const MOSIndividual<R, ADS, DS>*>& P)
   {
      vector<MOSIndividual<R, ADS, DS>*> v;
      v.push_back(&s);
      assignDiversityGroup(v, P);
   }

   // assign diversity to all individuals from population 'P'
   virtual void assignDiversityAll(vector<MOSIndividual<R, ADS, DS>*>& P)
   {
      vector<const MOSIndividual<R, ADS, DS>*> Pconst(P.begin(), P.end());
      assignDiversityGroup(P, Pconst);
   }

   // assign diversity to group of individuals 'g' according to population 'P'
   virtual void assignDiversityGroup(vector<MOSIndividual<R, ADS, DS>*>& g, const vector<const MOSIndividual<R, ADS, DS>*>& P) = 0;

   virtual void print() const
   {
      cout << "DiversityManagement" << endl;
   }
};

template<class DS = OPTFRAME_DEFAULT_DS>
struct DiversityIndividual
{
   int idx;
   double diversity;
   MultiEvaluation<DS>* mev;

   DiversityIndividual()
   {
      idx = -1;
      diversity = 0;
      mev = nullptr;
   }

   DiversityIndividual(int _idx, double _diversity, MultiEvaluation<DS>* _mev)
     : idx(_idx)
     , diversity(_diversity)
     , mev(_mev)
   {
   }
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class CrowdingDistance : public DiversityManagement<R, ADS, DS>
{
public:
   vector<Direction<DS>*> vDir;

   CrowdingDistance(vector<Direction<DS>*>& _vDir)
     : vDir(_vDir)
   {
   }

   virtual ~CrowdingDistance()
   {
   }

   static bool compare(const pair<double, int>& p1, const pair<double, int>& p2)
   {
      return p1.first < p2.first;
   }

   virtual void assignDiversityGroup(vector<MOSIndividual<R, ADS, DS>*>& g, const vector<const MOSIndividual<R, ADS, DS>*>& P)
   {
      const int INF = 10000000;

      vector<DiversityIndividual<DS>> I(P.size());
      for (unsigned s = 0; s < I.size(); s++)
         I[s] = DiversityIndividual<DS>(s, 0, P[s]->mev);

      int l = I.size();
      if (l == 0)
         return;

      // for each objective 'm'
      for (unsigned m = 0; m < vDir.size(); m++) {
         // I = sort(I, m)
         vector<pair<double, int>> fitness; // (fitness, id)

         for (int i = 0; i < I.size(); i++) {
            double fit = I[i].mev->at(m).evaluation();
            fitness.push_back(make_pair(fit, i));
         }

         sort(fitness.begin(), fitness.end(), compare);

         // I[1] dist = I[l] dist = 'infinity'
         // ADAPTATION
         /*
			 r[fitness[0].second]->diversity = numeric_limits<double>::infinity();
			 r[fitness[l - 1].second]->diversity = numeric_limits<double>::infinity();
			 */

         ////cout << "ORDER: (rank=" << r << ") " << fitness << endl;
         // for i=2 to l-1
         // ADAPTATION WITH ANOTHER LOOP
         for (int k = 0; k < fitness.size(); k++) {
            int i = fitness[k].second;

            if (k == 0) {
               I[i].diversity += numeric_limits<double>::infinity();
               continue;
            }

            int im1 = fitness[k - 1].second;

            if (k == ((int)fitness.size()) - 1) {
               I[i].diversity += numeric_limits<double>::infinity();
               continue;
            }

            int ip1 = fitness[k + 1].second;

            // I[i] dist += (I[i+1].m - I[i-1].m)/(fmax_m - fmin_m)
            // ADAPTATION
            I[i].diversity += abs(fitness[ip1].first - fitness[im1].first) / abs(vDir[m]->max() - vDir[m]->min());
         }

      } // for each objective

      for (unsigned s = 0; s < g.size(); s++)
         for (unsigned k = 0; k < I.size(); k++)
            if (g[s]->id == P[I[k].idx]->id) {
               g[s]->diversity = I[k].diversity;
               break;
            }
   } // end function
};

}

#endif /*OPTFRAME_DIVERSITY_MANAGEMENT_HPP_*/
