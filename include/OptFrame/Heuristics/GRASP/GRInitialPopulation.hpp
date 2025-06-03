#pragma once

namespace optframe {

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
template <XSolution S, X2Solution<S> X2S = VPopulation<S>>
#else
template <typename S, typename X2S = VPopulation<S>>
#endif
class GRInitialPopulation : public InitialPopulation<S, X2S> {
 public:
  GRConstructive<S>& constructive;
  RandGen& rg;
  double maxAlpha;  // limit the solution to be not so random

  GRInitialPopulation(GRConstructive<S>& _constructive, RandGen& _rg,
                      double _maxAlpha)
      : constructive(_constructive), rg(_rg), maxAlpha(_maxAlpha) {}

  virtual ~GRInitialPopulation() = default;

  // Population<XES>
  X2S generatePopulation(unsigned populationSize, double timelimit) override {
    // Population<XES> pop;
    X2S pop;
    for (unsigned i = 0; i < populationSize; i++) {
      float alpha = rg.rand01();
      while (alpha > maxAlpha) {
        alpha = rg.rand01();
      }

      if (alpha == 0) alpha = 0.00001;

      std::optional<S> s = constructive.generateGRSolution(alpha, timelimit);
      // XES se = { *s, Evaluation<>{} };
      S sol = (*s);
      pop.push_back(sol);  // the end of solution
    }
    return pop;
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << InitialPopulation<S, X2S>::idComponent() << ":GRInitialPopulation";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe