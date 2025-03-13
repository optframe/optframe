// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_NS_HPP_
#define OPTFRAME_NS_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <string>
#include <utility>
//

#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/Domain.hpp>
#include <OptFrame/Core/Move.hpp>
#include <OptFrame/Helper/MultiEvaluation.hpp>
// included for Neighborhood Exploration
#include <OptFrame/Core/GeneralEvaluator.hpp>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XESolution XES, XSearch<XES> XSH = XES>
#else
MOD_EXPORT template <typename XES, typename XSH = XES>
#endif
class NS : public Component {
  using XEv = typename XES::second_type;

 public:
  ~NS() override = default;

 public:
  // returns 'true' if moves generated from this neighborhood are
  // solution-independent this means that a move generated for solution s1 could
  // also be applied to solution s2 default is 'false' for safety precautions.
  // if this is 'false', one should be VERY careful on canBeApplied tests
  virtual bool isSolutionIndependent() const { return false; }

  virtual bool supportsMoveIndependence() const { return false; }

  virtual uptr<Move<XES, XSH>> randomMove(const XES&) = 0;

  // TODO(igormcoelho): rename to 'anyValidMove'
  // TODO(igormcoelho): should we require XES here? Or just S?
  virtual uptr<Move<XES, XSH>> validRandomMove(const XES& se) {
    uptr<Move<XES, XSH>> moveValid = this->randomMove(se);
    if (moveValid && moveValid->canBeApplied(se))
      return moveValid;
    else
      return nullptr;
  }

  // neighborhood id
  id_type nid() { return 0; }

 public:
  // =======================================
  // find section (neighborhood exploration)
  // =======================================
  // findAny: returns any move that strictly improves current solution 'se',
  // according 'gev' RETURNS: std::pair< uptr<Move<XES, XSH>>, op<XEv> > default
  // implementation tries method 'validRandomMove' for a *single time* (not
  // iterative) note that 'se' is not const, since moves may need to change it
  // (and revert)
  //   we could have "const_cast" here, or inside "moveCost", but at the moment
  //   let's fully respect "const"
  virtual std::pair<Move<XES, XSH>*, op<XEv>> findAny(
      GeneralEvaluator<XES, XSH>& gev, XES& se) {
    uptr<Move<XES, XSH>> pm = validRandomMove(se);
    if (!pm) return std::make_pair(nullptr, std::nullopt);
    Move<XES, XSH>& m = *pm;
    op<XEv> mvcost = gev.moveCost(m, se);
    // TODO: will we need 'non-strict' checks here
    if (!mvcost) return std::make_pair(nullptr, std::nullopt);
    if (gev.isStrictImprovement(*mvcost))
      return std::make_pair(pm.release(), mvcost);
    return std::make_pair(nullptr, std::nullopt);
  }

 public:
  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":NS"
       << Domain::getAlternativeDomain<XES>("<XESf64>");
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }
};

}  // namespace optframe

#endif  // OPTFRAME_NS_HPP_
