// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FXNSSEQ_HPP_
#define OPTFCORE_FXNSSEQ_HPP_

#include <functional>
#include <string>
#include <utility>
//
#include <OptFrame/Core/NSSeq.hpp>

//
#include "coro/Generator.hpp"  // this is a very special class!! coroutines support \o/
//

namespace optframe {

// =============================================================
// this NSSeq uses "Boring" iterator... first, next, bla, bla...
// -------------------------------------------------------------

template <class IMS, XESolution XES, uptr<Move<XES>> (*fRandom)(const XES&),
          IMS (*fIterator)(const XES&), void (*fFirst)(IMS&),
          void (*fNext)(IMS&), bool (*fIsDone)(IMS&),
          uptr<Move<XES>> (*fCurrent)(IMS&)>
class FxNSSeqBoring final : public NSSeq<XES> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

 public:
  //
  // function templates (exported)
  //
  using ims_type = IMS;
  static auto ft_iterator = fIterator;
  static auto ft_first = fFirst;
  static auto ft_next = fNext;
  static auto ft_isdone = fIsDone;
  static auto ft_current = fCurrent;

  // iterator initialization (fGenerator)
  static IMS sf_iterator(const XES& se) { return fIterator(se); }

  static void sf_first(std::pair<int, int>& p) { return fFirst(p); }

  static void sf_next(std::pair<int, int>& p) { return fNext(p); }

  static bool sf_isdone(std::pair<int, int>& p) { return fIsDone(p); }

  static uptr<Move<XES>> sf_current(std::pair<int, int>& p) {
    return std::move(fCurrent(p));
  }

 private:
  // internal class for iterator
  class FxNSIteratorBoring final : public NSIterator<XES> {
   public:
    IMS ims;
    FxNSIteratorBoring(const IMS& _ims) noexcept : ims(_ims) {}

    void first() override { fFirst(ims); }

    void next() override { fNext(ims); }

    bool isDone() override { return fIsDone(ims); }

    uptr<Move<XES>> current() override { return fCurrent(ims); }
  };

 public:
  uptr<Move<XES>> randomMove(const XES& se) override { return fRandom(se); }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    return uptr<NSIterator<XES>>{new FxNSIteratorBoring{fIterator(se)}};
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":FxNSSeqBoring";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

// =============================================================
// this NSSeq uses "fancy" iterator... Generators and coroutines
// -------------------------------------------------------------

template <XESolution XES, uptr<Move<XES>> (*fRandom)(const XES&),
          Generator<Move<XES>*> (*fGenerator)(const XES&)>
class FxNSSeqFancy final : public NSSeq<XES> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

  // internal class for iterator
  class FxNSIterator final : public NSIterator<XES> {
   public:
    // flag to inform that iterator is 'done'
    bool done = {true};
    // flag that indicates that current was already given (unique_ptr semantics)
    bool consumedCurrent = {true};
    //
    // must initialize via move semantics
    Generator<Move<XES>*> gen;

    FxNSIterator(Generator<Move<XES>*>&& _gen) : gen(std::move(_gen)) {}

    virtual void first() {
      done = !gen.next();       // advance and update bool
      consumedCurrent = false;  // can allow consuming again
    }

    virtual void next() {
      done = !gen.next();       // advance and update bool
      consumedCurrent = false;  // can allow consuming again
    }

    virtual bool isDone() {
      return done;  // verify bool
    }

    virtual uptr<Move<XES>> current() {
      // should never repeat pointer (enforce 'unique' semantics!)
      if (consumedCurrent) {
        if (Component::verbose)
          std::cerr << "FXNSSEQ ELEMENT ALREADY CONSUMED!!" << std::endl;
        return nullptr;
      }
      consumedCurrent = true;
      return uptr<Move<XES>>(gen.getValue());
    }
  };

 public:
  uptr<Move<XES>> randomMove(const XES& se) override { return fRandom(se); }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    return uptr<NSIterator<XES>>{new FxNSIterator{std::move(fGenerator(se))}};
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":FxNSSeqFancy";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

// ======================================================

template <XESolution XES>
class FxNSSeq final : public NSSeq<XES> {
  using super = NSSeq<XES>;
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

  std::function<uptr<Move<XES>>(const XES&)> fRandom;
  // fGenerator: must respect 'unique' semantics! never repeat pointer.
  std::function<Generator<Move<XES>*>(const XES&)> fGenerator;

 public:
  FxNSSeq(std::function<uptr<Move<XES>>(const XES&)> _fRandom,
          std::function<Generator<Move<XES>*>(const XES&)> _fGenerator)
      : fRandom{_fRandom}, fGenerator{_fGenerator} {}

 private:
  // internal class for iterator
  class FxNSIterator final : public NSIterator<XES> {
   public:
    // flag to inform that iterator is 'done'
    bool done = {true};
    // flag that indicates that current was already given (unique_ptr semantics)
    bool consumedCurrent = {true};
    //
    Generator<Move<XES>*> gen;  // must initialize via move semantics

    FxNSIterator(Generator<Move<XES>*>&& _gen) : gen(std::move(_gen)) {}

    virtual void first() {
      done = !gen.next();       // advance and update bool
      consumedCurrent = false;  // can allow consuming again
    }

    virtual void next() {
      done = !gen.next();       // advance and update bool
      consumedCurrent = false;  // can allow consuming again
    }

    virtual bool isDone() {
      return done;  // verify bool
    }

    virtual uptr<Move<XES>> current() {
      // should never repeat pointer (enforce 'unique' semantics!)
      if (consumedCurrent) {
        if (Component::verbose)
          std::cerr << "FXNSSEQ ELEMENT ALREADY CONSUMED (2)!" << std::endl;
        return nullptr;
      }
      consumedCurrent = true;
      return uptr<Move<XES>>(gen.getValue());
    }
  };

 public:
  uptr<Move<XES>> randomMove(const XES& se) override { return fRandom(se); }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    return uptr<NSIterator<XES>>{new FxNSIterator{std::move(fGenerator(se))}};
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << super::idComponent() << ":FxNSSeq";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_FXNSSEQ_HPP_
