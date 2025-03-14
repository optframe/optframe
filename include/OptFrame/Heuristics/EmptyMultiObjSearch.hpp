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

#ifndef OPTFRAME_EMPTY_MULTI_OBJ_SEARCH_HPP_
#define OPTFRAME_EMPTY_MULTI_OBJ_SEARCH_HPP_

#include <math.h>
#include <vector>

#include "../MultiObjSearch.hpp"

namespace optframe {

template<XESolution XMES>
class EmptyMultiObjSearch : public MultiObjSearch<XMES>
{
   using S = typename XMES::first_type;
   using XMEv = typename XMES::second_type;
   using XSH = Pareto<XMES>;

public:
   EmptyMultiObjSearch()
   {
   }

   virtual ~EmptyMultiObjSearch()
   {
   }

   //Pareto<XMES>* search(MOSC& mosc, Pareto<XMES>* _pf = nullptr) override
   SearchStatus search(std::optional<Pareto<XMES>>& p, const StopCriteria<XMEv>& stopCriteria) override
   {
      std::cout << "WARNING: RETURNING A EmptyMultiObjSearch!" << std::endl;
      //return nullptr;
      return SearchStatus::NO_REPORT;
   }

   static string idComponent()
   {
      std::stringstream ss;
      ss << MultiObjSearch<XMES>::idComponent() << "empty";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_EMPTY_MULTI_OBJ_SEARCH_HPP_*/
