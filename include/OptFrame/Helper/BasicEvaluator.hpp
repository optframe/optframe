#pragma once

namespace optframe {

template <XRepresentation R, XEvaluation XEv = Evaluation<>>
class BasicEvaluator : public Evaluator<RSolution<R>, XEv> {
 public:
  // only representation
  virtual XEv evaluate(const R&) = 0;

 private:
  /* Use this if don't need ADS */
  /* Use isto se você não precisa do ADS */

  using ADS = OPTFRAME_DEFAULT_ADS;

  XEv evaluate(const RSolution<R>& s) override { return evaluate(s.getR()); }
};

}  // namespace optframe