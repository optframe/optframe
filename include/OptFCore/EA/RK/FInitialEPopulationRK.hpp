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

#ifndef OPTFRAME_FCORE_FINITIAL_EPOPULATION_RK_HPP_
#define OPTFRAME_FCORE_FINITIAL_EPOPULATION_RK_HPP_

#include <functional>

#include <OptFrame/Heuristics/EA/RK/RK.h>
#include <OptFrame/InitialPopulation.hpp>

namespace optframe {

// only works for RK representation
//
template<class KeyType = double, XEvaluation XEv = Evaluation<>>
class FInitialEPopulationRK final : public InitialEPopulation<std::pair<std::vector<KeyType>, XEv>>
{
   using super = InitialEPopulation<std::pair<std::vector<KeyType>, XEv>>;

   using EPopType = VEPopulation<std::pair<std::vector<KeyType>, XEv>>;

public:
#ifdef OPTFCORE_FUNC_STATIC
   typedef EPopType (*FuncTypeGenerate)(unsigned);
#else
   typedef std::function<EPopType(unsigned)> FuncTypeGenerate;
#endif

   //S(*fGenerate)(); // constructive
   FuncTypeGenerate fGenerate;

   //S (*_fGenerate)())
   //S (*_fGenerate)())
   FInitialEPopulationRK(FuncTypeGenerate _fGenerate)
     : fGenerate{ _fGenerate }
   {
      //std::cout << "FConstructive()" << std::endl;
   }

   virtual ~FInitialEPopulationRK()
   {
      //std::cout << "~FConstructive()" << std::endl;
   }

   // fGenerate is supposed to be the simplest possible function.
   // Although fGenerate does not require optional or timelimit (two connected concepts),
   // the Constructive class must support... since it's completely general.
   //
   EPopType generateEPopulation(unsigned populationSize, double timelimit) override
   {
      return std::make_optional(fGenerate(populationSize));
   }

   static std::string idComponent()
   {
      std::stringstream ss;
      ss << super::idComponent() << RK::family() << ":FInitialEPopulationRK";
      return ss.str();
   }

   virtual std::string id() const
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FCORE_FINITIAL_EPOPULATION_RK_HPP_*/
