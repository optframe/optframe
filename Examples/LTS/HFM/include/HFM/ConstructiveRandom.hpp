#ifndef EFP_CONTRUCTIVE_RANDOM_HPP_
#define EFP_CONTRUCTIVE_RANDOM_HPP_

// C
#include <stdlib.h>
// C++
#include <algorithm>
#include <list>
#include <set>
//
#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Heuristics/GRASP/GRConstructive.hpp>
#include <OptFrame/Search/InitialSearch.hpp>

#include "Evaluator.hpp"
#include "ProblemInstance.hpp"
#include "Representation.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

namespace HFM {

// class ConstructiveRandom: public Constructive<SolutionHFM>
//  should decide if single or multi obj
template <XESolution XXES, XEvaluation XXEv>
// class ConstructiveRandom: public InitialSearch<ESolutionHFM>
class ConstructiveRandom : public InitialSearch<XXES> {
 private:
  HFMProblemInstance& pEFP;
  ProblemParameters problemParam;
  RandGen& rg;

  int precisionSP;
  int precisionMP;
  int precisionDP;
  int precision;
  // Your private vars

 public:
  ConstructiveRandom(HFMProblemInstance& _pEFP,
                     ProblemParameters& _problemParam, RandGen& _rg,
                     int _precision)
      :  // If necessary, add more parameters
        pEFP(_pEFP),
        problemParam(_problemParam),
        rg(_rg),
        precision(_precision) {
    if (precision == 0) {
      cerr << "Precision should be greater than 0 :" << precision << std::endl;
      getchar();
    }

    precisionSP = precision;
    precisionMP = precision;
    precisionDP = precision;
    precisionMP = 1;  // forcing to not have average rules
    precisionDP = 1;  // forcing to do not have derivative rules
  }

  virtual ~ConstructiveRandom() {}

  // std::optional<SolutionHFM> generateSolution(double timelimit) override
  // std::optional<ESolutionHFM> initialSearch(const
  // StopCriteria<EvaluationHFM>& sosc) override
  std::pair<std::optional<XXES>, SearchStatus> initialSearch(
      const StopCriteria<XXEv>& sosc) override {
    std::optional<SolutionHFM> s = generateSolutionAlpha(0.0, sosc.timelimit);
    // return make_pair(*s, EvaluationHFM(0)); // TODO: fix
    return make_pair(std::make_pair(*s, XXEv()), SearchStatus::NO_REPORT);
  }

