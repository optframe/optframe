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

#ifndef OPTFRAME_FCORE_FCONSTRUCTIVE_RK_HPP_
#define OPTFRAME_FCORE_FCONSTRUCTIVE_RK_HPP_

#include <functional>

#include <OptFrame/Constructive.hpp>
#include <OptFrame/Heuristics/EA/EA.h>
#include <OptFrame/Heuristics/EA/RK/ConstructiveRK.hpp>
#include <OptFrame/Heuristics/EA/RK/RK.h>

namespace optframe {

// only works for RK representation
//
template<class KeyType = double>
class FConstructiveRK final : public ConstructiveRK<KeyType>
{
   using super = ConstructiveRK<KeyType>;

   using S = std::vector<KeyType>;

public:
#ifdef OPTFCORE_FUNC_STATIC
   typedef S(*FuncTypeGenerate);
#else
   typedef std::function<S()> FuncTypeGenerate;
#endif

   //S(*fGenerate)(); // constructive
   FuncTypeGenerate fGenerate;

   //S (*_fGenerate)())
   //S (*_fGenerate)())
   FConstructiveRK(FuncTypeGenerate _fGenerate)
     : fGenerate{ _fGenerate }
   {
      //std::cout << "FConstructive()" << std::endl;
   }

   virtual ~FConstructiveRK()
   {
      //std::cout << "~FConstructive()" << std::endl;
   }

   // fGenerate is supposed to be the simplest possible function.
   // Although fGenerate does not require optional or timelimit (two connected concepts),
   // the Constructive class must support... since it's completely general.
   //
   std::optional<S> generateSolution(double timelimit) override
   {
      return std::make_optional(fGenerate());
   }

   virtual bool compatible(std::string s) override
   {
      // Experimental: trying substring matching
      // If MY id() INCLUDES 's' and position is ZERO, then we must accept it, right?
      std::string sid = id();
      std::size_t found_begin = sid.find(s);
      if (found_begin == 0)
         return true;
      //
      return (s == idComponent()) || (Constructive<std::vector<KeyType>>::compatible(s));
   }

   static std::string idComponent()
   {
      std::stringstream ss;
      ss << super::idComponent() << ":FConstructiveRK";
      return ss.str();
   }

   virtual std::string id() const override
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FCORE_FCONSTRUCTIVE_RK_HPP_*/
