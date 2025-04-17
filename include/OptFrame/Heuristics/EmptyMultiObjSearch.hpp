// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_EMPTY_MULTI_OBJ_SEARCH_HPP_
#define OPTFRAME_EMPTY_MULTI_OBJ_SEARCH_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <math.h>

#include <vector>

#include "../Search/MultiObjSearch.hpp"

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

MOD_EXPORT template <XESolution XMES>
class EmptyMultiObjSearch : public MultiObjSearch<XMES> {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  using XSH = Pareto<XMES>;

 public:
  EmptyMultiObjSearch() {}

  virtual ~EmptyMultiObjSearch() {}

  // Pareto<XMES>* search(MOSC& mosc, Pareto<XMES>* _pf = nullptr) override
  SearchStatus search(std::optional<Pareto<XMES>>& p,
                      const StopCriteria<XMEv>& stopCriteria) override {
    std::cout << "WARNING: RETURNING A EmptyMultiObjSearch!" << std::endl;
    // return nullptr;
    return SearchStatus::NO_REPORT;
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << MultiObjSearch<XMES>::idComponent() << "empty";
    return ss.str();
  }

  virtual std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /*OPTFRAME_EMPTY_MULTI_OBJ_SEARCH_HPP_*/
