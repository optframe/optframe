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

#ifndef OPTFRAME_BSA_HPP_
#define OPTFRAME_BSA_HPP_

#include "../../SingleObjSearch.hpp"
#include <math.h>

#include "SA.h"

namespace optframe
{

// forward declaration
template<XSolution S, XEvaluation XEv, XSearch<S, XEv> XSH, XSearchMethod XM>
class BasicSimulatedAnnealing;


//template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = std::pair<S, XEv>, XSearchMethod XM = Component, XStopCriteria<XEv, XM> XStop = DefaultStop >
//template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = std::pair<S, XEv>>
//class BasicSimulatedAnnealing: public SingleObjSearch<S, XEv, XSH, XM, XStop>
//template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = std::pair<S, XEv>, XSearchMethod XM = Component>
template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = std::pair<S, XEv>, XSearchMethod XM = BasicSimulatedAnnealing<S, XEv, XSH, Component>>
class BasicSimulatedAnnealing: public SingleObjSearch<S, XEv, XSH, XM>
{
private:
	Evaluator<S, XEv, XSH>& evaluator;
	Constructive<S>& constructive;
	vector<NS<S, XEv, XSH>*> neighbors;
	RandGen& rg;
	double alpha;
	int SAmax;
	double Ti;

   // --------------
   // local variables (beware that this is NOT thread-safe!)
   // --------------
   // if you need multiple threads of this method, you should instantiate multiple methods!!
   // this allows monitoring on progress and many other nice things, such as StopCriteria personalization ;)
   double T;
   Timer tnow;

public:

   // get current temperature
   double getT()
   {
      return T;
   };

   Timer& getTimer()
   {
      return tnow;
   }

	BasicSimulatedAnnealing(Evaluator<S, XEv, XSH>& _evaluator, Constructive<S>& _constructive, vector<NS<S, XEv, XSH>*> _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg) :
		evaluator(_evaluator), constructive(_constructive), neighbors(_neighbors), rg(_rg)
	{
		alpha = (_alpha);
		SAmax = (_SAmax);
		Ti = (_Ti);

	}

	BasicSimulatedAnnealing(Evaluator<S, XEv, XSH>& _evaluator, Constructive<S>& _constructive, NS<S, XEv, XSH>& _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg) :
		evaluator(_evaluator), constructive(_constructive), rg(_rg)
	{
		neighbors.push_back(&_neighbors);
		alpha = (_alpha);
		SAmax = (_SAmax);
		Ti = (_Ti);
	}

	virtual ~BasicSimulatedAnnealing()
	{
	}

   // TODO: make optional and consider input too (SingleObjSearch helper class)
   /*
   pair<S, XEv> genPair(double timelimit)
   {
      std::optional<S> sStar = constructive.generateSolution(timelimit);
		XEv eStar = evaluator.evaluate(*sStar);
      return make_pair(*sStar, eStar); 
   }
   */

	//pair<S, Evaluation<>>* search(StopCriteria<XEv>& stopCriteria, const S* _s = nullptr,  const Evaluation<>* _e = nullptr)
   //virtual std::optional<pair<S, XEv>> search(StopCriteria<XEv>& stopCriteria) override

