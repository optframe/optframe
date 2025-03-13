// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FXRNSENUM_HPP_
#define OPTFCORE_FXRNSENUM_HPP_

#include <functional>
#include <string>
#include <vector>
//
#include <OptFrame/Core/NSEnum.hpp>

// no need for coroutines

namespace optframe {

// Type I: RNSEnumIteratorOptimistic
// Issue 1: Repeated moves
// Issue 2: Incomplete (Missing moves)
// Spirit: believes in randomness
//
template <XESolution XES, XESolution XSH = XES>
class RNSEnumIteratorOptimistic : public NSIterator<XES, XSH> {
 private:
  NSEnum<XES, XSH>& ns;
  unsigned int move;
  unsigned int nsSize;
  sref<RandGen> rg;
  int count;

 public:
  RNSEnumIteratorOptimistic(NSEnum<XES, XSH>& _ns, sref<RandGen> _rg)
      : ns{_ns}, rg{_rg} {
    move = 0;
    nsSize = _ns.size();
    count = 0;
  }

  virtual ~RNSEnumIteratorOptimistic() {}

  void first() override {
    move = rg->rand(nsSize);
    count = 1;
  }

  void next() override {
    move = rg->rand(nsSize);
    count++;
  }

  bool isDone() override { return count > nsSize; }

  uptr<Move<XES, XSH>> current() override {
    if (isDone())
      // throw IteratorOutOfBound(move);
      return nullptr;
    return ns.indexMove(move);
  }
};

// Type I: RNSEnumIteratorShuffle
// Issue 1: -
// Issue 2: -
// Issue 3: O(neighborhood_size) instead of O(moves) - No Amortized Time!
// Spirit: always complete but may be slow
template <XESolution XES, XESolution XSH = XES>
class RNSEnumIteratorShuffle : public NSIterator<XES, XSH> {
 private:
  NSEnum<XES, XSH>& ns;
  unsigned int move;
  unsigned int nsSize;
  sref<RandGen> rg;
  std::vector<unsigned int> idMoves;
  int count;

 public:
  RNSEnumIteratorShuffle(NSEnum<XES, XSH>& _ns, sref<RandGen> _rg)
      : ns{_ns}, rg{_rg} {
    move = 0;
    nsSize = _ns.size();
    idMoves.reserve(nsSize);  // how much cost here?
    count = 0;
  }

  virtual ~RNSEnumIteratorShuffle() {}

  void first() override {
    count = 0;
    idMoves.clear();
    for (unsigned i = 0; i < nsSize; i++) idMoves.push_back(i);
    //
    rg->shuffle(this->idMoves);
    //
    move = idMoves[count];
    std::cout << "first = " << move << std::endl;
    count++;
  }

  void next() override {
    move = idMoves[count];
    count++;
    std::cout << "next = " << move << std::endl;
  }

  bool isDone() override {
    std::cout << "count =" << count << " sz=" << nsSize << std::endl;
    return count >= nsSize;
  }

  uptr<Move<XES, XSH>> current() override {
    if (isDone())
      // throw IteratorOutOfBound(move);
      return nullptr;
    return ns.indexMove(move);
  }
};

template <XESolution XES, unsigned int (*fSize)(),
          uptr<Move<XES>> (*fIndex)(unsigned int k),
          typename RNSEnumIterator = RNSEnumIteratorOptimistic<XES>>
class FxRNSEnum final : public NSEnum<XES> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

  // random generator
  sref<RandGen> rg;
  //
 public:
  FxRNSEnum(sref<RandGen> _rg) : NSEnum<XES>(_rg), rg{_rg} {}

  uptr<Move<XES, XSH>> indexMove(unsigned int index) override {
    return fIndex(index);
  }

  unsigned int size() const override { return fSize(); }

  virtual uptr<NSIterator<XES>> getIterator(const XES&) {
    return uptr<NSIterator<XES>>(new RNSEnumIterator(*this, rg));
  }

 private:
  // fGenerator: must respect 'unique' semantics! never repeat pointer.
  //

 public:
  // no need to reimplement 'randomMove': trivial with indexMove and size

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":FxRNSEnum";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_FXRNSENUM_HPP_
