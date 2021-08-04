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

#ifndef OPTFRAME_FXCORE_FXRNSSEQ_HPP_
#define OPTFRAME_FXCORE_FXRNSSEQ_HPP_

#include <functional>

#include "coro/Generator.hpp" // this is a very special class!! coroutines support \o/
#include <OptFrame/NSSeq.hpp>

namespace optframe {

// =============================================================
// this RNSSeq uses "fancy" iterator... Generators and coroutines
// -------------------------------------------------------------

template<
  XESolution XES,       // ESolution Type
  typename ContextType, // Context / Memory Type
  Generator<Move<XES>*> (*fRGenerator)(const XES&,
                                       ContextType& ctx,
                                       sref<RandGen> rg,
                                       bool singleMove) // fGenerator: IMPORTANT! must respect 'unique' semantics! never repeat pointer.
  >
class FxRNSSeq final : public NSSeq<XES>
{
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective

   // random generator
   sref<RandGen> rg;
   //
   ContextType ctx;
   //
public:
   FxRNSSeq(sref<RandGen> rg)
     : rg{ rg }
   {
   }

private:
   //
   // TODO: context could live inside iterator,
   // so we can pass an optional context (but passed by reference??)
   // This way, randomMove will not mess with active iterators

   // internal class for iterator
   class FxRNSIterator final : public NSIterator<XES>
   {
   public:
      bool done = { true };            // flag to inform that iterator is 'done'
      bool consumedCurrent = { true }; // flag that indicates that current was already given (unique_ptr semantics)
      //
      sref<RandGen> rg;
      //
      Generator<Move<XES>*> gen; // must initialize via move semantics

      FxRNSIterator(Generator<Move<XES>*>&& _gen, sref<RandGen> rg)
        : gen(std::move(_gen))
        , rg{ rg }
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
      // creates iterator for single use ('true')
      uptr<NSIterator<XES>> iter{
         new FxRNSIterator{
           std::move(fRGenerator(se, ctx, rg, true)),
           rg }
      };
      iter->first();
      uptr<Move<XES>> mv = iter->current();
      return mv;
   }

   virtual uptr<NSIterator<XES>> getIterator(const XES& se) override
   {
      return uptr<NSIterator<XES>>{
         new FxRNSIterator{
           std::move(fRGenerator(se, ctx, rg, false)),
           rg }
      };
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FxRNSSeqFancy";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FXCORE_FXRNSSEQ_HPP_*/
