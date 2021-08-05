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

#ifndef OPTFRAME_FXCORE_FXRNSENUM_HPP_
#define OPTFRAME_FXCORE_FXRNSENUM_HPP_

#include <functional>

#include <OptFrame/NSEnum.hpp>

// no need for coroutines

namespace optframe {

template<XESolution XES,
         unsigned int (*fSize)(),
         uptr<Move<XES>> (*fIndex)(unsigned int k)>
class FxRNSEnum final : public NSEnum<XES>
{
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective

   // random generator
   sref<RandGen> rg;
   //
public:
   FxRNSEnum(sref<RandGen> _rg)
     : NSEnum<XES>(_rg)
     , rg{ _rg }
   {
   }

   virtual uptr<Move<XES, XEv>> indexMove(unsigned int index) override
   {
      return fIndex(index);
   }

   virtual unsigned int size() const override
   {
      return fSize();
   }

   virtual uptr<NSIterator<XES, XEv>> getIterator(const XES&)
   {
      return uptr<NSIterator<XES, XEv>>(new RNSEnumIteratorOptimistic(*this, rg));
   }

private:
   // fGenerator: IMPORTANT! must respect 'unique' semantics! never repeat pointer.
   //

   // Type I: RNSEnumIteratorOptimistic
   // believes in random

   class RNSEnumIteratorOptimistic : public NSIterator<XES, XEv, XSH>
   {
   private:
      NSEnum<XES, XEv, XSH>& ns;
      unsigned int move;
      unsigned int nsSize;
      sref<RandGen> rg;
      int count;

   public:
      RNSEnumIteratorOptimistic(NSEnum<XES, XEv, XSH>& _ns, sref<RandGen> _rg)
        : ns{ _ns }
        , rg{ _rg }
      {
         move = 0;
         nsSize = _ns.size();
         count = 0;
      }

      virtual ~RNSEnumIteratorOptimistic()
      {
      }

      void first() override
      {
         move = rg->rand(nsSize);
         count = 1;
      }

      void next() override
      {
         move = rg->rand(nsSize);
         count++;
      }

      bool isDone() override
      {
         return count > nsSize;
      }

      uptr<Move<XES, XEv>> current() override
      {
         if (isDone())
            //throw IteratorOutOfBound(move);
            return nullptr;
         return ns.indexMove(move);
      }
   };

public:
   // no need to reimplement 'randomMove': trivial with indexMove and size
   /*
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
   */

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FxRNSEnum";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FXCORE_FXRNSENUM_HPP_*/
