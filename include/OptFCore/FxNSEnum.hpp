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

#ifndef OPTFRAME_FXCORE_FXNSENUM_HPP_
#define OPTFRAME_FXCORE_FXNSENUM_HPP_

#include <functional>

#include <OptFrame/NSEnum.hpp>

// no need for coroutines

namespace optframe {

template<XESolution XES,
         unsigned int (*fSize)(),
         uptr<Move<XES>> (*fIndex)(unsigned int k)>
class FxNSEnum final : public NSEnum<XES>
{
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective

   // random generator
   sref<RandGen> rg;
   //
public:
   FxNSEnum(sref<RandGen> _rg)
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

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FxNSEnum";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FXCORE_FXNSENUM_HPP_*/
