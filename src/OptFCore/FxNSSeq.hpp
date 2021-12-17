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

#ifndef OPTFRAME_FXCORE_FXNSSEQ_HPP_
#define OPTFRAME_FXCORE_FXNSSEQ_HPP_

#include <functional>

#include "coro/Generator.hpp" // this is a very special class!! coroutines support \o/
#include <OptFrame/NSSeq.hpp>
//

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
class FxNSSeqBoring final : public NSSeq<XES>
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
   class FxNSIteratorBoring final : public NSIterator<XES>
   {
   public:
      IMS ims;
      FxNSIteratorBoring(const IMS& _ims) noexcept
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
      return uptr<NSIterator<XES>>{ new FxNSIteratorBoring{ fIterator(se) } };
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FxNSSeqBoring";
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
class FxNSSeqFancy final : public NSSeq<XES>
{
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective

   // internal class for iterator
   class FxNSIterator final : public NSIterator<XES>
   {
   public:
      bool done = { true };            // flag to inform that iterator is 'done'
      bool consumedCurrent = { true }; // flag that indicates that current was already given (unique_ptr semantics)
      //
      Generator<Move<XES>*> gen; // must initialize via move semantics

      FxNSIterator(Generator<Move<XES>*>&& _gen)
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
      return uptr<NSIterator<XES>>{ new FxNSIterator{ std::move(fGenerator(se)) } };
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FxNSSeqFancy";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

// ======================================================

template<XESolution XES>
class FxNSSeq final : public NSSeq<XES>
{
   using super = NSSeq<XES>;
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective

   std::function<uptr<Move<XES>>(const XES&)> fRandom;
   // fGenerator: IMPORTANT! must respect 'unique' semantics! never repeat pointer.
   std::function<Generator<Move<XES>*>(const XES&)> fGenerator;

public:
   FxNSSeq(std::function<uptr<Move<XES>>(const XES&)> _fRandom,
           std::function<Generator<Move<XES>*>(const XES&)> _fGenerator)
     : fRandom{ _fRandom }
     , fGenerator{ _fGenerator }
   {
   }

private:
   // internal class for iterator
   class FxNSIterator final : public NSIterator<XES>
   {
   public:
      bool done = { true };            // flag to inform that iterator is 'done'
      bool consumedCurrent = { true }; // flag that indicates that current was already given (unique_ptr semantics)
      //
      Generator<Move<XES>*> gen; // must initialize via move semantics

      FxNSIterator(Generator<Move<XES>*>&& _gen)
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
      return uptr<NSIterator<XES>>{ new FxNSIterator{ std::move(fGenerator(se)) } };
   }

   static string idComponent()
   {
      stringstream ss;
      ss << super::idComponent() << ":FxNSSeq";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FXCORE_FXNSSEQ_HPP_*/
