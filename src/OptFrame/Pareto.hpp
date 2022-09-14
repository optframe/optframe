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

#ifndef OPTFRAME_PARETO_HPP_
#define OPTFRAME_PARETO_HPP_

#include <cstring>
#include <iostream>
#include <vector>

#include "BaseConcepts.hpp"

#include "Direction.hpp"
#include "Evaluation.hpp"
#include "Population.hpp"
#include "Solution.hpp"

#include "MultiEvaluation.hpp"
#include "MultiEvaluator.hpp"

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "ParetoDominance.hpp"
#include "ParetoDominanceWeak.hpp"

using namespace std;

// Pareto class is meant to work with Legacy OptFrame structures,
// meaning that it requires MultiEvaluation and also ParetoDominance.
// XMES -> Solution with MultiEvaluation (similar to XES: Solution with Evaluation)

namespace optframe {

//template<XSolution S, XEvaluation XMEv = MultiEvaluation<>, XESolution XMES = pair<S, XMEv>>
//
template<XESolution XMES>
class Pareto
{
   using S = typename XMES::first_type;
   using XMEv = typename XMES::second_type;

   //using XEv = decltype(declval<XMEv>.at(0)); // not enough contextual information...
   using XEv = Evaluation<>; // hardcoding this, for now...
   // its bad to have XEv here, as its just a requirement of MultiEvaluator, insider ParetoDominance managers!!
   // to solve this, individuals should always be treated in pairs XMES=<S, XMEv>, so evaluate() becomes unnecessary, and GeneralEvaluator is enough. (TODO:)

private:
   // all individuals are safely stored here
   vector<uptr<XMES>> paretoSetFront;
   // these are just mirrors of stored set above
   vector<S*> paretoSet;
   vector<XMEv*> paretoFront;
   //
   //mark if solution was added
   bool added;

public:
   Pareto()
   {
      added = false;
   }

   Pareto(const Pareto<XMES>& _pf)
   {
      this->clear();
      unsigned sizeNewPop = _pf.paretoSet.size();

      for (unsigned i = 0; i < sizeNewPop; i++) {
         //this->add_indWithMev(_pf.getNonDominatedSol(i), _pf.getIndMultiEvaluation(i));
         //
         this->add_indWithMev(_pf.getP(i));
         //
         //			S* sNew = new S(_pf.getNonDominatedSol(i));
         //			MultiEvaluation<>* mevNew = new MultiEvaluation(_pf.getIndMultiEvaluation(i));
         //			this->add_indWithMev(sNew, mevNew);
      }
      added = true;
   }

   Pareto(Pareto<XMES>&& _pf)
     : //paretoSet(std::move(_pf.paretoSet)), paretoFront(std::move(_pf.paretoFront))
     paretoSetFront(std::move(_pf.paretoSetFront))
   {
      added = false;

      if (paretoSetFront.size() > 0)
         added = true;
   }

   virtual Pareto<XMES>& operator=(const Pareto<XMES>& _pf)
   {
      if (&_pf == this) // auto ref check
         return *this;

      this->clear();
      unsigned sizeNewPop = _pf.paretoSet.size();

      for (unsigned i = 0; i < sizeNewPop; i++)
         //this->add_indWithMev(_pf.getNonDominatedSol(i), _pf.getIndMultiEvaluation(i));
         this->add_indWithMev(_pf.getP(i));

      if (sizeNewPop > 0)
         added = true;

      return (*this);
   }

   virtual ~Pareto()
   {
      this->clear();
   }

   bool getNewNonDominatedSolutionsStatus() const
   {
      return added;
   }

   void setNewNonDominatedSolutionsStatus(bool _added)
   {
      added = _added;
   }

   //	void push_back(S* s, vector<Evaluation<>*>& v_e)
   //	{
   //		cout << "something is called inside strange push_back" << endl;
   //		getchar();
   //		paretoSet.push_back(&s->clone()); // clone, otherwise it is deleted
   //		paretoFront.push_back(new MultiEvaluation(v_e));
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

   void add_indWithMev(const XMES& smev)
   {
      paretoSetFront.push_back(uptr<XMES>(new XMES(smev)));
      added = true;
   }

   unsigned size() const
   {
      //return paretoSet.size();
      return paretoSetFront.size();
   }

   /*
	pair<S&, XMEv*> at(unsigned index)
	{
		return make_pair(*paretoSet.at(index), paretoFront.at(index));
	}
*/

   const XMES& getP(unsigned index) const
   {
      return *paretoSetFront.at(index);
   }

   XMES& getP(unsigned index)
   {
      return *paretoSetFront.at(index);
   }

   // compatibility with X2ESolution
   XMES& at(unsigned index)
   {
      return getP(index);
   }

