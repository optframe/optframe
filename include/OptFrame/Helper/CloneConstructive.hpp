// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HELPER_CLONECONSTRUCTIVE_HPP_
#define OPTFRAME_HELPER_CLONECONSTRUCTIVE_HPP_

#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>

namespace optframe {

template <XSolution S>
class CloneConstructive : public Constructive<S> {
  // sptr<S> base;
  S base;

 public:
  // explicit CloneConstructive(S _base) : base{_base} {}

  explicit CloneConstructive(const S& _base) : base{_base} {}

  // virtual ~CloneConstructive() {
  //  delete &base;
  //}

  std::optional<S> generateSolution(double timelimit) override {
    /*
      S& s = base->clone();
      std::optional<S> retS(s); // TODO: what happens here? can we move at
      least?? S sc = s; delete &s;
      //return new S(sc);
      return retS;
      */
    return std::optional<S>(base);
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Constructive<S>::compatible(s));
  }

  static std::string idComponent() {
    stringstream ss;
    ss << Constructive<S>::idComponent() << ":CloneConstructive";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class CloneConstructiveBuilder : public ComponentBuilder<XES> {
  using S = typename XES::first_type;

 public:
  virtual ~CloneConstructiveBuilder() {}

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            string family = "") override {
    std::cout << "CloneConstructive NOT AVAILABLE... TODO!" << std::endl;
    assert(false);
    return nullptr;
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    // assert(false);
    //
    // params.push_back(make_pair(S::idComponent(), "solution"));
    params.push_back(make_pair("NO_TYPE", "solution"));
    //

    return params;
  }

  bool canBuild(std::string component) override {
    return component == CloneConstructive<S>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << "CloneConstructive";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};
}  // namespace optframe

#endif  // OPTFRAME_HELPER_CLONECONSTRUCTIVE_HPP_
