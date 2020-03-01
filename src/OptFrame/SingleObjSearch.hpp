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

#ifndef OPTFRAME_SINGLE_OBJ_SEARCH_HPP_
#define OPTFRAME_SINGLE_OBJ_SEARCH_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "BaseConcepts.hpp"

#include "Action.hpp"
#include "Evaluation.hpp"
#include "Solution.hpp"
#include "Constructive.hpp" // for helper only  (TODO: make special class)
#include "Evaluator.hpp" // for helper only (TODO: make special class)

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "Timer.hpp" // TODO: move together with StopCriteria to other file!!

#include "GlobalSearch.hpp"

namespace optframe {

//template<XSolution S, XEvaluation XEv = Evaluation<>>
//concept SolEv;

//template<class R, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
// if replacing types directly (no templates), concept deduction cannot appear in virtual functions. So, we need templates.
template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XSH = std::pair<S, XEv>>
class SingleObjSearch: public GlobalSearch<S, XEv, XSH> // public Component
{
   // if passing types directly here, error 'typedef declared auto'
   typedef vector<XEv*> FitnessValues;
   typedef const vector<const XEv*> ConstFitnessValues;

public:
   SingleObjSearch()
   {
   }

   virtual ~SingleObjSearch()
   {
   }

   // helper: constructive method or using 'input'
   static std::optional<pair<S, XEv>> genPair(Constructive<S>& constructive, Evaluator<S, XEv>& evaluator, double timelimit, const std::optional<pair<S, XEv>> input = std::nullopt)
   {
      if(input)
         return input;
      std::optional<S> sStar = constructive.generateSolution(timelimit);
      if(!sStar)
         return std::nullopt;
		XEv eStar = evaluator.evaluate(*sStar);
      return make_pair(*sStar, eStar);
   }


   // search method try to find a feasible solution within timelimit, if there is no such solution it returns nullptr.
   //virtual pair<S, XEv>* search(StopCriteria<XEv>& stopCriteria, const S* _s = nullptr, const XEv* _e = nullptr) = 0;
   //virtual std::optional<pair<S, XEv>> search(StopCriteria<XEv>& stopCriteria, const std::optional<pair<S, XEv>> input = std::nullopt) = 0;
   //virtual std::optional<pair<S, XEv>> search(StopCriteria<XEv>& stopCriteria) = 0;
   virtual SearchStatus search(std::optional<pair<S, XEv>>& inputOutput, const StopCriteria<XEv>& stopCriteria) = 0;

   virtual string log() const
   {
      return "Empty heuristic log.";
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (Component::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":SingleObjSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

//template<class R, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
//template<XSolution S, XEvaluation XEv = Evaluation<>>
template<XSolution S, XEvaluation XEv = Evaluation<>, X2ESolution<S, XEv> X2ES = MultiESolution<S, XEv>>
class SingleObjSearchBuilder : public ComponentBuilder<S, XEv, X2ES>
{
public:
   virtual ~SingleObjSearchBuilder()
   {
   }

   virtual SingleObjSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, X2ES>& hf, string family = "") = 0;

   virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, X2ES>& hf, string family = "")
   {
      return build(scanner, hf, family);
   }

   virtual vector<pair<string, string>> parameters() = 0;

   virtual bool canBuild(string) = 0;

   static string idComponent()
   {
      stringstream ss;
      ss << ComponentBuilder<S, XEv, X2ES>::idComponent() << "SingleObjSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

//template<class R, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
//template<XSolution S, XEvaluation XEv = Evaluation<>>
template<XSolution S, XEvaluation XEv = Evaluation<>, X2ESolution<S, XEv> X2ES = MultiESolution<S, XEv>>
class SingleObjSearchAction : public Action<S, XEv, X2ES>
{
public:
   virtual ~SingleObjSearchAction()
   {
   }

   virtual string usage()
   {
      return "OptFrame:SingleObjSearch idx   search    timelimit  target_f  OptFrame:Solution idx|-1   OptFrame:Evaluation idx|-1   [output_variable] => OptFrame:Solution|nullptr";
   }

   virtual bool handleComponent(string type)
   {
      return Component::compareBase(SingleObjSearch<S, XEv>::idComponent(), type);
   }

   virtual bool handleComponent(Component& component)
   {
      return component.compatible(SingleObjSearch<S, XEv>::idComponent());
   }

   virtual bool handleAction(string action)
   {
      return (action == "search");
   }

   virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<S, XEv, X2ES>& hf, map<string, string>& d)
   {
      if (!handleComponent(type)) {
         cout << "SingleObjSearchAction::doCast error: can't handle component type '" << type << " " << id << "'" << endl;
         return false;
      }

      Component* comp = hf.components[component].at(id);

      if (!comp) {
         cout << "SingleObjSearchAction::doCast error: nullptr component '" << component << " " << id << "'" << endl;
         return false;
      }

      if (!Component::compareBase(comp->id(), type)) {
         cout << "SingleObjSearchAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
         return false;
      }

      // remove old component from factory
      hf.components[component].at(id) = nullptr;

      // cast object to lower type
      Component* final = nullptr;

      if (type == SingleObjSearch<S, XEv>::idComponent()) {
         final = (SingleObjSearch<S, XEv>*)comp;
      } else {
         cout << "SingleObjSearchAction::doCast error: no cast for type '" << type << "'" << endl;
         return false;
      }

      // add new component
      Scanner scanner(variable);
      return ComponentAction<S, XEv>::addAndRegister(scanner, *final, hf, d);
   }

   virtual bool doAction(string content, HeuristicFactory<S, XEv, X2ES>& hf, map<string, string>& dictionary, map<string, vector<string>>& ldictionary)
   {
      //cout << "LocalSearch::doAction '" << content << "'" << endl;

      Scanner scanner(content);

      if (!scanner.hasNext())
         return false;

      SingleObjSearch<S, XEv>* sios;
      hf.assign(sios, scanner.nextInt(), scanner.next());

      if (!sios)
         return false;

      if (!scanner.hasNext())
         return false;

      string action = scanner.next();

      if (!handleAction(action))
         return false;

      if (action == "search") {
         if (!scanner.hasNext())
            return false;

         double timelimit = scanner.nextDouble();

         if (!scanner.hasNext())
            return false;

         double target_f = scanner.nextDouble();

         if (!scanner.hasNext())
            return false;

         S* s;
         hf.assign(s, scanner.nextInt(), scanner.next());

         if (!scanner.hasNext())
            return false;

         XEv* e;
         hf.assign(e, scanner.nextInt(), scanner.next());

         pair<S, XEv>* p = sios->search(StopCriteria(timelimit, target_f), s, e);

         if (!p)
            return true;

         // TODO: use Move Semantics
         S* s2 = new S(p->first);

         delete p;

         return Action<S, XEv, X2ES>::addAndRegister(scanner, *s2, hf, dictionary);
      }

      // no action found!
      return false;
   }
};
}

#endif /* OPTFRAME_SINGLE_OBJ_SEARCH_HPP_ */
