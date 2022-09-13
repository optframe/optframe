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

#ifndef OPTFRAME_FCORE_FMOVE_HPP_
#define OPTFRAME_FCORE_FMOVE_HPP_

#include <functional>

#include <OptFrame/Move.hpp>

namespace optframe {

// Remember: on g++-10.1, if passing default function directly on template, it will complain!
// "function ... has no linkage"
//      bool (*fCanBeApplied)(const XES&) = [](const XES&) -> bool { return false; } // fCanBeApplied
//  So, looks like we need to have function in global scope, for it to have linkage.
//
template<class M, XESolution XES>
auto fDefaultCanBeApplied =
  [](const M&, const XES&) -> bool {
   return true;
};
//
template<class M, XESolution XES>
auto fDefaultCompareEq =
  [](const M& me, const Move<XES>& other) -> bool {
   return false;
};
//
template<class M, XESolution XES>
class FMove final : public Move<XES, typename XES::second_type>
{
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective
   using Self = FMove<M, XES>;

public:
   M m; // internal structure for move

#ifdef OPTFCORE_FUNC_STATIC
#define OPTFCORE_FUNC_STATIC_FMOVE
#endif

   // force static on move, for now
   //#define OPTFCORE_FUNC_STATIC_FMOVE

#ifdef OPTFCORE_FUNC_STATIC_FMOVE
   typedef M (*FuncTypeMoveApply)(const M&, XES&);
   typedef bool (*FuncTypeMoveCBA)(const M&, XES&);
   typedef bool (*FuncTypeMoveEq)(const M&, const Move<XES>&);
#else
   typedef std::function<M(const M&, XES&)> FuncTypeMoveApply;
   typedef std::function<bool(const M&, const XES&)> FuncTypeMoveCBA;
   typedef std::function<bool(const M&, const Move<XES>&)> FuncTypeMoveEq;
#endif

   //M (*fApply)(const M&, XES&);                                                // fApply
   FuncTypeMoveApply fApply;
   //bool (*fCanBeApplied)(const M&, const XES&) = fDefaultCanBeApplied<M, XES>; // fCanBeApplied
   FuncTypeMoveCBA fCanBeApplied = fDefaultCanBeApplied<M, XES>;
   //bool (*fCompareEq)(const M&, const Move<XES>&) = fDefaultCompareEq<M, XES>; // fCompareEq
   FuncTypeMoveEq fCompareEq = fDefaultCompareEq<M, XES>;

   FMove(
     const M& _m,
     //M (*_fApply)(const M&, XES&),                                                // fApply
     FuncTypeMoveApply _fApply,
     //bool (*_fCanBeApplied)(const M&, const XES&) = fDefaultCanBeApplied<M, XES>, // fCanBeApplied
     FuncTypeMoveCBA _fCanBeApplied = fDefaultCanBeApplied<M, XES>,
     //bool (*_fCompareEq)(const M&, const Move<XES>&) = fDefaultCompareEq<M, XES>  // fCompareEq
     FuncTypeMoveEq _fCompareEq = fDefaultCompareEq<M, XES>)
     : m{ _m }
     , fApply{ _fApply }
     , fCanBeApplied{ _fCanBeApplied }
     , fCompareEq{ _fCompareEq }
   {
   }

   /*
   FMove(M&& _m) noexcept
     : m(std::move(_m))
   {
      //std::cout << "FMove() => " << m << " address=" << this << std::endl;
   }
   */

   /*
   FMove(const M& _m) noexcept
     : m(_m)
   {
      //std::cout << "FMove() => " << m << " address=" << this << std::endl;
   }
   */

   virtual bool canBeApplied(const XES& se) override
   {
      return fCanBeApplied(m, se);
   }

   virtual uptr<Move<XES, XEv, XSH>> apply(XSH& se) override
   {
      // fApply will require a reverse move to ALWAYS exist.
      // FCore must be simple! Otherwise, just use fallback class-based mode
      return uptr<Move<XES, XEv, XSH>>{ new Self{ fApply(m, se), fApply, fCanBeApplied, fCompareEq } };
   }

   virtual bool operator==(const Move<XES, XEv, XSH>& move) const override
   {
      const Move<XES>& move2 = (Move<XES>&)move;
      return fCompareEq(this->m, move2);
   }

   bool operator!=(const Move<XES, XEv, XSH>& m) const
   {
      return !(*this == m);
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FMove";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }

   // use 'operator<<' for M
   virtual void print() const override
   {
      std::cout << m << std::endl;
   }
};

} // namespace optframe

#endif /*OPTFRAME_FCORE_FMOVE_HPP_*/
