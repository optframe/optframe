// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_TIMER_BUILDER_HPP_
#define OPTFRAME_TIMER_BUILDER_HPP_

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class TimerBuilder : public ComponentBuilder<XES> {
 public:
  virtual ~TimerBuilder() {}

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            string family = "") override {
    return new Timer;
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    return params;
  }

  bool canBuild(std::string component) override {
    return component == Timer::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << "Timer";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif