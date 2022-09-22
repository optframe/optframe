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

template<class IMS, XESolution XES>
class FNSSeq final : public NSSeq<XES>
{
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective
public:
   //
   // function templates (exported)
   //
   // SHOULD NOT REQUIRE 'COPY' FROM IMS, IF NOT NECESSARY!
   using ims_type = IMS;

#ifdef OPTFCORE_FUNC_STATIC
   typedef uptr<Move<XES>> (*FuncTypeNSRand)(const XES&);
   typedef IMS (*FuncTypeNSSeqItInit)(const XES&);
   typedef void (*FuncTypeNSSeqItFirst)(IMS&);
   typedef void (*FuncTypeNSSeqItNext)(IMS&);
   typedef bool (*FuncTypeNSSeqItIsDone)(IMS&);
   typedef uptr<Move<XES>> (*FuncTypeNSSeqItCurrent)(IMS&);
#else
   typedef std::function<uptr<Move<XES>>(const XES&)> FuncTypeNSRand;
   typedef std::function<IMS(const XES&)> FuncTypeNSSeqItInit;
   typedef std::function<void(IMS&)> FuncTypeNSSeqItFirst;
   typedef std::function<void(IMS&)> FuncTypeNSSeqItNext;
   typedef std::function<bool(IMS&)> FuncTypeNSSeqItIsDone;
   typedef std::function<uptr<Move<XES>>(IMS&)> FuncTypeNSSeqItCurrent;
#endif

   //uptr<Move<XES>> (*fRandom)(const XES&); // fRandom
   FuncTypeNSRand fRandom;
   //IMS (*fIterator)(const XES&);      // fIterator (just initializes IMS)
   FuncTypeNSSeqItInit fIterator;
   //void (*fFirst)(IMS&);              // iterator.first()
   FuncTypeNSSeqItFirst fFirst;
   //void (*fNext)(IMS&);               // iterator.next()
   FuncTypeNSSeqItNext fNext;
   //bool (*fIsDone)(IMS&);             // iterator.isDone()
   FuncTypeNSSeqItIsDone fIsDone;
   //uptr<Move<XES>> (*fCurrent)(IMS&); // iterator.current()
   FuncTypeNSSeqItCurrent fCurrent;

   FNSSeq(
     FuncTypeNSRand _fRandom,
     FuncTypeNSSeqItInit _fIterator,
     FuncTypeNSSeqItFirst _fFirst,
     FuncTypeNSSeqItNext _fNext,
     FuncTypeNSSeqItIsDone _fIsDone,
     FuncTypeNSSeqItCurrent _fCurrent)
     : fRandom{ _fRandom }
     , fIterator{ _fIterator }
     , fFirst{ _fFirst }
     , fNext{ _fNext }
     , fIsDone{ _fIsDone }
     , fCurrent{ _fCurrent }
   {
   }

   // copy constructor
   FNSSeq(const FNSSeq& fnsseq)
     : fRandom{ fnsseq.fRandom }
     , fIterator{ fnsseq.fIterator }
     , fFirst{ fnsseq.fFirst }
     , fNext{ fnsseq.fNext }
     , fIsDone{ fnsseq.fIsDone }
     , fCurrent{ fnsseq.fCurrent }
   {
   }

private:
   // internal class for iterator
   class FNSIterator final : public NSIterator<XES>
   {
   public:
      // SHOULD NOT REQUIRE COPY FROM IMS, IF NOT NECESSARY!
      IMS ims;
      FNSSeq<IMS, XES>& nsseq;
      //
      FNSIterator(IMS&& _ims, FNSSeq<IMS, XES>& _nsseq) noexcept
        : ims{ std::move(_ims) }
        , nsseq{ _nsseq }
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
