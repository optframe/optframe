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

#ifndef OPTFRAME_GENETICALGORITHM_HPP_
#define OPTFRAME_GENETICALGORITHM_HPP_

#include <algorithm>

#include "../../SingleObjSearch.hpp"
#include "../../MultiSolution.hpp"

#include "InitialPopulation.hpp"

#include "Crossover.hpp"

#include "Mutation.hpp"
#include "Selection.hpp"
#include "BinarySelection.hpp"

#include "EA.h"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class BasicGeneticAlgorithm: public SingleObjSearch<R, ADS, DS>, public EA
{
protected:
	typedef Solution<R, ADS> Chromossome;
	typedef MultiSolution<R, ADS> Population;

	Evaluator<R, ADS, DS>& evaluator;

private:

	bool maxim; // true if maximizing
	unsigned popSize;

	double pCross, pMut, pLS;

	unsigned numGenerations;

	InitialPopulation<R, ADS>& initPop;

	Selection<R, ADS, DS>& selection;
	Crossover<R, ADS>& cross;
	Mutation<R, ADS, DS>& mut;
	LocalSearch<R, ADS, DS>& ls;

	RandGen& rg;

public:

	float assert01(float f)
	{
		if ((f < 0) || (f > 1))
			return 0;
		else
			return f;
	}

	BasicGeneticAlgorithm(Evaluator<R, ADS, DS>& _evaluator, InitialPopulation<R, ADS>& _initPop, unsigned populationSize, float crossoverRate, float mutationRate, float _pLS, unsigned numGenerations, Selection<R, ADS, DS>& _selection, Crossover<R, ADS>& _cross, Mutation<R, DS>& _mut, LocalSearch<R, ADS, DS>& _ls, RandGen& _rg) :
			evaluator(_evaluator), initPop(_initPop), selection(_selection), cross(_cross), mut(_mut), ls(_ls), rg(_rg)
	{
		maxim = !evaluator.isMinimization();
		pCross = assert01(crossoverRate);
		pMut = assert01(mutationRate);
		pLS = assert01(_pLS);
		popSize = populationSize;
		if (popSize == 0)
			popSize = 1;
		this->numGenerations = numGenerations;
	}

	void evaluate(const MultiSolution<R, ADS> &p, MultiEvaluation<DS>& mev)
	{
		for (unsigned i = 0; i < p.size(); i++)
			mev.addEvaluation(&evaluator.evaluate(p.at(i)));
	}

	int getBest(const MultiEvaluation<DS>& mev)
	{
		if (mev.size() == 0)
			return -1;
		int best = 0;
		for (unsigned i = 1; i < mev.size(); i++)
		{
			if (!maxim && (mev.at(i).evaluation() < mev.at(best).evaluation()))
				best = i;
			if (maxim && (mev.at(i).evaluation() > mev.at(best).evaluation()))
				best = i;
		}
		return best;
	}

	// basic implementation of low diversity scheme that prevents populations of clones (TODO: improve!)
	bool lowDiversity(const MultiSolution<R, ADS> &p, const MultiEvaluation<DS>& mev)
	{
		for(unsigned i=1; i<mev.size(); i++)
			if(mev.at(i-1).evaluation() != mev.at(i).evaluation())
				return false;
		return true;
	}

	/*
	 * In the canonical genetic algorithm, fitness (maximization) is defined by: fi/f
	 * where: fi: is the evaluation associated with the i-th Chromossome.
	 *    f: is the average population evaluation.
	 */

	virtual void setFitness(const MultiSolution<R, ADS> &p, const MultiEvaluation<DS>& mev, vector<double>& fv)
	{
		fv.resize(mev.size());
		for (int i = 0; i < mev.size(); i++)
			fv[i] = mev.at(i).evaluation();

		// convert to maximization
		if (!maxim)
		{
			double lmax = Selection<R, ADS, DS>::getMax(fv);
			for (int i = 0; i < p.size(); i++)
				fv[i] = lmax - fv[i];
		}

		// calculate average
		double sumEvals = Selection<R, ADS, DS>::getSum(fv);
		double avgEvalsPop = sumEvals / p.size();
		if(avgEvalsPop == 0)
			avgEvalsPop = 1;

		for (int i = 0; i < fv.size(); i++)
		{
			fv[i] = (fv[i] / avgEvalsPop);
			if(fv[i] != fv[i]) // is nan
			{
				cout << "Selection::setFitness()::NAN VALUE!" << endl;
				cout << "average=" << avgEvalsPop << endl;
				cout << fv << endl;
				exit(1);
			}
		}

		// normalize to [0,1]
		Selection<R, ADS, DS>::normalize(fv);
	}

	void mayMutate(Chromossome& c, Evaluation<DS>& e)
	{
		double xMut = rg.rand01();
		if (xMut <= pMut) // mutate!
			mut.mutate(c, e);
	}

	void mayLocalSearch(Chromossome& c, Evaluation<DS>& e, double timelimit, double target_f)
	{
		double xLS = rg.rand01();
		if (xLS <= pLS) // local search!
			ls.exec(c, e, timelimit, target_f);
	}

	pair<Solution<R, ADS>&, Evaluation<DS>&>* search(double timelimit = 100000000, double target_f = 0, const Solution<R, ADS>* _s = NULL, const Evaluation<DS>* _e = NULL)
	{
		Timer t;
		cout << id() << "(timelimit=" << timelimit << "; target_f=" << target_f << ")" << endl;
		cout << "Population Size: " << popSize << " Total of Generations: " << numGenerations << endl;
		cout << "Crossover Rate: " << pCross << " Mutation Rate: " << pMut << " Local Search Rate: " << pLS << endl;

		cout << "Generating the Initial Population" << endl;

		MultiSolution<R, ADS>* p = &initPop.generatePopulation(popSize);
		MultiEvaluation<DS>* mev = new MultiEvaluation<DS>;
		evaluate(*p, *mev);
		vector<double> fv;
		setFitness(*p, *mev, fv);
		//cout << fv << " = " << Selection<R, ADS, DS>::getSum(fv) << endl;

		int best = getBest(*mev);

		Chromossome* sStar = new Chromossome(p->at(best));
		Evaluation<DS>* eStar = new Evaluation<DS>(mev->at(best));
		cout << "GA iter=0 ";
		eStar->print();

		unsigned g = 0;

		while ((g < numGenerations) && (evaluator.betterThan(target_f, eStar->evaluation()) && (t.now() < timelimit)))
		{
			if(lowDiversity(*p, *mev))
			{
				//cout << "WARNING: Genetic Algorithm leaving with low diversity at iteration g=" << g << endl;
				//cout << "Try different solution generators or better mutation and crossover operators!" << endl;
				break;
			}

			MultiSolution<R, ADS>* p2 = new MultiSolution<R, ADS>;
			MultiEvaluation<DS>* mev2 = new MultiEvaluation<DS>;

			best = getBest(*mev);
			p2->push_back(&p->at(best).clone());
			mev2->addEvaluation(&mev->at(best).clone());

			if (evaluator.betterThan(mev->at(best), *eStar))
			{
				delete eStar;
				delete sStar;
				eStar = &mev->at(best).clone();
				sStar = &p->at(best).clone();
				cout << "GA iter=" << g << " ";
				eStar->print();
				g = 0;
			}

			while (p2->size() < popSize)
			{
				pair<unsigned, unsigned> idx = selection.select(*p, *mev, fv);
				if (idx.first == idx.second)
				{
					cout << "ERROR IN GENETIC SELECTION! SAME ELEMENTS!" << endl;
					exit(1);
				}

				double xCross = rg.rand01();
				if (xCross > pCross) // do not make any cross
				{
					p2->push_back(p->at(idx.first).clone());
					p2->push_back(p->at(idx.second).clone());
					mev2->addEvaluation(mev->at(idx.first).clone());
					mev2->addEvaluation(mev->at(idx.second).clone());
					continue;
				}

				pair<Chromossome*, Chromossome*> rCross = cross.cross(p->at(idx.first), p->at(idx.second));
				if (!rCross.first && !rCross.second)
				{
					cout << "ERROR IN GENETIC CROSS! NO RESULT!" << endl;
					exit(1);
				}

				Evaluation<DS>* e1 = NULL;
				Evaluation<DS>* e2 = NULL;

				if (rCross.first)
				{
					e1 = &evaluator.evaluate(*rCross.first);
					mayMutate(*rCross.first, *e1);
					mayLocalSearch(*rCross.first, *e1, timelimit, target_f);
					p2->push_back(rCross.first);
					mev2->addEvaluation(e1);
				}

				if (rCross.second)
				{
					e2 = &evaluator.evaluate(*rCross.second);
					mayMutate(*rCross.second, *e2);
					mayLocalSearch(*rCross.second, *e2, timelimit, target_f);
					p2->push_back(rCross.second);
					mev2->addEvaluation(e2);
				}
			}

			while (p2->size() > popSize)
			{
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

		return new pair<Solution<R, ADS>&, Evaluation<DS>&>(*sStar, *eStar);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS, DS>::idComponent() << ":" << EA::family() << ":BasicGeneticAlgorithm";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

}
;

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class BasicGeneticAlgorithmBuilder: public EA, public SingleObjSearchBuilder<R, ADS, DS>
{
public:
	virtual ~BasicGeneticAlgorithmBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS, DS>* build(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		Evaluator<R, ADS, DS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		InitialPopulation<R, ADS>* initPop;
		hf.assign(initPop, scanner.nextInt(), scanner.next()); // reads backwards!

		int popSize = scanner.nextInt();
		float pCross = scanner.nextFloat();
		float pMut = scanner.nextFloat();
		float pLS = scanner.nextFloat();
		int nGen = scanner.nextInt();

		Selection<R, ADS, DS>* sel;
		hf.assign(sel, scanner.nextInt(), scanner.next()); // reads backwards!

		Crossover<R, ADS, DS>* cross;
		hf.assign(cross, scanner.nextInt(), scanner.next()); // reads backwards!

		Mutation<R, ADS, DS>* mut;
		hf.assign(mut, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();
		pair<LocalSearch<R, ADS, DS>*, std::string> method;
		method = hf.createLocalSearch(rest);
		LocalSearch<R, ADS, DS>* h = method.first;
		scanner = Scanner(method.second);

		return new BasicGeneticAlgorithm<R, ADS, DS>(*eval, *initPop, popSize, pCross, pMut, pLS, nGen, *sel, *cross, *mut, *h, hf.getRandGen());
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(InitialPopulation<R, ADS>::idComponent(), "generator for initial population"));
		params.push_back(make_pair("OptFrame:int", "population size"));
		params.push_back(make_pair("OptFrame:float", "cross probability"));
		params.push_back(make_pair("OptFrame:float", "mutation probability"));
		params.push_back(make_pair("OptFrame:float", "local search probability"));
		params.push_back(make_pair("OptFrame:int", "number of generations without improvement"));
		params.push_back(make_pair(Selection<R, ADS, DS>::idComponent(), "selection"));
		params.push_back(make_pair(Crossover<R, ADS, DS>::idComponent(), "crossover"));
		params.push_back(make_pair(Mutation<R, ADS, DS>::idComponent(), "mutation"));
		params.push_back(make_pair(LocalSearch<R, ADS, DS>::idComponent(), "local search"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicGeneticAlgorithmBuilder<R, ADS, DS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<R, ADS, DS>::idComponent() << ":" << EA::family() << ":BasicGeneticAlgorithm";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_GENETICALGORITHM_HPP_*/
