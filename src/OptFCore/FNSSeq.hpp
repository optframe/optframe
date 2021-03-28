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

#ifndef OPTFRAME_FCORE_FNSSEQ_HPP_
#define OPTFRAME_FCORE_FNSSEQ_HPP_

#include <functional>

#include <OptFrame/NSSeq.hpp>
//#include "coro/Generator.hpp" // this is a very special class!! coroutines support \o/

namespace optframe {

//virtual uptr<Move<XES, XEv, XSH>> randomMove(const XES&) = 0;
//virtual uptr<NSIterator<XES, XEv, XSH>> getIterator(const XES&) = 0;

// =============================================================
// this NSSeq uses "Boring" iterator... first, next, bla, bla...
// -------------------------------------------------------------

template< class IMS, XESolution XES>
class FNSSeq final : public NSSeq<XES>
{
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective
public:
   //
   // function templates (exported)
   //
   using ims_type = IMS;

   uptr<Move<XES>> (*fRandom)(const XES&); // fRandom
   IMS (*fIterator)(const XES&);           // fIterator (just initializes IMS)
   void (*fFirst)(IMS&);                   // iterator.first()
   void (*fNext)(IMS&);                    // iterator.next()
   bool (*fIsDone)(IMS&);                  // iterator.isDone()
   uptr<Move<XES>> (*fCurrent)(IMS&);       // iterator.current()

   FNSSeq(
      uptr<Move<XES>> (*_fRandom)(const XES&), // fRandom
      IMS (*_fIterator)(const XES&),           // fIterator (just initializes IMS)
      void (*_fFirst)(IMS&),                   // iterator.first()
      void (*_fNext)(IMS&),                    // iterator.next()
      bool (*_fIsDone)(IMS&),                  // iterator.isDone()
      uptr<Move<XES>> (*_fCurrent)(IMS&)       // iterator.current()
   )
   :
      fRandom{ _fRandom },
      fIterator { _fIterator },
      fFirst{ _fFirst },
      fNext{ _fNext },
      fIsDone { _fIsDone },
      fCurrent { _fCurrent }
   {
   }

   // copy constructor
   FNSSeq(const FNSSeq& fnsseq) 
      :
      fRandom{ fnsseq.fRandom },
      fIterator { fnsseq.fIterator },
      fFirst{ fnsseq.fFirst },
      fNext{ fnsseq.fNext },
      fIsDone { fnsseq.fIsDone },
      fCurrent { fnsseq.fCurrent }
   {
   }

private:
   // internal class for iterator
   class FNSIterator final : public NSIterator<XES>
   {
   public:
      IMS ims;
      FNSSeq<IMS, XES>& nsseq;
      //
      FNSIterator(const IMS& _ims, FNSSeq<IMS, XES>& _nsseq) noexcept
        : 
        ims{ _ims },
        nsseq{ _nsseq }
      {
      }

      virtual void first()
      {
         nsseq.fFirst(ims);
      }

      virtual void next()
      {
         nsseq.fNext(ims);
      }

      virtual bool isDone()
      {
         return nsseq.fIsDone(ims);
      }

      virtual uptr<Move<XES>> current()
      {
         return nsseq.fCurrent(ims);
      }
   };

public:
   virtual uptr<Move<XES>> randomMove(const XES& se) override
   {
      return fRandom(se);
   }

   virtual uptr<NSIterator<XES>> getIterator(const XES& se) override
   {
      return uptr<NSIterator<XES>>{ new FNSIterator{ fIterator(se), *this } };
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FNSSeq";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FCORE_FNSSEQ_HPP_*/
