// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_HEURISTIC_MULTI_OBJ_HPP_
#define OPTFRAME_HEURISTIC_MULTI_OBJ_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Solution.hpp"
#include "Population.hpp"
#include "Evaluation.hpp"

#include "OptFrameComponent.hpp"

template< class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY >
class HeuristicMultiObj :
      public OptFrameComponent
{
   typedef vector<vector<Evaluation<M>*> > MultiEvaluations;
   typedef const vector<vector<const Evaluation<M>*> > ConstMultiEvaluations;

public:

   HeuristicMultiObj()
   {
   }

   virtual ~HeuristicMultiObj()
   {
   }

   // no-optimization
   virtual Population<R, ADS>& search(const Population<R, ADS>& p, double timelimit = 100000000, double target_f = 0)
   {
      Population<R, ADS>* pop = new Population<R, ADS> ();

      for(unsigned i = 0; i < p.size(); i++)
         pop->push_back(p.at(i).clone());

      exec(*pop, timelimit, target_f);

      return *pop;
   }

   // optimizated version
   virtual pair<Population<R, ADS>&, MultiEvaluations&>& search(const Population<R, ADS>& p, ConstMultiEvaluations& ev, double timelimit = 100000000, double target_f = 0)
   {
      //Population* p2 = new Population;
      Population<R, ADS>* p2 = new Population<R, ADS> ;

      for(unsigned i = 0; i < p.size(); i++)
         p2->push_back(p.at(i).clone());

      MultiEvaluations* ev2 = new MultiEvaluations;
      for(unsigned i = 0; i < ev.size(); i++)
      {
         vector<Evaluation<M>* > v;
         for(unsigned j = 0; j < ev[i].size(); j++)
         v->push_back(&ev[i][j]->clone());

         ev2->push_back(v);
      }

      exec(*p2, *ev2, timelimit, target_f);

      return *new pair<Population<R, ADS>&, MultiEvaluations&> (*p2, *ev2);
   }

   // core methods

   // 5
   virtual void exec(Population<R, ADS>& p, double timelimit, double target_f) = 0;

   // 6
   virtual void exec(Population<R, ADS>& p, MultiEvaluations& ev, double timelimit, double target_f) = 0;

   virtual string log()
   {
      return "Empty heuristic log.";
   }

   static string idComponent()
   {
      return "OptFrame:HMulti:";
   }

   virtual string id() const
   {
      return idComponent();
   }

};

#endif /* OPTFRAME_HEURISTIC_MULTI_MULTI_OBJ_HPP_ */