   // return "observer pointers" mirroring internal pareto set / front
   vector<XMES*> getParetoSetFrontPtr()
   {
      // mirror raw pointers (observers)
      // not using observer_ptr now, or optr, just to keep it more 'friendly' :)
      vector<XMES*> paretoSetFrontPtr(paretoSetFront.size(), nullptr);
      for (unsigned i = 0; i < paretoSetFront.size(); i++)
         paretoSetFrontPtr[i] = paretoSetFront[i].get();
      //
      return paretoSetFrontPtr;
   }

   // return "observer pointers" mirroring internal pareto front
   vector<XMEv*> getParetoFrontPtr()
   {
      // mirror raw pointers (observers)
      // not using observer_ptr now, or optr, just to keep it more 'friendly' :)
      vector<XMEv*> paretoFrontPtr(paretoSetFront.size(), nullptr);
      for (unsigned i = 0; i < paretoSetFront.size(); i++)
         paretoFrontPtr[i] = &paretoSetFront[i]->second;
      //
      return paretoFrontPtr;
   }

   /*
	vector<S*> getParetoSet()
	{
		return paretoSet;
	}

   vector<XMEv*> getParetoFront()
	{
		return paretoFront;
	}

	void setParetoSet(vector<S*> pSNew)
	{
		paretoSet = std::move(pSNew);
	}

	void setParetoFront(vector<XMEv*> pFNew)
	{
		paretoFront = std::move(pFNew);

	}
*/

   S& getNonDominatedSol(int ind)
   {
      return paretoSetFront[ind]->first;
   }

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

   XMEv& getIndMultiEvaluation(int ind) const
   {
      return paretoSetFront.at(ind)->second;
   }

   /*
	XMEv& getCloneIndMultiEvaluation(int ind) const
	{
		return paretoFront.at(ind)->clone();
	}
*/

   void erase(unsigned pos)
   {
      //		delete paretoSet[pos];
      //		delete paretoFront[pos];
      //		paretoSet.erase(paretoSet.begin() + pos);
      //		paretoFront.erase(paretoFront.begin() + pos);
      paretoSetFront.erase(paretoSetFront.begin() + pos);
   }

   virtual Pareto<XMES>& clone() const
   {
      return *new Pareto<XMES>(*this);
   }

   void clear()
   {
      for (unsigned i = 0; i < paretoSetFront.size(); i++) {
         //delete paretoSet[i];
         //delete paretoFront[i];
      }

      //paretoSet.clear();
      //paretoFront.clear();
      paretoSetFront.clear();
   }

   void print()
   {
      cout << "Printing Pareto!" << endl;
      //cout << "paretoSet.size():" << paretoSet.size() << endl;
      //cout << "paretoFront.size():" << paretoFront.size() << endl;
      cout << "paretoSetFront.size():" << paretoSetFront.size() << endl;
      for (unsigned i = 0; i < paretoSetFront.size(); i++) {
         //cout << paretoSet[i]->getR() << endl;
         cout << paretoSetFront[i]->first.getR() << endl;
         //paretoFront[i]->print();
         paretoSetFront[i]->second.print();
      }
   }

   //TODO fix export with new evaluation type
   void exportParetoFront(string output, const char* exportType)
   {
      FILE* fPF = fopen(output.c_str(), exportType);
      assert(fPF);
      for (int i = 0; i < (int)paretoSetFront.size(); i++) {
         for (int e = 0; e < (int)paretoFront[i]->size(); e++)
            //fprintf(fPF, "%.7f\t", double(paretoFront.at(i)->at(e).evaluation()));
            fprintf(fPF, "%.7f\t", double(paretoSetFront.at(i)->second.at(e).evaluation()));

         fprintf(fPF, "\n");
      }

      fclose(fPF);
   }

   static vector<MultiEvaluation<>*> filterDominated(vector<Direction<>*>& vdir, const vector<MultiEvaluation<>*>& candidates)
   {
      vector<MultiEvaluation<>*> nonDom;

      ParetoDominance<S, XEv, XMEv> pDom(vdir);
      ParetoDominanceWeak<S, XEv, XMEv> pDomWeak(vdir);

      for (unsigned i = 0; i < candidates.size(); i++)
         addSolution(pDom, pDomWeak, nonDom, candidates[i]);

      return nonDom;
   }

   static vector<pair<S*, XMEv*>> filterDominated(vector<Direction<>*>& vdir, const vector<pair<S*, XMEv*>>& candidates)
   {
      vector<pair<S*, XMEv*>> nonDom;

      ParetoDominance<S, XEv, XMEv> pDom(vdir);
      ParetoDominanceWeak<S, XEv, XMEv> pDomWeak(vdir);

      for (unsigned i = 0; i < candidates.size(); i++)
         addSolution(pDom, pDomWeak, nonDom, candidates[i]);

      return nonDom;
   }

