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

#ifndef OPTFRAME_HEURISTICS_MULTIOBJECTIVE_MOPOPULATIONMANAGEMENT_HPP_
#define OPTFRAME_HEURISTICS_MULTIOBJECTIVE_MOPOPULATIONMANAGEMENT_HPP_
//
#include <algorithm>
#include <optional>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Helper/InitialMultiESolution.hpp>
#include <OptFrame/Heuristics/EA/GeneralCrossover.hpp>
#include <OptFrame/Heuristics/MultiObjective/MOSIndividual.hpp>

namespace optframe {

// template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
template <XESolution XMES2>
class MOPopulationManagement : public Component {
 public:
  MOPopulationManagement() {
  }

  virtual ~MOPopulationManagement() {
  }

  // create a new population
  virtual vector<MOSIndividual<XMES2>> initialize(unsigned pSize) = 0;

  // create next population
  virtual vector<MOSIndividual<XMES2>> createNext(
      unsigned target_size,
      const vector<MOSIndividual<XMES2>>& P) = 0;

  virtual void print() const {
    cout << "MOPopulationManagement" << endl;
  }
};

// template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
template <XESolution XMES2>
class BasicMOPopulationManagement : public MOPopulationManagement<XMES2> {
  using S = typename XMES2::first_type;
  using XMEv = typename XMES2::second_type;
  using XEv = typename XMEv::XEv;

 public:
  sref<InitialMultiESolution<XMES2>> initEPop;
  vsref<NS<XMES2>> mutations;
  double mutationRate;  // probability of applying a mutation
  vsref<GeneralCrossover<S>> crossovers;
  // percentage of children population to be entirely reconstructed
  double renewRate;
  sref<RandGen> rg;

  BasicMOPopulationManagement(
      sref<InitialMultiESolution<XMES2>> _initEPop,
      vsref<NS<XMES2>> _mutations,
      double _mutationRate,
      vsref<GeneralCrossover<S>> _crossovers,
      double _renewRate,
      sref<RandGen> _rg)
      : initEPop(_initEPop),
        mutations(_mutations),
        mutationRate(_mutationRate),
        crossovers(_crossovers),
        renewRate(_renewRate),
        rg(_rg) {
    if (renewRate > 1)
      renewRate = 1;
    if (renewRate < 0)
      renewRate = 0;
  }

  virtual ~BasicMOPopulationManagement() {
  }

  // giving ownership of 's'
  virtual MOSIndividual<XMES2> createIndividual(XMES2&& se) {
    MOSIndividual<XMES2> mosind(se.first, se.second);
    // MultiEvaluation<DS>* mev = nullptr;

    // no evaluation here!

    return mosind;  //new MOSIndividual<XMES2>(s, mev);
  }

  virtual vector<MOSIndividual<XMES2>> initialize(unsigned pSize) {
    // Population<XES>* p = &initPop.generatePopulation(pSize);
    VEPopulation<XMES2> mes = initEPop->generateEPopulation(pSize);

    vector<MOSIndividual<XMES2>> r;
    for (unsigned i = 0; i < mes.size(); i++)
      r.push_back(createIndividual(std::move(mes.at(i))));

    mes.clear();

    return r;
  }

