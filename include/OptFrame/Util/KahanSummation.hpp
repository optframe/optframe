#ifndef OPTFRAME_KAHANACCUMULATION_HPP_
#define OPTFRAME_KAHANACCUMULATION_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <numeric>  //accumlate
#include <vector>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

MOD_EXPORT struct KahanAccumulation {
  double sum;
  double correction;
};

MOD_EXPORT class KahanSummation {
 public:
  static KahanAccumulation KahanSum(KahanAccumulation accumulation,
                                    double value) {
    KahanAccumulation result;
    double y = value - accumulation.correction;
    double t = accumulation.sum + y;
    result.correction = (t - accumulation.sum) - y;
    result.sum = t;
    return result;
  }

  // return pair of avgStdDev
  static std::pair<double, double> calculateAvgStd(
      const std::vector<double> vTimeSamples) {
    int nSamples = vTimeSamples.size();
    KahanAccumulation init = {0};
    KahanAccumulation kahanSum = std::accumulate(
        vTimeSamples.begin(), vTimeSamples.end(), init, KahanSum);
    double avg = kahanSum.sum / nSamples;

    double stdev = 0.0;

    for_each(vTimeSamples.begin(), vTimeSamples.end(),
             [&](const double d) { stdev += (d - avg) * (d - avg); });

    if ((nSamples - 1) == 0)
      stdev = 0;
    else
      stdev = std::sqrt(stdev / (nSamples - 1));

    return std::make_pair(avg, stdev);
  }
};

}  // namespace optframe

#endif /* OPTFRAME_KAHANACCUMULATION_HPP_ */
