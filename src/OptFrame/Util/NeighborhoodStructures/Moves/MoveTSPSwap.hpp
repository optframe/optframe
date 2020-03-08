// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

#ifndef OPTFRAME_MOVETSPSWAP_HPP_
#define OPTFRAME_MOVETSPSWAP_HPP_

// Framework includes
#include "../../../Move.hpp"

using namespace std;

// Working structure: vector<T>

namespace optframe
{

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class MoveTSPSwap : public Move<XES, XEv>
{
   typedef vector<T> Route;

protected:
   int p1, p2; // position 1 and position 2, respectively

   OPTFRAME_DEFAULT_PROBLEM* problem;

public:
   MoveTSPSwap(int _p1, int _p2, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr)
     : p1(_p1)
     , p2(_p2)
     , problem(_problem)
   {
   }

   virtual ~MoveTSPSwap()
   {
   }

   int get_p1() const
   {
      return p1;
   }

   int get_p2() const
   {
      return p2;
   }

   bool canBeApplied(const XES& se) override
   {
      const Route& rep = se.first.getR();
      bool all_positive = (p1 >= 0) && (p2 >= 0);
      bool size_ok = (p1 < ((int)rep.size())) && (p2 < ((int)rep.size()));
      return all_positive && size_ok && (rep.size() >= 2);
   }

   uptr<Move<XES, XEv>> apply(XES& se) override
   {
      Route& rep = se.first.getR();
      T t = rep[p1];
      rep[p1] = rep[p2];
      rep[p2] = t;

      return uptr<Move<XES, XEv>>(new MoveTSPSwap(p1, p2));
   }

   virtual bool operator==(const Move<XES, XEv>& _m) const
   {
      const MoveTSPSwap& m1 = (const MoveTSPSwap&)_m;
      return ((m1.p1 == p1) && (m1.p2 == p2));
   }

   void print() const
   {
      cout << "MoveTSPSwap( " << p1 << " <=> " << p2 << " )" << endl;
   }
};

}

#endif /*OPTFRAME_MOVETSPSWAP_HPP_*/
