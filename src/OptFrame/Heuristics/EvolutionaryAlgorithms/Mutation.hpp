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

#ifndef MUTATION_HPP_
#define MUTATION_HPP_

#include <vector>
#include <chrono>
#include <random>

#include "../../Component.hpp"
#include "../../Solution.hpp"
#include "../../Evaluation.hpp"
#include "../../RandGen.hpp"
#include "../../NS.hpp"

#include "EA.h"

#ifndef _OPTFRAME_DBG_MUTATION_
#   ifdef OPTFRAME_DEBUG
#       define _OPTFRAME_DBG_MUTATION_ 
#   else
#       define _OPTFRAME_DBG_MUTATION_ while(false)
#   endif /* OPTFRAME_DEBUG */
#endif /* _OPTFRAME_DBG_MUTATION_ */

namespace optframe
{

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class Mutation: public Component, public EA
{

public:

	virtual ~Mutation()
	{
	}

	virtual void mutate(S& individual, XEv& e) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":" << EA::family() << ":Mutation";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicMutation: public Mutation<S, XEv>
{
protected:
	unsigned n;
	vector<NS<XES, XEv>*> vNS;
	RandGen& rg;

public:

	BasicMutation(unsigned _n, vector<NS<XES, XEv>*> _vNS, RandGen& _rg) :
			n(_n), vNS(_vNS), rg(_rg)
	{
	}

	virtual ~BasicMutation()
	{
	}

	virtual void mutate(S& s, XEv& e)
	{
		for (unsigned i = 0; i < n; i++)
		{
			int x = rg.rand(vNS.size());
			Move<S, XEv>* mp = vNS[x]->validMove(s);
			if (!mp)
				cout << "Warning: no move in BasicMutation!" << endl;
			else
			{
				delete &mp->apply(e, s);
				delete mp;
			}
		}
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":" << EA::family() << ":Mutation";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class BasicMutationBuilder: public ComponentBuilder<S, XEv, XES, X2ES>
{
public:
	virtual ~BasicMutationBuilder()
	{
	}

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		int n = scanner.nextInt();

		vector<NS<XES, XEv>*> ns_list;
		hf.assignList(ns_list, scanner.nextInt(), scanner.next()); // reads backwards!

		return new BasicMutation<S, XEv>(n, ns_list, hf.getRandGen());
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair("OptFrame:int", "number of moves"));
		stringstream ss;
		ss << NS<XES, XEv>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of neighborhood structures"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicMutation<S, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "" << EA::family() << ":BasicMutation";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

//temporary fix for the true basic genetic algorithm! I will revisit this in the future to perform a proper naming convention
template<XESolution XES, XEvaluation XEv = Evaluation<>>
class SimpleMutation {
protected:
	using Individual = S;
    //using Chromossome = R;
    using Fitness = XEv*; //nullptr means there's no evaluation
    using Population = std::vector< std::pair<Individual, Fitness> >;

public:
	SimpleMutation() = default;
	virtual ~SimpleMutation() = default;

	virtual void mutate(Population& population) = 0;
};

/**********************/
/* MUTATION EXAMPLES */
/**********************/

//changes 100beta% individuals chosen randomly -- may choose the same individual more than once
//user should program the function that changes the individual
template<XESolution XES, XEvaluation XEv = Evaluation<>>
class DefaultMutation : public SimpleMutation<S, XEv> {
protected:
	using Individual = S;
    //using Chromossome = R;
    using Fitness = XEv*; //nullptr means there's no evaluation
    using Population = std::vector< std::pair<Individual, Fitness> >;
	double omega;

public:
	DefaultMutation() = delete;
	DefaultMutation(double mutationRate) : omega(mutationRate) { assert(mutationRate >= 0.0 && mutationRate <= 1.0); };
	virtual ~DefaultMutation() = default;

	virtual void mutate(Individual&) = 0; //Should change the solution unpredicatelly
										  //Individual is passed so the user may change ADS if he wants to

	virtual void mutate(Population& population) override {
		_OPTFRAME_DBG_MUTATION_  std::cerr << "-OptDebug- Starting mutation operator. Will insert " << static_cast<int>(population.size() * omega) << " mutants into the population." << std::endl;

		//todo: use randgen
		std::mt19937 mersenne_twister(std::chrono::steady_clock::now().time_since_epoch().count());
		std::uniform_int_distribution<unsigned int> dist(0u, population.size()-1);
		int iteration_count = population.size() * omega;
		for(int i = 0; i < iteration_count; ++i){
			int mut_pos = dist(mersenne_twister);
			_OPTFRAME_DBG_MUTATION_  std::cerr << "-OptDebug- Mutating individual " << mut_pos << std::endl;
			mutate(population[mut_pos].first.getR());
			if(population[mut_pos].second) delete population[mut_pos].second;
			population[mut_pos].second = nullptr;
		}

		_OPTFRAME_DBG_MUTATION_  std::cerr << "-OptDebug- Mutation operator ended" << std::endl;
	}	

};


}

#endif /* MUTATION_HPP_ */
