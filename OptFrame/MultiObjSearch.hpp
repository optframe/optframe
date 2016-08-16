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

#ifndef OPTFRAME_MULTI_OBJ_SEARCH_HPP_
#define OPTFRAME_MULTI_OBJ_SEARCH_HPP_

#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

#include "Solution.hpp"
#include "Population.hpp"
#include "Evaluation.hpp"
#include "Direction.hpp"

#include "MultiEvaluator.hpp"

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

public:

	Pareto()
	{
	}

	Pareto(const Pareto<R, ADS>& _pf)
	{
		unsigned sizeNewPop = _pf.paretoSet.size();
		for (unsigned i = 0; i < sizeNewPop; i++)
		{
			this->push_back(&_pf.getNonDominatedSol(i), &_pf.getIndMultiEvaluation(i));
		}
	}

	virtual ~Pareto()
	{
		this->clear();
	}

	void push_back(Solution<R, ADS>* s, vector<Evaluation*>& v_e)
	{
		cout << "something is called inside strange push_back" << endl;
		getchar();
		paretoSet.push_back(&s->clone()); // clone, otherwise it is deleted
		paretoFront.push_back(new MultiEvaluation(v_e));
	}

	void push_back(Solution<R, ADS>* s, MultiEvaluation* mev)
	{
		paretoSet.push_back(&s->clone());
		paretoFront.push_back(&mev->clone());
	}

	unsigned size()
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
		paretoSet = pSNew;
	}

	void setParetoFront(vector<MultiEvaluation*> pFNew)
	{
		paretoFront = pFNew;
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

	MultiEvaluation& getIndMultiEvaluation(int ind)
	{
		return *paretoFront[ind];
	}

	MultiEvaluation& getIndMultiEvaluation(int ind) const
	{
		return *paretoFront[ind];
	}

	MultiEvaluation& getCloneIndMultiEvaluation(int ind) const
	{
		return paretoFront[ind]->clone();
	}

	void erase(unsigned pos)
	{
		delete paretoSet[pos];
		for (int e = 0; e < paretoFront[pos]->size(); e++)
			delete &paretoFront[pos]->at(e);
//		delete paretoFront[pos]; // invalid error valgrid 8
		paretoSet.erase(paretoSet.begin() + pos);
		paretoFront.erase(paretoFront.begin() + pos);
//		cout<<"revemod finished.."<<endl;
	}

	virtual Pareto<R, ADS>& clone() const
	{
		return *new Pareto<R, ADS>(*this);
	}

	//How to change P to const ? TODO

	virtual Pareto<R, ADS>& operator=(const Pareto<R, ADS>& pCopy)
	{
		if (&pCopy == this) // auto ref check
			return *this;

		this->clear();

		unsigned sizeNewPop = pCopy.paretoSet.size();

		for (unsigned i = 0; i < sizeNewPop; i++)
			this->push_back(&pCopy.getNonDominatedSol(i), &pCopy.getIndMultiEvaluation(i));

		return (*this);
	}

	void clear()
	{
		for (unsigned i = 0; i < paretoSet.size(); i++)
		{
			delete paretoSet[i];
			for (int e = 0; e < paretoFront[i]->size(); e++)
			{
				delete &paretoFront[i]->at(e);
			}
//			delete paretoFront[i];
		}

		paretoSet.clear();
		paretoFront.clear();
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

	// T. Lust et al (method addSolution)

	// class T must be handled by ParetoDominance operators (candidate: vector<double>, vector<Evaluation*>, MultiEvaluation*)

	template<class T>
	static bool addSolution(vector<Direction*>& vDir, vector<T*>& nonDom, T* candidate)
	{
		ParetoDominance<R, ADS> dom(vDir);
		ParetoDominanceWeak<R, ADS> domWeak(vDir);
		return addSolution(dom, domWeak, nonDom, candidate);
	}

	template<class T>
	static bool addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, vector<T*>& nonDom, T* candidate)
	{
		for (int ind = 0; ind < nonDom.size(); ind++)
		{
			if (domWeak.dominates(*nonDom.at(ind), *candidate))
				return false;

			if (dom.dominates(*candidate, *nonDom.at(ind)))
			{
				nonDom.erase(nonDom.begin() + ind);
				ind--;
			}
		}

		nonDom.push_back(candidate);
		return true;
	}

	static bool addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, Pareto<R, ADS>& p, Solution<R, ADS>* candidate)
	{
		vector<Evaluator<R, ADS>*> v_e = dom.getEvaluators();
		vector<Evaluation*> fitnessNewInd;

		for (int eI = 0; eI < v_e.size(); eI++)
		{
			Evaluation* e = &v_e[eI]->evaluate(*candidate);

			if (!e->isFeasible())
			{
				for (int eIA = 0; eIA < fitnessNewInd.size(); eIA++)
					delete fitnessNewInd[eIA];
				delete e;

				return false;
			}

			fitnessNewInd.push_back(e);
		}

		bool added = true;
		for (int ind = 0; ind < p.size(); ind++)
		{
			MultiEvaluation popIndFitness = p.getIndMultiEvaluation(ind);

			if (domWeak.dominates(popIndFitness, fitnessNewInd))
			{
				for (int eI = 0; eI < fitnessNewInd.size(); eI++)
					delete fitnessNewInd[eI];
				return false;
			}

			if (dom.dominates(fitnessNewInd, popIndFitness))
			{
				p.erase(ind);
				ind--;
			}

		}
		if (added == true)
			p.push_back(candidate, fitnessNewInd);

		for (int eI = 0; eI < fitnessNewInd.size(); eI++)
			delete fitnessNewInd[eI];

		return added;
	}

	static bool addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, pair<Pareto<R, ADS>, vector<vector<bool> > >& p, Solution<R, ADS>* candidate, int neighboorsSize)

	{
		vector<Evaluator<R, ADS>*> v_e = dom.getEvaluators();
		vector<Evaluation*> fitnessNewInd;

		for (int evalIndex = 0; evalIndex < v_e.size(); evalIndex++)
		{
			Evaluation* e = &v_e[evalIndex]->evaluate(*candidate);

			if (!e->isFeasible())
			{
				for (int eIA = 0; eIA < fitnessNewInd.size(); eIA++)
					delete fitnessNewInd[eIA];
				delete e;
				return false;
			}

			fitnessNewInd.push_back(e);
		}

		bool added = true;
		for (int ind = 0; ind < p.first.size(); ind++)
		{
			MultiEvaluation popIndFitness = p.first.getIndMultiEvaluation(ind);

			if (domWeak.dominates(popIndFitness, fitnessNewInd))
			{
				for (int eI = 0; eI < fitnessNewInd.size(); eI++)
					delete fitnessNewInd[eI];
				return false;
			}

			if (dom.dominates(fitnessNewInd, popIndFitness))
			{
				p.first.erase(ind);
				p.second.erase(p.second.begin() + ind);
				ind--;
			}

		}

		if (added == true)
		{
			p.first.push_back(candidate, fitnessNewInd);
			vector<bool> neigh;
			for (int n = 0; n < neighboorsSize; n++)
				neigh.push_back(false);
			p.second.push_back(neigh);
		}

		for (int eI = 0; eI < fitnessNewInd.size(); eI++)
			delete fitnessNewInd[eI];

		return added;

	}

	template<class T>
	static bool addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, vector<T>& nonDom, T candidate)
	{
		for (int ind = 0; ind < nonDom.size(); ind++)
		{
			if (domWeak.dominates(nonDom[ind], candidate))
				return false;

			if (dom.dominates(candidate, nonDom[ind]))
			{
				nonDom.erase(nonDom.begin() + ind);
				ind--;
			}
		}

		nonDom.push_back(candidate);
		return true;
	}

	static bool addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, Population<R, ADS>& p, Solution<R, ADS>& s)
	{
		vector<Evaluator<R, ADS>*> v_e = dom.getEvaluators();
		vector<double> fitnessNewInd;

		for (int evalIndex = 0; evalIndex < v_e.size(); evalIndex++)
		{
			Evaluation &e = v_e[evalIndex]->evaluate(s);

			if (!e.isFeasible())
			{
				delete &e;
				return false;
			}

			fitnessNewInd.push_back(e.evaluation());
			delete &e;
		}

		bool added = true;
		for (int ind = 0; ind < p.size(); ind++)
		{

			vector<double> popIndFitness = p.getFitness(ind);
			if (domWeak.dominates(popIndFitness, fitnessNewInd))
				return false;

			if (dom.dominates(fitnessNewInd, popIndFitness))
			{
				delete &p.remove(ind);
				ind--;
			}

		}
		if (added == true)
			p.push_back(s, fitnessNewInd);

		return added;
	}

	//Special addSolution used in the 2PPLS speedUp
	static bool addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, pair<Population<R, ADS>, vector<vector<bool> > >& p, Solution<R, ADS>& s, int neighboorsSize)
	{
		vector<Evaluator<R, ADS>*> v_e = dom.getEvaluators();
		vector<double> fitnessNewInd;

		for (int evalIndex = 0; evalIndex < v_e.size(); evalIndex++)
		{
			Evaluation &e = v_e[evalIndex]->evaluate(s);

			if (!e.isFeasible())
			{
				delete &e;
				return false;
			}

			fitnessNewInd.push_back(e.evaluation());
			delete &e;
		}

		bool added = true;
		for (int ind = 0; ind < p.first.size(); ind++)
		{

			vector<double> popIndFitness = p.first.getFitness(ind);
			if (domWeak.dominates(popIndFitness, fitnessNewInd))
				return false;

			if (dom.dominates(fitnessNewInd, popIndFitness))
			{
				delete &p.first.remove(ind);
				p.second.erase(p.second.begin() + ind);
				ind--;
			}

		}

		if (added == true)
		{
			p.first.push_back(s, fitnessNewInd);
			vector<bool> neigh;
			for (int n = 0; n < neighboorsSize; n++)
				neigh.push_back(false);
			p.second.push_back(neigh);
		}

		return added;
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

	static void addSolution(ParetoDominance<R, ADS>& dom, ParetoDominanceWeak<R, ADS>& domWeak, vector<pair<Solution<R>*, MultiEvaluation*> >& nonDom, pair<Solution<R>*, MultiEvaluation*> candidate)
	{
		for (int ind = 0; ind < nonDom.size(); ind++)
		{
			if (domWeak.dominates(*nonDom.at(ind).second, *candidate.second))
				return;

			if (dom.dominates(*candidate.second, *nonDom.at(ind).second))
			{
				nonDom.erase(nonDom.begin() + ind);
				ind--;
			}
		}

		nonDom.push_back(candidate);
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class paretoManager
{
public:
	MultiEvaluator<R, ADS>& multiEval;
	ParetoDominance<R, ADS> dom;
	ParetoDominanceWeak<R, ADS> domWeak;
	Pareto<R, ADS> x_e;

public:

	paretoManager(MultiEvaluator<R, ADS>& _multiEval) :
			multiEval(_multiEval), dom(ParetoDominance<R, ADS>(*_multiEval.getEvaluators())), domWeak(ParetoDominanceWeak<R, ADS>(*_multiEval.getEvaluators()))
	{
	}

	virtual ~paretoManager()
	{

	}

	virtual Pareto<R, ADS>& getParetoInsideManager()
	{
		return x_e;
	}
//	MultiEvaluator<R, ADS>& getMultiEvaluator()
//	{
//		return multiEval;
//	}

	//Special addSolution used in the 2PPLS speedUp
	bool addSolution(Solution<R, ADS>* candidate)
	{
		MultiEvaluation& mev = multiEval.evaluate(*candidate);
		bool added = addSolution(candidate, &mev);
		mev.clear();
//		delete &mev;
		return added;
	}

	virtual bool addSolution(Solution<R, ADS>* candidate, MultiEvaluation* mev)
	{
		cout << "Something wrong has happen! \n It is inside addSolution candidate,mev! \n This should be reimplemented" << endl;
		exit(1);
//		return false;
	}

	bool addSolution(Pareto<R, ADS>& p, Solution<R, ADS>* candidate)
	{
		MultiEvaluation& mev = multiEval.evaluate(*candidate);
		bool added = addSolution(p, candidate, &mev);
		mev.clear();
//		delete &mev;
		return added;

	}

	virtual bool addSolution(Pareto<R, ADS>& p, Solution<R, ADS>* candidate, MultiEvaluation* candidateMev)
	{
		bool added = true;
		for (int ind = 0; ind < p.size(); ind++)
		{
			MultiEvaluation popIndFitness = p.getIndMultiEvaluation(ind);

			if (domWeak.dominates(popIndFitness, *candidateMev))
				return false;

			if (dom.dominates(*candidateMev, popIndFitness))
			{
				p.erase(ind);
				ind--;
			}

		}
		if (added == true)
			p.push_back(candidate, candidateMev);

		//Check if it is also not deleted in the origin todo
		//		for (int eI = 0; eI < fitnessNewInd.size(); eI++)
		//			delete fitnessNewInd[eI];

		return added;
	}

	virtual bool checkDominance(Pareto<R, ADS>& p, MultiEvaluation* candidateMev, vector<MoveCost*>& candidateMovCost)
	{
		MultiEvaluation* tempMev = new MultiEvaluation(*candidateMev);

		bool checkedValue = checkDominance(p, tempMev);

		return checkedValue;
	}

	virtual bool checkDominance(Pareto<R, ADS>& p, MultiEvaluation* candidateMev)
	{
		for (int ind = 0; ind < x_e.size(); ind++)
		{
			MultiEvaluation popIndFitness = x_e.getIndMultiEvaluation(ind);
			if (domWeak.dominates(popIndFitness, *candidateMev))
				return false;
		}
		return true;
	}

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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class MOMETRICS
{
protected:
//	vector<Direction*> v_d;
	ParetoDominance<R, ADS>* pDom;
	ParetoDominanceWeak<R, ADS>* pDomWeak;
	Pareto<R, ADS> p;

//	bool addSolution(vector<vector<double> >& p, vector<double>& s)
//	{
//		bool added = true;
//		for (int ind = 0; ind < p.size(); ind++)
//		{
//
//			if (weakDominates(p[ind], s))
//				return false;
//
//			if (dominates(s, p.at(ind)))
//			{
//				p.erase(p.begin() + ind); //Esta perdendo o individuo retornado,tem problema? todo
//				ind--;
//			}
//		}
//
//		if (added == true)
//			p.push_back(s);
//
//		return added;
//	}
//
//	// true if 's1' dominates 's2'
//	virtual bool weakDominates(const vector<double>& s1, const vector<double>& s2)
//	{
//
//		int better = 0;
//		int equals = 0;
//
//		for (int e = 0; e < s1.size(); e++)
//		{
//			if (v_e[e]->betterThan(s1[e], s2[e]))
//				better++;
//
//			if (abs(s1[e] - s2[e]) < 0.0001)
//				equals++;
//		}
//
//		return ((better + equals == v_e.size()));
//	}
//
//	// true if 's1' weak dominates 's2'
//	virtual bool dominates(const vector<double>& s1, const vector<double>& s2)
//	{
//
//		int better = 0;
//		int equals = 0;
//
//		for (int e = 0; e < s1.size(); e++)
//		{
//			if (v_e[e]->betterThan(s1[e], s2[e]))
//				better++;
//
//			if (abs(s1[e] - s2[e]) < 0.0001)
//				equals++;
//		}
//
//		return ((better + equals == v_e.size()) && (better > 0));
//	}

public:

	MOMETRICS(vector<Direction*> _v_d)
	{
		pDom = new ParetoDominance<R, ADS>(_v_d);
		pDomWeak = new ParetoDominanceWeak<R, ADS>(_v_d);
	}

	MOMETRICS(vector<Evaluator<R, ADS>*> _v_e)
	{
		pDom = new ParetoDominance<R, ADS>(_v_e);
		pDomWeak = new ParetoDominanceWeak<R, ADS>(_v_e);
	}

	MOMETRICS(int nObj)
	{
		cout << "Be careful, some methods of MOMETRICS might results in error! \n Direction is being created only as Minimization! " << endl;
		vector<Direction*> v_d;
		for (int o = 0; o < nObj; o++)
		{
			Minimization* m = new Minimization;
			v_d.push_back(m);
		}

		pDom = new ParetoDominance<R, ADS>(v_d);
		pDomWeak = new ParetoDominanceWeak<R, ADS>(v_d);

		//		cout << "Be careful, some methods of MOMETRICS might results in error! \n pDom and pDomWeak were not initialized." << endl;
	}

	File* createFile(string filename)
	{
		File* file;

		try
		{
			file = new File(filename);
		} catch (FileNotFound& f)
		{
			cout << "File '" << filename << "' not found" << endl;
			exit(1);
		}
		return file;
	}

	vector<vector<double> > readPF(string caminho, int nTests, int nOF)
	{
		vector<vector<double> > D;

		for (int e = 0; e < nTests; e++)
		{
			stringstream ss;
			ss << caminho;

			Scanner scanner(createFile(caminho));
			while (scanner.hasNext())
			{
				vector<double> ind;
				for (int o = 0; o < nOF; o++)
					ind.push_back(scanner.nextDouble());

				p.addSolution(*pDom, *pDomWeak, D, ind);
			}

		}

		return D;
	}

	void addSol(vector<vector<double> >& D, vector<double> ind)
	{
		p.addSolution(*pDom, *pDomWeak, D, ind);
	}

	vector<vector<double> > unionSets(vector<vector<double> > D1, vector<vector<double> > D2)
	{
		vector<vector<double> > ref = D1;

		for (int ind = 0; ind < D2.size(); ind++)
			p.addSolution(*pDom, *pDomWeak, ref, D2[ind]);

		return ref;
	}

	vector<vector<double> > getParetoEvaluations(Pareto<R, ADS>& pf, int nEv)
	{
		vector<MultiEvaluation*> vEval = pf.getParetoFront();
		int nObtainedParetoSol = vEval.size();

		vector<vector<double> > paretoDoubleEval;

		for (int i = 0; i < nObtainedParetoSol; i++)
		{
			int nObtainedParetoSol = vEval.size();
			vector<double> solEvaluations;
			for (int ev = 0; ev < nEv; ev++)
				solEvaluations.push_back(vEval[i]->at(ev).getObjFunction());

			paretoDoubleEval.push_back(solEvaluations);
		}
		return paretoDoubleEval;
	}

	vector<vector<double> > createParetoSet(vector<vector<double> > D2)
	{
		vector<vector<double> > ref;

		for (int ind = 0; ind < D2.size(); ind++)
			p.addSolution(*pDom, *pDomWeak, ref, D2[ind]);

		return ref;
	}

	int cardinalite(vector<vector<double> > D, vector<vector<double> > ref)
	{
		int card = 0;
		for (int i = 0; i < D.size(); i++)
		{
			for (int j = 0; j < ref.size(); j++)
			{

				if (pDomWeak->dominates(D[i], ref[j]))
				{
					card++;
					j = ref.size();
				}
			}
		}

		return card;
	}

	double setCoverage(vector<vector<double> > a, vector<vector<double> > b)
	{
		double cover = 0;

		for (int i = 0; i < b.size(); i++)
		{
			for (int j = 0; j < a.size(); j++)
			{
				bool wD = pDomWeak->dominates(a[j], b[i]);
				if (wD)
				{
					j = a.size();
					cover += 1;
					//cout<<" i = "<<i<<" j = "<<j<<endl;
					//getchar();
				}
			}
		}

		//cout<<" cover = "<<cover<<endl;

		double sCover = cover / b.size();

		return sCover;
	}

	//Delta Metric and Hipervolume are working requires Minimization problems
	double deltaMetric(vector<vector<double> > pareto, vector<double> utopicEval, bool minimization)
	{
		int nSol = pareto.size();
		int nObj = utopicEval.size();

		if (minimization == false)
		{
			for (int i = 0; i < nSol; i++)
				for (int ev = 0; ev < nObj; ev++)
					pareto[i][ev] *= -1;
		}

		if (pareto.size() == 1)
			return 1;

		vector<double> vDist;
		vector<double> minEval(nObj, 1000000);
		double dMean = 0;
		for (int nS = 0; nS < nSol; nS++)
		{
			//vector with the evaluation of solution nS
			vector<double> eval = pareto[nS];
			for (int e = 0; e < nObj; e++)
			{
				if (eval[e] < minEval[e])
					minEval[e] = eval[e];
			}

			double minDist = 10000000;
			for (int nS2 = 0; nS2 < pareto.size(); nS2++)
			{
				double dist = 0;
				if (nS2 != nS)
				{
					vector<double> eval2 = pareto[nS2];
					for (int e = 0; e < nObj; e++)
						dist += pow(eval[e] - eval2[e], 2);
					dist = sqrt(dist);
					if (dist < minDist)
						minDist = dist;
				}

			}

			vDist.push_back(minDist);
			dMean += minDist;

		}
		dMean /= nSol;

		double dUtopic = 0;
		for (int e = 0; e < nObj; e++)
		{
			dUtopic += pow(minEval[e] - utopicEval[e], 2);
			//cout<<minEval[e]<<endl;
			//cout<<utopicEval[e]<<endl;
		}

		dUtopic = sqrt(dUtopic);
		//cout<<dUtopic<<endl;
		//getchar();

		double sumDist = 0;
		for (int nS = 0; nS < nSol; nS++)
		{
			sumDist += vDist[nS] - dMean;
		}

		double delta = (dUtopic + sumDist) / (nSol * dMean + dUtopic);
		return delta;
		//cout << "delta = " << delta << endl;
		//getchar();

	}

	double spacing(vector<vector<double> > a)
	{
		double ss = 0;
		vector<double> distMin;
		int N = a.size();
		int m = a[0].size();
		for (int i = 0; i < a.size(); i++)
		{
			double di = 100000000;

			for (int j = 0; j < a.size(); j++)
			{
				if (i != j)
				{
					double diMin = 0;
					for (int eval = 0; eval < m; eval++)
						diMin += abs(a[i][eval] - a[j][eval]);

					if (diMin < di)
						di = diMin;
				}
			}
			distMin.push_back(di);
		}

		double dMean = 0;

		for (int i = 0; i < N; i++)
			dMean += distMin[i];

		dMean = dMean / N;

		for (int i = 0; i < N; i++)
			ss += pow(distMin[i] - dMean, 2);

		ss = ss / N;
		ss = sqrt(ss);

		return ss;
	}

	vector<double> spacingMultiplePF(vector<vector<vector<double> > > D)
	{
		vector<double> spacings;
		for (int frente = 0; frente < D.size(); frente++)
		{
			vector<vector<double> > a = D[frente];
			double ss = spacing(a);
			spacings.push_back(ss);
		}
		return spacings;
	}

	char* execCommand(const char* command)
	{

		FILE* fp;
		char* line = NULL;
		// Following initialization is equivalent to char* result = ""; and just
		// initializes result to an empty string, only it works with
		// -Werror=write-strings and is so much less clear.
		char* result = (char*) calloc(1, 1);
		size_t len = 0;

		fflush(NULL);
		fp = popen(command, "r");
		if (fp == NULL)
		{
			printf("Cannot execute command:\n%s\n", command);
			return NULL;
		}

		while (getline(&line, &len, fp) != -1)
		{
			// +1 below to allow room for null terminator.
			result = (char*) realloc(result, ::strlen(result) + strlen(line) + 1);
			// +1 below so we copy the final null terminator.
			strncpy(result + strlen(result), line, strlen(line) + 1);
			free(line);
			line = NULL;
		}

		fflush(fp);
		if (pclose(fp) != 0)
		{
			perror("Cannot close stream.\n");
		}

		return result;
	}

	//Delta Metric and Hipervolume are working requires Minimization problems
	double hipervolumeWithExecRequested(vector<vector<double> > pareto, vector<double> refPoints, bool minimization)
	{
		int nSol = pareto.size();
		int nObj = pareto[0].size();

		if (minimization == false)
		{
			for (int i = 0; i < nSol; i++)
				for (int ev = 0; ev < nObj; ev++)
					pareto[i][ev] *= -1;
		}

		string tempFile = "tempFileHipervolueFunc";
		FILE* fTempHV = fopen(tempFile.c_str(), "w");

		for (int s = 0; s < nSol; s++)
		{
			for (int o = 0; o < nObj; o++)
			{
				fprintf(fTempHV, "%.7f\t", pareto[s][o]);
			}
			fprintf(fTempHV, "\n");
		}

		fclose(fTempHV);
		stringstream ss;
		ss << "./hv\t -r \"";
		for (int o = 0; o < nObj; o++)
			ss << refPoints[o] << " ";
		ss << "\" \t" << tempFile.c_str();
		string hvValueString = execCommand(ss.str().c_str());
		double hvValue = atof(hvValueString.c_str());
		return hvValue;
	}

	double hipervolumeWithExecRequested(string filename, vector<double> refPoints)
	{
		stringstream ss;
		ss << "./hv\t -r \"";
		for (int o = 0; o < refPoints.size(); o++)
			ss << refPoints[o] << " ";
		ss << "\" \t" << filename.c_str();
		string hvValueString = execCommand(ss.str().c_str());
		double hvValue = atof(hvValueString.c_str());
		return hvValue;
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class MultiObjSearch: public Component
{
public:

	MultiObjSearch()
	{
	}

	virtual ~MultiObjSearch()
	{
	}

	virtual Pareto<R, ADS>* search(double timelimit = 100000000, double target_f = 0, Pareto<R, ADS>* _pf = NULL) = 0;

	virtual string log()
	{
		return "Empty heuristic log.";
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << "MultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class MultiObjSearchBuilder: public ComponentBuilder<R, ADS>
{
public:
	virtual ~MultiObjSearchBuilder()
	{
	}

	virtual MultiObjSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS>::idComponent() << "MultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* OPTFRAME_MULTI_OBJ_SEARCH_HPP_ */

