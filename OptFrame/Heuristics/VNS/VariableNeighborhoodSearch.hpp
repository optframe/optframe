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

#ifndef OPTFRAME_VNS_HPP_
#define OPTFRAME_VNS_HPP_

#include <math.h>
#include <vector>

#include "../../Constructive.h"
#include "../../SingleObjSearch.hpp"
#include "../../Evaluator.hpp"

#include "VNS.h"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class VariableNeighborhoodSearch: public VNS, public SingleObjSearch<R, ADS, DS>
{
protected:
	Evaluator<R, ADS, DS>& evaluator;
	Constructive<R, ADS>& constructive;
	vector<NS<R, ADS, DS>*> vshake;
	vector<NSSeq<R, ADS, DS>*> vsearch;

	unsigned shakeTries;

public:

	VariableNeighborhoodSearch(Evaluator<R, ADS, DS>& _evaluator, Constructive<R, ADS>& _constructive, vector<NS<R, ADS, DS>*> _vNS, vector<NSSeq<R, ADS, DS>*> _vNSSeq) :
			evaluator(_evaluator), constructive(_constructive), vshake(_vNS), vsearch(_vNSSeq)
	{
		shakeTries = 50;
	}

	virtual ~VariableNeighborhoodSearch()
	{
	}

	//virtual void improvement(Solution<R, ADS>& s, Evaluation<DS>& e, double timelimit, double target_f) = 0;

	virtual void shake(Solution<R, ADS>& s, Evaluation<DS>& e, unsigned int k_shake, double timelimit, double target_f)
	{
		for(unsigned t=0; t<shakeTries; t++)
		{
			Move<R, ADS, DS>& move = vshake.at(k_shake)->move(s);
			if(move.canBeApplied(s))
			{
				delete& move.apply(e, s);
				evaluator.evaluate(e, s); // refresh 'e'
				delete& move;
				break;
			}
			else
				delete &move;
		}
	}

	virtual pair<pair<Solution<R, ADS>&, Evaluation<DS>&>, unsigned int> neighborhoodChange(const Solution<R, ADS>& sStar, const Evaluation<DS>& eStar, const Solution<R, ADS>& s2, const Evaluation<DS>& e2, unsigned int k)
	{
		if (evaluator.betterThan(e2, eStar))
		{
			// IMPROVEMENT!
			pair<Solution<R, ADS>&, Evaluation<DS>&> p(s2.clone(), e2.clone());
			return make_pair(p, 0); // k <- 0
		}
		else
		{
			pair<Solution<R, ADS>&, Evaluation<DS>&> p(sStar.clone(), eStar.clone());
			return make_pair(p, k+1);
		}
	}

	virtual LocalSearch<R, ADS, DS>& buildSearch(unsigned k_search) = 0;

	pair<Solution<R, ADS>&, Evaluation<DS>&>* search(double timelimit = 100000000, double target_f = 0,  const Solution<R, ADS>* _s = NULL,  const Evaluation<DS>* _e = NULL)
	{
		cout << id() << " search(" << target_f << "," << timelimit << ")" << endl;

		Timer tnow;

		Solution<R, ADS>& sStar = constructive.generateSolution();
		Evaluation<DS>&   eStar = evaluator.evaluate(sStar);

		while(evaluator.betterThan(target_f, eStar.evaluation()) && ((tnow.now()) < timelimit))
		{
			unsigned k = 0;

			while(k < vshake.size())
			{
				Solution<R, ADS>& s = sStar.clone();
				Evaluation<DS>&   e = eStar.clone();

				shake(s, e, k, timelimit, target_f);

				LocalSearch<R, ADS, DS>& improve = buildSearch(k);
				improve.exec(s, e, timelimit, target_f);
				delete& improve; // save trajectory history?

				pair<pair<Solution<R, ADS>&, Evaluation<DS>&>, unsigned int> nc = neighborhoodChange(sStar, eStar, s, e, k);

				sStar = nc.first.first;
				eStar = nc.first.second;

				delete& nc.first.first;
				delete& nc.first.second;

				k = nc.second;
			}
		}

		return new pair<Solution<R, ADS>&, Evaluation<DS>&> (sStar, eStar);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS, DS>::idComponent() << VNS::family();
		//ss << SingleObjSearch<R, ADS, DS>::idComponent() << VNS::family << "VariableNeighborhoodSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

#endif /*OPTFRAME_VNS_HPP_*/
