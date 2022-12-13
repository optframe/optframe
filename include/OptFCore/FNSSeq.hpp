// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FNSSEQ_HPP_
#define OPTFCORE_FNSSEQ_HPP_

#include <functional>
#include <string>
#include <utility>
//
#include <OptFrame/NSSeq.hpp>

namespace optframe {

// =============================================================
// this NSSeq uses "Boring" iterator... first, next, bla, bla...
// -------------------------------------------------------------

template <class IMS, XESolution XES, typename ProblemType = void>
class FNSSeq final : public NSSeq<XES> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

 public:
  //
  // function templates (exported)
  //
  // SHOULD NOT REQUIRE 'COPY' FROM IMS, IF NOT NECESSARY!
  using ims_type = IMS;

  sref<ProblemType> p;
#ifdef OPTFCORE_FUNC_STATIC
  typedef uptr<Move<XES>> (*FuncTypeNSRand)(sref<ProblemType>, const XES&);
  typedef IMS (*FuncTypeNSSeqItInit)(sref<ProblemType>, const XES&);
  typedef void (*FuncTypeNSSeqItFirst)(sref<ProblemType>, IMS&);
  typedef void (*FuncTypeNSSeqItNext)(sref<ProblemType>, IMS&);
  typedef bool (*FuncTypeNSSeqItIsDone)(sref<ProblemType>, IMS&);
  typedef uptr<Move<XES>> (*FuncTypeNSSeqItCurrent)(sref<ProblemType>, IMS&);
#else
  typedef std::function<uptr<Move<XES>>(sref<ProblemType>, const XES&)>
      FuncTypeNSRand;
  typedef std::function<IMS(sref<ProblemType>, const XES&)> FuncTypeNSSeqItInit;
  typedef std::function<void(sref<ProblemType>, IMS&)> FuncTypeNSSeqItFirst;
  typedef std::function<void(sref<ProblemType>, IMS&)> FuncTypeNSSeqItNext;
  typedef std::function<bool(sref<ProblemType>, IMS&)> FuncTypeNSSeqItIsDone;
  typedef std::function<uptr<Move<XES>>(sref<ProblemType>, IMS&)>
      FuncTypeNSSeqItCurrent;
#endif

  FuncTypeNSRand fRandom;
  FuncTypeNSSeqItInit fIterator;
  FuncTypeNSSeqItFirst fFirst;
  FuncTypeNSSeqItNext fNext;
  FuncTypeNSSeqItIsDone fIsDone;
  FuncTypeNSSeqItCurrent fCurrent;

  FNSSeq(sref<ProblemType> _p, FuncTypeNSRand _fRandom,
         FuncTypeNSSeqItInit _fIterator, FuncTypeNSSeqItFirst _fFirst,
         FuncTypeNSSeqItNext _fNext, FuncTypeNSSeqItIsDone _fIsDone,
         FuncTypeNSSeqItCurrent _fCurrent)
      : p{_p},
        fRandom{_fRandom},
        fIterator{_fIterator},
        fFirst{_fFirst},
        fNext{_fNext},
        fIsDone{_fIsDone},
        fCurrent{_fCurrent} {}

  // copy constructor
  FNSSeq(const FNSSeq& fnsseq)
      : p{fnsseq.p},
        fRandom{fnsseq.fRandom},
        fIterator{fnsseq.fIterator},
        fFirst{fnsseq.fFirst},
        fNext{fnsseq.fNext},
        fIsDone{fnsseq.fIsDone},
        fCurrent{fnsseq.fCurrent} {}

  virtual ~FNSSeq() = default;

 private:
  // internal class for iterator
  class FNSIterator final : public NSIterator<XES> {
   public:
    // SHOULD NOT REQUIRE COPY FROM IMS, IF NOT NECESSARY!
    // MOVE IS REQUIRED FROM IMS
    // TODO(igormcoelho): Should we require printability from IMS?
    //
    IMS ims;
    FNSSeq<IMS, XES, ProblemType>* nsseq;
    //
    FNSIterator(IMS&& _ims, FNSSeq<IMS, XES, ProblemType>* _nsseq) noexcept
        : ims{std::move(_ims)}, nsseq{_nsseq} {}

    virtual void first() { nsseq->fFirst(nsseq->p, ims); }

    virtual void next() { nsseq->fNext(nsseq->p, ims); }

