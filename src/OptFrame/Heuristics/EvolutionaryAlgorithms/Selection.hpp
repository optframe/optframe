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

#ifndef SELECTION_HPP_
#define SELECTION_HPP_

#include <utility>
#include <vector>

#include "../../Evaluator.hpp"
#include "../../MultiSolution.hpp"
#include "../../MultiEvaluation.hpp"
#include "EA.h"

#include "../../Util/printable.h"

#ifndef _OPTFRAME_DBG_SELECTION_
#   ifdef OPTFRAME_DEBUG
#       define _OPTFRAME_DBG_SELECTION_ 
#   else
#       define _OPTFRAME_DBG_SELECTION_ while(false)
#   endif /* OPTFRAME_DEBUG */
#endif /* _OPTFRAME_DBG_SELECTION_ */

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class Selection : public Component, public EA
{
public:

	virtual ~Selection()
	{
	}

	virtual pair<unsigned, unsigned> select(const MultiSolution<R, ADS>& population, const MultiEvaluation& mev, const std::vector<double>& fv) = 0;


	static double getMax(const std::vector<double>& fv)
	{
		double lmax = -10000000;
		for (int i = 0; i < fv.size(); i++)
			if (fv[i] > lmax)
				lmax = fv[i];
		return lmax;
	}

	static double getSum(const std::vector<double>& fv)
	{
		double s = 0;
		for (int i = 0; i < fv.size(); i++)
			s += fv[i];
		return s;
	}

	static void normalize(std::vector<double>& fv)
	{
		double sum = getSum(fv);
		if(sum == 0)
			sum = 1;
		for (int i = 0; i < fv.size(); i++)
		{
			fv[i] = fv[i] / sum;
			if(fv[i] != fv[i]) // is nan
			{
				cout << "Selection::normalize()::NAN VALUE!" << endl;
				cout << fv << endl;
				exit(1);
			}
		}
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":" << EA::family() << ":Selection";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

//temporary fix for the true basic genetic algorithm! I will revisit this in the future to perform a proper naming convention
template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class SimpleSelection {
protected:
	using Individual = Solution<R, ADS>;
    using Chromossome = R;
    using Fitness = Evaluation*; //nullptr means there's no evaluation
    using Population = vector< pair<Individual, Fitness> >;

    Evaluator<R, ADS>& evaluator;

public:

	SimpleSelection(Evaluator<R, ADS>& _evaluator) : evaluator(_evaluator) { };
	virtual ~SimpleSelection() = default;

	virtual void select(Population& population) = 0;

	//this is a support function to be used by programmers who need to rank the population before selection. See GA manual to check if the population is ranked when select is called.
	virtual void sortPopulation(Population& population){
		_OPTFRAME_DBG_SELECTION_ std::cerr << "-OptDebug- Selection operator is sorting the population" << std::endl;
		auto compare = [&](const pair<Individual, Fitness>& a, const pair<Individual, Fitness>& b)->bool{
			if(a.second && b.second)
				return evaluator.betterThan(*a.second, *b.second);
			else return a.second != nullptr;
		};

		std::sort(population.begin(), population.end(), compare);
		_OPTFRAME_DBG_SELECTION_ std::cerr << "-OptDebug- Population ranked with selection operator" << std::endl;
	}
};

/**********************/
/* SELECTION EXAMPLES */
/**********************/

//Selects the 100alpha% most fit individuals 
template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class ElitismSelection final : public SimpleSelection<R, ADS> {
protected:
	using Individual = Solution<R, ADS>;
    using Chromossome = R;
    using Fitness = Evaluation*; //nullptr means there's no evaluation
    using Population = vector< pair<Individual, Fitness> >;

private:
	double alpha; //selectionRate

public:
	//optional parameter
	bool sortPopulationBeforeSelect = false; //this selection need to operate over a ranked population. If the GA used doesn't rank them, then you should flip this to true 

	ElitismSelection(Evaluator<R, ADS>& _evaluator, double selectionRate) : SimpleSelection<R, ADS>(_evaluator), alpha(selectionRate) { assert(selectionRate >= 0.0 && selectionRate <= 1.0); };
	~ElitismSelection() = default;

	void select(Population& population) override {
		_OPTFRAME_DBG_SELECTION_ std::cerr << "-OptDebug- ElitismSelection is selecting the " << population.size() * alpha << " most fit individuals and killing the rest" << std::endl;
		if(this->sortPopulationBeforeSelect) this->sortPopulation(population);
		int oldSize = population.size();
		int newSize = oldSize * alpha;
		for(int i = oldSize-1; i >= newSize; --i)
			if(population[i].second)
				delete population[i].second;
		population.erase(population.begin() + newSize, population.end()); 
		_OPTFRAME_DBG_SELECTION_ std::cerr << "-OptDebug- ElitismSelection old size: " << oldSize << " new size: " << population.size() << std::endl;
	}
};

}

#endif /* SELECTION_HPP_ */
