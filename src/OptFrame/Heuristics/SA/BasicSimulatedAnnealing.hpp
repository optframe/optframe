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

      // initial setup for incumbent (if has star, copy star, otherwise generate one)
      if (!incumbent)
         incumbent = star ? star : constructive->initialSearch(sosc).first;

#ifdef OPTFRAME_AC
      // requires first part of solution to be a component
      static_assert(XRSolution<typename XSH::first_type, typename XSH::first_type::typeR>);
      {
         std::stringstream ss;
         ss << incumbent->second.evaluation();
         incumbent->first.listAC.push_back(ContextAC{ .id{ "SA_INIT" }, .message{ ss.str() }, .payload{ incumbent->first.sharedClone() } });
      }
#endif

      // if no star and has incumbent, star is incumbent
      if (!star && incumbent)
         star = incumbent;

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

      // initialize search context for Simulated Annealing
      SearchContext ctx{ .self = *this, .best = star, .incumbent = incumbent };

      // ===================
      // Simulated Annealing
      // ===================

      XSH& se = *incumbent;

      ctx.T = Ti;
      ctx.iterT = 0;

      //XES se = *star; // copy (implicit cloning guaranteed??)
      //
      //XSolution& s = se.first;
      //
      //XEv& e = se.second;

      //pair<S, XEv> star = se; // copy (implicit cloning guaranteed??)
      //
      ////S* sStar = &s.clone();
      //S& sStar = star.first;
      ////Evaluation<>* eStar = &e.clone();
      //
      //XEv& eStar = star->second;

      // try specific stop criteria, otherwise just use standard one
      //while (stop.specific ? stop.shouldStop(eStar, reinterpret_cast<XM*>(this)) : ((T > 0.000001) && (tnow.now() < timelimit)))
      //while (sosc.specific ? sosc.shouldStop(star, reinterpret_cast<XM*>(this)) : ((T > 0.000001) && (tnow.now() < timelimit)))
      //
      //op<XEv> opEvStar = make_optional(eStar);
      /*
      while (
        !(
          specificStopBy(*star, sosc, this) ||
          sosc.shouldStop(opEvStar))) {
         while ((iterT < SAmax) && (tnow.now() < timelimit)) {
            */
      while (onLoopCtx(ctx, sosc)) {
         int n = rg->rand(neighbors.size());
         uptr<Move<XES, XEv, XSH>> move = neighbors[n]->validRandomMove(se); // TODO: pass 'se.first' here (even 'se' should also work...)

         if (!move) {
            if (Component::warning)
               cout << "SA warning: no move in iter=" << ctx.iterT << " T=" << ctx.T << "! continue..." << endl;
            // TODO: return FAIL?
            //continue;
            return { SearchStatus::NO_REPORT, star };
         }

         XES current(se); // implicit clone??

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

         move->applyUpdate(current);
         evaluator->reevaluate(current);

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
      sptr<GeneralEvaluator<XES, XEv>> eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      //Constructive<S>* constructive;
      sptr<InitialSearch<XES, XEv>> constructive;
      hf.assign(constructive, *scanner.nextInt(), scanner.next()); // reads backwards!

      vsptr<NS<XES, XEv>> _hlist;
      hf.assignList(_hlist, *scanner.nextInt(), scanner.next()); // reads backwards!
      vsref<NS<XES, XEv>> hlist;
      for (auto x : _hlist)
         hlist.push_back(x);

      double alpha = *scanner.nextDouble();
      int SAmax = *scanner.nextInt();
      double Ti = *scanner.nextDouble();

      return new BasicSimulatedAnnealing<XES>(eval, constructive, hlist, alpha, SAmax, Ti, hf.getRandGen());
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
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
      return component == BasicSimulatedAnnealing<XES>::idComponent();
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

// Attempt on implementing SpecificMethodStop, from what I gathered
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

   //
   //private:
public:
   using XEv = typename XES::second_type;
   using XSH = XES; // XSearch
   //
   sref<GeneralEvaluator<XES, XEv>> evaluator;
   //Constructive<S>& constructive; // TODO: this must become InitialSearch, already starting from "optional" XES element.
   sref<InitialSearch<XES, XEv>> constructive;

   vsref<NS<XES, XEv, XSH>> neighbors;
   sref<RandGen> rg;
   double alpha;
   int SAmax;
   double Ti;

   // --------------
   // local variables (beware that this is NOT thread-safe!)
   // --------------
   // if you need multiple threads of this method, you should instantiate multiple methods!!
   // this allows monitoring on progress and many other nice things, such as StopCriteria personalization ;)
   //SpecificMethodStop<XES, decltype(*this)> specificStop;

public: // Are both public declarations on purpose?
   //SpecificMethodStop<XES, XEv, BasicSimulatedAnnealing<XES>> specificStopBy {nullptr};
   SpecificMethodStop<XES, XEv, BasicSimulatedAnnealing<XES>> specificStopBy{
      [&](const XES& best, const StopCriteria<XEv>& sosc, BasicSimulatedAnnealing<XES>* me) -> bool {
         return (me->T <= 0.000001) || (me->tnow.now() >= sosc.timelimit);
      }
   };

   /*private:
   SpecificMethodStop<XES, XEv, BasicSimulatedAnnealing<XES>> defaultStopBy
    {
      [&](const XES& best, const StopCriteria<XEv>& sosc, BasicSimulatedAnnealing<XES>* me) -> bool {
         return (me->T <= 0.000001) || (me->tnow.now() >= sosc.timelimit);
      }   
   };
*/

   //private:
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

   /*
   BasicSimulatedAnnealing(GeneralEvaluator<XES, XEv>& _evaluator, InitialSearch<XES, XEv>& _constructive, vector<NS<XES, XEv, XSH>*> _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg)
     : evaluator(_evaluator)
     , constructive(_constructive)
     , neighbors(_neighbors)
     , rg(_rg) //, specificStopBy(defaultStopBy)
   {
      alpha = (_alpha);
      SAmax = (_SAmax);
      Ti = (_Ti);
   }

   BasicSimulatedAnnealing(GeneralEvaluator<XES, XEv>& _evaluator, InitialSearch<XES, XEv>& _constructive, NS<XES, XEv, XSH>& _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg)
     : evaluator(_evaluator)
     , constructive(_constructive)
     , rg(_rg) //, specificStopBy(defaultStopBy)
   {
      neighbors.push_back(&_neighbors);
      alpha = (_alpha);
      SAmax = (_SAmax);
      Ti = (_Ti);
   }

   */

   /*
src/mainTSP-fcore.cpp:67:4: error: no matching function for call to 'optframe::BasicSimulatedAnnealing<std::pair<std::vector<int>, optframe::Evaluation<> > >::BasicSimulatedAnnealing(optframe::sref<optframe::GeneralEvaluator<std::pair<std::vector<int>, optframe::Evaluation<> >, optframe::Evaluation<>, std::pair<std::vector<int>, optframe::Evaluation<> > > >&, optframe::sref<optframe::BasicInitialSearch<std::pair<std::vector<int>, optframe::Evaluation<> > > >&, optframe::sref<optframe::NSSeq<std::pair<std::vector<int>, optframe::Evaluation<> >, optframe::Evaluation<>, std::pair<std::vector<int>, optframe::Evaluation<> > > >&, double, int, int, optframe::sref<optframe::RandGen>&)'
   67 |    );
WHERE IS THE ERROR?
*/

   BasicSimulatedAnnealing(sref<GeneralEvaluator<XES, XEv>> _evaluator, sref<InitialSearch<XES, XEv>> _constructive, sref<NS<XES, XEv, XSH>> _neighbors, double _alpha, int _SAmax, double _Ti, sref<RandGen> _rg)
     : evaluator(_evaluator)
     , constructive(_constructive)
     , rg(_rg) //, specificStopBy(defaultStopBy)
   {
      neighbors.push_back(_neighbors);
      alpha = (_alpha);
      SAmax = (_SAmax);
      Ti = (_Ti);
   }

   /*
   BasicSimulatedAnnealing(sref<GeneralEvaluator<XES, XEv>> _evaluator, sref<InitialSearch<XES, XEv>> _constructive, vsref<NS<XES, XEv, XSH>> _neighbors, double _alpha, int _SAmax, double _Ti, sref<RandGen> _rg)
     : evaluator(_evaluator)
     , constructive(_constructive)
     , neighbors(_neighbors)
     , rg(_rg) //, specificStopBy(defaultStopBy)
   {
      alpha = (_alpha);
      SAmax = (_SAmax);
      Ti = (_Ti);
   }


   BasicSimulatedAnnealing(sref<GeneralEvaluator<XES, XEv>> _evaluator, sref<InitialSearch<XES, XEv>> _constructive, sref<NS<XES, XEv, XSH>> _neighbors, double _alpha, int _SAmax, double _Ti, sref<RandGen> _rg)
     : evaluator(_evaluator)
     , constructive(_constructive)
     , rg(_rg) //, specificStopBy(defaultStopBy)
   {
      neighbors.push_back(_neighbors);
      alpha = (_alpha);
      SAmax = (_SAmax);
      Ti = (_Ti);
   }
*/

   //virtual ~BasicSimulatedAnnealing()
   //{
   // }

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

   struct SearchContext
   {
      double T;
      int iterT;
      op<XES> star;
   };

   bool (*onLoopCtx)(const SearchContext& ctx, const StopCriteria<XEv>& sosc) =
     [](auto& ctx, auto& sosc) {
        return (ctx->T >= 0.000001) && !sosc.shouldStop(std::make_optional(ctx.star->second));
     };

   SearchOutput<XES, XSH> search(const StopCriteria<XEv>& sosc) override
   {
      //op<XES>& star = this->best;
      op<XES> star = std::nullopt;
      double timelimit = sosc.timelimit;

      //XEv target_f(stop.target_f); // 'target_f' will break... removing

      //cout << "SA search(" << target_f << "," << timelimit << ")" << endl;
      cout << "SA search(" << timelimit << ")" << endl; // TODO: require 'stop'.toString()

      tnow = Timer();

      // TODO: verify 's' and 'input'
      //pair<S, XEv> se = genPair(timelimit);
      if (!star)
         //star = SingleObjSearch<XES>::genPair(constructive, evaluator, timelimit);
         star = constructive->initialSearch(sosc).first;
      if (!star)
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
          sosc.shouldStop(opEvStar))) {
         while ((iterT < SAmax) && (tnow.now() < timelimit)) {
            int n = rg->rand(neighbors.size());
            uptr<Move<XES, XEv, XSH>> move = neighbors[n]->validRandomMove(se); // TODO: pass 'se.first' here (even 'se' should also work...)

            if (!move) {
               if (Component::warning)
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
            evaluator->reevaluate(current);

            //if (evaluator.betterThan(eCurrent, e)) // TODO: replace by 'se' here, and use 'se.second' to compare directly
            //if(eCurrent.betterStrict(e))
            if (evaluator->betterStrict(eCurrent, e)) {
               se = current;
               // if improved, accept it
               //e = *eCurrent;
               //s = *sCurrent;
               //delete sCurrent;
               //delete eCurrent;

               //if (evaluator.betterThan(e, eStar))
               //if(e.betterStrict(eStar))
               if (evaluator->betterStrict(e, eStar)) {
                  star = make_optional(se);
                  //delete sStar;
                  //sStar = &s.clone();
                  //delete eStar;
                  //eStar = &e.clone();

                  cout << "Best fo: " << e.evaluation() << " Found on Iter = " << iterT << " and T = " << T;
                  cout << endl;
               }
            } else {
               // 'current' didn't improve, but may accept it anyway
               double x = rg->rand01();
               double delta = ::fabs(eCurrent.evaluation() - e.evaluation());

               if (x < exp(-delta / T)) {
                  se = current;
                  //s = *sCurrent;
                  //e = *eCurrent;
                  //delete sCurrent;
                  //delete eCurrent;
                  //} else {
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
      //this->best = star;
      return { SearchStatus::NO_REPORT, star };
   }

   // reimplementing searchBy, just to make it more explicit (visible)
   // maybe add some specific logs?
   virtual SearchOutput<XES, XSH> searchBy(
     std::optional<XES>& _best,
     std::optional<XES>& _inc,
     const StopCriteria<XEv>& stopCriteria) override
   {
      assert(false); // TODO: implement... and check if 'best' and 'incumbent' are both useful and necessary!
      //this->best = _best;
      //this->incumbent = _inc;
      return search(stopCriteria);
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

   virtual SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      GeneralEvaluator<XES, XEv>* eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      //Constructive<S>* constructive;
      InitialSearch<XES, XEv>* constructive;
      hf.assign(constructive, *scanner.nextInt(), scanner.next()); // reads backwards!

      vector<NS<XES, XEv>*> hlist;
      hf.assignList(hlist, *scanner.nextInt(), scanner.next()); // reads backwards!

      double alpha = *scanner.nextDouble();
      int SAmax = *scanner.nextInt();
      double Ti = *scanner.nextDouble();

      return new BasicSimulatedAnnealing<XES>(*eval, *constructive, hlist, alpha, SAmax, Ti, hf.getRandGen());
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
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
      return component == BasicSimulatedAnnealing<XES>::idComponent();
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
