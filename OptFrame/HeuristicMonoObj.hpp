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

#ifndef OPTFRAME_HEURISTIC_MONO_HPP_
#define OPTFRAME_HEURISTIC_MONO_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Solution.hpp"
#include "Population.hpp"
#include "Evaluation.hpp"

#include "OptFrameComponent.hpp"

template< class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY >
class HeuristicMono :
      public OptFrameComponent
{
   typedef vector<Evaluation<M>*> FitnessValues;
   typedef const vector<const Evaluation<M>*> ConstFitnessValues;

public:

   HeuristicMono()
   {
   }

   virtual ~HeuristicMono()
   {
   }

   // no-optimization
   Solution<R, ADS>& search(const Solution<R, ADS>& s, double timelimit = 100000000, double target_f = 0)
   {
      Solution<R, ADS>& s2 = s.clone();
      exec(s2, timelimit, target_f);
      return s2;
   }

   // optimizated version
   pair<Solution<R, ADS>&, Evaluation<M>&>& search(const Solution<R, ADS>& s, const Evaluation<M>& e, double timelimit = 100000000, double target_f = 0)
   {
      Solution<R, ADS>& s2 = s.clone();
      Evaluation<M>& e2 = e.clone();
      exec(s2, e2, timelimit, target_f);
      return *new pair<Solution<R, ADS>&, Evaluation<M>&> (s2, e2);
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
   virtual pair<Population<R, ADS>&, FitnessValues&>& search(const Population<R, ADS>& p, ConstFitnessValues& ev, double timelimit = 100000000, double target_f = 0)
   {
      //Population* p2 = new Population;
      Population<R, ADS>* p2 = new Population<R, ADS> ;

      for(unsigned i = 0; i < p.size(); i++)
         p2->push_back(p.at(i).clone());

      FitnessValues* ev2 = new FitnessValues;
      for(unsigned i = 0; i < p.size(); i++)
         ev2->push_back(&ev[i]->clone());

      exec(*p2, *ev2, timelimit, target_f);

      return *new pair<Population<R, ADS>&, FitnessValues&> (*p2, *ev2);
   }

   // core methods

   // 1
   virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f) = 0;

   // 2
   virtual void exec(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit, double target_f) = 0;

   // 3
   virtual void exec(Population<R, ADS>& p, double timelimit, double target_f) = 0;

   // 4
   virtual void exec(Population<R, ADS>& p, FitnessValues& ev, double timelimit, double target_f) = 0;

   virtual string log()
   {
      return "Empty heuristic log.";
   }

   static string idComponent()
   {
      return "OptFrame:HMono:";
   }

   virtual string id() const
   {
      return idComponent();
   }

};

#endif /* OPTFRAME_HEURISTIC_MONO_HPP_ */
