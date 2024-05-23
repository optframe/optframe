// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MO_LOCAL_SEARCH_HPP_
#define OPTFRAME_MO_LOCAL_SEARCH_HPP_

#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "Component.hpp"
#include "Evaluation.hpp"
#include "Helper/Solution.hpp"
#include "Hyper/ComponentBuilder.hpp"
#include "MultiObjSearch.hpp"
#include "ParetoManager.hpp"

namespace optframe {

template <XESolution XES, XEMSolution XMES>
class MOLocalSearch : public Component {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  static_assert(is_same<S, typename XMES::first_type>::value);
  static_assert(is_same<XMEv, typename XMES::second_type>::value);

 public:
  MOLocalSearch() {}

  ~MOLocalSearch() override {}

  // core methods

  // 1
  // virtual void moSearchFrom(Pareto<XMES>& p, S& s, paretoManager<S,
  // XMEv, XMES>& pManager, const StopCriteria<XMEv>& stopCriteria) = 0; removed
  // this method.. now only pair XMES!

  // 2
  virtual void moSearchFrom(Pareto<XMES>& p, XMES& smev,
                            ParetoManager<XES, XMES>& pManager,
                            const StopCriteria<XMEv>& stopCriteria) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  std::string toString() const override { return id(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":MOLocalSearch";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /* OPTFRAME_MO_LOCAL_SEARCH_HPP_ */
