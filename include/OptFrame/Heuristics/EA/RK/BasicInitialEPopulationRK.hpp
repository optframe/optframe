// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_BASIC_INITIAL_EPOPULATION_RK_HPP_
#define OPTFRAME_BASIC_INITIAL_EPOPULATION_RK_HPP_

#include "ConstructiveRK.hpp"
#include "InitialEPopulationRK.hpp"

namespace optframe {

template <XESolution XES, class KeyType = double,
          X2ESolution<XES> X2ES = VEPopulation<XES>>
class BasicInitialEPopulationRK
    : public InitialEPopulationRK<XES, KeyType, X2ES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  //
  // TODO: static_assert that S = std::vector<KeyType>
  using RealS = std::vector<KeyType>;

  static_assert(std::is_same_v<S, RealS>);

 public:
  sref<ConstructiveRK<KeyType>> constructiveRK;

  BasicInitialEPopulationRK(sref<ConstructiveRK<KeyType>> _constructiveRK)
      : constructiveRK{_constructiveRK} {}

  virtual ~BasicInitialEPopulationRK() {}

  // cannot evaluate here (return 'false')
  virtual bool canEvaluate() const override { return false; }

  virtual X2ES generateEPopulation(unsigned populationSize,
                                   double timelimit) override {
    X2ES p;
    for (unsigned i = 0; i < populationSize; i++) {
      auto ops = constructiveRK->generateSolution(timelimit);
      // cannot accept nullopt solutions at this point...
      assert((bool)ops);
      p.push_back(XES{*ops, XEv{}});
    }
    return p;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) ||
           (InitialEPopulationRK<XES, KeyType, X2ES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << InitialEPopulationRK<XES, KeyType, X2ES>::idComponent()
       << ":BasicInitialEPopulationRK";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  virtual string id() const override { return idComponent(); }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
class BasicInitialEPopulationRKBuilder
    : public ComponentBuilder<S, XEv, XES, X2ES> {
  using KeyType = double;
  using RealS = std::vector<KeyType>;
  using RealXEv = Evaluation<>;
  using RealXES = std::pair<RealS, RealXEv>;

 public:
  virtual ~BasicInitialEPopulationRKBuilder() {}

  Component* buildComponent(Scanner& scanner,
                            HeuristicFactory<S, XEv, XES, X2ES>& hf,
                            string family = "") override {
    sptr<ConstructiveRK<KeyType>> crk;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(crk, id_0, sid_0);

    return new BasicInitialEPopulationRK<RealXES, double>(crk);
  }

  virtual vector<pair<string, string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(ConstructiveRK<KeyType>::idComponent(), "constructive_rk"));

    return params;
  }

  virtual bool canBuild(string component) override {
    return component ==
           BasicInitialEPopulationRK<RealXES, double>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << EA::family()
       << ":" << RK::family() << "BasicInitialEPopulationRKBuilder";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  virtual string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /*OPTFRAME_BASIC_INITIAL_EPOPULATION_RK_HPP_*/