  std::optional<SolutionHFM> generateSolutionAlpha(float notUsed,
                                                   double timelimit) {
    // std::cout << "ACF generating solution.." << std::endl;

    vector<vector<double> > data = pEFP.getForecastingsVector();

    int numberExplanatoryVariables = data.size();

    int earliestInput = 0;
    std::vector<std::pair<int, int> > singleIndex;
    vector<vector<double> > singleFuzzyRS;
    vector<vector<pair<int, int> > > averageIndex;
    vector<vector<double> > averageFuzzyRS;
    vector<vector<pair<int, int> > > derivativeIndex;
    vector<vector<double> > derivativeFuzzyRS;

    for (int nEXV = 0; nEXV < numberExplanatoryVariables; nEXV++) {
      int mean = pEFP.getMean(nEXV);
      int stdDesv = pEFP.getStdDesv(nEXV);
      double meanWeight = pEFP.getMean(0);  // File 0 is the target file
      double stdDesvWeight = pEFP.getStdDesv(0);

      int maxLag = problemParam.getMaxLag(nEXV);
      int maxUpperLag = problemParam.getMaxUpperLag(nEXV);

      int pSP = rg.rand(precisionSP);
      for (int p = 0; p < pSP; p++) {
        int K;
        if (maxUpperLag > 0) {
          int sign = rg.rand(2);
          if (sign == 0)
            K = rg.rand(maxLag) + 1;
          else
            K = (rg.rand(maxUpperLag) + 1) * -1;
        } else
          K = rg.rand(maxLag) + 1;

        singleIndex.push_back(std::make_pair(nEXV, K));

        if (K > earliestInput) earliestInput = K;

        double greater = rg.randG(mean, stdDesv);
        double lower = rg.randG(mean, stdDesv);
        double greaterWeight = rg.randG(meanWeight, stdDesvWeight);
        double lowerWeight = rg.randG(meanWeight, stdDesvWeight);

        int fuzzyFunction = rg.rand(N_Activation_Functions);

        vector<double> fuzzyRules;
        fuzzyRules.resize(NCOLUMNATRIBUTES);
        fuzzyRules[GREATER] = greater;
        fuzzyRules[GREATER_WEIGHT] = greaterWeight;
        fuzzyRules[LOWER] = lower;
        fuzzyRules[LOWER_WEIGHT] = lowerWeight;
        fuzzyRules[EPSILON] = 1;                     // TODO TEST FOR TRAPEZOID
        fuzzyRules[PERTINENCEFUNC] = fuzzyFunction;  // PERTINENCE FUNCTION

        singleFuzzyRS.push_back(fuzzyRules);
      }

      int pMP = rg.rand(precisionMP);
      for (int p = 0; p < pMP; p++) {
        int nAveragePoints = rg.rand(5) + 2;
        std::vector<std::pair<int, int> > aInputs;
        for (int aI = 0; aI < nAveragePoints; aI++) {
          int K;
          if (maxUpperLag > 0) {
            int sign = rg.rand(2);
            if (sign == 0)
              K = rg.rand(maxLag) + 1;
            else
              K = (rg.rand(maxUpperLag) + 1) * -1;
          } else
            K = rg.rand(maxLag) + 1;

          aInputs.push_back(std::make_pair(nEXV, K));
          if (K > earliestInput) earliestInput = K;
        }
        averageIndex.push_back(aInputs);

        double greater = rg.randG(mean, stdDesv);
        double lower = rg.randG(mean, stdDesv);
        double greaterWeight = rg.randG(meanWeight, stdDesvWeight);
        double lowerWeight = rg.randG(meanWeight, stdDesvWeight);

        int fuzzyFunction = rg.rand(N_Activation_Functions);

        vector<double> fuzzyRules;
        fuzzyRules.resize(NCOLUMNATRIBUTES);
        fuzzyRules[GREATER] = greater;
        fuzzyRules[GREATER_WEIGHT] = greaterWeight;
        fuzzyRules[LOWER] = lower;
        fuzzyRules[LOWER_WEIGHT] = lowerWeight;
        fuzzyRules[EPSILON] = 1;                     // TODO TEST FOR TRAPEZOID
        fuzzyRules[PERTINENCEFUNC] = fuzzyFunction;  // PERTINENCE FUNCTION

        averageFuzzyRS.push_back(fuzzyRules);
      }

      // std::cout << pMP << std::endl;
      // std::cout << averageFuzzyRS << std::endl;
      // std::cout << averageIndex << std::endl;
      // getchar();

      int pDP = rg.rand(precisionDP);
      for (int p = 0; p < pDP; p++) {
        int nDerivativePoints = 2;
        std::vector<std::pair<int, int> > aInputs;
        for (int aI = 0; aI < nDerivativePoints; aI++) {
          int K;
          if (maxUpperLag > 0) {
            int sign = rg.rand(2);
            if (sign == 0)
              K = rg.rand(maxLag) + 1;
            else
              K = (rg.rand(maxUpperLag) + 1) * -1;
          } else
            K = rg.rand(maxLag) + 1;

          aInputs.push_back(std::make_pair(nEXV, K));
          if (K > earliestInput) earliestInput = K;
        }
        derivativeIndex.push_back(aInputs);

        double greater = rg.randG(mean, stdDesv);
        double lower = rg.randG(mean, stdDesv);
        double greaterWeight = rg.randG(meanWeight, stdDesvWeight);
        double lowerWeight = rg.randG(meanWeight, stdDesvWeight);

        int fuzzyFunction = rg.rand(N_Activation_Functions);

        vector<double> fuzzyRules;
        fuzzyRules.resize(NCOLUMNATRIBUTES);
        fuzzyRules[GREATER] = greater;
        fuzzyRules[GREATER_WEIGHT] = greaterWeight;
        fuzzyRules[LOWER] = lower;
        fuzzyRules[LOWER_WEIGHT] = lowerWeight;
        fuzzyRules[EPSILON] = 1;                     // TODO TEST FOR TRAPEZOID
        fuzzyRules[PERTINENCEFUNC] = fuzzyFunction;  // PERTINENCE FUNCTION

        derivativeFuzzyRS.push_back(fuzzyRules);
      }

      // std::cout << pDP << std::endl;
      // std::cout << derivativeFuzzyRS << std::endl;
      // std::cout << derivativeIndex << std::endl;
      // getchar();
    }

    RepHFM newRep;
    newRep.singleIndex = singleIndex;
    newRep.singleFuzzyRS = singleFuzzyRS;
    newRep.averageIndex = averageIndex;
    newRep.averageFuzzyRS = averageFuzzyRS;
    newRep.derivativeIndex = derivativeIndex;
    newRep.derivativeFuzzyRS = derivativeFuzzyRS;
    // newRep.earliestInput = earliestInput;

    vector<double> vAlpha(NAJUSTS, rg.randG(0, 0.1));
    newRep.alpha = rg.rand(100) / 10000.0;
    newRep.vAlpha = vAlpha;

    vector<int> vIndex;
    vector<double> vIndexAlphas;

    int nAdjust = rg.rand(1) + 1;
    for (int nA = 0; nA <= nAdjust; nA++) {
      int K = rg.rand(problemParam.getMaxLag(problemParam.getTargetFile()));

      vIndex.push_back(K);
      //			double std = rg.rand(300) / 1000; //TODO check
      // the use of this variable
      vIndexAlphas.push_back(rg.randG(0, 0.1));
    }

    newRep.vIndex = vIndex;
    newRep.vIndexAlphas = vIndexAlphas;

    /*		cout << newRep.earliestInput << std::endl;
     std::cout << newRep.singleIndex << std::endl;
     std::cout << newRep.singleFuzzyRS << std::endl;
     std::cout << newRep.averageIndex << std::endl;
     std::cout << newRep.averageFuzzyRS << std::endl;
     std::cout << newRep.derivativeIndex << std::endl;
     std::cout << newRep.derivativeFuzzyRS << std::endl;
     getchar();*/

    //		cout << "End of Random Hybrid Fuzzy Model Sol generation!" <<
    // std::endl; 		getchar();
    return make_optional(SolutionHFM(newRep));
  }
};

}  // namespace HFM

#endif /*EFP_CONTRUCTIVE_RANDOM_HPP_*/
