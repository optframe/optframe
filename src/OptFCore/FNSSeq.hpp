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

#include "../OptFrame/NSSeq.hpp"
#include "coro/Generator.hpp" // this is a very special class!! coroutines support \o/

namespace optframe {

//virtual uptr<Move<XES, XEv, XSH>> randomMove(const XES&) = 0;
//virtual uptr<NSIterator<XES, XEv, XSH>> getIterator(const XES&) = 0;

// =============================================================
// this NSSeq uses "Boring" iterator... first, next, bla, bla...
// -------------------------------------------------------------

template<
  class IMS,                              // Iterator Memory State structure
  XESolution XES,                         // ESolution Type
  uptr<Move<XES>> (*fRandom)(const XES&), // fRandom
  IMS (*fIterator)(const XES&),           // fIterator (just initializes IMS)
  void (*fFirst)(IMS&),                   // iterator.first()
  void (*fNext)(IMS&),                    // iterator.next()
  bool (*fIsDone)(IMS&),                  // iterator.isDone()
  uptr<Move<XES>> (*fCurrent)(IMS&)       // iterator.current()
  >
class FNSSeqBoring final : public NSSeq<XES>
{
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective
public:
   //
   // function templates (exported)
   //
   using ims_type = IMS;
   static auto ft_iterator = fIterator;
   static auto ft_first = fFirst;
   static auto ft_next = fNext;
   static auto ft_isdone = fIsDone;
   static auto ft_current = fCurrent;

   // iterator initialization (fGenerator)
   static IMS sf_iterator(const XES& se)
   {
      return fIterator(se);
   }

   static void sf_first(std::pair<int, int>& p)
   {
      return fFirst(p);
   }

   static void sf_next(std::pair<int, int>& p)
   {
      return fNext(p);
   }

   static bool sf_isdone(std::pair<int, int>& p)
   {
      return fIsDone(p);
   }

   static uptr<Move<XES>> sf_current(std::pair<int, int>& p)
   {
      return std::move(fCurrent(p));
   }
private:
   // internal class for iterator
   class FNSIteratorBoring final : public NSIterator<XES>
   {
   public:
      IMS ims;
      FNSIteratorBoring(const IMS& _ims) noexcept
        : ims(_ims)
      {
      }

      virtual void first()
      {
         fFirst(ims);
      }

      virtual void next()
      {
         fNext(ims);
      }

      virtual bool isDone()
      {
         return fIsDone(ims);
      }

      virtual uptr<Move<XES>> current()
      {
         return fCurrent(ims);
      }
   };

public:
   virtual uptr<Move<XES>> randomMove(const XES& se) override
   {
      return fRandom(se);
   }

   virtual uptr<NSIterator<XES>> getIterator(const XES& se) override
   {
      return uptr<NSIterator<XES>>{ new FNSIteratorBoring{ fIterator(se) } };
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FNSSeqBoring";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

// =============================================================
// this NSSeq uses "fancy" iterator... Generators and coroutines
// -------------------------------------------------------------

template<
  XESolution XES,                                 // ESolution Type
  uptr<Move<XES>> (*fRandom)(const XES&),         // fRandom
  Generator<Move<XES>*> (*fGenerator)(const XES&) // fGenerator: IMPORTANT! must respect 'unique' semantics! never repeat pointer.
  >
class FNSSeqFancy final : public NSSeq<XES>
{
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective

   // internal class for iterator
   class FNSIterator final : public NSIterator<XES>
   {
   public:
      bool done = { true };            // flag to inform that iterator is 'done'
      bool consumedCurrent = { true }; // flag that indicates that current was already given (unique_ptr semantics)
      //
      Generator<Move<XES>*> gen; // must initialize via move semantics

      FNSIterator(Generator<Move<XES>*>&& _gen)
        : gen(std::move(_gen))
      {
      }

      virtual void first()
      {
         done = !gen.next();      // advance and update bool
         consumedCurrent = false; // can allow consuming again
      }

      virtual void next()
      {
         done = !gen.next();      // advance and update bool
         consumedCurrent = false; // can allow consuming again
      }

      virtual bool isDone()
      {
         return done; // verify bool
      }

      virtual uptr<Move<XES>> current()
      {
         // should never repeat pointer (enforce 'unique' semantics!)
         if (consumedCurrent) {
            //std::cerr << "ALREADY CONSUMED!!" << std::endl;
            return nullptr;
         }
         consumedCurrent = true;
         return uptr<Move<XES>>(gen.getValue());
      }
   };

public:
   virtual uptr<Move<XES>> randomMove(const XES& se) override
   {
      return fRandom(se);
   }

   virtual uptr<NSIterator<XES>> getIterator(const XES& se) override
   {
      return uptr<NSIterator<XES>>{ new FNSIterator{ std::move(fGenerator(se)) } };
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FNSSeqFancy";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FCORE_FNSSEQ_HPP_*/
