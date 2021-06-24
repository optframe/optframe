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

#ifndef OPTFRAME_FXCORE_FXCONSTRUCTIVE_HPP_
#define OPTFRAME_FXCORE_FXCONSTRUCTIVE_HPP_

#include <functional>

#include <OptFrame/Constructive.hpp>

using namespace std; // TODO: remove

namespace optframe {

template<
  XSolution S,                          // Solution Type
  std::optional<S> (*fGenerate)(double) // constructive
  >
class FxConstructive final : public Constructive<S>
{
public:
   virtual std::optional<S> generateSolution(double timelimit)
   {
      return fGenerate(timelimit);
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":FxConstructive";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_FXCORE_FXCONSTRUCTIVE_HPP_*/
