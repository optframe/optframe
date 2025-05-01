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

#ifndef PCAP_INITIALSOLUTION_InitalSolutionGreedy_HPP_
#define PCAP_INITIALSOLUTION_InitalSolutionGreedy_HPP_

#include <stdlib.h>

#include <algorithm>
#include <list>
//
#include <OptFrame/printable/printable.hpp>
using namespace optframe;
//

#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Search/InitialSearch.hpp>
// #include "../../OptFrame/Util/TestSolution.hpp"

#include <OptFrame/Core/RandGen.hpp>

#include "Evaluator.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

class PCAPInitialSolutionGreedy : public InitialSearch<ESolutionPCAP> {
 private:
  PCAPProblemInstance& pPCAP;
  GeneralEvaluator<ESolutionPCAP>& eval;
  RandGen& rg;

 public:
  static bool compara(pair<int, double> p1, pair<int, double> p2) {
    return p1.second < p2.second;
  }

  PCAPInitialSolutionGreedy(PCAPProblemInstance& _pPCAP,
                            GeneralEvaluator<ESolutionPCAP>& _eval,
                            RandGen& _rg)
      : pPCAP(_pPCAP), eval(_eval), rg(_rg) {}

  std::pair<std::optional<ESolutionPCAP>, SearchStatus> initialSearch(
      const StopCriteria<EvaluationPCAP>& stop) override {
    // double timelimit = stop.timelimit;
    RepPCAP newRep;

    // std::cout << "pPCAP.nCidades = " << pPCAP.nCidades << std::endl;
    vector<bool> medianasUtilizadas;
    for (int i = 0; i < pPCAP.nCidades; i++)
      medianasUtilizadas.push_back(false);

    for (int i = 0; i < pPCAP.nMedianas; i++) {
      int med = rg.rand(pPCAP.nCidades);
      while (medianasUtilizadas[med] == true) med = rg.rand(pPCAP.nCidades);
      newRep.first.push_back(med);
      medianasUtilizadas[med] = true;
    }

    vector<int> utilizacao;
    for (int i = 0; i < pPCAP.nMedianas; i++) utilizacao.push_back(0);

    for (int i = 0; i < pPCAP.nCidades; i++) newRep.second.push_back(-1);

    vector<bool> vertices;
    for (int i = 0; i < pPCAP.nCidades; i++) vertices.push_back(false);

    for (int i = 0; i < pPCAP.nMedianas; i++) {
      int cidade = newRep.first[i];
      newRep.second[cidade] = i;
      utilizacao[i] += pPCAP.vecCidades[cidade].demanda;
      vertices[cidade] = true;

      // std::cout<<"i = "<<i<<"  cidade "<<cidade<<" newRep.second[cidade] =
      // "<<newRep.second[cidade]<<endl;
    }

    vector<bool> verticesAlocados;
    for (int i = 0; i < pPCAP.nCidades; i++) verticesAlocados.push_back(false);
    for (int k = 0; k < pPCAP.nCidades; k++) {
      int i = rg.rand(pPCAP.nCidades);
      while (verticesAlocados[i] == true) i = rg.rand(pPCAP.nCidades);

      verticesAlocados[i] = true;

      double x = pPCAP.vecCidades[i].x;
      double y = pPCAP.vecCidades[i].y;
      int lowerCost = -1;

      if (vertices[i] == false) {
        double distMin = 100000000;

        for (int j = 0; j < pPCAP.nMedianas; j++) {
          int med = newRep.first[j];
          double x2 = pPCAP.vecCidades[med].x;
          double y2 = pPCAP.vecCidades[med].y;
          double dist = sqrt(pow((x - x2), 2) + pow((y - y2), 2));

          if ((dist < distMin) &&
              ((utilizacao[j] + pPCAP.vecCidades[i].demanda) <
               pPCAP.vecCidades[med].capacidade)) {
            distMin = dist;
            lowerCost = j;
          }
        }

        if (lowerCost != -1) {
          newRep.second[i] = lowerCost;
          utilizacao[lowerCost] += pPCAP.vecCidades[i].demanda;
        } else {
          newRep.second[i] = 0;
          utilizacao[0] += pPCAP.vecCidades[i].demanda;
        }
      }
    }

    SolutionPCAP s1(newRep);
    pair<SolutionPCAP, EvaluationPCAP> es(s1, EvaluationPCAP());
    eval.reevaluate(es);
    // ESolutionPCAP es = make_pair(s1, EvaluationPCAP);
    return make_pair(make_optional(es), SearchStatus::NO_REPORT);
  }
};

#endif /*PCAP_INITIALSOLUTION_InitalSolutionGreedy_HPP_*/
