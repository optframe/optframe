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

#ifndef OPTFRAME_RKGA_HPP_
#define OPTFRAME_RKGA_HPP_

#include <algorithm>

#include "../../SingleObjSearch.hpp"
#include "../../Population.hpp"
#include "../../InitialPopulation.h"

#include "DecoderRandomKeys.hpp"
#include "Crossover.hpp"
#include "Mutation.hpp"
#include "Selection.hpp"
#include "Elitism.hpp"

// RKGA - Random Key Genetic Algorithms
// Bean, J.C.: Genetic algorithms and random keys for
// sequencing and optimization. ORSA J. Comput. 6, 154-160 (1994)
// http://pubsonline.informs.org/doi/abs/10.1287/ijoc.6.2.154
namespace optframe
{

class RandomKeysInitPop: public InitialPopulation<random_keys>
{
private:
	int sz;

public:
	RandomKeysInitPop(int size) :
			sz(size)
	{
	}

	virtual Population<random_keys>& generatePopulation(unsigned populationSize) final
	{
		Population<random_keys>* pop = new Population<random_keys>;

		for (unsigned i = 0; i < populationSize; i++)
		{
			vector<double>* d = new vector<double>(sz);
			for (int j = 0; j < sz; j++)
				d->at(j) = (rand() % 100000) / 100000.0;
			Solution<random_keys>* sol = new Solution<random_keys>(d);
			pop->push_back(sol);
		}

		return *pop;
	}
};

template<class R = random_keys>
class RKGA: public SingleObjSearch<random_keys>
{
protected:
	DecoderRandomKeys<R>& decoder;
	Evaluator<R>* evaluator; // Check to avoid memory leaks

	InitialPopulation<random_keys>& initPop;
	int sz; // Check to avoid memory leaks

	unsigned popSize, eliteSize, randomSize;
	unsigned numGenerations;

public:

	RKGA(DecoderRandomKeys<R>& _decoder, InitialPopulation<random_keys>& _initPop, unsigned numGenerations, unsigned _popSize, double fracTOP, double fracBOT) :
			decoder(_decoder), evaluator(nullptr), initPop(_initPop), sz(-1), popSize(_popSize), eliteSize(fracTOP * _popSize), randomSize(fracBOT * _popSize)
	{
		this->numGenerations = numGenerations;
		assert(eliteSize < popSize);
		assert(randomSize + eliteSize < popSize);
	}

	RKGA(DecoderRandomKeys<R>& _decoder, int key_size, unsigned numGenerations, unsigned _popSize, double fracTOP, double fracBOT) :
			decoder(_decoder), evaluator(nullptr), initPop(*new RandomKeysInitPop(key_size)), sz(key_size), popSize(_popSize), eliteSize(fracTOP * _popSize), randomSize(fracBOT * _popSize)
	{
		this->numGenerations = numGenerations;
		assert(eliteSize < popSize);
		assert(randomSize + eliteSize < popSize);
	}

	RKGA(Evaluator<random_keys>& _evaluator, int key_size, unsigned numGenerations, unsigned _popSize, double fracTOP, double fracBOT) :
			decoder(*new DecoderRandomKeysEvaluator(_evaluator)), evaluator(&_evaluator), initPop(*new RandomKeysInitPop(key_size)), sz(key_size), popSize(_popSize), eliteSize(fracTOP * _popSize), randomSize(fracBOT * _popSize)
	{
		this->numGenerations = numGenerations;
		assert(eliteSize < popSize);
		assert(randomSize + eliteSize < popSize);
	}

	virtual ~RKGA()
	{
		if (evaluator)
			delete &decoder;
		if (sz != -1)
			delete &initPop;
	}

	void decodePopulation(Population<random_keys>& p)
	{
		for (unsigned i = 0; i < p.size(); ++i)
		{
			//p.at(i).print();
			random_keys& rk = p.at(i).getR();
			pair<Evaluation&, Solution<R>*> pe = decoder.decode(rk);
			p.setFitness(i, pe.first.evaluation());
			delete &pe.first;
			if(pe.second)
				delete pe.second;
		}
	}

	virtual Solution<random_keys>& cross(const Population<random_keys>& pop) const
	{
		assert(sz > 0); // In case of using InitPop, maybe must receive a Selection or Crossover object...

		const Solution<random_keys>& p1 = pop.at(rand() % pop.size());
		const Solution<random_keys>& p2 = pop.at(rand() % pop.size());

		random_keys* v = new random_keys(sz, 0.0);
		for (int i = 0; i < sz; i++)
			if (rand() % 2 == 0)
				v->at(i) = p1.getR()[i];
			else
				v->at(i) = p2.getR()[i];
		return *new Solution<random_keys>(v);
	}

	pair<Solution<random_keys>&, Evaluation&>* search(double timelimit = 100000000, double target_f = 0, const Solution<random_keys>* _s = nullptr, const Evaluation* _e = nullptr)
	{
		// count generations
		int count_gen = 0;

		// initialize population
		Population<random_keys>* p = &initPop.generatePopulation(popSize);
		// decode population
		decodePopulation(*p);

		// sort population
		p->sort(decoder.isMinimization());

		evtype best_f = p->getSingleFitness(0);
		if(Component::information)
			cout << "RKGA: best fitness at initial population: " << best_f << endl;

		// stop by number of generations.
		// other stopping criteria? TIME, GAP, ...
		while (count_gen < int(numGenerations))
		{
			// create mutants in new population
			Population<random_keys>* nextPop = &initPop.generatePopulation(randomSize);

			// move 'eliteSize' elements to new population
			for (unsigned i = 0; i < eliteSize; i++)
				nextPop->push_back(p->at(i));
			// TODO: we could get the current evaluations and avoid new decodifications

			// populate the rest
			while (nextPop->size() < popSize)
			{
				Solution<random_keys>* s = &cross(*p);
				nextPop->push_back(s);
			}

			delete p; // KILL ALL INDIVIDUALS
			p = nextPop;

			// decode solutions
			decodePopulation(*p);

			// sort population
			p->sort(decoder.isMinimization());

			evtype pop_best = p->getSingleFitness(0);

			if(decoder.isMinimization() && pop_best < best_f)
			{
				best_f = pop_best;
				cout << "RKGA: best fitness " << best_f << " at generation " << count_gen << endl;
			}

			if(!decoder.isMinimization() && pop_best > best_f)
			{
				best_f = pop_best;
				if(Component::information)
					cout << "RKGA: best fitness " << best_f << " at generation " << count_gen << endl;
			}

			count_gen++;
		}

		// sort to get best (not necessary)
		p->sort(decoder.isMinimization());

		Solution<random_keys>& best = p->remove(0);
		pair<Evaluation&, Solution<R>*> pe = decoder.decode(best.getR());
		Evaluation& e = pe.first;
		// ignoring second value
		if(pe.second)
			delete pe.second;

		delete p;

		return new pair<Solution<random_keys>&, Evaluation&>(best, e);
	}


};

}

#endif /*OPTFRAME_RKGA_HPP_*/