   //====================STILL USED BY 2PPLS OR MOVNS ======================
   // SOON IT SHOULD BE DELETED --- 2PPLS WILL BECOME G2PPLS -- MOVNS UPDATED TO A MORE GENERIC VERSION

   // T. Lust et al (method addSolution)
   // class T must be handled by ParetoDominance operators (candidate: vector<double>, vector<Evaluation<>*>, MultiEvaluation<>*)

   //	template<class T>
   //	static bool addSolution(vector<Direction*>& vDir, vector<T*>& nonDom, T* candidate)
   //	{
   //		ParetoDominance<S, XEv, XMEv> dom(vDir);
   //		ParetoDominanceWeak<S, XEv, XMEv> domWeak(vDir);
   //		return addSolution(dom, domWeak, nonDom, candidate);
   //	}
   //
   //	template<class T>
   //	static bool addSolution(ParetoDominance<S, XEv, XMEv>& dom, ParetoDominanceWeak<S, XEv, XMEv>& domWeak, vector<T*>& nonDom, T* candidate)
   //	{
   //		for (int ind = 0; ind < nonDom.size(); ind++)
   //		{
   //			if (domWeak.dominates(*nonDom.at(ind), *candidate))
   //				return false;
   //
   //			if (dom.dominates(*candidate, *nonDom.at(ind)))
   //			{
   //				nonDom.erase(nonDom.begin() + ind);
   //				ind--;
   //			}
   //		}
   //
   //		nonDom.push_back(candidate);
   //		return true;
   //	}

   //	static bool addSolution(ParetoDominance<S, XEv, XMEv>& dom, ParetoDominanceWeak<S, XEv, XMEv>& domWeak, Pareto<XMES>& p, S* candidate)
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
   //			MultiEvaluation<> popIndFitness = p.getIndMultiEvaluation(ind);
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

   //	static bool addSolution(ParetoDominance<S, XEv, XMEv>& dom, ParetoDominanceWeak<S, XEv, XMEv>& domWeak, pair<Pareto<XMES>, vector<vector<bool> > >& p, S* candidate, int neighboorsSize)
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
   //				for (int eIA = 0; eIA < fitnessNewInd.size(); eIA++)
   //					delete fitnessNewInd[eIA];
   //				delete e;
   //				return false;
   //			}
   //
   //			fitnessNewInd.push_back(e);
   //		}
   //
   //		bool added = true;
   //		for (int ind = 0; ind < p.first.size(); ind++)
   //		{
   //			MultiEvaluation<> popIndFitness = p.first.getIndMultiEvaluation(ind);
   //
   //			if (domWeak.dominates(popIndFitness, fitnessNewInd))
   //			{
   //				for (int eI = 0; eI < fitnessNewInd.size(); eI++)
   //					delete fitnessNewInd[eI];
   //				return false;
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
   //	static bool addSolution(ParetoDominance<S, XEv, XMEv>& dom, ParetoDominanceWeak<S, XEv, XMEv>& domWeak, vector<T>& nonDom, T candidate)
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

   //	static bool addSolution(ParetoDominance<S, XEv, XMEv>& dom, ParetoDominanceWeak<S, XEv, XMEv>& domWeak, Population<XES>& p, S& s)
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

   //Special addSolution used in the 2PPLS speedUp
   //	static bool addSolution(ParetoDominance<S, XEv, XMEv>& dom, ParetoDominanceWeak<S, XEv, XMEv>& domWeak, pair<Population<XES>, vector<vector<bool> > >& p, S& s, int neighboorsSize)
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
   //	static void addSolution(ParetoDominance<S, XEv, XMEv>& dom, ParetoDominanceWeak<S, XEv, XMEv>& domWeak, vector<pair<S*, MultiEvaluation<>*> >& nonDom, pair<S*, MultiEvaluation<>*> candidate)
   //	{
   //		for (int ind = 0; ind < nonDom.size(); ind++)
   //		{
   //			if (domWeak.dominates(*nonDom.at(ind).second, *candidate.second))
   //				return;
   //
   //			if (dom.dominates(*candidate.second, *nonDom.at(ind).second))
   //			{
   //				nonDom.erase(nonDom.begin() + ind);
   //				ind--;
   //			}
   //		}
   //
   //		nonDom.push_back(candidate);
   //	}

   //====================STILL USED BY 2PPLS OR MOVNS ======================
   // SOON IT SHOULD BE DELETED --- 2PPLS WILL BECOME G2PPLS -- MOVNS UPDATED TO A MORE GENERIC VERSION
};

} // namespace optframe

namespace optframe {
// compilation tests
static_assert(X2ESolution<Pareto<ESolution<double>>, ESolution<double>>);
} // namespace optframeas

// compilation tests for concepts (these are NOT unit tests)

#ifdef HAS_CONCEPTS_TESTS
#include "Pareto.ctest.hpp"
#endif

#endif /* OPTFRAME_PARETO_HPP_ */
