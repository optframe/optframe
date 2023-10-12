// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_SA_HELPERSA_HPP_
#define OPTFRAME_HEURISTICS_SA_HELPERSA_HPP_

#include <string>

// Helper for Simulated Annealing

namespace optframe {

template <XESolution XES>
class HelperSA {
 public:
  using XEv = typename XES::second_type;
  using XSH = XES;  // XSearch

  // technique found in lecture notes of prof. Marcone Jamilson Freitas Souza
  static double estimateInitialTemperature(
      sref<GeneralEvaluator<XES>> evaluator, sref<InitialSearch<XES>> is,
      vsref<NS<XES, XSH>> neighbors, double beta, double gama, int SAmax,
      double T0, sref<RandGen> rg, bool verbose) {
    // begin method
    double T = T0;     // current temperature
    bool cont = true;  // continue flag
    std::optional<XES> ose = is->initialSearch({99999.9}).first;  // inf time
    assert(ose);
    XES& se1 = *ose;
    //
    while (cont) {
      int accept = 0;  // neighbors accepted in temperature T
      std::optional<XES> ose2 = is->initialSearch({99999.9}).first;  // inf time
      XES& se2 = *ose2;
      for (int IterT = 0; IterT < SAmax; IterT++) {
        XES se = se2;
        int n = rg->rand(neighbors.size());
        uptr<Move<XES, XSH>> move = neighbors[n]->validRandomMove(se);
        XES se_line(se);
        //
        move->applyUpdate(se_line);
        evaluator->reevaluate(se_line);
        //
        if (evaluator->betterStrict(se_line.second, se.second)) {
          accept++;
        } else {
          double x = rg->rand01();
          double delta =
              ::fabs(se_line.second.evaluation() - se.second.evaluation());
          if (x < ::exp(-delta / T)) accept++;
        }
      }  // end-for
      if (verbose) {
        std::cout << accept << " >= " << gama * SAmax << " ~ "
                  << (accept / (double)SAmax) << "?" << std::endl;
      }
      //
      if (accept >= gama * SAmax)
        cont = false;
      else
        T = beta * T;
    }  // end-while

    return T;  // return temperature
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_SA_HELPERSA_HPP_
