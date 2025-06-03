// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_PARETO_HPP_
#define OPTFRAME_PARETO_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C++
#include <cstring>
#include <iostream>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Core/Direction.hpp>
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Pareto/MultiEvaluation.hpp>
// #include <OptFrame/Helper/Population.hpp>
#include <OptFrame/Helper/Solutions/ESolution.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
// #include <OptFrame/Pareto/MultiEvaluator.hpp>
#include <OptFrame/Pareto/ParetoDominance.hpp>
#include <OptFrame/Pareto/ParetoDominanceWeak.hpp>

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

// using namespace std;

// Pareto class is meant to work with Legacy OptFrame structures,
// meaning that it requires MultiEvaluation and also ParetoDominance.
// XMES -> Solution with MultiEvaluation (similar to XES: Solution with
// Evaluation)

namespace optframe {

// NOTE: Pareto should be XMES only, theoretically...
// however, ParetoDominance<XES, XMES> is contaminated...
// due to MultiEvaluator<XES, XMES> dependency.
// Must fix ParetoDominance or create cleaner alternative,
// to finally have XMES only here.
//
// WORKAROUND FOR NOW! Just use DEFAULT_PARETO_XES!!!

#define DEFAULT_PARETO_XES std::pair<S, XEv>

MOD_EXPORT template <XEMSolution XMES>
class Pareto {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
  static_assert(XEvaluation<typename XMEv::XEv>);
#endif

  using XEv = typename XMEv::XEv;
// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
  static_assert(XEvaluation<XEv>);
#endif
  using XES = DEFAULT_PARETO_XES;

 public:
  using value_type = XMES;

 private:
  // all individuals are safely stored here
  std::vector<uptr<XMES>> paretoSetFront;
  // these are just mirrors of stored set above
  std::vector<S*> paretoSet;
  std::vector<XMEv*> paretoFront;
  //
  // mark if solution was added
  bool added;

 public:
  Pareto() { added = false; }

  Pareto(const Pareto<XMES>& _pf) {
    // this->clear();
    // unsigned sizeNewPop = _pf.paretoSet.size();
    for (unsigned i = 0; i < _pf.paretoSetFront.size(); i++) {
      auto* se_ptr = new XMES{*_pf.paretoSetFront[i]};
      paretoSet.push_back(&se_ptr->first);
      paretoFront.push_back(&se_ptr->second);
      paretoSetFront.push_back(uptr<XMES>{se_ptr});
    }
    //
    this->added = _pf.added;
    /*
    for (unsigned i = 0; i < sizeNewPop; i++) {
      //this->add_indWithMev(_pf.getNonDominatedSol(i),
    _pf.getIndMultiEvaluation(i));
      //
      this->add_indWithMev(_pf.getP(i));
      //
      //			S* sNew = new S(_pf.getNonDominatedSol(i));
      //			MultiEvaluation<>* mevNew = new
    MultiEvaluation(_pf.getIndMultiEvaluation(i));
      //			this->add_indWithMev(sNew, mevNew);
    }
    added = true;
    */
  }

  // paretoSet(std::move(_pf.paretoSet)),
  // paretoFront(std::move(_pf.paretoFront))
  Pareto(Pareto<XMES>&& _pf)
      : paretoSetFront(std::move(_pf.paretoSetFront)),
        paretoSet{std::move(_pf.paretoSet)},
        paretoFront{std::move(_pf.paretoFront)} {
    added = _pf.added;
    // if (paretoSetFront.size() > 0)
    //   added = true;
  }

  virtual Pareto<XMES>& operator=(const Pareto<XMES>& _pf) {
    if (&_pf == this)  // auto ref check
      return *this;
    //
    this->clear();
    //
    for (unsigned i = 0; i < _pf.paretoSetFront.size(); i++) {
      auto* se_ptr = new XMES{*_pf.paretoSetFront[i]};
      this->paretoSet.push_back(&se_ptr->first);
      this->paretoFront.push_back(&se_ptr->second);
      this->paretoSetFront.push_back(uptr<XMES>{se_ptr});
    }
    //
    this->added = _pf.added;

    return (*this);
  }

  virtual ~Pareto() { this->clear(); }

  bool getNewNonDominatedSolutionsStatus() const { return added; }

  void setNewNonDominatedSolutionsStatus(bool _added) { added = _added; }

