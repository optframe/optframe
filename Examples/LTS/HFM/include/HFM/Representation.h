#ifndef HFM_REPRESENTATION_H_
#define HFM_REPRESENTATION_H_

// C
#include <assert.h>
// C++
#include <vector>
//
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

const int GREATER = 0;           // A
const int GREATER_WEIGHT = 1;    // V
const int LOWER = 2;             // B
const int LOWER_WEIGHT = 3;      // W
const int EPSILON = 4;           // E
const int PERTINENCEFUNC = 5;    // Pertinence function index
const int NCOLUMNATRIBUTES = 6;  // Solution Atributes

const int NAJUSTS = 6;  // FOR THE APROX 2

using namespace std;

enum InputTypes {
  Single_Input,
  Average_Inputs,
  Derivative_Inputs,
  N_Inputs_Types
};

// Solution Representation

enum ActivationFunction { Heaviside, Trapezoid, N_Activation_Functions };

struct RepHFM {
  // single index [ (file,K - x) ,...] ex:  [ (file = 0,K - 10),(file = 0,K -
  // 2), ...]
  std::vector<std::pair<int, int>> singleIndex;
  vector<vector<double>> singleFuzzyRS;  // single inputs relationships

  vector<vector<pair<int, int>>> averageIndex;
  vector<vector<double>> averageFuzzyRS;

  vector<vector<pair<int, int>>> derivativeIndex;
  vector<vector<double>> derivativeFuzzyRS;

  int earliestInput;

  double alpha;

  vector<double> vAlpha;
  vector<int> vIndex;
  vector<double> vIndexAlphas;
  /*
        RepEFP(const RepEFP& rep)
        {
        singleIndex = rep.singleIndex;
        averageIndex = rep.averageIndex;
        derivativeIndex = rep.derivativeIndex;
        singleFuzzyRS = rep.singleFuzzyRS;
        averageFuzzyRS = rep.averageFuzzyRS;
        derivativeFuzzyRS = rep.derivativeFuzzyRS;
        }

        RepEFP()
        {
        }
        */
};

//#include <ostream>

ostream& operator<<(ostream& s, const RepHFM rep) {
  assert(rep.singleIndex.size() == rep.singleFuzzyRS.size());
  s << "NRules:" << rep.singleIndex.size() << "/" << rep.averageIndex.size()
    << "/" << rep.derivativeIndex.size() << std::endl;

  /*
       s << " ======================== \n Solution printing \n" << std::endl;
       s << "Single Inputs" << std::endl;
       vector<int> nFuzzyFunction(N_Activation_Functions, 0);
       double counter = 0;

       for (int i = 0; i < (int) rep.singleIndex.size(); i++)
       {
               s << "(" << rep.singleIndex[i].first << "," <<
     rep.singleIndex[i].second << ")" << std::endl; s << "\t (" <<
     rep.singleFuzzyRS[i][GREATER] << "->" <<
     rep.singleFuzzyRS[i][GREATER_WEIGHT] << ")"; s << "\t (" <<
     rep.singleFuzzyRS[i][LOWER] << "->" << rep.singleFuzzyRS[i][LOWER_WEIGHT]
     << ")"; s << "\t Epsilon:" << rep.singleFuzzyRS[i][EPSILON]; s << "\t
     FuzzyFunction:" << rep.singleFuzzyRS[i][PERTINENCEFUNC] << std::endl;

               if (rep.singleFuzzyRS[i][PERTINENCEFUNC] == Heaviside)
                       nFuzzyFunction[Heaviside]++;
               if (rep.singleFuzzyRS[i][PERTINENCEFUNC] == Trapezoid)
                       nFuzzyFunction[Trapezoid]++;
               counter++;
       }

       s << "Averaged Inputs" << std::endl;
       for (int i = 0; i < (int) rep.averageIndex.size(); i++)
       {
               s << "([" << rep.averageIndex[i][0].first << "," <<
     rep.averageIndex[i][0].second << "]"; for (int j = 1; j < (int)
     rep.averageIndex[i].size(); j++)
               {
                       s << "\t [" << rep.averageIndex[i][j].first << "," <<
     rep.averageIndex[i][j].second << "]";
               }
               s << ")" << std::endl;
               s << "\t (" << rep.averageFuzzyRS[i][GREATER] << "->" <<
     rep.averageFuzzyRS[i][GREATER_WEIGHT] << ")"; s << "\t (" <<
     rep.averageFuzzyRS[i][LOWER] << "->" << rep.averageFuzzyRS[i][LOWER_WEIGHT]
     << ")"; s << "\t Epsilon:" << rep.averageFuzzyRS[i][EPSILON]; s << "\t
     FuzzyFunction:" << rep.averageFuzzyRS[i][PERTINENCEFUNC] << std::endl;

               if (rep.averageFuzzyRS[i][PERTINENCEFUNC] == Heaviside)
                       nFuzzyFunction[Heaviside]++;
               if (rep.averageFuzzyRS[i][PERTINENCEFUNC] == Trapezoid)
                       nFuzzyFunction[Trapezoid]++;
               counter++;
       }

       s << "Derivative Inputs" << std::endl;
       for (int i = 0; i < (int) rep.derivativeIndex.size(); i++)
       {
               s << "([" << rep.derivativeIndex[i][0].first << "," <<
     rep.derivativeIndex[i][0].second << "]"; for (int j = 1; j < (int)
     rep.derivativeIndex[i].size(); j++)
               {
                       s << "\t [" << rep.derivativeIndex[i][j].first << "," <<
     rep.derivativeIndex[i][j].second << "]";
               }
               s << ")" << std::endl;
               s << "\t (" << rep.derivativeFuzzyRS[i][GREATER] << "->" <<
     rep.derivativeFuzzyRS[i][GREATER_WEIGHT] << ")"; s << "\t (" <<
     rep.derivativeFuzzyRS[i][LOWER] << "->" <<
     rep.derivativeFuzzyRS[i][LOWER_WEIGHT] << ")"; s << "\t Epsilon:" <<
     rep.derivativeFuzzyRS[i][EPSILON]; s << "\t FuzzyFunction:" <<
     rep.derivativeFuzzyRS[i][PERTINENCEFUNC] << std::endl;

               if (rep.derivativeFuzzyRS[i][PERTINENCEFUNC] == Heaviside)
                       nFuzzyFunction[Heaviside]++;
               if (rep.derivativeFuzzyRS[i][PERTINENCEFUNC] == Trapezoid)
                       nFuzzyFunction[Trapezoid]++;
               counter++;
       }

       //s << "earliestInput: " << rep.earliestInput << std::endl;
       s << "counter: " << counter << std::endl;
       s << "Heaviside functions on rules: " << nFuzzyFunction[Heaviside] /
     counter * 100 << std::endl; s << "Trapezoid functions on rules: " <<
     nFuzzyFunction[Trapezoid] / counter * 100 << std::endl;

       if ((nFuzzyFunction[Heaviside] + nFuzzyFunction[Trapezoid]) != counter)
       {
               s << "BUGOU!" << std::endl;
       }

       s << " Solution printed \n ======================== \n";
*/
  return s;
}

