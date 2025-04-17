// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_EMPTYSINGLEOBJSEARCH_HPP_
#define OPTFRAME_HEURISTICS_EMPTYSINGLEOBJSEARCH_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C
#include <math.h>
// C++
#include <string>
#include <vector>
//

#include "../Search/SingleObjSearch.hpp"

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

MOD_EXPORT template <XESolution XES, XEvaluation XEv = Evaluation<>>
class EmptySingleObjSearch : public SingleObjSearch<XES> {
 public:
  EmptySingleObjSearch() {}

  ~EmptySingleObjSearch() override = default;

  SearchOutput<XES> searchBy(const StopCriteria<XEv>& stopCriteria,
                             std::optional<XES> _best) override {
    if (_best)
      std::cout << "WARNING: RETURNING A EmptySingleObjSearch!" << std::endl;
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
