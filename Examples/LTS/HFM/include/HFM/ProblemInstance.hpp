#ifndef HFM_PROBLEMINSTANCE_HPP_
#define HFM_PROBLEMINSTANCE_HPP_

#include <iostream>
//

#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/KahanSummation.hpp>

using namespace std;
using namespace scannerpp;
using namespace optframe;

namespace HFM {

class HFMProblemInstance {
 private:
  vector<vector<double> > forecastings;

  vector<double> means;
  vector<double> vMax;
  vector<double> stdDesvs;

 public:
  HFMProblemInstance(const vector<vector<double> >& _forecastings)
      : forecastings(_forecastings) {
    // number of different explanatory variables
    int nExVar = forecastings.size();
    means.resize(nExVar, 0);
    stdDesvs.resize(nExVar, 0);
    vMax.resize(nExVar, 0);

    for (int exVar = 0; exVar < nExVar; exVar++) {
      pair<double, double> avgStd =
          KahanSummation::calculateAvgStd(forecastings[exVar]);
      means[exVar] = avgStd.first;
      stdDesvs[exVar] = avgStd.second;
      vMax[exVar] =
          *max_element(forecastings[exVar].begin(), forecastings[exVar].end());
    }

    // cout <<"File means:"<< means << std::endl;
    // cout <<"File std:"<< stdDesvs << std::endl;
    // getchar();

    // ==================== Check if #stepsAhead are possible to be done

    /*
     std::stringstream ss;
     ss << problemParam.getValidationPath();
     File* file;

     try
     {
     file = new File(ss.str());
     } catch (FileNotFound& f)
     {
     std::cout << "File '" << ss.str() << "' not found" << std::endl;
     }

     Scanner scannerBlind(file);

     int nForecastingsBlind = scannerBlind.nextInt();
     std::cout << "nBlind: " << nForecastingsBlind << std::endl;
     */

    //================================================
  }

  double getMean(int file) { return means[file]; }

  double getMax(int file) { return vMax[file]; }

  vector<double> getMeans() { return means; }

  vector<double> getStdDesvs() { return stdDesvs; }

  double getStdDesv(int file) { return stdDesvs[file]; }

  int getForecatingsSizeFile(int file) { return forecastings[file].size(); }

  int getNumberExplanatoryVariables() { return forecastings.size(); }

  inline vector<vector<double> >& getForecastingsVector() {
    return forecastings;
  }

  inline double getForecastings(int file, int i) {
    return forecastings[file][i];
  }

  virtual ~HFMProblemInstance() {}
};

}  // namespace HFM

#endif /*HFM_PROBLEMINSTANCE_HPP_*/
