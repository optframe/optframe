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

#ifndef OPTFRAME_FCORE_FEVALUATOR_HPP_
#define OPTFRAME_FCORE_FEVALUATOR_HPP_

#include <functional>

#include <OptFrame/Evaluator.hpp>

namespace optframe {

enum MinOrMax
{
   MINIMIZE,
   MAXIMIZE
};

template<
  XESolution XES,     // ESolution Type
  MinOrMax Minimizing // is minimization
  >
class FEvaluator final : public Evaluator<typename XES::first_type, typename XES::second_type, XES>
{
   using super = Evaluator<typename XES::first_type, typename XES::second_type, XES>;
   using S = typename XES::first_type;
   using XEv = typename XES::second_type;
   using XSH = XES; // only single objective

public:
#ifdef OPTFCORE_FUNC_STATIC
   typedef XEv (*FuncTypeEvaluate)(const S&);
#else
   typedef std::function<XEv(const S&)> FuncTypeEvaluate;
#endif

   //XEv (*fEvaluate)(const S&); // evaluation function
   FuncTypeEvaluate fEvaluate;

   //FEvaluator(XEv (*_fEvaluate)(const S&))

   FEvaluator(FuncTypeEvaluate _fEvaluate)
     : super(
         (Minimizing == MinOrMax::MINIMIZE) ? sref<Direction<XEv>>{ new Minimization<XEv>() }
                                            : sref<Direction<XEv>>{ new Maximization<XEv>() })
     , fEvaluate{ _fEvaluate }
   {
   }

   virtual XEv evaluate(const S& s)
   {
      return fEvaluate(s);
   }

   //virtual constexpr bool isMinimization() const
   // note that c++-10.1 accepts this on --std=c++17, which is a mistake (should only exist on c++20)
   /*
   virtual bool isMinimization() const
   {
      return Minimizing == MinOrMax::MINIMIZE;
   }
   */

   virtual bool compatible(string s) override
   {
      // forcing comparison here (with GeneralEvaluator) due to Multiple Inheritance
      // TODO: find better solution to this
      //return (s == idComponent()) || (direction->compatible(s)) || (s == "OptFrame:GeneralEvaluator"); //|| (GeneralEvaluator<XES, XEv, XES>::compatible(s));
      // FIXED!
      return (s == idComponent()) || (super::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << super::idComponent() << ":FEvaluator";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FCORE_FEVALUATOR_HPP_*/
