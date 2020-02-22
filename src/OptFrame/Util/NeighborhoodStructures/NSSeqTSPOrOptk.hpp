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

#ifndef OPTFRAME_NSSEQ_TSP_OROPTK_HPP_
#define OPTFRAME_NSSEQ_TSP_OROPTK_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSSeq.hpp"

#include "BaseSolutionTSP.hpp"

#include "Moves/MoveTSPOrOptk.hpp"
#include "NSIterators/IteratorTSPOrOptk.hpp"

using namespace std;

// Working structure: vector<vector<T> >


namespace optframe
{

template<class T, class ADS, XBaseSolution<vector<T>,ADS> S, class MOVE = MoveTSPOrOptk<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorTSPOrOptk<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>>
//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, class MOVE = MoveTSPSwap<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorTSPSwap<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>>
class NSSeqTSPOrOptk : public NSSeq<S, XEv>
{
   typedef vector<T> Route;

private:
   int k;
   P* p; // has to be the last

public:
   NSSeqTSPOrOptk(int _k, P* _p = nullptr)
     : k(_k)
     , p(_p)
   {
   }

   virtual ~NSSeqTSPOrOptk()
   {
   }

   Move<S, XEv>* randomMove(const S& s) override
   {
      const Route& rep = s.getR();
      int n = rep.size();

      if (n - k <= 0) {
         // THROW EXCEPTION!
         cerr << "CANNOT GENERATE MOVE OPTK FOR SOLUTION " << rep << endl;
         exit(1);
         //return *new MOVE(0, 0, k, p);
      }

      int i = rand() % (n - k + 1);

      int j = i;
      while (abs(i - j) < k)
         j = rand() % (n - k + 1);

      Move<S, XEv>* m = new MOVE(i, j, k, p);
      S sol(rep); // TODO: think
      if (!m->canBeApplied(sol)) {
         cout << "ERROR IN GENERATION!" << endl;
         m->print();
         exit(1);
      }
      return m;
   }

   Move<S, XEv>* validRandomMove(const S& s) override
   {
      //const Route& r = s.getR();
      Move<S, XEv>* m = randomMove(s);
      if (m->canBeApplied(s))
         return m;
      else {
         delete m;
         return nullptr;
      }
   }

   virtual NSIterator<S, XEv>* getIterator(const S& s) override
   {
      const Route& r = s.getR();
      return new NSITERATOR(r.size(), k, p);
   }

   static string idComponent()
   {
      stringstream ss;
      ss << NSSeq<S, XEv>::idComponent() << ":NSSeqTSPOrOptk";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (NSSeq<S, XEv>::compatible(s));
   }

   virtual string toString() const
   {
      stringstream ss;
      ss << "NSSeqTSPOrOpt{K=" << k << "} with move: " << MOVE::idComponent();
      return ss.str();
   }
};

}  // namespace optframe

#endif /*OPTFRAME_NSSEQ_TSP_OROPTK_HPP_*/
