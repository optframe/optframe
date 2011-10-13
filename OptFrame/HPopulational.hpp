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

#ifndef OPTFRAME_HPOPULATIONAL_HPP_
#define OPTFRAME_HPOPULATIONAL_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Solution.hpp"
#include "Population.hpp"
#include "Evaluation.hpp"

#include "HeuristicMono.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class HPopulational: public HeuristicMono<R, ADS, M>
{
   typedef vector<Evaluation<M>*> FitnessValues;
   typedef const vector<const Evaluation<M>*> ConstFitnessValues;

public:

   HPopulational()
   {
   }

   virtual ~HPopulational()
   {
   }

   // core methods

   // 1
   virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
   {
      // call 3
      Population<R, ADS> p;

      p.push_back(s);

      exec(p, timelimit, target_f);
   }

   // 2
   virtual void exec(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit, double target_f)
   {
      // call 4
      Population<R, ADS> p;

      FitnessValues ev;

      p.push_back(s);

      ev.push_back(&e);

      exec(p, ev, timelimit, target_f);
   }

   // 3
   virtual void exec(Population<R, ADS>& p, double timelimit, double target_f) = 0;

   // 4
   virtual void exec(Population<R, ADS>& p, FitnessValues& ev, double timelimit, double target_f) = 0;

   static string idComponent()
   {
      return "OptFrame:HMono:HP:";
   }

   virtual string id() const
   {
      return idComponent();
   }

};

#endif /* OPTFRAME_HPOPULATIONAL_HPP_ */
