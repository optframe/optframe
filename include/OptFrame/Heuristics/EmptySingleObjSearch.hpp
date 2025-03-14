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

#ifndef OPTFRAME_HEURISTICS_EMPTYSINGLEOBJSEARCH_HPP_
#define OPTFRAME_HEURISTICS_EMPTYSINGLEOBJSEARCH_HPP_

// C
#include <math.h>
// C++
#include <string>
#include <vector>
//

#include "../Search/SingleObjSearch.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class EmptySingleObjSearch : public SingleObjSearch<XES> {
 public:
  EmptySingleObjSearch() {}

  ~EmptySingleObjSearch() override = default;

  // pair<S, Evaluation<>>* search(StopCriteria<XEv>& sosc,  const S* _s =
  // nullptr,  const Evaluation<>* _e = nullptr) override virtual
  // std::optional<pair<S, XEv>> search(StopCriteria<XEv>& stopCriteria)
  // override
  //
  // SearchStatus search(const StopCriteria<XEv>& stopCriteria) override
  SearchOutput<XES> searchBy(const StopCriteria<XEv>& stopCriteria,
                             std::optional<XES> _best) override {
    if (_best)
      std::cout << "WARNING: RETURNING A EmptySingleObjSearch!" << std::endl;
    // return std::nullopt; // nothing to return
    return SearchStatus::NO_REPORT;
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << "empty";
    return ss.str();
  }

  virtual std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EMPTYSINGLEOBJSEARCH_HPP_
