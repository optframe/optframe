// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FMULTIEVALUATOR_HPP_
#define OPTFCORE_FMULTIEVALUATOR_HPP_

// DISABLED, for now! We can simply use MultiEvaluation
// with multiple FEvaluator

#ifdef 0

#include <functional>
#include <string>
//
#include <OptFrame/Core/MultiEvaluator.hpp>

namespace optframe {

enum MinOrMax { MINIMIZE, MAXIMIZE };

template <XESolution XMES>
class FMultiEvaluator final
    : public MultiEvaluator<typename XMES::first_type,
                            typename XMES::second_type, XMES> {
  using super = MultiEvaluator<typename XMES::first_type,
                               typename XMES::second_type, XMES>;
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  using XMSH = XMES;  // for multi objective

 public:
#ifdef OPTFCORE_FUNC_STATIC
  typedef XEv (*FuncTypeEvaluate)(const S&);
#else
  typedef std::function<XEv(const S&)> FuncTypeEvaluate;
#endif

  FuncTypeEvaluate fEvaluate;

  FEvaluator(FuncTypeEvaluate _fEvaluate)
      : super((Minimizing == MinOrMax::MINIMIZE)
                  ? sref<Direction<XEv>>{new Minimization<XEv>()}
                  : sref<Direction<XEv>>{new Maximization<XEv>()}),
        fEvaluate{_fEvaluate} {}

  virtual XEv evaluate(const S& s) { return fEvaluate(s); }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (super::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << super::idComponent() << ":FMultiEvaluator";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // DISABLED

#endif  // OPTFCORE_FMULTIEVALUATOR_HPP_
