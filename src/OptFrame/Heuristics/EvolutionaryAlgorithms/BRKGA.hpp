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

#ifndef OPTFRAME_BRKGA_HPP_
#define OPTFRAME_BRKGA_HPP_

#include <algorithm>

#include "../../SingleObjSearch.hpp"
#include "../../Population.hpp"
#include "../../InitialPopulation.h"

#include "RKGA.hpp"

// BRKGA - Biased-Random Key Genetic Algorithms

namespace optframe
{

template<class R=random_keys>
class BRKGA: public RKGA<R>
{
protected:
	double probElitism;

public:

	BRKGA(DecoderRandomKeys<R>& _decoder, InitialPopulation<random_keys>& _initPop, unsigned numGen, unsigned _popSize, double fracTOP, double fracBOT, double probElielitismRate, double _probElitism) :
		RKGA<R>(_decoder, _initPop, numGen, _popSize, fracTOP, fracBOT), probElitism(_probElitism)
	{
		assert(probElitism > 0.5);
		assert(probElitism <= 1.0);
	}

	BRKGA(DecoderRandomKeys<R>& _decoder, int key_size, unsigned numGen, unsigned popSize, double fracTOP, double fracBOT, double _probElitism) :
			RKGA<R>(_decoder, key_size, numGen, popSize, fracTOP, fracBOT), probElitism(_probElitism)
	{
		assert(probElitism > 0.5);
		assert(probElitism <= 1.0);
	}

	BRKGA(Evaluator<random_keys>& _evaluator, int key_size, unsigned numGen, unsigned _popSize, double fracTOP, double fracBOT, double _probElitism) :
			RKGA<R>(_evaluator, key_size, numGen, _popSize, fracTOP, fracBOT), probElitism(_probElitism)
	{
		assert(probElitism > 0.5);
		assert(probElitism <= 1.0);
	}

	virtual ~BRKGA()
	{
	}

	virtual Solution<random_keys>& cross(const Population<random_keys>& pop) const
	{
		assert(this->sz > 0); // In case of using InitPop, maybe must receive a Selection or Crossover object...

		const Solution<random_keys>& p1 = pop.at(rand() % this->eliteSize);
		const Solution<random_keys>& p2 = pop.at(this->eliteSize + rand() % (pop.size()-this->eliteSize));

		random_keys* v = new random_keys(this->sz, 0.0);
		for (int i = 0; i < this->sz; i++)
			if ((rand() % 1000000)/1000000.0 < probElitism)
				v->at(i) = p1.getR()[i];
			else
				v->at(i) = p2.getR()[i];
		return *new Solution<random_keys>(v);
	}

};

}

#endif /*OPTFRAME_BRKGA_HPP_*/
