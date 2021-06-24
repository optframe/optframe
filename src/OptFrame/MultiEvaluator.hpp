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

#ifndef OPTFRAME_MULTI_EVALUATOR_HPP_
#define OPTFRAME_MULTI_EVALUATOR_HPP_

#include <iostream>

#include "Action.hpp"
#include "Evaluator.hpp"
#include "MultiDirection.hpp"
#include "MultiEvaluation.hpp"
#include "Solution.hpp"

using namespace std;
using namespace scannerpp;

namespace optframe {

// MultiEvaluator is not a REAL evaluator... a bunch/pack of evaluators... TODO: unify

template<XSolution S, XEvaluation XEv, XEvaluation XMEv = MultiEvaluation<>, XESolution XMES = pair<S, XMEv>, XSearch<XMES> XSH = XMES> //, XSearch<XES> XSH = MultiESolution<XES> >
class MultiEvaluator : public MultiDirection
  , public GeneralEvaluator<XMES, XMEv, XSH>
{
   //XESolution XES = pair<S, XMEv>,
   using XES = pair<S, XEv>;

protected:
   vector<Evaluator<S, XEv, XES>*> sngEvaluators; // single evaluators
   bool allowCosts;                               // move.cost() is enabled or disabled for this Evaluator

public:
   MultiEvaluator(vector<Evaluator<S, XEv, XES>*> _veval)
     : sngEvaluators(_veval)
     , allowCosts(false)
   {
      for (unsigned i = 0; i < _veval.size(); i++)
         if (_veval[i])
            vDir.push_back(_veval[i]);
      nObjectives = vDir.size();
   }

   MultiEvaluator(bool _allowCosts = false)
     : allowCosts(_allowCosts)
   {
   }

   virtual void addEvaluator(Evaluator<S, XEv, XES>& ev)
   {
      sngEvaluators.push_back(&ev);
   }

   //	MultiEvaluator(MultiDirection& mDir, bool _allowCosts = false) :
   //			MultiDirection(mDir), allowCosts(_allowCosts)
   //	{
   //	}
   //
   //	MultiEvaluator(vector<Direction*>& vDir, bool _allowCosts = false) :
   //			MultiDirection(vDir), allowCosts(_allowCosts)
   //	{
   //	}

   //	MultiEvaluator(MultiEvaluator<S, XEv>& _mev) :
   //			sngEvaluators(*_mev.getEvaluators2()), allowCosts(false)
   //	{
   //		cout<<"sngEvaluators.size():"<<sngEvaluators.size()<<endl;
   //		for (unsigned i = 0; i < sngEvaluators.size(); i++)
   //			if (sngEvaluators[i])
   //				vDir.push_back(sngEvaluators[i]);
   //		nObjectives = vDir.size();
   //	}

   virtual ~MultiEvaluator()
   {
   }

   unsigned size()
   {
      return sngEvaluators.size();
   }

   unsigned size() const
   {
      return sngEvaluators.size();
   }

   virtual bool betterThan(const Evaluation<>& ev1, const Evaluation<>& ev2, int index)
   {
      return sngEvaluators[index]->betterThan(ev1, ev2);
   }

   virtual bool equals(const Evaluation<>& ev1, const Evaluation<>& ev2, int index)
   {
      return sngEvaluators[index]->equals(ev1, ev2);
   }

   //changed to Meval without point TODO
   virtual XMEv evaluate(const S& s)
   {
      cout << "inside mother class" << endl;
      getchar();
      MultiEvaluation<> nev;
      for (unsigned i = 0; i < sngEvaluators.size(); i++) {
         Evaluation<> ev{ sngEvaluators[i]->evaluate(s) };
         nev.addEvaluation(ev);
      }

      return nev;
   }

   void clear()
   {
      for (int e = 0; e < int(sngEvaluators.size()); e++)
         delete sngEvaluators[e];
   }

   //virtual void reevaluateMEV(MultiEvaluation<>& mev, const XES& se)
   //
   //virtual void reevaluate(pair<S, MultiEvaluation<>>& se) override
   //virtual void reevaluate(pair<S, XMEv>& se) override
   virtual void reevaluate(XMES& se) override
   {
      MultiEvaluation<>& mev = se.second;
      //
      for (unsigned i = 0; i < sngEvaluators.size(); i++) {
         //Evaluation<> e { std::move(mev[i]) }; // TODO (IGOR): why move????
         //sngEvaluators[i]->reevaluate(e, s);
         //mev[i] = std::move(e);
         //
         Evaluation<>& e = mev[i]; // TODO: embed MEV in 'se'

         pair<decltype(se.first), XEv> pse(se.first, e); // TODO: we should AVOID this 's' and 'e' copy... by keeping s,e together.
         sngEvaluators[i]->reevaluate(pse);
         e = std::move(pse.second); // TODO: verify if this works

         //mev[i] = std::move(e);
      }
   }

   //	bool getAllowCosts()
   //	{
   //		return allowCosts;
   //	}

   //	vector<Evaluator<XES, XEv>*> getEvaluators2()
   //	{
   //		return sngEvaluators;
   //	}

   //	// TODO: check
   //	const vector<const Evaluator<XES, XEv>*>* getEvaluatorsConstTest() const
   //	{
   //		if (sngEvaluators.size() > 0)
   //			return new vector<const Evaluator<XES, XEv>*>(sngEvaluators);
   //		else
   //			return nullptr;
   //	}

   //	Evaluator<XES, XEv>& at(unsigned index)
   //	{
   //		return *sngEvaluators.at(index);
   //	}
   //
   //	const Evaluator<XES, XEv>& at(unsigned index) const
   //	{
   //		return *sngEvaluators.at(index);
   //	}
   //
   //	Evaluator<XES, XEv>& operator[](unsigned index)
   //	{
   //		return *sngEvaluators[index];
   //	}
   //
   //	const Evaluator<XES, XEv>& operator[](unsigned index) const
   //	{
   //		return *sngEvaluators[index];
   //	}

   //	void addEvaluator(const Evaluator<XES, XEv>& ev)
   //	{
   //		sngEvaluators.push_back(&ev.clone());
   //	}

   // =======================

   // this strictly better than parameter 'e' (for mini, 'this' < 'e')
   virtual bool betterStrict(const XMEv& e1, const XMEv& e2)
   {
      assert(false);
      return false;
   }

   // returns 'true' if this 'cost' (represented by this Evaluation) is improvement
   virtual bool isStrictImprovement(const XMEv& e)
   {
      assert(false);
      return false;
   }

   // returns 'true' if this 'cost' (represented by this Evaluation) is improvement
   virtual bool isNonStrictImprovement(const XMEv& e)
   {
      assert(false);
      return false;
   }

   virtual bool equals(const XMEv& e1, const XMEv& e2)
   {
      assert(false);
      return false;
   }

   // ================================================

protected:
   // ============= Component ===============
   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (MultiDirection::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << MultiDirection::idComponent() << ":MultiEvaluator";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class MultiEvaluatorAction : public Action<S, XEv, X2ES>
{
public:
   virtual ~MultiEvaluatorAction()
   {
   }

   virtual string usage()
   {
      return ":MultiEvaluator idx  evaluate   :Solution idx  [output_variable] => OptFrame:Evaluation";
   }

   virtual bool handleComponent(string type)
   {
      return ComponentHelper::compareBase(MultiEvaluator<S, XEv>::idComponent(), type);
   }

   virtual bool handleComponent(Component& component)
   {
      return component.compatible(MultiEvaluator<S, XEv>::idComponent());
   }

   virtual bool handleAction(string action)
   {
      return (action == "evaluate"); //|| (action == "betterThan") || (action == "betterOrEquals");
   }

   virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<S, XEv, XES, X2ES>& hf, map<string, string>& d)
   {
      cout << "MultiEvaluator::doCast: NOT IMPLEMENTED!" << endl;
      return false;

      if (!handleComponent(type)) {
         cout << "EvaluatorAction::doCast error: can't handle component type '" << type << " " << id << "'" << endl;
         return false;
      }

      Component* comp = hf.components[component].at(id);

      if (!comp) {
         cout << "EvaluatorAction::doCast error: nullptr component '" << component << " " << id << "'" << endl;
         return false;
      }

      if (!ComponentHelper::compareBase(comp->id(), type)) {
         cout << "EvaluatorAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
         return false;
      }

      // remove old component from factory
      hf.components[component].at(id) = nullptr;

      // cast object to lower type
      Component* final = nullptr;

      if (type == Evaluator<XES, XEv>::idComponent()) {
         final = (Evaluator<XES, XEv>*)comp;
      } else {
         cout << "EvaluatorAction::doCast error: no cast for type '" << type << "'" << endl;
         return false;
      }

      // add new component
      Scanner scanner(variable);
      return ComponentAction<S, XEv>::addAndRegister(scanner, *final, hf, d);
   }

   virtual bool doAction(string content, HeuristicFactory<S, XEv, XES, X2ES>& hf, map<string, string>& dictionary, map<string, vector<string>>& ldictionary)
   {
      cout << "MultiEvaluator::doAction: NOT IMPLEMENTED!" << endl;
      return false;

      //cout << "Evaluator::doAction '" << content << "'" << endl;

      Scanner scanner(content);

      if (!scanner.hasNext())
         return false;

      Evaluator<XES, XEv>* ev;
      hf.assign(ev, *scanner.nextInt(), scanner.next());

      if (!ev)
         return false;

      if (!scanner.hasNext())
         return false;

      string action = scanner.next();

      if (!handleAction(action))
         return false;

      if (action == "evaluate") {
         if (!scanner.hasNext())
            return false;

         S* s;
         hf.assign(s, *scanner.nextInt(), scanner.next());

         if (!s)
            return false;

         Evaluation<>& e = ev->evaluate(*s);

         return Action<S, XEv, X2ES>::addAndRegister(scanner, e, hf, dictionary);
      }

      // no action found!
      return false;
   }
};

} // namespace optframe

#endif /*OPTFRAME_MULTI_EVALUATOR_HPP_*/
