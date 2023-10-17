// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_CLONE_CONSTRUCTIVE_HPP_
#define OPTFRAME_CLONE_CONSTRUCTIVE_HPP_

#include <OptFrame/Constructive.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>

namespace optframe {

template <XSolution S>
class CloneConstructive : public Constructive<S> {
  // sptr<S> base;
  S base;

 public:
  CloneConstructive(S _base)  //(sptr<S> _base)
      : base{_base}           // base(&_base->clone())
  {}

  CloneConstructive(const S& _base)
      : base{_base}  //(&_base.clone())
  {}

  virtual ~CloneConstructive() {
    // delete &base;
  }

  virtual std::optional<S> generateSolution(double timelimit) override {
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
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
#else
template <typename S, typename XEv = Evaluation<>, typename XES = pair<S, XEv>,
          typename X2ES = MultiESolution<XES>>
#endif
class CloneConstructiveBuilder : public ComponentBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~CloneConstructiveBuilder() {}

  Component* buildComponent(Scanner& scanner,
                            HeuristicFactory<S, XEv, XES, X2ES>& hf,
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
    ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent()
       << "CloneConstructive";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};
}  // namespace optframe

#endif /*OPTFRAME_CLONE_CONSTRUCTIVE_HPP_*/