   //template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = std::pair<S, XEv>, XSearchMethod XM = Component, XStopCriteria<XEv, XM> XStop = DefaultStop >
   SearchStatus search(std::optional<pair<S, XEv>>& star, const StopCriteria<XEv, XM>& stop) override
   //SearchStatus search(std::optional<pair<S, XEv>>& star, const StopCriteria<XEv>& stopCriteria) override
	{
		double timelimit = stop.timelimit;
		XEv target_f(stop.target_f);
		cout << "SA search(" << target_f << "," << timelimit << ")" << endl;

		tnow = Timer();

		// TODO: verify 's' and 'input'
		//pair<S, XEv> se = genPair(timelimit);
      if(!star)
         star = SingleObjSearch<S, XEv>::genPair(constructive, evaluator, timelimit);
      if(!star)
         return SearchStatus::NO_NEWSOL; // no possibility to continue.
      
      pair<S, XEv> se = *star; // copy (implicit cloning guaranteed??)
      //
      S& s = se.first;
      XEv& e = se.second;
      //
      
      // local variables
		T = Ti;
		int iterT = 0;

      //pair<S, XEv> star = se; // copy (implicit cloning guaranteed??)
      //
		////S* sStar = &s.clone();
      //S& sStar = star.first;
		////Evaluation<>* eStar = &e.clone();
      XEv& eStar = star->second;

      // TODO: freezing parameter should be passed somewhere
		//while ((T > 0.000001) && (tnow.now() < timelimit))

      bool bstop = stop.shouldStop(eStar, reinterpret_cast<XM*>(this));
      while (stop.specificStopBy ? bstop : ((T > 0.000001) && (tnow.now() < timelimit)))
		{
			while ((iterT < SAmax) && (tnow.now() < timelimit))
			{
				int n = rg.rand(neighbors.size());
				uptr<Move<S, XEv, XSH>> move = neighbors[n]->validRandomMove(s);

				if(!move)
				{
					if(Component::warning)
						cout << "SA warning: no move in iter=" << iterT << " T=" << T << "! continue..." << endl;
               // TODO: return FAIL?
					continue;
				}

				//S* sCurrent = &s.clone();
				//Evaluation<>* eCurrent = &e.clone();
            pair<S, XEv> current(se); // implicit clone??
            //S& sCurrent = current.first;
            XEv& eCurrent = current.second;

				move->applyUpdate(current);
				evaluator.reevaluate(current);

				if (evaluator.betterThan(eCurrent, e))
				{
               // if improved, accept it
					//e = *eCurrent;
					//s = *sCurrent;
					//delete sCurrent;
					//delete eCurrent;
               se = current;


					if (evaluator.betterThan(e, eStar))
					{
						//delete sStar;
						//sStar = &s.clone();
						//delete eStar;
						//eStar = &e.clone();
                  star = make_optional(se);

						cout << "Best fo: " << e.evaluation() << " Found on Iter = " << iterT << " and T = " << T;
						cout << endl;
					}
				}
				else
				{
               // 'current' didn't improve, but may accept it anyway
					double x = rg.rand01();
					double delta = ::fabs(eCurrent.evaluation() - e.evaluation());

					if (x < exp(-delta / T))
					{
						//s = *sCurrent;
						//e = *eCurrent;
						//delete sCurrent;
						//delete eCurrent;
                  se = current;
					}
					else
					{
						//delete sCurrent;
						//delete eCurrent;
					}
				}

				iterT++;
			}
			T = alpha * T;
			iterT = 0;
		}
      cout << "T=" << T << endl;

		//s = *sStar;
		//e = *eStar;
		//delete sStar;
		//delete eStar;

		//return new pair<S, Evaluation<>> (s, e);
      //
      //return make_optional(star);
      return SearchStatus::VALID_SOL;
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<S, XEv>::idComponent() << ":SA:BasicSA";
		return ss.str();
	}
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<S, XEv> X2ES = MultiESolution<S, XEv>>
class BasicSimulatedAnnealingBuilder: public SA, public SingleObjSearchBuilder<S, XEv, X2ES>
{
   //using XM = BasicSimulatedAnnealing<S, XEv, pair<S, XEv>, Component>;
   using XM = Component; // only general builds here
public:
	virtual ~BasicSimulatedAnnealingBuilder()
	{
	}

	virtual SingleObjSearch<S, XEv, pair<S, XEv>, XM>* build(Scanner& scanner, HeuristicFactory<S, XEv, X2ES>& hf, string family = "")
	{
		Evaluator<S, XEv>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		Constructive<S>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<NS<S, XEv>* > hlist;
		hf.assignList(hlist, scanner.nextInt(), scanner.next()); // reads backwards!

		double alpha = scanner.nextDouble();
		int SAmax = scanner.nextInt();
		double Ti = scanner.nextDouble();

		return new BasicSimulatedAnnealing<S, XEv, pair<S, XEv>, XM> (*eval, *constructive, hlist, alpha, SAmax, Ti, hf.getRandGen());
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<S, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
		stringstream ss;
		ss << NS<S, XEv>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of NS"));
		params.push_back(make_pair("OptFrame:double", "cooling factor"));
		params.push_back(make_pair("OptFrame:int", "number of iterations for each temperature"));
		params.push_back(make_pair("OptFrame:double", "initial temperature"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicSimulatedAnnealing<S, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<S, XEv>::idComponent() << ":" << SA::family() << "BasicSA";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

/*

 Procedure originally proposed by Kirkpatrick et al.

 S. Kirkpatrick, D.C. Gellat, and M.P. Vecchi. Optimization by Simulated Annealing.
 Science, 220:671–680, 1983.

 LATEX:

 */

}

#endif /*OPTFRAME_BSA_HPP_*/
