// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FEVALUATOR_HPP_
#define OPTFCORE_FEVALUATOR_HPP_

#include <functional>
#include <string>
#include <type_traits>
//
#include <OptFCore/FDirection.hpp>
#include <OptFrame/Core/Evaluator.hpp>

namespace optframe {

template <XESolution XES, MinOrMax Minimizing, typename ProblemType = void>
class FEvaluator final : public Evaluator<typename XES::first_type,
                                          typename XES::second_type, XES> {
  using super =
      Evaluator<typename XES::first_type, typename XES::second_type, XES>;
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

 public:
//
#ifdef OPTFCORE_FUNC_STATIC
  typedef XEv (*FuncTypeEvaluate)(sref<ProblemType>, const S&);
#else
  typedef std::function<XEv(sref<ProblemType>, const S&)> FuncTypeEvaluate;
#endif

  sref<ProblemType> p;
  FuncTypeEvaluate fEvaluate;

  explicit FEvaluator(sref<ProblemType> _p, FuncTypeEvaluate _fEvaluate)
      : super((Minimizing == MinOrMax::MINIMIZE)
                  ? sref<Direction<XEv>>{new Minimization<XEv>()}
                  : sref<Direction<XEv>>{new Maximization<XEv>()}),
        p{_p},
        fEvaluate{_fEvaluate} {}

  XEv evaluate(const S& s) override { return fEvaluate(p, s); }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (super::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << super::idComponent() << ":FEvaluator";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

// template specialization for no problem (=void)
template <XESolution XES, MinOrMax Minimizing>
class FEvaluator<XES, Minimizing, void> final
    : public Evaluator<typename XES::first_type, typename XES::second_type,
                       XES> {
  using super =
      Evaluator<typename XES::first_type, typename XES::second_type, XES>;
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

 public:
//
#ifdef OPTFCORE_FUNC_STATIC
  typedef XEv (*FuncTypeEvaluate)(const S&);
#else
  typedef std::function<XEv(const S&)> FuncTypeEvaluate;
#endif

  FuncTypeEvaluate fEvaluate;

  explicit FEvaluator(FuncTypeEvaluate _fEvaluate)
      : super((Minimizing == MinOrMax::MINIMIZE)
                  ? sref<Direction<XEv>>{new Minimization<XEv>()}
                  : sref<Direction<XEv>>{new Maximization<XEv>()}),
        fEvaluate{_fEvaluate} {}

  XEv evaluate(const S& s) override { return fEvaluate(s); }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (super::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << super::idComponent() << ":FEvaluator";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_FEVALUATOR_HPP_