  // multiple crossovers in a binary tournament and apply many mutations
  vector<MOSIndividual<XMES2>> createNext(
      unsigned target_size,
      const vector<MOSIndividual<XMES2>>& P) override {
    //
    unsigned size_renew = renewRate * target_size;
    if (Component::verbose) {
      std::cout << "BasicPopManager: createNext()" << std::endl;
      std::cout << "target_size=" << target_size << std::endl;
      std::cout << "size_renew=" << size_renew << " ";
      std::cout << "(" << renewRate << "*" << target_size << ")" << std::endl;
    }

    vector<MOSIndividual<XMES2>> children;
    //
    if (size_renew > 0) {
      children = initialize(size_renew);
    } else {
      if ((renewRate > 0) && Component::warning) {
        std::cout << "WARNING MOPopManagemnt: size_renew is ZERO!" << std::endl;
        std::cout << "size_renew=" << size_renew << " ";
        std::cout << "(" << renewRate << "*" << target_size << ")" << std::endl;
      }
    }

    unsigned rest = target_size - size_renew;

    if (Component::verbose) {
      std::cout << "children.size()=" << children.size() << std::endl;
      std::cout << "rest=" << rest << std::endl;
    }

    if (Component::verbose) {
      std::cout << std::endl;
      std::cout << "will run binaryTournment(rest=" << rest << ")" << std::endl;
    }
    vector<MOSIndividual<XMES2>> pool_best = binaryTournment(rest, P);
    if (Component::verbose)
      std::cout << "pool_best.size() = " << pool_best.size() << std::endl;

    if (Component::verbose) {
      std::cout << std::endl;
      std::cout << "will run basicCrossoverMutation(|pool|=";
      std::cout << pool_best.size() << ")" << std::endl;
    }
    vector<MOSIndividual<XMES2>> crossMut =
        basicCrossoverMutation(rest, pool_best);
    if (Component::verbose)
      std::cout << "crossMut.size() = " << crossMut.size() << std::endl;

    children.insert(children.end(), crossMut.begin(), crossMut.end());
    if (Component::verbose)
      std::cout << "children.size()=" << children.size() << std::endl;

    if (children.size() != target_size) {
      if (Component::warning) {
        cout << "WARNING: BasicPopulationManagement::createNext()";
        cout << " tried to create population of " << target_size;
        cout << " but got " << children.size();
        cout << endl;
        cout << "I THINK NSGA-II requires double population, not more...";
        cout << endl;
      }
    }

    return children;
  }

  vector<MOSIndividual<XMES2>> binaryTournment(
      unsigned poolSize, const vector<MOSIndividual<XMES2>>& P) {
    vector<MOSIndividual<XMES2>> pool;

    for (unsigned t = 1; t <= poolSize; t++) {
      int i = rg->rand(P.size());
      int j = i;
      while (i == j)
        j = rg->rand(P.size());

      if (P[i].betterThan(P[j]))
        pool.push_back(P[i]);
      else
        pool.push_back(P[j]);
    }

    return pool;
  }

  vector<MOSIndividual<XMES2>> basicCrossoverMutation(
      unsigned targetSize,
      const vector<MOSIndividual<XMES2>>& pool) {
    //
    if (Component::verbose)
      std::cout << "DEBUG: |pool| = " << pool.size() << std::endl;
    //
    if (pool.size() == 0) {
      if (targetSize != 0) {
        if (Component::warning) {
          std::cout << "WARNING: expected |children|=0";
          std::cout << " targetSize=" << targetSize << std::endl;
        }
      }
      return vector<MOSIndividual<XMES2>>{};
    }

    // perform many crossovers and mutations, then choose
    vector<MOSIndividual<XMES2>> childrenPool;

    for (unsigned i = 0; i < pool.size(); i++) {
      unsigned j = i + 1;
      if (j == pool.size())
        j = 0;

      // pair<Solution<R, ADS>*, Solution<R, ADS>*> crossChildren;
      std::pair<op<S>, op<S>> crossChildren;

      if (crossovers.size() > 0) {
        int crossId = rg->rand(crossovers.size());
        crossChildren = crossovers[crossId]->cross(
            pool[i].first, pool[j].first);
      } else {
        crossChildren = make_pair(pool[i].first, pool[j].first);
      }

      // vector<Solution<R, ADS>*> vChildren(2, nullptr);
      vector<op<S>> vChildren(2, std::nullopt);
      vChildren[0] = crossChildren.first;
      vChildren[1] = crossChildren.second;

      for (unsigned c = 0; c < 2; c++) {
        XMES2 se{*vChildren[c], XMEv{}};
        // mutation rate
        if ((mutations.size() > 0) && rg->randP(mutationRate)) {
          int neigh = rg->rand(mutations.size());

          uptr<Move<XMES2>> move =
              mutations[neigh]->validRandomMove(se);

          if (move) {
            move->apply(se);
            // delete move;
            // move = nullptr;
          }
        }

        childrenPool.push_back(MOSIndividual<XMES2>{se});
      }
    }

    // will select 'targetSize' children from childrenPool
    vector<MOSIndividual<XMES2>> children;
    //
    if (Component::verbose)
      std::cout << "DEBUG: |childrenPool|=" << childrenPool.size() << std::endl;
    //
    rg->shuffle(childrenPool);
    //
    if (targetSize > childrenPool.size()) {
      if (Component::warning) {
        std::cout << "WARNING: childrenPool too small!" << std::endl;
      }
    }
    //
    int min_sz = ::min((int)childrenPool.size(), (int)targetSize);
    for (int i = 0; i < min_sz; i++)
      children.push_back(std::move(childrenPool[i]));
    // drop skeletons
    childrenPool.clear();

    if (targetSize != children.size()) {
      if (Component::warning) {
        std::cout << "WARNING: expected |children|=" << children.size();
        std::cout << " targetSize=" << targetSize << std::endl;
      }
    }
    //
    return children;
  }

