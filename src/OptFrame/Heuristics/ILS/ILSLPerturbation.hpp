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

#ifndef OPTFRAME_ILSLPerturbation_HPP_
#define OPTFRAME_ILSLPerturbation_HPP_

#include <math.h>
#include <vector>

#include "../../NS.hpp"
#include "../../RandGen.hpp"

#include "ILS.h"

namespace optframe {

template<XESolution XES, XEvaluation XEv = typename XES::second_type>
class ILSLPerturbation : public Component
  , public ILS
{
public:
   virtual ~ILSLPerturbation()
   {
   }

   virtual void perturb(XES& se, const StopCriteria<XEv>& stopCriteria, int level) = 0;

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (Component::compatible(s));
   }

   std::string toString() const override
   {
      return id();
   }

   virtual string id() const
   {
      return idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":" << ILS::family() << "LevelPert";
      return ss.str();
   }
};

template<XESolution XES, XEvaluation XEv = typename XES::second_type>
class ILSLPerturbationLPlus2 : public ILSLPerturbation<XES, XEv>
{
private:
   vsref<NS<XES, XEv>> ns;
   sref<GeneralEvaluator<XES, XEv>> evaluator;
   sref<RandGen> rg;

public:
   ILSLPerturbationLPlus2(sref<GeneralEvaluator<XES, XEv>> e, sref<NS<XES, XEv>> _ns, sref<RandGen> _rg)
     : evaluator(e)
     , rg(_rg)
   {
      ns.push_back(_ns);
   }

   virtual ~ILSLPerturbationLPlus2()
   {
   }

   void add_ns(sref<NS<XES, XEv>> _ns)
   {
      ns.push_back(_ns);
   }

   void perturb(XES& se, const StopCriteria<XEv>& stopCriteria, int level) override
   {
      int a = 0; // number of appliable moves

      level += 2; // level 0 applies 2 moves

      // local bind
      //XSolution& s = se.first;
      //XEv& e = se.second;

      while (a < level) {
         int x = rg->rand(ns.size());

         uptr<Move<XES, XEv>> m = ns[x]->validRandomMove(se);

         if (m) {
            a++;
            m->applyUpdate(se);
         } else if (Component::warning)
            cout << "ILS Warning: perturbation had no effect in level " << a << "!" << endl;
      }

      evaluator->reevaluate(se); // updates 'e'
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (ILSLPerturbation<XES, XEv>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << ILSLPerturbation<XES, XEv>::idComponent() << ":LPlus2";
      return ss.str();
   }

   std::string toString() const override
   {
      return id();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XESolution XES, XEvaluation XEv = typename XES::second_type>
class ILSLPerturbationLPlus2Prob : public ILSLPerturbation<XES, XEv>
{
private:
   vsref<NS<XES, XEv>> ns;
   vector<pair<int, double>> pNS;
   sref<GeneralEvaluator<XES, XEv>> evaluator;
   sref<RandGen> rg;

public:
   ILSLPerturbationLPlus2Prob(sref<GeneralEvaluator<XES, XEv>> e, sref<NS<XES, XEv>> _ns, sref<RandGen> _rg)
     : evaluator(e)
     , rg(_rg)
   {
      ns.push_back(_ns);
      pNS.push_back(make_pair(1, 1));
   }

   virtual ~ILSLPerturbationLPlus2Prob()
   {
   }

   void add_ns(sref<NS<XES, XEv>> _ns)
   {
      ns.push_back(_ns);
      pNS.push_back(make_pair(1, 1));

      double soma = 0;
      for (int i = 0; i < ns.size(); i++)
         soma += pNS[i].first;
      for (int i = 0; i < ns.size(); i++)
         pNS[i].second = pNS[i].first / soma;
   }

   void changeProb(vector<int> pri)
   {
      int nNeighborhoods = ns.size();
      if (pri.size() != nNeighborhoods) {
         cout << "ERROR ON PRIORITES SIZE!" << endl;
         return;
      }

      double soma = 0;
      for (int i = 0; i < nNeighborhoods; i++) {
         pNS[i].first = pri[i];
         soma += pri[i];
      }

      for (int i = 0; i < ns.size(); i++)
         pNS[i].second = pNS[i].first / soma;

      cout << "Printing probabilities ILSLPerturbationLPlus2Prob:" << endl;
      for (int i = 0; i < ns.size(); i++)
         cout << "pNS[i].first: " << pNS[i].first << "\t pNS[i].second: " << pNS[i].second << endl;
      cout << endl;
   }

   void perturb(XES& se, const StopCriteria<XEv>& stopCriteria, int level) override
   {
      //XSolution& s = se.first;
      //XEv& e = se.second;
      //
      int a = 0; // number of appliable moves

      level += 2; // level 0 applies 2 moves

      while (a < level) {
         double prob = rg->rand01();
         int x = 0;
         double sum = pNS[x].second;

         while (prob > sum) {
            x++;
            sum += pNS[x].second;
         }

         uptr<Move<XES, XEv>> m = ns[x]->validRandomMove(se);

         if (m) {
            a++;
            m->applyUpdate(se);
         } else if (Component::warning)
            cout << "ILS Warning: perturbation had no effect in level " << a << "!" << endl;
      }

      evaluator->reevaluate(se); // updates 'e'
   }

   static string idComponent()
   {
      stringstream ss;
      ss << ILSLPerturbation<XES, XEv>::idComponent() << ":LPlus2Prob";
      return ss.str();
   }

   std::string toString() const override
   {
      return id();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class ILSLPerturbationLPlus2Builder : public ComponentBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~ILSLPerturbationLPlus2Builder()
   {
   }

   virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      sptr<GeneralEvaluator<XES, XEv>> eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      sptr<NS<XES, XEv>> ns;
      hf.assign(ns, *scanner.nextInt(), scanner.next()); // reads backwards!

      return new ILSLPerturbationLPlus2<XES, XEv>(eval, ns, hf.getRandGen());
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
      params.push_back(make_pair(NS<XES, XEv>::idComponent(), "neighborhood structure"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == ILSLPerturbationLPlus2<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << ILS::family() << "LevelPert:LPlus2";
      return ss.str();
   }

   std::string toString() const override
   {
      return id();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class ILSLPerturbationLPlus2ProbBuilder : public ComponentBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~ILSLPerturbationLPlus2ProbBuilder()
   {
   }

   virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      sptr<GeneralEvaluator<XES, XEv>> eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      sptr<NS<XES, XEv>> ns;
      hf.assign(ns, *scanner.nextInt(), scanner.next()); // reads backwards!

      return new ILSLPerturbationLPlus2Prob<XES, XEv>(eval, ns, hf.getRandGen());
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
      params.push_back(make_pair(NS<XES, XEv>::idComponent(), "neighborhood structure"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == ILSLPerturbationLPlus2Prob<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << ILS::family() << "LevelPert:LPlus2Prob";
      return ss.str();
   }

   std::string toString() const override
   {
      return id();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

}

#endif /*OPTFRAME_ILSLPerturbation_HPP_*/
