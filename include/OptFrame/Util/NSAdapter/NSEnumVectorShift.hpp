// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_UTIL_NSADAPTER_NSENUMVECTORSHIFT_HPP_
#define OPTFRAME_UTIL_NSADAPTER_NSENUMVECTORSHIFT_HPP_

// C++
#include <vector>
// Framework includes
#include "../../NSEnum.hpp"
#include "./Moves/MoveVectorShift.hpp"

namespace optframe {

//============================================================================
//                  Shift Neighborhood Structure
//============================================================================

template <XESolution XES>
class NSEnumVectorShift : public NSEnum<XES, XES> {
 private:
  int n;

  using Route = typename XES::first_type;

  static_assert(std::is_same_v<Route, std::vector<int>>);

 public:
  explicit NSEnumVectorShift(int n) : n{n} {}

  uptr<Move<XES>> indexMove(unsigned int k) override {
    if (k > size()) {
      cerr << "Neighborhood Shift Error! Move " << k
           << " doesnt exist! Valid Interval from 0 to " << (size() - 1) << "."
           << endl;
      exit(1);

      return nullptr;
    }

    // NOLINTNEXTLINE
    return new MoveVectorShift<XES>((k / n), (k % n));
  }

  int size() const override { return n * n; }

  void print() const override {
    cout << "NSEnum Vector Shift (" << size() << ")" << endl;
  }
};

}  // namespace optframe

#endif  // OPTFRAME_UTIL_NSADAPTER_NSENUMVECTORSHIFT_HPP_
