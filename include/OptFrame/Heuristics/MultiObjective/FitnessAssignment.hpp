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

#ifndef OPTFRAME_HEURISTICS_MULTIOBJECTIVE_FITNESSASSIGNMENT_HPP_
#define OPTFRAME_HEURISTICS_MULTIOBJECTIVE_FITNESSASSIGNMENT_HPP_

// C++
#include <algorithm>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Heuristics/MultiObjective/MOSIndividual.hpp>

namespace optframe {

// template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS =
// OPTFRAME_DEFAULT_DS>
template <XESolution XMES2>
class FitnessAssignment : public Component {
 public:
  FitnessAssignment() {}

  ~FitnessAssignment() override = default;

  // assign fitness to individual 's' according to population 'P'
  virtual void assignFitnessIndividual(
      // MOSIndividual<XMES2>& s,
      int id_s, vector<MOSIndividual<XMES2>>& P) {
    // vector<MOSIndividual<XMES2>> v;
    vector<int> v_id;
    v_id.push_back(id_s);
    assignFitnessGroup(v_id, P);
  }

  // assign fitness to all individuals from population 'P'
  virtual void assignFitnessAll(std::vector<MOSIndividual<XMES2>>& P) {
    // vector<MOSIndividual<XMES2>> Pconst(P.begin(), P.end());
    vector<int> v_id;
    for (unsigned i = 0; i < P.size(); i++) v_id.push_back(i);
    assignFitnessGroup(v_id, P);
  }

  // assign fitness to group of individuals 'g' according to population 'P'
  virtual void assignFitnessGroup(
      // vector<MOSIndividual<XMES2>>& g,
      const vector<int>& g, vector<MOSIndividual<XMES2>>& P) = 0;

  void print() const override { std::cout << "FitnessAssignment" << std::endl; }

  string id() const override { return "FitnessAssignment"; }
};

template <XEvaluation XMEv = MultiEvaluation<double>>
struct FitnessIndividual {
  int idx;
  double fitness;
  // could be non-owning XMEv* ... but few gains here, I guess.
  XMEv mev;

  FitnessIndividual() : idx{-1}, fitness{0} {}

  FitnessIndividual(int _idx, double _fitness, XMEv _mev)
      : idx(_idx), fitness(_fitness), mev(_mev) {}

  friend std::ostream& operator<<(std::ostream& os,
                                  const FitnessIndividual& me) {
    os << "FitInd{idx=" << me.idx;
    os << "; fit=" << me.fitness;
    os << "; mev=" << me.mev << "}";
    return os;
  }

  std::string toString() const {
    std::stringstream ss;
    ss << "FitInd{idx=" << this->idx;
    ss << "; fit=" << this->fitness;
    ss << "; mev=" << this->mev << "}";
    return ss.str();
  }

  static void printPopulationFitness(
      const std::vector<FitnessIndividual<XMEv>>& P) {
    std::cout << "VEPopulation<FitInd>(|P|=" << P.size() << "):{" << std::endl;
    for (unsigned i = 0; i < P.size(); i++)
      std::cout << "P[" << i << "]:" << P[i].toString() << ";" << std::endl;
    std::cout << "}" << std::endl;
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_MULTIOBJECTIVE_FITNESSASSIGNMENT_HPP_
