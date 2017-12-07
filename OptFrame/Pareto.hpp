// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

#ifndef OPTFRAME_PARETO_HPP_
#define OPTFRAME_PARETO_HPP_

#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

#include "Solution.hpp"
#include "Population.hpp"
#include "Evaluation.hpp"
#include "Direction.hpp"

#include "MultiEvaluator.hpp"
#include "MultiEvaluation.hpp"

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "ParetoDominance.hpp"
#include "ParetoDominanceWeak.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class Pareto
{
private:
	vector<Solution<R, ADS>*> paretoSet;
	vector<MultiEvaluation*> paretoFront;
	bool added; //mark if solution was added

public:

	Pareto()
	{
		added = false;
	}

	Pareto(const Pareto<R, ADS>& _pf)
	{
		this->clear();
		unsigned sizeNewPop = _pf.paretoSet.size();

		for (unsigned i = 0; i < sizeNewPop; i++)
		{
			this->add_indWithMev(_pf.getNonDominatedSol(i), _pf.getIndMultiEvaluation(i));
//			Solution<R, ADS>* sNew = new Solution<R, ADS>(_pf.getNonDominatedSol(i));
//			MultiEvaluation* mevNew = new MultiEvaluation(_pf.getIndMultiEvaluation(i));
//			this->add_indWithMev(sNew, mevNew);
		}
		added = true;
	}

	Pareto(Pareto<R, ADS> && _pf) :
			paretoSet(std::move(_pf.paretoSet)), paretoFront(std::move(_pf.paretoFront))
	{
		added = false;

		if (paretoSet.size() > 0)
			added = true;

	}

	virtual Pareto<R, ADS>& operator=(const Pareto<R, ADS>& _pf)
	{
		if (&_pf == this) // auto ref check
			return *this;

		this->clear();
		unsigned sizeNewPop = _pf.paretoSet.size();

		for (unsigned i = 0; i < sizeNewPop; i++)
			this->add_indWithMev(_pf.getNonDominatedSol(i), _pf.getIndMultiEvaluation(i));

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

//	void push_back(Solution<R, ADS>* s, vector<Evaluation*>& v_e)
//	{
//		cout << "something is called inside strange push_back" << endl;
//		getchar();
//		paretoSet.push_back(&s->clone()); // clone, otherwise it is deleted
//		paretoFront.push_back(new MultiEvaluation(v_e));
//	}

//	void add_ind(const Solution<R, ADS>* s, const MultiEvaluator<R, ADS>& mEval)
//	{
//		MultiEvaluation* mev = mEval.evaluateSolution(s);
//		add_ind(s, mev);
//	}

	void add_indWithMev(const Solution<R, ADS>& s, const MultiEvaluation& mev)
	{
		paretoSet.push_back(new Solution<R, ADS>(s));
		paretoFront.push_back(new MultiEvaluation(mev));

		added = true;
	}

	unsigned size() const
	{
		return paretoSet.size();
	}

	pair<Solution<R, ADS>&, vector<Evaluation*> > at(unsigned index)
	{
		return make_pair(*paretoSet.at(index), paretoFront.at(index));
	}

	vector<Solution<R, ADS>*> getParetoSet()
	{
		return paretoSet;
	}

	vector<MultiEvaluation*> getParetoFront()
	{
		return paretoFront;
	}

	void setParetoSet(vector<Solution<R, ADS>*> pSNew)
	{
		paretoSet = std::move(pSNew);
	}

	void setParetoFront(vector<MultiEvaluation*> pFNew)
	{
		paretoFront = std::move(pFNew);

	}

	Solution<R, ADS>& getNonDominatedSol(int ind)
	{
		return *paretoSet[ind];
	}

	Solution<R, ADS>& getNonDominatedSol(int ind) const
	{
		return *paretoSet[ind];
	}

	Solution<R, ADS>& getCloneNonDominatedSol(int ind) const
	{
		return paretoSet[ind]->clone();
	}

//	MultiEvaluation& getIndMultiEvaluation(int ind)
//	{
//		return *paretoFront[ind];
//	}

	MultiEvaluation& getIndMultiEvaluation(int ind) const
	{
		return *paretoFront.at(ind);
	}

	MultiEvaluation& getCloneIndMultiEvaluation(int ind) const
	{
		return paretoFront.at(ind)->clone();
	}

	void erase(unsigned pos)
	{
		delete paretoSet[pos];
		delete paretoFront[pos];
		paretoSet.erase(paretoSet.begin() + pos);
		paretoFront.erase(paretoFront.begin() + pos);
	}

	virtual Pareto<R, ADS>& clone() const
	{
		return *new Pareto<R, ADS>(*this);
	}

	void clear()
	{
		for (unsigned i = 0; i < paretoSet.size(); i++)
		{
			delete paretoSet[i];
			delete paretoFront[i];
		}

		paretoSet.clear();
		paretoFront.clear();
	}

	void print()
	{
		cout << "Printing Pareto!" << endl;
		cout << "paretoSet.size():" << paretoSet.size() << endl;
		cout << "paretoSet.size():" << paretoFront.size() << endl;
		for (unsigned i = 0; i < paretoSet.size(); i++)
		{
			cout << paretoSet[i]->getR() << endl;
			paretoFront[i]->print();
		}

	}

	//TODO fix export with new evaluation type
	void exportParetoFront(string output, const char* exportType)
	{
		FILE* fPF = fopen(output.c_str(), exportType);
		assert(fPF);
		for (int i = 0; i < (int) paretoFront.size(); i++)
		{
			for (int e = 0; e < (int) paretoFront[i]->size(); e++)
				fprintf(fPF, "%.7f\t", double(paretoFront.at(i)->at(e).evaluation()));

			fprintf(fPF, "\n");
		}

		fclose(fPF);
	}

	static vector<MultiEvaluation*> filterDominated(vector<Direction*>& vdir, const vector<MultiEvaluation*>& candidates)
	{
		vector<MultiEvaluation*> nonDom;

		ParetoDominance<R, ADS> pDom(vdir);
		ParetoDominanceWeak<R, ADS> pDomWeak(vdir);

		for (unsigned i = 0; i < candidates.size(); i++)
			addSolution(pDom, pDomWeak, nonDom, candidates[i]);

		return nonDom;
	}

	static vector<pair<Solution<R>*, MultiEvaluation*> > filterDominated(vector<Direction*>& vdir, const vector<pair<Solution<R>*, MultiEvaluation*> >& candidates)
	{
		vector<pair<Solution<R>*, MultiEvaluation*> > nonDom;

		ParetoDominance<R, ADS> pDom(vdir);
		ParetoDominanceWeak<R, ADS> pDomWeak(vdir);

		for (unsigned i = 0; i < candidates.size(); i++)
			addSolution(pDom, pDomWeak, nonDom, candidates[i]);

		return nonDom;
	}

	//====================STILL USED BY 2PPLS OR MOVNS ======================
	// SOON IT SHOULD BE DELETED --- 2PPLS WILL BECOME G2PPLS -- MOVNS UPDATED TO A MORE GENERIC VERSION

	// T. Lust et al (method addSolution)
	// class T must be handled by ParetoDominance operators (candidate: vector<double>, vector<Evaluation*>, MultiEvaluation*)

//	template<class T>
//	static bool addSolution(vector<Direction*>& vDir, vector<T*>& nonDom, T* candidate)
//	{
//		ParetoDominance<R, ADS> dom(vDir);
//		ParetoDominanceWeak<R, ADS> domWeak(vDir);
//		return addSolution(dom, domWeak, nonDom, candidate);
//	}
//
//	template<class T>
//	static bool addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, vector<T*>& nonDom, T* candidate)
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

//	static bool addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, Pareto<R, ADS>& p, Solution<R, ADS>* candidate)
//	{
//		MultiEvaluator<R, ADS>& mEval = dom.getMultiEvaluator();
//		MultiEvaluation& mevCandidate = mEval.evaluate(*candidate);
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
//			MultiEvaluation popIndFitness = p.getIndMultiEvaluation(ind);
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

//	static bool addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, pair<Pareto<R, ADS>, vector<vector<bool> > >& p, Solution<R, ADS>* candidate, int neighboorsSize)
//
//	{
//		vector<Evaluator<R, ADS>*> v_e = dom.getEvaluators();
//		vector<Evaluation*> fitnessNewInd;
//
//		for (int evalIndex = 0; evalIndex < v_e.size(); evalIndex++)
//		{
//			Evaluation* e = &v_e[evalIndex]->evaluate(*candidate);
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
//			MultiEvaluation popIndFitness = p.first.getIndMultiEvaluation(ind);
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
//	static bool addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, vector<T>& nonDom, T candidate)
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

//	static bool addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, Population<R, ADS>& p, Solution<R, ADS>& s)
//	{
//		vector<Evaluator<R, ADS>*> v_e = dom.getEvaluators();
//		vector<double> fitnessNewInd;
//
//		for (int evalIndex = 0; evalIndex < v_e.size(); evalIndex++)
//		{
//			Evaluation &e = v_e[evalIndex]->evaluate(s);
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
//	static bool addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, pair<Population<R, ADS>, vector<vector<bool> > >& p, Solution<R, ADS>& s, int neighboorsSize)
//	{
//		vector<Evaluator<R, ADS>*> v_e = dom.getEvaluators();
//		vector<double> fitnessNewInd;
//
//		for (int evalIndex = 0; evalIndex < v_e.size(); evalIndex++)
//		{
//			Evaluation &e = v_e[evalIndex]->evaluate(s);
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
//	static void addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, vector<pair<Solution<R>*, MultiEvaluation*> >& nonDom, pair<Solution<R>*, MultiEvaluation*> candidate)
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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class paretoManager
{
public:
	MultiEvaluator<R, ADS>& multiEval;
	ParetoDominance<R, ADS> dom;
	ParetoDominanceWeak<R, ADS> domWeak;
//	Pareto<R, ADS> x_e;

public:

	paretoManager(MultiEvaluator<R, ADS>& _multiEval) :
			multiEval(_multiEval), dom(ParetoDominance<R, ADS>(_multiEval)), domWeak(ParetoDominanceWeak<R, ADS>(_multiEval))
	{
	}

	virtual ~paretoManager()
	{

	}

//	virtual Pareto<R, ADS>& getParetoInsideManager()
//	{
//		return x_e;
//	}

//	MultiEvaluator<R, ADS>& getMultiEvaluator()
//	{
//		return multiEval;
//	}

//	//Special addSolution used in the 2PPLS speedUp
//	bool addSolution(Solution<R, ADS>* candidate)
//	{
//		MultiEvaluation& mev = multiEval.evaluate(*candidate);
//		bool added = addSolution(candidate, &mev);
//		delete &mev;
//		return added;
//	}
//
//	virtual bool addSolution(const Solution<R, ADS>* candidate, const MultiEvaluation* mev)
//	{
//		cout << "Something wrong has happen! \n It is inside addSolution candidate,mev! \n This should be reimplemented" << endl;
//		getchar();
//		exit(1);
////		return false;
//	}

	bool addSolution(Pareto<R, ADS>& p, const Solution<R, ADS>& candidate)
	{
		MultiEvaluation mev(std::move(multiEval.evaluateSolution(candidate)));
		bool added = addSolutionWithMEV(p, candidate, mev);

		return added;
	}

	virtual bool addSolutionWithMEV(Pareto<R, ADS>& p, const Solution<R, ADS>& candidate, const MultiEvaluation& candidateMev)
	{
		bool added = true;
		for (int ind = 0; ind < (int) p.size(); ind++)
		{
			const MultiEvaluation& popIndFitness = p.getIndMultiEvaluation(ind);

			if (domWeak.dominates(popIndFitness, candidateMev))
				return false;

			if (dom.dominates(candidateMev, popIndFitness))
			{
				p.erase(ind);
				ind--;
			}

		}
		if (added)
			p.add_indWithMev(candidate, candidateMev);

		return added;
	}

	bool addSolutionWithMEVReevaluation(Pareto<R, ADS>& p, Solution<R, ADS>& candidate, MultiEvaluation& candidateMev)
	{
		multiEval.reevaluateSolutionMEV(candidateMev, candidate);
		bool added = addSolutionWithMEV(p, candidate, candidateMev);

		return added;
	}

	bool checkDominance(const Pareto<R, ADS>& p)
	{
		Pareto<R, ADS> pFiltered;
		int nInd = p.size();
		for (int ind = 0; ind < nInd; ind++)
			addSolutionWithMEV(pFiltered, p.getNonDominatedSol(ind), p.getIndMultiEvaluation(ind));

		if ((int) pFiltered.size() == nInd)
			return true;

		cout << "CheckDominance, inside MOSearch, found dominated solution inside the Pareto!" << endl;
		exit(1);
		return false;
	}

//	virtual bool checkDominance(Pareto<R, ADS>& p, MultiEvaluation* candidateMev, vector<MoveCost*>& candidateMovCost)
//	{
//		MultiEvaluation* tempMev = new MultiEvaluation(*candidateMev);
//
//		bool checkedValue = checkDominance(p, tempMev);
//
//		return checkedValue;
//	}
//
//	virtual bool checkDominance(Pareto<R, ADS>& p, MultiEvaluation* candidateMev)
//	{
//		for (int ind = 0; ind < x_e.size(); ind++)
//		{
//			MultiEvaluation popIndFitness = x_e.getIndMultiEvaluation(ind);
//			if (domWeak.dominates(popIndFitness, *candidateMev))
//				return false;
//		}
//		return true;
//	}

//	template<class T>
//	bool addSolution(vector<T>& nonDom, T candidate)
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

	//	template<class T>
	//	bool addSolution(vector<T*>& nonDom, T* candidate)
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

};

}

#endif /* OPTFRAME_PARETO_HPP_ */
