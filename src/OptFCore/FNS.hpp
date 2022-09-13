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

#ifndef OPTFRAME_FCORE_FNS_HPP_
#define OPTFRAME_FCORE_FNS_HPP_

#include <functional>

#include <OptFrame/NS.hpp>

namespace optframe {

template<XESolution XES>
class FNS final : public NS<XES> // typename XES::second_type
{
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective

public:
#ifdef OPTFCORE_FUNC_STATIC
   typedef uptr<Move<XES>> (*FuncTypeNSRand)(const XES&);
#else
   typedef std::function<uptr<Move<XES>>(const XES&)> FuncTypeNSRand;
#endif

   FuncTypeNSRand fRandom;

   FNS(
     //uptr<Move<XES>> (*_fRandom)(const XES&))
     FuncTypeNSRand _fRandom)
     : fRandom{ _fRandom }
   {
   }

   virtual uptr<Move<XES, XEv, XSH>> randomMove(const XES& se) override
   {
      return fRandom(se);
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FNS";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FCORE_FNS_HPP_*/
