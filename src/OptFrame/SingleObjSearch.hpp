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

#ifndef OPTFRAME_SINGLE_OBJ_SEARCH_HPP_
#define OPTFRAME_SINGLE_OBJ_SEARCH_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "BaseConcepts.hpp"

#include "Action.hpp"
#include "Constructive.hpp" // for helper only  (TODO: make special class)
#include "Evaluation.hpp"
#include "Evaluator.hpp" // for helper only (TODO: make special class)
#include "Solution.hpp"

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "Timer.hpp" // TODO: move together with StopCriteria to other file!!

#include "GlobalSearch.hpp"

namespace optframe {

//template<XESolution XES, XEvaluation XEv = Evaluation<>>
//concept SolEv;

// This is a XES, XES global search... using space XES = <S, XEv>
//template<XESolution XES, XSearchMethod XM = Component>
//
//template<XESolution XES, XEvaluation XEv = Evaluation<>>
//
// (Primary) Search space is decided by XES
// Secondary search space XSH2 is undecided... could be trajectory-based (as default) or population-based
template<XESolution XES, XESolution XES2 = XES, XSearch<XES2> XSH2 = XES2>
class SingleObjSearch : public GlobalSearch<XES, XES, XES2, XSH2> // public Component
{
   using XEv = typename XES::second_type;
   // if passing types directly here, error 'typedef declared auto'
   //typedef vector<XEv*> FitnessValues;
   //typedef const vector<const XEv*> ConstFitnessValues;

   using XSH = XES; // XSearch<XES> = XES

public:
   SingleObjSearch()
   {
   }

   virtual ~SingleObjSearch()
   {
   }

   // search method try to find a feasible solution within timelimit, if there is no such solution it returns nullptr.
   //virtual pair<S, XEv>* search(StopCriteria<XEv>& stopCriteria, const S* _s = nullptr, const XEv* _e = nullptr) = 0;
   //virtual std::optional<pair<S, XEv>> search(StopCriteria<XEv>& stopCriteria, const std::optional<pair<S, XEv>> input = std::nullopt) = 0;

   //virtual std::optional<pair<S, XEv>> search(StopCriteria<XEv>& stopCriteria) = 0;
   //virtual SearchStatus search(op<XSH>& inputOutput, const XStop& stopCriteria) = 0;
   //virtual SearchStatus search(op<XSH>& inputOutput, const StopCriteria<XEv>& stopCriteria) = 0;
   //
   // inputOutput now passed directly on local variable 'this->best'
   //
   //virtual SearchStatus search(const StopCriteria<XEv>& stopCriteria) = 0;
   virtual SearchOutput<XES> search(const StopCriteria<XEv>& stopCriteria) override = 0;

   virtual string log() const
   {
      return "Empty heuristic log.";
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (GlobalSearch<XES, XES, XES2, XSH2>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << GlobalSearch<XES, XES, XES2, XSH2>::idComponent() << "SingleObjSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual std::string toString() const override
   {
      return id();
   }
};

//template<class R, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
//template<XESolution XES, XEvaluation XEv = Evaluation<>>
template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class SingleObjSearchBuilder : public ComponentBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~SingleObjSearchBuilder()
   {
   }

   virtual SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") = 0;

   virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      return build(scanner, hf, family);
   }

   virtual vector<pair<string, string>> parameters() = 0;

   virtual bool canBuild(string) = 0;

   static string idComponent()
   {
      stringstream ss;
      ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "SingleObjSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual std::string toString() const override
   {
      return id();
   }
};

//template<class R, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
//template<XESolution XES, XEvaluation XEv = Evaluation<>>
template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
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
      return ComponentHelper::compareBase(SingleObjSearch<XES>::idComponent(), type);
   }

   virtual bool handleComponent(Component& component)
   {
      return component.compatible(SingleObjSearch<XES>::idComponent());
   }

   virtual bool handleAction(string action)
   {
      return (action == "search");
   }

   virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<S, XEv, XES, X2ES>& hf, map<string, string>& d)
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

      if (!ComponentHelper::compareBase(comp->id(), type)) {
         cout << "SingleObjSearchAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
         return false;
      }

      // remove old component from factory
      hf.components[component].at(id) = nullptr;

      // cast object to lower type
      Component* final = nullptr;

      if (type == SingleObjSearch<XES>::idComponent()) {
         final = (SingleObjSearch<XES>*)comp;
      } else {
         cout << "SingleObjSearchAction::doCast error: no cast for type '" << type << "'" << endl;
         return false;
      }

      // add new component
      Scanner scanner(variable);
      return ComponentAction<S, XEv>::addAndRegister(scanner, *final, hf, d);
   }

   virtual bool doAction(string content, HeuristicFactory<S, XEv, XES, X2ES>& hf, map<string, string>& dictionary, map<string, vector<string>>& ldictionary)
   {
      //cout << "LocalSearch::doAction '" << content << "'" << endl;

      Scanner scanner(content);

      if (!scanner.hasNext())
         return false;

      SingleObjSearch<XES>* sios;
      hf.assign(sios, *scanner.nextInt(), scanner.next());

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

         double timelimit = *scanner.nextDouble();

         if (!scanner.hasNext())
            return false;

         double target_f = *scanner.nextDouble();

         if (!scanner.hasNext())
            return false;

         S* s;
         hf.assign(s, *scanner.nextInt(), scanner.next());

         if (!scanner.hasNext())
            return false;

         XEv* e;
         hf.assign(e, *scanner.nextInt(), scanner.next());

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
