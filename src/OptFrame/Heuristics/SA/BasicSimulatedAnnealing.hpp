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
//template<XESolution XES, XEvaluation XEv, XSearchMethod XM>
//class BasicSimulatedAnnealing;


//template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = std::pair<S, XEv>>
//class BasicSimulatedAnnealing: public SingleObjSearch<S, XEv, XSH, XM, XStop>
//template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = std::pair<S, XEv>, XSearchMethod XM = Component>
//template<XESolution XES, XEvaluation XEv = Evaluation<>, XSearchMethod XM = BasicSimulatedAnnealing<XES, XEv, Component>>
template<XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicSimulatedAnnealing: public SingleObjSearch<XES, XEv>
{
   using XSH = XES; // XSearch
private:
	GeneralEvaluator<XES, XEv>& evaluator;
	//Constructive<S>& constructive; // TODO: this must become InitialSearch, already starting from "optional" XES element.
   InitialSearch<XES, XEv>& constructive;

	vector<NS<XES, XEv, XSH>*> neighbors;
	RandGen& rg;
	double alpha;
	int SAmax;
	double Ti;

   // --------------
   // local variables (beware that this is NOT thread-safe!)
   // --------------
   // if you need multiple threads of this method, you should instantiate multiple methods!!
   // this allows monitoring on progress and many other nice things, such as StopCriteria personalization ;)
   //SpecificMethodStop<XES, decltype(*this)> specificStop;
public:
   //SpecificMethodStop<XES, XEv, BasicSimulatedAnnealing<XES, XEv>> specificStopBy {nullptr};
   SpecificMethodStop<XES, XEv, BasicSimulatedAnnealing<XES, XEv>> specificStopBy 
   {
      [&](const XES& best, const StopCriteria<XEv>& sosc, BasicSimulatedAnnealing<XES, XEv>* me) -> bool {
         return (me->T <= 0.000001) || (me->tnow.now() >= sosc.timelimit);
      }
   };

private:
/*
   SpecificMethodStop<XES, XEv, BasicSimulatedAnnealing<XES, XEv>> defaultStopBy
    {
      [&](const XES& best, const StopCriteria<XEv>& sosc, BasicSimulatedAnnealing<XES, XEv>* me) -> bool {
         return (me->T <= 0.000001) || (me->tnow.now() >= sosc.timelimit);
      }   
   };
*/

private:
   //
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

	BasicSimulatedAnnealing(GeneralEvaluator<XES, XEv>& _evaluator, InitialSearch<XES, XEv>& _constructive, vector<NS<XES, XEv, XSH>*> _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg) :
		evaluator(_evaluator), constructive(_constructive), neighbors(_neighbors), rg(_rg) //, specificStopBy(defaultStopBy)
	{
		alpha = (_alpha);
		SAmax = (_SAmax);
		Ti = (_Ti);
	}

/*
   BasicSimulatedAnnealing(GeneralEvaluator<XES, XEv>& _evaluator, InitialSearch<XES, XEv>& _constructive, vector<NS<XES, XEv, XSH>*> _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg, SpecificMethodStop<XES, XEv, BasicSimulatedAnnealing<XES, XEv>> _spec) :
		evaluator(_evaluator), constructive(_constructive), neighbors(_neighbors), rg(_rg), specificStopBy(_spec)
	{
		alpha = (_alpha);
		SAmax = (_SAmax);
		Ti = (_Ti);
	}
*/

	BasicSimulatedAnnealing(GeneralEvaluator<XES, XEv>& _evaluator, InitialSearch<XES, XEv>& _constructive, NS<XES, XEv, XSH>& _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg) :
		evaluator(_evaluator), constructive(_constructive), rg(_rg) //, specificStopBy(defaultStopBy)
	{
		neighbors.push_back(&_neighbors);
		alpha = (_alpha);
		SAmax = (_SAmax);
		Ti = (_Ti);
	}

/*
	BasicSimulatedAnnealing(GeneralEvaluator<XES, XEv>& _evaluator, InitialSearch<XES, XEv>& _constructive, NS<XES, XEv, XSH>& _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg, SpecificMethodStop<XES, XEv, BasicSimulatedAnnealing<XES, XEv>> _spec) :
		evaluator(_evaluator), constructive(_constructive), rg(_rg), specificStopBy(_spec)
	{
		neighbors.push_back(&_neighbors);
		alpha = (_alpha);
		SAmax = (_SAmax);
		Ti = (_Ti);
	}
*/

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

   SearchStatus search(const StopCriteria<XEv>& sosc) override
	{
      op<XES>& star = this->best;
		double timelimit = sosc.timelimit;

		//XEv target_f(stop.target_f); // 'target_f' will break... removing

		//cout << "SA search(" << target_f << "," << timelimit << ")" << endl;
      cout << "SA search(" << timelimit << ")" << endl; // TODO: require 'stop'.toString()

		tnow = Timer();

		// TODO: verify 's' and 'input'
		//pair<S, XEv> se = genPair(timelimit);
      if(!star)
         //star = SingleObjSearch<XES>::genPair(constructive, evaluator, timelimit);
         star = constructive.initialSearch(sosc);
      if(!star)
         return SearchStatus::NO_SOLUTION; // no possibility to continue.
      
      XES se = *star; // copy (implicit cloning guaranteed??)
      //
      //XSolution& s = se.first;
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

      // try specific stop criteria, otherwise just use standard one
      //while (stop.specific ? stop.shouldStop(eStar, reinterpret_cast<XM*>(this)) : ((T > 0.000001) && (tnow.now() < timelimit)))
      //while (sosc.specific ? sosc.shouldStop(star, reinterpret_cast<XM*>(this)) : ((T > 0.000001) && (tnow.now() < timelimit)))
      op<XEv> opEvStar = make_optional(eStar);
      while ( 
         !(
            specificStopBy(*star, sosc, this) || 
            sosc.shouldStop(opEvStar) 
          )
      )
		{
			while ((iterT < SAmax) && (tnow.now() < timelimit))
			{
				int n = rg.rand(neighbors.size());
				uptr<Move<XES, XEv, XSH>> move = neighbors[n]->validRandomMove(se); // TODO: pass 'se.first' here (even 'se' should also work...)

				if(!move)
				{
					if(Component::warning)
						cout << "SA warning: no move in iter=" << iterT << " T=" << T << "! continue..." << endl;
               // TODO: return FAIL?
					continue;
				}

				//S* sCurrent = &s.clone();
				//Evaluation<>* eCurrent = &e.clone();
            XES current(se); // implicit clone??
            //S& sCurrent = current.first;
            XEv& eCurrent = current.second;

				move->applyUpdate(current);
				evaluator.reevaluate(current);

				//if (evaluator.betterThan(eCurrent, e)) // TODO: replace by 'se' here, and use 'se.second' to compare directly
            //if(eCurrent.betterStrict(e))
            if(evaluator.betterStrict(eCurrent, e))
				{
               // if improved, accept it
					//e = *eCurrent;
					//s = *sCurrent;
					//delete sCurrent;
					//delete eCurrent;
               se = current;


					//if (evaluator.betterThan(e, eStar))
               //if(e.betterStrict(eStar))
               if(evaluator.betterStrict(e, eStar))
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
      this->best = star;
      return SearchStatus::NO_REPORT;
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<XES>::idComponent() << ":SA:BasicSA";
		return ss.str();
	}
};


template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class BasicSimulatedAnnealingBuilder: public SA, public SingleObjSearchBuilder<S, XEv, XES>
{
   //using XM = BasicSimulatedAnnealing<S, XEv, pair<S, XEv>, Component>;
   //using XM = Component; // only general builds here
public:
	virtual ~BasicSimulatedAnnealingBuilder()
	{
	}

	virtual SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		GeneralEvaluator<XES, XEv>* eval;
		hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

		//Constructive<S>* constructive;
      InitialSearch<XES, XEv>* constructive;
		hf.assign(constructive, *scanner.nextInt(), scanner.next()); // reads backwards!

		vector<NS<XES, XEv>* > hlist;
		hf.assignList(hlist, *scanner.nextInt(), scanner.next()); // reads backwards!

		double alpha = *scanner.nextDouble();
		int SAmax = *scanner.nextInt();
		double Ti = *scanner.nextDouble();

		return new BasicSimulatedAnnealing<XES, XEv> (*eval, *constructive, hlist, alpha, SAmax, Ti, hf.getRandGen());
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
		//params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
      params.push_back(make_pair(InitialSearch<XES, XEv>::idComponent(), "constructive heuristic"));
		stringstream ss;
		ss << NS<XES, XEv>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of NS"));
		params.push_back(make_pair("OptFrame:double", "cooling factor"));
		params.push_back(make_pair("OptFrame:int", "number of iterations for each temperature"));
		params.push_back(make_pair("OptFrame:double", "initial temperature"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicSimulatedAnnealing<XES, XEv>::idComponent();
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