  void push_back(const XMES& se) {
    XMES* u_se_ptr = new XMES{se};
    // data is divided into two auxiliary std::vectors (why?)
    paretoSet.push_back(&u_se_ptr->first);
    paretoFront.push_back(&u_se_ptr->second);
    // data is safely stored on paretoSetFront
    paretoSetFront.push_back(uptr<XMES>(u_se_ptr));
  }

  //	void push_back(S* s, std::vector<Evaluation<>*>& v_e)
  //	{
  //		cout << "something is called inside strange push_back" <<
  // std::endl; 		getchar();
  // paretoSet.push_back(&s->clone()); // clone, otherwise it is
  // deleted 		paretoFront.push_back(new MultiEvaluation(v_e));
  //	}

  //	void add_ind(const S* s, const MultiEvaluator<S, XEv>& mEval)
  //	{
  //		MultiEvaluation<>* mev = mEval.evaluate(s);
  //		add_ind(s, mev);
  //	}

  /*
        void add_indWithMev(const S& s, const XMEv& mev)
        {
                paretoSet.push_back(new S(s));
                paretoFront.push_back(new MultiEvaluation(mev));

                added = true;
        }
*/

  void add_indWithMev(const XMES& smev) {
    paretoSetFront.push_back(uptr<XMES>(new XMES(smev)));
    added = true;
  }

  unsigned size() const {
    // return paretoSet.size();
    return paretoSetFront.size();
  }

  /*
        pair<S&, XMEv*> at(unsigned index)
        {
                return make_pair(*paretoSet.at(index), paretoFront.at(index));
        }
*/

  const XMES& getP(unsigned index) const { return *paretoSetFront.at(index); }

  XMES& getP(unsigned index) { return *paretoSetFront.at(index); }

  // compatibility with X2ESolution
  XMES& at(unsigned index) { return getP(index); }

  // return "observer pointers" mirroring internal pareto set / front
  std::vector<XMES*> getParetoSetFrontPtr() {
    // mirror raw pointers (observers)
    // not using observer_ptr now, or optr, just to keep it more 'friendly' :)
    std::vector<XMES*> paretoSetFrontPtr(paretoSetFront.size(), nullptr);
    for (unsigned i = 0; i < paretoSetFront.size(); i++)
      paretoSetFrontPtr[i] = paretoSetFront[i].get();
    //
    return paretoSetFrontPtr;
  }

  // return "observer pointers" mirroring internal pareto front
  std::vector<XMEv*> getParetoFrontPtr() {
    // mirror raw pointers (observers)
    // not using observer_ptr now, or optr, just to keep it more 'friendly' :)
    std::vector<XMEv*> paretoFrontPtr(paretoSetFront.size(), nullptr);
    for (unsigned i = 0; i < paretoSetFront.size(); i++)
      paretoFrontPtr[i] = &paretoSetFront[i]->second;
    //
    return paretoFrontPtr;
  }

  /*
        std::vector<S*> getParetoSet()
        {
                return paretoSet;
        }

   std::vector<XMEv*> getParetoFront()
        {
                return paretoFront;
        }

        void setParetoSet(std::vector<S*> pSNew)
        {
                paretoSet = std::move(pSNew);
        }

        void setParetoFront(std::vector<XMEv*> pFNew)
        {
                paretoFront = std::move(pFNew);

        }
*/

  S& getNonDominatedSol(int ind) { return paretoSetFront[ind]->first; }

  /*
        S& getNonDominatedSol(int ind) const
        {
                return *paretoSet[ind];
        }

        S& getCloneNonDominatedSol(int ind) const
        {
                return paretoSet[ind]->clone();
        }
*/

  //	MultiEvaluation<>& getIndMultiEvaluation(int ind)
  //	{
  //		return *paretoFront[ind];
  //	}

  XMEv& getIndMultiEvaluation(int ind) const {
    return paretoSetFront.at(ind)->second;
  }

  /*
        XMEv& getCloneIndMultiEvaluation(int ind) const
        {
                return paretoFront.at(ind)->clone();
        }
*/

  void erase(unsigned pos) {
    //		delete paretoSet[pos];
    //		delete paretoFront[pos];
    //		paretoSet.erase(paretoSet.begin() + pos);
    //		paretoFront.erase(paretoFront.begin() + pos);
    paretoSetFront.erase(paretoSetFront.begin() + pos);
  }

  virtual Pareto<XMES>& clone() const { return *new Pareto<XMES>(*this); }

  void clear() {
    for (unsigned i = 0; i < paretoSetFront.size(); i++) {
      // delete paretoSet[i];
      // delete paretoFront[i];
    }

    // paretoSet.clear();
    // paretoFront.clear();
    paretoSetFront.clear();
  }

