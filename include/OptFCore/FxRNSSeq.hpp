// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FXRNSSEQ_HPP_
#define OPTFCORE_FXRNSSEQ_HPP_

#include <functional>
#include <string>
#include <utility>
//
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/RandGen.hpp>
//
#include "coro/Generator.hpp"  // this is a very special class!! coroutines support \o/

namespace optframe {

// =============================================================
// this RNSSeq uses "fancy" iterator... Generators and coroutines
// -------------------------------------------------------------

// XES: ESolution Type
// ContextType: Context / Memory Type
// fRGenerator:  must respect 'unique' semantics! never repeat pointer.
//
template <XESolution XES,
          typename ContextType,
          Generator<Move<XES>*> (*fRGenerator)(const XES&,
                                               ContextType& ctx,
                                               sref<RandGen> rg,
                                               bool singleMove)>
class FxRNSSeq final : public NSSeq<XES> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

  // random generator
  sref<RandGen> rg;
  //
  ContextType ctx;
  //
 public:
  FxRNSSeq(sref<RandGen> rg)
      : rg{rg} {
  }

 private:
  //
  // TODO(igormcoelho): context could live inside iterator,
  // so we can pass an optional context (but passed by reference??)
  // This way, randomMove will not mess with active iterators

  // internal class for iterator
  class FxRNSIterator final : public NSIterator<XES> {
   public:
    // flag to inform that iterator is 'done'
    bool done = {true};
    // flag that indicates that current was already given (unique_ptr semantics)
    bool consumedCurrent = {true};
    //
    sref<RandGen> rg;
    //
    Generator<Move<XES>*> gen;  // must initialize via move semantics

    FxRNSIterator(Generator<Move<XES>*>&& _gen, sref<RandGen> rg)
        : gen(std::move(_gen)), rg{rg} {
    }

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
          std::cerr << "FxRNSIterator Element ALREADY CONSUMED!!" << std::endl;
        return nullptr;
      }
      consumedCurrent = true;
      return uptr<Move<XES>>(gen.getValue());
    }
  };

 public:
  uptr<Move<XES>> randomMove(const XES& se) override {
    // creates iterator for single use ('true')
    uptr<NSIterator<XES>> iter{
        new FxRNSIterator{
            std::move(fRGenerator(se, ctx, rg, true)),
            rg}};
    iter->first();
    uptr<Move<XES>> mv = iter->current();
    return mv;
  }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    return uptr<NSIterator<XES>>{
        new FxRNSIterator{
            std::move(fRGenerator(se, ctx, rg, false)),
            rg}};
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":FxRNSSeqFancy";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif  // OPTFCORE_FXRNSSEQ_HPP_
