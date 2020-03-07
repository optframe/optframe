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


using namespace optframe;

//template<class T, class ADS, XBaseSolution<vector<T>,ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, class MOVE = MoveTSPOrOptk<T, ADS, S, XEv, XES>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorTSPOrOptk<T, ADS, S, XEv, XES, MOVE, P>, XSearch<XES> XSH = std::pair<S, XEv>>
template<class T, class ADS, XBaseSolution<vector<T>,ADS> S, class MOVE = MoveTSPOrOptk<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorTSPOrOptk<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class NSSeqTSPOrOptk : public NSSeq<XES, XEv>
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
      static_assert(XSolution<S>);
      static_assert(XESolution<XES>);
      static_assert(XEvaluation<XEv>);
   }

   virtual ~NSSeqTSPOrOptk()
   {
   }

   uptr<Move<XES, XEv>> randomMove(const XES& se) override
   {
      const Route& rep = se.first.getR();
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

      uptr<Move<XES, XEv>> m(new MOVE(i, j, k, p));
      S sol(rep); // TODO: think
      if (!m->canBeApplied(sol)) {
         cout << "ERROR IN GENERATION!" << endl;
         m->print();
         exit(1);
      }
      return m;
   }

   uptr<Move<XES, XEv>> validRandomMove(const XES& se) override
   {
      //const Route& r = s.getR();
      uptr<Move<XES, XEv>> m = randomMove(se);
      if (m->canBeApplied(se))
         return m;
      else {
         ///delete m;
         return nullptr;
      }
   }

   virtual uptr<NSIterator<XES, XEv>> getIterator(const XES& se) override
   {
      const Route& r = se.first.getR();
      return uptr<NSIterator<XES, XEv>>(new NSITERATOR(r.size(), k, p));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << NSSeq<XES, XEv>::idComponent() << ":NSSeqTSPOrOptk";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (NSSeq<XES, XEv>::compatible(s));
   }

   virtual string toString() const
   {
      stringstream ss;
      ss << "NSSeqTSPOrOpt{K=" << k << "} with move: " << MOVE::idComponent();
      return ss.str();
   }
};


// compile tests
//using mynsseq_nsseq_tsp_oroptk_test = NSSeqTSPOrOptk<int, short, IsSolution<vector<int>, short>, IsEvaluation<double>, pair<IsSolution<vector<int>, short>, IsEvaluation<double>> >;
using mynsseq_nsseq_tsp_oroptk_test = NSSeqTSPOrOptk<int, short, IsSolution<vector<int>, short>>;
//
static_assert(std::is_base_of<nsseq_test_base, mynsseq_nsseq_tsp_oroptk_test>::value,  "not inherited from NSSeq");
static_assert(!std::is_abstract<mynsseq_nsseq_tsp_oroptk_test>::value,  "abstract nsseq");



#endif /*OPTFRAME_NSSEQ_TSP_OROPTK_HPP_*/