  bool setVerboseR() override {
    this->setVerbose();
    //
    initEPop->setVerboseR();
    for (unsigned i = 0; i < mutations.size(); i++)
      mutations[i]->setVerboseR();
    for (unsigned i = 0; i < crossovers.size(); i++)
      crossovers[i]->setVerboseR();
    return true;
  }

  std::string toString() const override {
    std::stringstream ss;
    ss << "BasicMOPopulationManagement";
    return ss.str();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class BasicMOPopulationManagementBuilder : public ComponentBuilder<S, XEv, XES, X2ES> {
  using XMEv = MultiEvaluation<typename XEv::objType>;
  using XMES = std::pair<S, XMEv>;

 public:
  virtual ~BasicMOPopulationManagementBuilder() {
  }

  // has sptr instead of sref, is that on purpose or legacy class?
  virtual Component* buildComponent(Scanner& scanner,
                                    HeuristicFactory<S, XEv, XES, X2ES>& hf,
                                    string family = "") {
    if (Component::debug)
      std::cout << "BasicMOPopulationBuilder Loading Parameter #0" << std::endl;

    sptr<InitialMultiESolution<XMES>> _initEPop;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(_initEPop, id_0, sid_0);
    assert(_initEPop);
    sref<InitialMultiESolution<XMES>> initEPop{_initEPop};

    vsptr<NS<XMES>> _hlist;
    std::string sid_1 = scanner.next();
    int id_1 = *scanner.nextInt();
    hf.assignList(_hlist, id_1, sid_1);
    vsref<NS<XMES>> hlist;
    for (sptr<NS<XMES>> x : _hlist) {
      assert(x);
      hlist.push_back(x);
    }

    if (!scanner.hasNext()) {
      if (Component::debug)
        std::cout << "no next mutation rate! aborting..." << std::endl;
      return nullptr;
    }
    double _mutationRate = *scanner.nextDouble();

    vsptr<GeneralCrossover<S>> _clist;
    std::string sid_3 = scanner.next();
    int id_3 = *scanner.nextInt();
    hf.assignList(_clist, id_3, sid_3);
    vsref<GeneralCrossover<S>> clist;
    for (sptr<GeneralCrossover<S>> x : _clist) {
      assert(x);
      clist.push_back(x);
    }

    if (!scanner.hasNext()) {
      if (Component::debug)
        std::cout << "no next renew rate! aborting..." << std::endl;
      return nullptr;
    }

    double _renew_rate = *scanner.nextDouble();

    return new BasicMOPopulationManagement<XMES>(
        initEPop,
        hlist,
        _mutationRate,
        clist,
        _renew_rate,
        hf.getRandGen());
  }

  virtual vector<pair<string, string>> parameters() {
    vector<pair<string, string>> params;
    params.push_back(make_pair(
        InitialMultiESolution<XMES>::idComponent(),
        "initial epopulation"));
    stringstream ss;
    ss << NS<XES, XEv>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of NS"));
    params.push_back(make_pair("OptFrame:double", "mutation rate"));
    stringstream ss2;
    ss2 << GeneralCrossover<S>::idComponent() << "[]";
    params.push_back(make_pair(ss2.str(), "list of crossover"));
    params.push_back(make_pair("OptFrame:double", "renew rate"));
    return params;
  }

  virtual bool canBuild(string component) {
    return component == BasicMOPopulationManagement<XMES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent();
    ss << "BasicMOPopulationManagementBuilder";
    return ss.str();
  }

  std::string toString() const override {
    return id();
  }

  string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_MULTIOBJECTIVE_MOPOPULATIONMANAGEMENT_HPP_
