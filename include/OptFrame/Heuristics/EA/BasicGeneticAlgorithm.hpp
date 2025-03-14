// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_EA_BASICGENETICALGORITHM_HPP_
#define OPTFRAME_HEURISTICS_EA_BASICGENETICALGORITHM_HPP_

#include <OptFrame/Search/LocalSearch.hpp>
#include <OptFrame/Search/SingleObjSearch.hpp>
#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "../../MultiSolution.hpp"
#include "../../RandGen.hpp"
#include "BinarySelection.hpp"
#include "Crossover.hpp"
#include "EA.hpp"
#include "InitialMultiSolution.hpp"
#include "Mutation.hpp"
#include "Selection.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicGeneticAlgorithm : public SingleObjSearch<XES>, public EA {
 protected:
  typedef S Chromossome;
  typedef MultiSolution<S> MSPopulation;

  Evaluator<S>& evaluator;

 private:
  bool maxim;  // true if maximizing
  unsigned popSize;

  double pCross, pMut, pLS;

  unsigned numGenerations;

  InitialMultiSolution<S>& initPop;

  Selection<R, ADS>& selection;
  Crossover<R, ADS>& cross;
  Mutation<R, ADS>& mut;
  LocalSearch<XES>& ls;

  RandGen& rg;

 public:
  float assert01(float f) {
    if ((f < 0) || (f > 1))
      return 0;
    else
      return f;
  }

  BasicGeneticAlgorithm(Evaluator<S>& _evaluator,
                        InitialMultiSolution<S>& _initPop,
                        unsigned populationSize, float crossoverRate,
                        float mutationRate, float _pLS, unsigned numGenerations,
                        Selection<R, ADS>& _selection,
                        Crossover<R, ADS>& _cross, Mutation<R>& _mut,
                        LocalSearch<XES>& _ls, RandGen& _rg)
      : evaluator(_evaluator),
        initPop(_initPop),
        selection(_selection),
        cross(_cross),
        mut(_mut),
        ls(_ls),
        rg(_rg) {
    maxim = !evaluator.isMinimization();
    pCross = assert01(crossoverRate);
    pMut = assert01(mutationRate);
    pLS = assert01(_pLS);
    popSize = populationSize;
    if (popSize == 0) popSize = 1;
    this->numGenerations = numGenerations;
  }

  void evaluate(const MultiSolution<S>& p, MultiEvaluation<>& mev) {
    for (unsigned i = 0; i < p.size(); i++)
      mev.addEvaluation(&evaluator.evaluate(p.at(i)));
  }

  int getBest(const MultiEvaluation<>& mev) {
    if (mev.size() == 0) return -1;
    int best = 0;
    for (unsigned i = 1; i < mev.size(); i++) {
      if (!maxim && (mev.at(i).evaluation() < mev.at(best).evaluation()))
        best = i;
      if (maxim && (mev.at(i).evaluation() > mev.at(best).evaluation()))
        best = i;
    }
    return best;
  }

  // basic implementation of low diversity scheme that prevents populations of
  // clones (TODO: improve!)
  bool lowDiversity(const MultiSolution<S>& p, const MultiEvaluation<>& mev) {
    for (unsigned i = 1; i < mev.size(); i++)
      if (mev.at(i - 1).evaluation() != mev.at(i).evaluation()) return false;
    return true;
  }

  /*
   * In the canonical genetic algorithm, fitness (maximization) is defined by:
   * fi/f where: fi: is the evaluation associated with the i-th Chromossome. f:
   * is the average population evaluation.
   */

  virtual void setFitness(const MultiSolution<S>& p,
                          const MultiEvaluation<>& mev, vector<double>& fv) {
    fv.resize(mev.size());
    for (int i = 0; i < mev.size(); i++) fv[i] = mev.at(i).evaluation();

    // convert to maximization
    if (!maxim) {
      double lmax = Selection<R, ADS>::getMax(fv);
      for (int i = 0; i < (int)p.size(); i++) fv[i] = lmax - fv[i];
    }

    // calculate average
    double sumEvals = Selection<R, ADS>::getSum(fv);
    double avgEvalsPop = sumEvals / p.size();
    if (avgEvalsPop == 0) avgEvalsPop = 1;

    for (int i = 0; i < fv.size(); i++) {
      fv[i] = (fv[i] / avgEvalsPop);
      // if is nan
      if (fv[i] != fv[i]) {
        std::cout << "Selection::setFitness()::NAN VALUE!" << std::endl;
        std::cout << "average=" << avgEvalsPop << std::endl;
        std::cout << fv << std::endl;
        exit(1);
      }
    }

    // normalize to [0,1]
    Selection<R, ADS>::normalize(fv);
  }

  void mayMutate(Chromossome& c, Evaluation<>& e) {
    double xMut = rg.rand01();
    if (xMut <= pMut)  // mutate!
      mut.mutate(c, e);
  }

  void mayLocalSearch(Chromossome& c, Evaluation<>& e, double timelimit,
                      double target_f) {
    double xLS = rg.rand01();
    if (xLS <= pLS)  // local search!
      ls.searchFrom(c, e, timelimit, target_f);
  }

  pair<Solution<R, ADS>&, Evaluation<>&>* search(
      double timelimit = 100000000, double target_f = 0,
      const Solution<R, ADS>* _s = nullptr, const Evaluation<>* _e = nullptr) {
    Timer t;
    std::cout << id() << "(timelimit=" << timelimit << "; target_f=" << target_f
              << ")" << std::endl;
    std::cout << "Population Size: " << popSize
              << " Total of Generations: " << numGenerations << std::endl;
    std::cout << "Crossover Rate: " << pCross << " Mutation Rate: " << pMut
              << " Local Search Rate: " << pLS << std::endl;

    std::cout << "Generating the Initial Population" << std::endl;

    MultiSolution<S>* p = &initPop.generatePopulation(popSize);
    MultiEvaluation<>* mev = new MultiEvaluation;
    evaluate(*p, *mev);
    vector<double> fv;
    setFitness(*p, *mev, fv);
    // std::cout << fv << " = " << Selection<R, ADS>::getSum(fv) << std::endl;

    int best = getBest(*mev);

    Chromossome* sStar = new Chromossome(p->at(best));
    Evaluation<>* eStar = new Evaluation(mev->at(best));
    std::cout << "GA iter=0 ";
    eStar->print();

    unsigned g = 0;

    while ((g < numGenerations) &&
           (evaluator.betterThan(target_f, eStar->evaluation()) &&
            (t.now() < timelimit))) {
      if (lowDiversity(*p, *mev)) {
        // std::cout << "WARNING: Genetic Algorithm leaving with low diversity
        // at iteration g=" << g << std::endl; std::cout << "Try different
        // solution generators or better mutation and crossover operators!" <<
        // std::endl;
        break;
      }

      MultiSolution<S>* p2 = new MultiSolution<S>;
      MultiEvaluation<>* mev2 = new MultiEvaluation;

      best = getBest(*mev);
      p2->push_back(&p->at(best).clone());
      mev2->addEvaluation(&mev->at(best).clone());

      if (evaluator.betterThan(mev->at(best), *eStar)) {
        delete eStar;
        delete sStar;
        eStar = &mev->at(best).clone();
        sStar = &p->at(best).clone();
        std::cout << "GA iter=" << g << " ";
        eStar->print();
        g = 0;
      }

      while (p2->size() < popSize) {
        pair<unsigned, unsigned> idx = selection.select(*p, *mev, fv);
        if (idx.first == idx.second) {
          std::cout << "ERROR IN GENETIC SELECTION! SAME ELEMENTS!"
                    << std::endl;
          exit(1);
        }

        double xCross = rg.rand01();
        if (xCross > pCross)  // do not make any cross
        {
          p2->push_back(p->at(idx.first).clone());
          p2->push_back(p->at(idx.second).clone());
          mev2->addEvaluation(mev->at(idx.first).clone());
          mev2->addEvaluation(mev->at(idx.second).clone());
          continue;
        }

        pair<Chromossome*, Chromossome*> rCross =
            cross.cross(p->at(idx.first), p->at(idx.second));
        if (!rCross.first && !rCross.second) {
          std::cout << "ERROR IN GENETIC CROSS! NO RESULT!" << std::endl;
          exit(1);
        }

        Evaluation<>* e1 = nullptr;
        Evaluation<>* e2 = nullptr;

        if (rCross.first) {
          e1 = &evaluator.evaluate(*rCross.first);
          mayMutate(*rCross.first, *e1);
          mayLocalSearch(*rCross.first, *e1, timelimit, target_f);
          p2->push_back(rCross.first);
          mev2->addEvaluation(e1);
        }

        if (rCross.second) {
          e2 = &evaluator.evaluate(*rCross.second);
          mayMutate(*rCross.second, *e2);
          mayLocalSearch(*rCross.second, *e2, timelimit, target_f);
          p2->push_back(rCross.second);
          mev2->addEvaluation(e2);
        }
      }

      while (p2->size() > popSize) {
        delete &p2->remove(p2->size() - 1);
        mev2->erase(mev2->size() - 1);
      }

      p->clear();
      delete p;
      mev->clear();
      delete mev;

      p = p2;
      mev = mev2;
      setFitness(*p, *mev, fv);

      g++;
    }

    p->clear();
    mev->clear();

    return new pair<Solution<R, ADS>&, Evaluation<>&>(*sStar, *eStar);
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << ":" << EA::family()
       << ":BasicGeneticAlgorithm";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class BasicGeneticAlgorithmBuilder : public EA,
                                     public SingleObjSearchBuilder<XES> {
 public:
  virtual ~BasicGeneticAlgorithmBuilder() {}

  virtual SingleObjSearch<XES>* build(Scanner& scanner,
                                      HeuristicFactory<R, ADS>& hf,
                                      string family = "") {
    Evaluator<S>* eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    InitialMultiSolution<S>* initPop;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(initPop, id2, comp_id2);

    int popSize = *scanner.nextInt();
    float pCross = *scanner.nextFloat();
    float pMut = *scanner.nextFloat();
    float pLS = *scanner.nextFloat();
    int nGen = *scanner.nextInt();

    Selection<R, ADS>* sel;
    std::string comp_id3 = scanner.next();
    int id3 = *scanner.nextInt();
    hf.assign(sel, id3, comp_id3);

    Crossover<R, ADS>* cross;
    std::string comp_id4 = scanner.next();
    int id4 = *scanner.nextInt();
    hf.assign(cross, id4, comp_id4);

    Mutation<R, ADS>* mut;
    std::string comp_id5 = scanner.next();
    int id5 = *scanner.nextInt();
    hf.assign(mut, id5, comp_id5);

    std::string rest = scanner.rest();
    pair<LocalSearch<XES>*, std::string> method;
    method = hf.createLocalSearch(rest);
    LocalSearch<XES>* h = method.first;
    scanner = Scanner(method.second);

    return new BasicGeneticAlgorithm<R, ADS>(*eval, *initPop, popSize, pCross,
                                             pMut, pLS, nGen, *sel, *cross,
                                             *mut, *h, hf.getRandGen());
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(Evaluator<S>::idComponent(), "evaluation function"));
    params.push_back(std::make_pair(InitialMultiSolution<S>::idComponent(),
                                    "generator for initial population"));
    params.push_back(std::make_pair("OptFrame:int", "population size"));
    params.push_back(std::make_pair("OptFrame:float", "cross probability"));
    params.push_back(std::make_pair("OptFrame:float", "mutation probability"));
    params.push_back(
        std::make_pair("OptFrame:float", "local search probability"));
    params.push_back(
        make_pair("OptFrame:int", "number of generations without improvement"));
    params.push_back(
        std::make_pair(Selection<R, ADS>::idComponent(), "selection"));
    params.push_back(
        std::make_pair(Crossover<R, ADS>::idComponent(), "crossover"));
    params.push_back(
        std::make_pair(Mutation<R, ADS>::idComponent(), "mutation"));
    params.push_back(
        make_pair(LocalSearch<XES>::idComponent(), "local search"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicGeneticAlgorithmBuilder<R, ADS>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearchBuilder<XES>::idComponent() << ":" << EA::family()
       << ":BasicGeneticAlgorithm";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_BASICGENETICALGORITHM_HPP_