  void print() {
    std::cout << "Printing Pareto!" << std::endl;
    // std::cout << "paretoSet.size():" << paretoSet.size() << std::endl;
    // std::cout << "paretoFront.size():" << paretoFront.size() << std::endl;
    std::cout << "paretoSetFront.size():" << paretoSetFront.size() << std::endl;
    for (unsigned i = 0; i < paretoSetFront.size(); i++) {
      // std::cout << paretoSet[i]->getR() << std::endl;
      std::cout << paretoSetFront[i]->first << std::endl;
      // paretoFront[i]->print();
      paretoSetFront[i]->second.print();
    }
  }

  // TODO fix export with new evaluation type
  void exportParetoFront(std::string output, const char* exportType) {
    // OLD C VERSION!
    /*
    std::FILE* fPF = fopen(output.c_str(), exportType);
    assert(fPF);
    for (int i = 0; i < (int)paretoSetFront.size(); i++) {
      for (int e = 0; e < (int)paretoFront[i]->size(); e++)
        // fprintf(fPF, "%.7f\t",
        // double(paretoFront.at(i)->at(e).evaluation()));
        fprintf(fPF, "%.7f\t",
                double(paretoSetFront.at(i)->second.at(e).evaluation()));

      fprintf(fPF, "\n");
    }

    fclose(fPF);
    */
    // NEWER C++ VERSION
    std::string sExp{exportType};
    std::ios_base::openmode mode =
        (sExp == "w") ? std::ios::out : std::ios::app;
    std::ofstream fPF(output, mode);

    if (!fPF.is_open()) {
      std::cerr << "Error opening file: " << output << std::endl;
      return;
    }

    /*
    for (const auto& front : paretoSetFront) {
      for (const auto& element : front.second) {
        fPF << static_cast<double>(element.evaluation()) << "\t";
      }
      fPF << "\n";
    }
    */

    for (const auto& front_element : paretoSetFront) {
      for (const auto& e : front_element->second.evaluation())
        fPF << static_cast<double>(e.evaluation()) << "\t";

      fPF << "\n";
    }
  }

  static std::vector<XMEv> filterDominatedMEV(
      sref<IEvaluator<XMES>> mev, const std::vector<XMEv>& candidates) {
    std::vector<XMEv> nonDom;

    ParetoDominance<XES, XMES> pDom(mev);
    ParetoDominanceWeak<XES, XMES> pDomWeak(mev);

    for (unsigned i = 0; i < candidates.size(); i++)
      addSolution(pDom, pDomWeak, nonDom, candidates[i]);

    return nonDom;
  }

  static std::vector<XMES> filterDominated(
      sref<MultiEvaluator<XES, XMES>> mev,
      const std::vector<XMES>& candidates) {
    // store non dominated on 'nonDom'
    std::vector<XMES> nonDom;
    //
    ParetoDominance<XES, XMES> pDom(mev);
    ParetoDominanceWeak<XES, XMES> pDomWeak(mev);

    for (unsigned i = 0; i < candidates.size(); i++)
      addSolution(pDom, pDomWeak, nonDom, candidates[i]);

    return nonDom;
  }

  //====================STILL USED BY 2PPLS OR MOVNS ======================
  // SOON IT SHOULD BE DELETED --- 2PPLS WILL BECOME G2PPLS -- MOVNS UPDATED TO
  // A MORE GENERIC VERSION

  // T. Lust et al (method addSolution)
  // class T must be handled by ParetoDominance operators (candidate:
  // std::vector<double>, std::vector<Evaluation<>*>, MultiEvaluation<>*)

  //	template<class T>
  //	static bool addSolution(std::vector<Direction*>& vDir, std::vector<T*>&
  // nonDom, T*
  // candidate)
  //	{
  //		ParetoDominance<XES, XMES> dom(vDir);
  //		ParetoDominanceWeak<XES, XMES> domWeak(vDir);
  //		return addSolution(dom, domWeak, nonDom, candidate);
  //	}
  //
  // template <class T>
  static bool addSolution(ParetoDominance<XES, XMES>& dom,
                          ParetoDominanceWeak<XES, XMES>& domWeak,
                          std::vector<XMES>& nonDom, const XMES& candidate) {
    for (int ind = 0; ind < (int)nonDom.size(); ind++) {
      if (domWeak.dominates(nonDom.at(ind).second, candidate.second))
        return false;

      if (dom.dominates(candidate.second, nonDom.at(ind).second)) {
        nonDom.erase(nonDom.begin() + ind);
        ind--;
      }
    }

    nonDom.push_back(candidate);
    return true;
  }

