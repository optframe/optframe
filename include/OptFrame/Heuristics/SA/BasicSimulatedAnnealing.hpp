// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_BSA_HPP_
#define OPTFRAME_BSA_HPP_

#include <math.h>

#include "../../ITrajectory.hpp"
#include "../../SingleObjSearch.hpp"

#include "SA.h"

namespace optframe {

// forward declaration
//template<XESolution XES, XEvaluation XEv, XSearchMethod XM>
//class BasicSimulatedAnnealing;

//template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = std::pair<S, XEv>>
//class BasicSimulatedAnnealing: public SingleObjSearch<S, XEv, XSH, XM, XStop>
//template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = std::pair<S, XEv>, XSearchMethod XM = Component>
//template<XESolution XES, XEvaluation XEv = Evaluation<>, XSearchMethod XM = BasicSimulatedAnnealing<XES, XEv, Component>>
//
template<XESolution XES>
//
class BasicSimulatedAnnealing : public SingleObjSearch<XES>
  , public ITrajectory<XES>
{
public:
   using XEv = typename XES::second_type;
   using XSH = XES; // XSearch
   //
   sref<GeneralEvaluator<XES, XEv>> evaluator;
   sref<InitialSearch<XES, XEv>> constructive;
   vsref<NS<XES, XEv, XSH>> neighbors;
   sref<RandGen> rg;
   double alpha;
   int SAmax;
   double Ti;

   // set verbose level recursive: returns 'false' if not supported.
   virtual bool setVerboseR() override
   {
      this->setVerbose();
      //
      // evaluator ?? how to do this on GeneralEvaluator...
      constructive->setVerbose();
      for (unsigned i = 0; i < neighbors.size(); i++)
         neighbors[i]->setVerbose();
      return true;
   }

   // single neighborhood
   // reordered the different term to the front because of peeve, heheheh
   // standart RandGen in declaration could be new standart
   BasicSimulatedAnnealing(sref<GeneralEvaluator<XES, XEv>> _evaluator, sref<InitialSearch<XES, XEv>> _constructive, sref<NS<XES, XEv, XSH>> _neighbors, double _alpha, int _SAmax, double _Ti, sref<RandGen> _rg = new RandGen)
     : evaluator(_evaluator)
     , constructive(_constructive)
     , rg(_rg) // does the new RandGen thing affect this? doesn't look like it but i'm no specialist
               //, specificStopBy(defaultStopBy)
   {
      neighbors.push_back(_neighbors);
      alpha = (_alpha);
      SAmax = (_SAmax);
      Ti = (_Ti);
   }

   // vector of neighborhoods
   // reordered the different term to the front because of peeve, heheheh
   // standart RandGen in declaration could be new standart
   BasicSimulatedAnnealing(sref<GeneralEvaluator<XES, XEv>> _evaluator, sref<InitialSearch<XES, XEv>> _constructive, vsref<NS<XES, XEv, XSH>> _neighbors, double _alpha, int _SAmax, double _Ti, sref<RandGen> _rg = new RandGen)
     : evaluator(_evaluator)
     , constructive(_constructive)
     , neighbors(_neighbors)
     , rg(_rg) //, specificStopBy(defaultStopBy)
   {
      alpha = (_alpha);
      SAmax = (_SAmax);
      Ti = (_Ti);
   }

   virtual ~BasicSimulatedAnnealing()
   {
      //for (unsigned i = 0; i < neighbors.size(); i++)
      //   neighbors[i] = nullptr;
      neighbors.clear();
   }

   struct SearchContext
   {
      BasicSimulatedAnnealing<XES>& self;
      std::optional<XSH>& best;
      std::optional<XSH>& incumbent;
      //
      double T;
      int iterT;
   };

   // callback to update local variables
   void (*onBeforeLoopCtx)(SearchContext& ctx) =
     [](auto& ctx) {
        if (ctx.iterT < ctx.self.SAmax)
           ctx.iterT++;
        else {
           ctx.iterT = 0;
           ctx.T = ctx.self.alpha * ctx.T; // geometric cooling
        }
     };

   // callback to handle main loop and stop criteria
   bool (*onLoopCtx)(const SearchContext& ctx, const StopCriteria<XEv>& sosc) =
     [](auto& ctx, auto& sosc) {
        return (ctx.T >= 0.000001) && !sosc.shouldStop(ctx.best->second);
     };

   // technique found in lecture notes of prof. Marcone Jamilson Freitas Souza
   static double estimateInitialTemperature(
     sref<GeneralEvaluator<XES, XEv>> evaluator,
     sref<InitialSearch<XES, XEv>> constructive,
     vsref<NS<XES, XEv, XSH>> neighbors,
     double beta,
     double gama,
     int SAmax,
     double T0,
     sref<RandGen> rg)
   {
      // http://www.decom.ufop.br/prof/marcone/Disciplinas/InteligenciaComputacional/InteligenciaComputacional.pdf
      double T = T0; // {Temp eraturacorrente}
      bool continua = true;
      std::optional<XES> ose = constructive->initialSearch({ 99999.9 }).first; // inf time
      assert(ose);
      XES& se1 = *ose;
      //
      while (continua) {
         int aceitos = 0; // { número de vizinhos aceitos na temperatura T }
         //std::cout << "continua. T=" << T << std::endl;
         //
         std::optional<XES> ose2 = constructive->initialSearch({ 99999.9 }).first; // inf time
         XES& se2 = *ose2;
         for (int IterT = 0; IterT < SAmax; IterT++) {
            //XES se = se1;
            XES se = se2;

            int n = rg->rand(neighbors.size());
            uptr<Move<XES, XEv, XSH>> move = neighbors[n]->validRandomMove(se); // TODO: pass 'se.first' here (even 'se' should also work...)
            XES se_line(se);
            //
            move->applyUpdate(se_line);
            evaluator->reevaluate(se_line);
            //
            if (evaluator->betterStrict(se_line.second, se.second))
               aceitos++;
            else {
               double x = rg->rand01();
               double delta = ::fabs(se_line.second.evaluation() - se.second.evaluation());
               if (x < ::exp(-delta / T))
                  aceitos++;
            }
         } // fim para
         //std::cout << aceitos << " >= " << gama*SAmax << " ~ " << (aceitos/(double)SAmax) << "?" << std::endl;
         //
         if (aceitos >= gama * SAmax)
            continua = false;
         else {
            T = beta * T;
         }
      } // fim enquanto
      //
      return T; // { retorna temperatura }
   }

   // search (TODO: consider _best and _incumbent parameters)
   SearchOutput<XES, XSH> search(const StopCriteria<XEv>& sosc) override
   {
      std::cout << "SA search(" << sosc.timelimit << ")" << std::endl;

      // TODO: receive on 'searchBy'
      std::optional<XSH> star;
      std::optional<XSH> incumbent;

      // Note that no comparison is necessarily made between star and incumbent, as types/spaces could be different. Since we are trajectory here, spaces are equal.
      // We assume star is better than incumbent, if provided. So, if star is worse than incumbent, we won't re-check that.

      if (Component::verbose)
         std::cout << "SA: will build initialSearch" << std::endl;
      // initial setup for incumbent (if has star, copy star, otherwise generate one)
      if (!incumbent)
         incumbent = star ? star : constructive->initialSearch(sosc).first;

      if (Component::verbose)
         std::cout << "SA: post build initialSearch" << std::endl;

#ifdef OPTFRAME_AC
      // requires first part of solution to be a component
      static_assert(XRSolution<typename XSH::first_type, typename XSH::first_type::typeR>);
      {
         std::stringstream ss;
         ss << incumbent->second.evaluation();
         incumbent->first.listAC.push_back(ContextAC{ .id{ "SA_INIT" }, .message{ ss.str() }, .payload{ incumbent->first.sharedClone() } });
      }
#endif

      if (Component::verbose) {
         std::cout << "SA begin: incumbent? " << (bool)incumbent << std::endl;
         std::cout << "SA begin: star? " << (bool)star << std::endl;
      }

      // if no star and has incumbent, star is incumbent
      if (!star && incumbent)
         star = incumbent;

      if (Component::verbose) {
         std::cout << "SA begin: incumbent? " << (bool)incumbent << std::endl;
         std::cout << "SA begin: star? " << (bool)star << std::endl;
      }

#ifdef OPTFRAME_AC
      // requires first part of solution to be a component
      static_assert(XRSolution<typename XSH::first_type, typename XSH::first_type::typeR>);
      {
         std::stringstream ss;
         ss << star->second.evaluation();
         star->first.listAC.push_back(ContextAC{ .id{ "SA_NEW_STAR" }, .message{ ss.str() }, .payload{ star->first.sharedClone() } });
      }
      //
      //star->first.printListAC();
#endif

      // abort if no solution exists
      if (!star)
         return SearchStatus::NO_SOLUTION; // no possibility to continue.

      if (Component::verbose) {
         std::cout << "star value: " << star->second.evaluation() << std::endl;
         std::cout << "star: " << star->first << std::endl;
      }

      // initialize search context for Simulated Annealing
      SearchContext ctx{ .self = *this, .best = star, .incumbent = incumbent };

      if (Component::verbose)
         std::cout << "SA: begin SearchContext" << std::endl;

      // ===================
      // Simulated Annealing
      // ===================

      if (Component::verbose)
         std::cout << "incumbent? " << (bool)incumbent << std::endl;

      XSH& se = *incumbent;

      ctx.T = Ti;
      ctx.iterT = 0;

      if (Component::verbose)
         std::cout << "SA(verbose): BEGIN se: " << se << std::endl;
      // SANITY CHECK
      assert(!se.second.outdated);

      while (onLoopCtx(ctx, sosc)) {
         if (Component::verbose)
            std::cout << "SA(verbose): after onLoopCtx" << std::endl;
         int n = rg->rand(neighbors.size());

         uptr<Move<XES, XEv, XSH>> move = neighbors[n]->validRandomMove(se); // TODO: pass 'se.first' here (even 'se' should also work...)

         if (!move) {
            if (Component::warning)
               cout << "SA warning: no move in iter=" << ctx.iterT << " T=" << ctx.T << "! continue..." << endl;
            // TODO: return FAIL?
            //continue;
            return { SearchStatus::NO_REPORT, star };
         }

         if (Component::verbose)
            std::cout << "SA(verbose): will copy to current=se: " << se << std::endl;
         XES current(se); // implicit clone??

         if (Component::verbose)
            std::cout << "SA(verbose): current: " << current << std::endl;

         /*
#ifdef OPTFRAME_AC
         std::cout << "begin current: " << std::endl;
         current.first.printListAC();
         assert(current.first.hasInListAC("SA_INIT"));
         std::cout << "begin se: " << std::endl;
         se.first.printListAC();
         assert(se.first.hasInListAC("SA_INIT"));
         std::cout << std::endl;
#endif
*/
         //S* sCurrent = &s.clone();
         //Evaluation<>* eCurrent = &e.clone();
         //S& sCurrent = current.first;
         //XEv& eCurrent = current.second;

         if (Component::verbose)
            std::cout << "SA(verbose): will apply move. current=" << current << std::endl;

         move->applyUpdate(current);

         if (Component::verbose)
            std::cout << "SA(verbose): will reevaluate. current.first=" << current.first << std::endl;

         evaluator->reevaluate(current);

         // SANITY CHECK
         assert(!current.second.outdated);

         if (Component::verbose)
            std::cout << "SA(verbose): after reevaluate. current=" << current << std::endl;

         /*
#ifdef OPTFRAME_AC
         // requires first part of solution to be a component
         static_assert(XRSolution<typename XSH::first_type, typename XSH::first_type::typeR>);
         {
            std::stringstream ss;
            ss << move->toString();
            current.first.listAC.push_back(ContextAC{ .id{ "SA_MOVE_APPLY" }, .message{ ss.str() }, .payload{ move->sharedClone() } });
         }
         //
         //star->first.printListAC();
#endif
*/

         if (Component::verbose)
            std::cout << "SA(verbose): will compare betterStrict" << std::endl;

         //if (evaluator.betterThan(eCurrent, e)) // TODO: replace by 'se' here, and use 'se.second' to compare directly
         //if(eCurrent.betterStrict(e))
         if (evaluator->betterStrict(current.second, se.second)) {
            se = current;

// CONSIDER THAT MOVE IS PERFORMED HERE!!
#ifdef OPTFRAME_AC
            // requires first part of solution to be a component
            static_assert(XRSolution<typename XSH::first_type, typename XSH::first_type::typeR>);
            {
               std::stringstream ss;
               ss << move->toString();
               se.first.listAC.push_back(ContextAC{ .id{ "SA_MOVE_APPLY_GOOD" }, .message{ ss.str() }, .payload{ move->sharedClone() } });
            }
            //
            //star->first.printListAC();
#endif

            /*
#ifdef OPTFRAME_AC
            std::cout << "current: " << std::endl;
            current.first.printListAC();
            assert(current.first.hasInListAC("SA_MOVE_APPLY"));
            std::cout << "se: " << std::endl;
            se.first.printListAC();
            assert(se.first.hasInListAC("SA_MOVE_APPLY"));
            std::cout << std::endl;
#endif
*/
            // if improved, accept it
            //e = *eCurrent;
            //s = *sCurrent;
            //delete sCurrent;
            //delete eCurrent;

            //if (evaluator.betterThan(e, eStar))
            //if(e.betterStrict(eStar))
            if (evaluator->betterStrict(se.second, star->second)) {
               //delete sStar;
               //sStar = &s.clone();
               //delete eStar;
               //eStar = &e.clone();
               star = make_optional(se);
               /*
#ifdef OPTFRAME_AC
               std::cout << "se: " << std::endl;
               se.first.printListAC();
               assert(se.first.hasInListAC("SA_MOVE_APPLY"));
               std::cout << "star: " << std::endl;
               star->first.printListAC();
               //assert(star->first.hasInListAC("SA_MOVE_APPLY"));
               assert(star->first.listAC.size() == se.first.listAC.size());
#endif
   */

               cout << "Best fo: " << se.second.evaluation() << " Found on Iter = " << ctx.iterT << " and T = " << ctx.T;
               cout << endl;

#ifdef OPTFRAME_AC
               // requires first part of solution to be a component
               static_assert(XRSolution<typename XSH::first_type, typename XSH::first_type::typeR>);
               {
                  std::stringstream ss;
                  ss << star->second.evaluation();
                  star->first.listAC.push_back(ContextAC{ .id{ "SA_NEW_STAR" }, .message{ ss.str() }, .payload{ star->first.sharedClone() } });
               }
               //
               //star->first.printListAC();
#endif

               /*
#ifdef OPTFRAME_AC
               assert(se.first.hasInListAC("SA_MOVE_APPLY"));
               assert(star->first.hasInListAC("SA_MOVE_APPLY"));
#endif
*/
            }
         } else {
            // 'current' didn't improve, but may accept it anyway
            double x = rg->rand01();
            double delta = ::fabs(current.second.evaluation() - se.second.evaluation());

            if (x < ::exp(-delta / ctx.T)) {
               se = current;
               //s = *sCurrent;
               //e = *eCurrent;
               //delete sCurrent;
               //delete eCurrent;
               // } else {
               //delete sCurrent;
               //delete eCurrent;

// CONSIDER THAT MOVE IS PERFORMED HERE!!
#ifdef OPTFRAME_AC
               // requires first part of solution to be a component
               static_assert(XRSolution<typename XSH::first_type, typename XSH::first_type::typeR>);
               {
                  std::stringstream ss;
                  ss << move->toString();
                  se.first.listAC.push_back(ContextAC{ .id{ "SA_MOVE_APPLY_BAD" }, .message{ ss.str() }, .payload{ move->sharedClone() } });
               }
               //
               //star->first.printListAC();
#endif
            }
         }

         // update iterT and temperature in next method
         onBeforeLoopCtx(ctx);
      }
      std::cout << "T=" << ctx.T << std::endl;

#ifdef OPTFRAME_AC
      // requires first part of solution to be a component
      static_assert(XRSolution<typename XSH::first_type, typename XSH::first_type::typeR>);
      {
         std::stringstream ss;
         ss << star->second.evaluation();
         star->first.listAC.push_back(ContextAC{ .id{ "SA_FINAL" }, .message{ ss.str() }, .payload{ star->first.sharedClone() } });
      }
      //
      //std::cout << "SA: final star" << std::endl;
      //star->first.printListAC();
#endif

      return { SearchStatus::NO_REPORT, star };
   }

   // reimplementing searchBy, just to make it more explicit (visible)
   // maybe add some specific logs?
   virtual SearchOutput<XES, XSH> searchBy(
     XES& _best,
     XES& _inc,
     const StopCriteria<XEv>& stopCriteria) override
   {
      assert(false); // TODO: implement... and check if 'best' and 'incumbent' are both useful and necessary!
      //this->best = _best;
      //this->incumbent = _inc;
      return search(stopCriteria);
   }

   virtual string id() const override
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

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class BasicSimulatedAnnealingBuilder : public SA
  , public SingleObjSearchBuilder<S, XEv, XES>
{
   //using XM = BasicSimulatedAnnealing<S, XEv, pair<S, XEv>, Component>;
   //using XM = Component; // only general builds here
public:
   virtual ~BasicSimulatedAnnealingBuilder()
   {
   }

   // has sptr instead of sref, is that on purpose or legacy class?
   virtual SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {

      if (Component::debug)
         std::cout << "BasicSA Builder Loading Parameter #0" << std::endl;
      if (!scanner.hasNext()) {
         if (Component::warning)
            std::cout << "no next1a! aborting..." << std::endl;
         return nullptr;
      }

      sptr<GeneralEvaluator<XES, XEv>> eval;
      std::string sid_0 = scanner.next();
      int id_0 = *scanner.nextInt();
      hf.assign(eval, id_0, sid_0);
      assert(eval);

      sptr<GeneralEvaluator<XES, XEv>> ge{ eval };

      if (Component::debug)
         std::cout << "BasicSA Builder Loading Parameter #2" << std::endl;
      if (!scanner.hasNext()) {
         if (Component::warning)
            std::cout << "no next1b! aborting..." << std::endl;
         return nullptr;
      }
      //Constructive<S>* constructive;
      sptr<InitialSearch<XES, XEv>> constructive;
      std::string sid_1 = scanner.next();
      int id_1 = *scanner.nextInt();
      hf.assign(constructive, id_1, sid_1);
      assert(constructive);

      //return nullptr;

      if (Component::debug)
         std::cout << "BasicSA Builder Loading Parameter #3" << std::endl;
      if (!scanner.hasNext()) {
         if (Component::warning)
            std::cout << "no next1c! aborting..." << std::endl;
         return nullptr;
      }
      vsptr<NS<XES, XEv>> _hlist;

      std::string sid_2 = scanner.next();
      int id_2 = *scanner.nextInt();
      hf.assignList(_hlist, id_2, sid_2);
      vsref<NS<XES, XEv>> hlist;
      for (sptr<NS<XES, XEv>> x : _hlist) {
         assert(x);
         hlist.push_back(x);
      }

      if (Component::debug)
         std::cout << "list ok!" << hlist.size() << std::endl;

      if (Component::debug)
         std::cout << "BasicSA Builder Loading Parameter #4" << std::endl;
      if (!scanner.hasNext()) {
         if (Component::debug)
            std::cout << "no next alpha! aborting..." << std::endl;
         return nullptr;
      }
      double alpha = *scanner.nextDouble();

      if (Component::debug)
         std::cout << "BasicSA Builder Loading Parameter #5" << std::endl;
      if (!scanner.hasNext()) {
         if (Component::warning)
            std::cout << "no next SAmax! aborting..." << std::endl;
         return nullptr;
      }
      int SAmax = *scanner.nextInt();

      if (Component::debug)
         std::cout << "BasicSA Builder Loading Parameter #6" << std::endl;
      if (!scanner.hasNext()) {
         if (Component::warning)
            std::cout << "no next Ti! aborting..." << std::endl;
         return nullptr;
      }
      double Ti = *scanner.nextDouble();

      if (Component::debug) {
         std::cout << "BasicSA Builder: got all parameters!" << std::endl;
         std::cout << "BasicSimulatedAnnealing with:" << std::endl;
         std::cout << "\teval=" << ge->id() << std::endl;
         std::cout << "\tconstructive=" << constructive->id() << std::endl;
         std::cout << "\t|hlist|=" << hlist.size() << std::endl;
         std::cout << "\thlist[0]=" << hlist[0]->id() << std::endl;
         std::cout << "\talpha=" << alpha << std::endl;
         std::cout << "\tSAmax=" << SAmax << std::endl;
         std::cout << "\tTi=" << Ti << std::endl;
      }

      return new BasicSimulatedAnnealing<XES>(ge, constructive, hlist, alpha, SAmax, Ti, hf.getRandGen());
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      //params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
      params.push_back(make_pair(Evaluator<typename XES::first_type, typename XES::second_type, XES>::idComponent(), "evaluation function"));
      //
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
      return component == BasicSimulatedAnnealing<XES>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << SingleObjSearchBuilder<S, XEv>::idComponent() << ":" << SA::family() << "BasicSA";
      return ss.str();
   }

   virtual string id() const override
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

#endif