/*
 ostream & operator<<(ostream & s, RepEFP rep)
 {
 std::cout << " ======================== \n Solution printing \n" << std::endl;
 std::cout << "Single Inputs" << std::endl;
 for (int i = 0; i < rep.singleIndex.size(); i++)
 {
 std::cout << "(" << rep.singleIndex[i].first << "," << rep.singleIndex[i].second <<
 ")" << std::endl; std::cout << "\t (" << rep.singleFuzzyRS[GREATER][i] << "->" <<
 rep.singleFuzzyRS[GREATER_WEIGHT][i] << ")"; std::cout << "\t (" <<
 rep.singleFuzzyRS[LOWER][i] << "->" << rep.singleFuzzyRS[LOWER_WEIGHT][i] <<
 ")" << std::endl;
 }

 std::cout << "Averaged Inputs" << std::endl;
 for (int i = 0; i < rep.averageIndex.size(); i++)
 {
 std::cout << "([" << rep.averageIndex[i][0].first << "," <<
 rep.averageIndex[i][0].second << "]"; for (int j = 1; j <
 rep.averageIndex[i].size(); j++)
 {
 std::cout << "\t [" << rep.averageIndex[i][j].first << "," <<
 rep.averageIndex[i][j].second << "]";
 }
 std::cout << ")" << std::endl;
 std::cout << "\t (" << rep.averageFuzzyRS[GREATER][i] << "->" <<
 rep.averageFuzzyRS[GREATER_WEIGHT][i] << ")"; std::cout << "\t (" <<
 rep.averageFuzzyRS[LOWER][i] << "->" << rep.averageFuzzyRS[LOWER_WEIGHT][i] <<
 ")" << std::endl;
 }

 std::cout << "Derivative Inputs" << std::endl;
 for (int i = 0; i < rep.derivativeIndex.size(); i++)
 {
 std::cout << "([" << rep.derivativeIndex[i][0].first << "," <<
 rep.derivativeIndex[i][0].second << "]"; for (int j = 1; j <
 rep.derivativeIndex[i].size(); j++)
 {
 std::cout << "\t [" << rep.derivativeIndex[i][j].first << "," <<
 rep.derivativeIndex[i][j].second << "]";
 }
 std::cout << ")" << std::endl;
 std::cout << "\t (" << rep.derivativeFuzzyRS[GREATER][i] << "->" <<
 rep.derivativeFuzzyRS[GREATER_WEIGHT][i] << ")"; std::cout << "\t (" <<
 rep.derivativeFuzzyRS[LOWER][i] << "->" <<
 rep.derivativeFuzzyRS[LOWER_WEIGHT][i] << ")" << std::endl;
 }
 std::cout << " Solution printed \n ======================== \n";

 }
 */

// typedef Solution<RepHFM> SolutionHFM;

#endif /*HFM_REPRESENTATION_H_*/