  //	static bool addSolution(ParetoDominance<XES, XMES>& dom,
  // ParetoDominanceWeak<XES, XMES>& domWeak, Pareto<XMES>& p, S*
  // candidate)
  //	{
  //		MultiEvaluator<S, XEv>& mEval = dom.getMultiEvaluator();
  //		MultiEvaluation<>& mevCandidate = mEval.evaluate(*candidate);
  //
  //		for (int eI = 0; eI < mevCandidate.size(); eI++)
  //		{
  //			if (!mevCandidate[eI].isFeasible())
  //			{
  //				delete &mevCandidate;
  //				return false;
  //			}
  //
  //		}
  //
  //		bool added = true;
  //		for (int ind = 0; ind < p.size(); ind++)
  //		{
  //			MultiEvaluation<> popIndFitness =
  // p.getIndMultiEvaluation(ind);
  //
  //			if (domWeak.dominates(popIndFitness, mevCandidate))
  //			{
  //				delete &mevCandidate;
  //				return false;
  //			}
  //
  //			if (dom.dominates(mevCandidate, mevCandidate))
  //			{
  //				p.erase(ind);
  //				ind--;
  //			}
  //
  //		}
  //
  //		if (added)
  //			p.push_back(candidate, &mevCandidate);
  //
  //		delete &mevCandidate;
  //
  //		return added;
  //	}

  //	static bool addSolution(ParetoDominance<XES, XMES>& dom,
  // ParetoDominanceWeak<XES, XMES>& domWeak, pair<Pareto<XMES>,
  // std::vector<vector<bool>
  // >
  //>& p, S* candidate, int neighboorsSize)
  //
  //	{
  //		vector<Evaluator<XES, XEv>*> v_e = dom.getEvaluators();
  //		vector<Evaluation<>*> fitnessNewInd;
  //
  //		for (int evalIndex = 0; evalIndex < v_e.size(); evalIndex++)
  //		{
  //			Evaluation<>* e = &v_e[evalIndex]->evaluate(*candidate);
  //
  //			if (!e->isFeasible())
  //			{
  //				for (int eIA = 0; eIA < fitnessNewInd.size();
  // eIA++) 					delete fitnessNewInd[eIA];
  // delete e; return false;
  //			}
  //
  //			fitnessNewInd.push_back(e);
  //		}
  //
  //		bool added = true;
  //		for (int ind = 0; ind < p.first.size(); ind++)
  //		{
  //			MultiEvaluation<> popIndFitness =
  // p.first.getIndMultiEvaluation(ind);
  //
  //			if (domWeak.dominates(popIndFitness, fitnessNewInd))
  //			{
  //				for (int eI = 0; eI < fitnessNewInd.size();
  // eI++) 					delete fitnessNewInd[eI];
  // return false;
  //			}
  //
  //			if (dom.dominates(fitnessNewInd, popIndFitness))
  //			{
  //				p.first.erase(ind);
  //				p.second.erase(p.second.begin() + ind);
  //				ind--;
  //			}
  //
  //		}
  //
  //		if (added)
  //		{
  //			p.first.push_back(candidate, fitnessNewInd);
  //			vector<bool> neigh;
  //			for (int n = 0; n < neighboorsSize; n++)
  //				neigh.push_back(false);
  //			p.second.push_back(neigh);
  //		}
  //
  //		for (int eI = 0; eI < fitnessNewInd.size(); eI++)
  //			delete fitnessNewInd[eI];
  //
  //		return added;
  //
  //	}

  //	template<class T>
  //	static bool addSolution(ParetoDominance<XES, XMES>& dom,
  // ParetoDominanceWeak<XES, XMES>& domWeak, std::vector<T>& nonDom, T
  // candidate)
  //	{
  //		for (int ind = 0; ind < nonDom.size(); ind++)
  //		{
  //			if (domWeak.dominates(nonDom[ind], candidate))
  //				return false;
  //
  //			if (dom.dominates(candidate, nonDom[ind]))
  //			{
  //				nonDom.erase(nonDom.begin() + ind);
  //				ind--;
  //			}
  //		}
  //
  //		nonDom.push_back(candidate);
  //		return true;
  //	}