    virtual bool isDone() { return nsseq->fIsDone(nsseq->p, ims); }

    virtual uptr<Move<XES>> current() { return nsseq->fCurrent(nsseq->p, ims); }
  };

 public:
  uptr<Move<XES>> randomMove(const XES& se) override { return fRandom(p, se); }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    return uptr<NSIterator<XES>>{new FNSIterator{fIterator(p, se), this}};
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << NSSeq<XES>::idComponent() << ":FNSSeq";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

// template specialization for no problem

template <class IMS, XESolution XES>
class FNSSeq<IMS, XES, void> final : public NSSeq<XES> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

 public:
  //
  // function templates (exported)
  //
  // SHOULD NOT REQUIRE 'COPY' FROM IMS, IF NOT NECESSARY!
  using ims_type = IMS;

#ifdef OPTFCORE_FUNC_STATIC
  typedef uptr<Move<XES>> (*FuncTypeNSRand)(const XES&);
  typedef IMS (*FuncTypeNSSeqItInit)(const XES&);
  typedef void (*FuncTypeNSSeqItFirst)(IMS&);
  typedef void (*FuncTypeNSSeqItNext)(IMS&);
  typedef bool (*FuncTypeNSSeqItIsDone)(IMS&);
  typedef uptr<Move<XES>> (*FuncTypeNSSeqItCurrent)(IMS&);
#else
  typedef std::function<uptr<Move<XES>>(const XES&)> FuncTypeNSRand;
  typedef std::function<IMS(const XES&)> FuncTypeNSSeqItInit;
  typedef std::function<void(IMS&)> FuncTypeNSSeqItFirst;
  typedef std::function<void(IMS&)> FuncTypeNSSeqItNext;
  typedef std::function<bool(IMS&)> FuncTypeNSSeqItIsDone;
  typedef std::function<uptr<Move<XES>>(IMS&)> FuncTypeNSSeqItCurrent;
#endif

  FuncTypeNSRand fRandom;
  FuncTypeNSSeqItInit fIterator;
  FuncTypeNSSeqItFirst fFirst;
  FuncTypeNSSeqItNext fNext;
  FuncTypeNSSeqItIsDone fIsDone;
  FuncTypeNSSeqItCurrent fCurrent;

  FNSSeq(FuncTypeNSRand _fRandom, FuncTypeNSSeqItInit _fIterator,
         FuncTypeNSSeqItFirst _fFirst, FuncTypeNSSeqItNext _fNext,
         FuncTypeNSSeqItIsDone _fIsDone, FuncTypeNSSeqItCurrent _fCurrent)
      : fRandom{_fRandom},
        fIterator{_fIterator},
        fFirst{_fFirst},
        fNext{_fNext},
        fIsDone{_fIsDone},
        fCurrent{_fCurrent} {}

  // copy constructor
  FNSSeq(const FNSSeq& fnsseq)
      : fRandom{fnsseq.fRandom},
        fIterator{fnsseq.fIterator},
        fFirst{fnsseq.fFirst},
        fNext{fnsseq.fNext},
        fIsDone{fnsseq.fIsDone},
        fCurrent{fnsseq.fCurrent} {}

  virtual ~FNSSeq() = default;

 private:
  // internal class for iterator
  class FNSIterator final : public NSIterator<XES> {
   public:
    // SHOULD NOT REQUIRE COPY FROM IMS, IF NOT NECESSARY!
    // MOVE IS REQUIRED FROM IMS
    // TODO(igormcoelho): Should we require printability from IMS?
    //
    IMS ims;
    FNSSeq<IMS, XES>* nsseq;
    //
    FNSIterator(IMS&& _ims, FNSSeq<IMS, XES>* _nsseq) noexcept
        : ims{std::move(_ims)}, nsseq{_nsseq} {}

    virtual void first() { nsseq->fFirst(ims); }

    virtual void next() { nsseq->fNext(ims); }

    virtual bool isDone() { return nsseq->fIsDone(ims); }

    virtual uptr<Move<XES>> current() { return nsseq->fCurrent(ims); }
  };

 public:
  uptr<Move<XES>> randomMove(const XES& se) override { return fRandom(se); }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    return uptr<NSIterator<XES>>{new FNSIterator{fIterator(se), this}};
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << NSSeq<XES>::idComponent() << ":FNSSeq";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_FNSSEQ_HPP_
