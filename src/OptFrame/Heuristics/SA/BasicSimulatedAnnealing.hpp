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

#include <vector>
#include <math.h>

#include "../../SingleObjSearch.hpp"
#include "../../Evaluator.hpp"
#include "../../Constructive.hpp"
#include "../../RandGen.hpp"
#include "../../NS.hpp"
#include "../../Timer.hpp"

#include "SA.h"

namespace optframe
{

    template<Representation R, Structure ADS = OPTFRAME_DEFAULT_ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>>
    class BasicSimulatedAnnealing: public SingleObjSearch<R, ADS, S>
    {
    private:
        Evaluator<R, ADS, S>& evaluator;
        Constructive<R, ADS, S>& constructive;
        std::vector<NS<R, ADS, S>*> neighbors;
        RandGen& rg;
        double alpha;
        int SAmax;
        double Ti;

    public:

        BasicSimulatedAnnealing(Evaluator<R, ADS, S>& _evaluator, Constructive<R, ADS, S>& _constructive, std::vector<NS<R, ADS, S>*> _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg) :
                evaluator(_evaluator), constructive(_constructive), neighbors(_neighbors), rg(_rg)
        {
            alpha = (_alpha);
            SAmax = (_SAmax);
            Ti = (_Ti);

        }

        BasicSimulatedAnnealing(Evaluator<R, ADS, S>& _evaluator, Constructive<R, ADS, S>& _constructive, NS<R, ADS, S>& _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg) :
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

        pair<S, Evaluation>* git search(SOSC& stopCriteria, const S* _s = nullptr,  const Evaluation* _e = nullptr)
        {
            double timelimit = stopCriteria.timelimit;
            double target_f = stopCriteria.target_f;
            cout << "SA search(" << target_f << "," << timelimit << ")" << endl;

            Timer tnow;

            // store initial value in s (TODO: remove workaround to deal with pointer directly)
            S* sP = constructive.generateSolution(timelimit);
            S s(std::move(*sP)); // workaround
            delete sP;                         // workaround
            Evaluation e = evaluator.evaluateSolution(s);

            double T = Ti;
            int iterT = 0;
            S* sStar = &s.clone();
            Evaluation* eStar = &e.clone();

            while ((T > 0.000001) && (tnow.now() < timelimit))
            {
                while ((iterT < SAmax) && (tnow.now() < timelimit))
                {
                    int n = rg.rand(neighbors.size());
                    Move<R, ADS, S>* move = neighbors[n]->validRandomMoveSolution(s);

                    if(!move)
                    {
                        if(Component::warning)
                            cout << "SA warning: no move in iter=" << iterT << " T=" << T << "! continue..." << endl;
                        continue;
                    }

                    S* sCurrent = &s.clone();
                    Evaluation* eCurrent = &e.clone();
                    Component::safe_delete(move->applyUpdateSolution(*eCurrent, *sCurrent));
                    evaluator.reevaluateSolution(*eCurrent, *sCurrent);

                    if (evaluator.betterThan(*eCurrent, e))
                    {
                        e = *eCurrent;
                        s = *sCurrent;
                        delete sCurrent;
                        delete eCurrent;

                        if (evaluator.betterThan(e, *eStar))
                        {
                            delete sStar;
                            sStar = &s.clone();
                            delete eStar;
                            eStar = &e.clone();

                            cout << "Best fo: " << e.evaluation() << " Found on Iter = " << iterT << " and T = " << T;
                            cout << endl;
                        }
                    }
                    else
                    {
                        double x = rg.rand01();
                        double delta = ::fabs(eCurrent->evaluation() - e.evaluation());

                        if (x < exp(-delta / T))
                        {
                            s = *sCurrent;
                            e = *eCurrent;
                            delete sCurrent;
                            delete eCurrent;
                        }
                        else
                        {
                            delete sCurrent;
                            delete eCurrent;
                        }
                    }

                    iterT++;
                    delete move;
                }
                T = alpha * T;
                iterT = 0;
            }
            cout << "T=" << T << endl;

            s = *sStar;
            e = *eStar;
            delete sStar;
            delete eStar;

            return new pair<S, Evaluation> (s, e);
        }

        virtual string id() const
        {
            return idComponent();
        }

        static string idComponent()
        {
            stringstream ss;
            ss << SingleObjSearch<R, ADS, S>::idComponent() << ":SA:BasicSA";
            return ss.str();
        }
    };

    template<class R, class ADS = OPTFRAME_DEFAULT_ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>>
    class BasicSimulatedAnnealingBuilder: public SA, public SingleObjSearchBuilder<R, ADS, S>
    {
    public:
        virtual ~BasicSimulatedAnnealingBuilder()
        {
        }

        virtual SingleObjSearch<R, ADS, S>* build(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
        {
            Evaluator<R, ADS, S>* eval;
            hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

            Constructive<R, ADS, S>* constructive;
            hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

            std::vector<NS<R, ADS, S>* > hlist;
            hf.assignList(hlist, scanner.nextInt(), scanner.next()); // reads backwards!

            double alpha = scanner.nextDouble();
            int SAmax = scanner.nextInt();
            double Ti = scanner.nextDouble();

            return new BasicSimulatedAnnealing<R, ADS, S> (*eval, *constructive, hlist, alpha, SAmax, Ti, hf.getRandGen());
        }

        virtual std::vector<pair<string, string> > parameters()
        {
            std::vector<pair<string, string> > params;
            params.push_back(make_pair(Evaluator<R, ADS, S>::idComponent(), "evaluation function"));
            params.push_back(make_pair(Constructive<R, ADS, S>::idComponent(), "constructive heuristic"));
            stringstream ss;
            ss << NS<R, ADS, S>::idComponent() << "[]";
            params.push_back(make_pair(ss.str(), "list of NS"));
            params.push_back(make_pair("OptFrame:double", "cooling factor"));
            params.push_back(make_pair("OptFrame:int", "number of iterations for each temperature"));
            params.push_back(make_pair("OptFrame:double", "initial temperature"));

            return params;
        }

        virtual bool canBuild(string component)
        {
            return component == BasicSimulatedAnnealing<R, ADS, S>::idComponent();
        }

        static string idComponent()
        {
            stringstream ss;
            ss << SingleObjSearchBuilder<R, ADS, S>::idComponent() << ":" << SA::family() << "BasicSA";
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