  //	static bool addSolution(ParetoDominance<XES, XMES>& dom,
  // ParetoDominanceWeak<XES, XMES>& domWeak, Population<XES>& p, S& s)
  //	{
  //		vector<Evaluator<XES, XEv>*> v_e = dom.getEvaluators();
  //		vector<double> fitnessNewInd;
  //
  //		for (int evalIndex = 0; evalIndex < v_e.size(); evalIndex++)
  //		{
  //			Evaluation<> &e = v_e[evalIndex]->evaluate(s);
  //
  //			if (!e.isFeasible())
  //			{
  //				delete &e;
  //				return false;
  //			}
  //
  //			fitnessNewInd.push_back(e.evaluation());
  //			delete &e;
  //		}
  //
  //		bool added = true;
  //		for (int ind = 0; ind < p.size(); ind++)
  //		{
  //
  //			vector<double> popIndFitness = p.getFitness(ind);
  //			if (domWeak.dominates(popIndFitness, fitnessNewInd))
  //				return false;
  //
  //			if (dom.dominates(fitnessNewInd, popIndFitness))
  //			{
  //				delete &p.remove(ind);
  //				ind--;
  //			}
  //
  //		}
  //		if (added)
  //			p.push_back(s, fitnessNewInd);
  //
  //		return added;
  //	}

  // Special addSolution used in the 2PPLS speedUp
  //	static bool addSolution(ParetoDominance<XES, XMES>& dom,
  // ParetoDominanceWeak<XES, XMES>& domWeak, pair<Population<XES>,
  // std::vector<vector<bool> > >& p, S& s, int neighboorsSize)
  //	{
  //		vector<Evaluator<XES, XEv>*> v_e = dom.getEvaluators();
  //		vector<double> fitnessNewInd;
  //
  //		for (int evalIndex = 0; evalIndex < v_e.size(); evalIndex++)
  //		{
  //			Evaluation<> &e = v_e[evalIndex]->evaluate(s);
  //
  //			if (!e.isFeasible())
  //			{
  //				delete &e;
  //				return false;
  //			}
  //
  //			fitnessNewInd.push_back(e.evaluation());
  //			delete &e;
  //		}
  //
  //		bool added = true;
  //		for (int ind = 0; ind < p.first.size(); ind++)
  //		{
  //
  //			vector<double> popIndFitness = p.first.getFitness(ind);
  //			if (domWeak.dominates(popIndFitness, fitnessNewInd))
  //				return false;
  //
  //			if (dom.dominates(fitnessNewInd, popIndFitness))
  //			{
  //				delete &p.first.remove(ind);
  //				p.second.erase(p.second.begin() + ind);
  //				ind--;
  //			}
  //
  //		}
  //
  //		if (added)
  //		{
  //			p.first.push_back(s, fitnessNewInd);
  //			vector<bool> neigh;
  //			for (int n = 0; n < neighboorsSize; n++)
  //				neigh.push_back(false);
  //			p.second.push_back(neigh);
  //		}
  //
  //		return added;
  //	}
  //
  //	static void addSolution(ParetoDominance<XES, XMES>& dom,
  // ParetoDominanceWeak<XES, XMES>& domWeak, std::vector<std::pair<S*,
  // MultiEvaluation<>*> >& nonDom, pair<S*, MultiEvaluation<>*> candidate)
  //	{
  //		for (int ind = 0; ind < nonDom.size(); ind++)
  //		{
  //			if (domWeak.dominates(*nonDom.at(ind).second,
  //*candidate.second)) 				return;
  //
  //			if (dom.dominates(*candidate.second,
  //*nonDom.at(ind).second))
  //			{
  //				nonDom.erase(nonDom.begin() + ind);
  //				ind--;
  //			}
  //		}
  //
  //		nonDom.push_back(candidate);
  //	}

  //====================STILL USED BY 2PPLS OR MOVNS ======================
  // SOON IT SHOULD BE DELETED --- 2PPLS WILL BECOME G2PPLS -- MOVNS UPDATED TO
  // A MORE GENERIC VERSION
};

}  // namespace optframe

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS

namespace optframe {
using ParetoSolTest = std::vector<int>;
using EMSolution_Test = std::pair<ParetoSolTest, MultiEvaluation<double>>;
// compilation tests
static_assert(X2ESolution<Pareto<EMSolution_Test>, EMSolution_Test>);
}  // namespace optframe

#endif  // cpp_concepts

// compilation tests for concepts (these are NOT unit tests)

/*
#ifdef HAS_CONCEPTS_TESTS
#include "Pareto.ctest.hpp"
#endif
*/

#endif /* OPTFRAME_PARETO_HPP_ */
