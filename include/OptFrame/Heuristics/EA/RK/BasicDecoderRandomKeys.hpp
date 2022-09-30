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
// GNU Lesser General Public License v3 for more details.DecoderRandomKeys.hpp

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_BASIC_DECODER_RANDOM_KEYS_HPP_
#define OPTFRAME_BASIC_DECODER_RANDOM_KEYS_HPP_

#include "../../../Evaluator.hpp"
#include "../../../VEPopulation.hpp"
#include "DecoderRandomKeysNoEvaluation.hpp"

namespace optframe {

// this is an adapter based on Evaluator and DecoderRandomKeysNoEvaluation

template<XESolution XES, optframe::comparability KeyType>
class BasicDecoderRandomKeys : public DecoderRandomKeys<XES, KeyType>
{
   using S = typename XES::first_type;
   using XEv = typename XES::second_type;
   using RSK = std::vector<KeyType>;
   using XES2 = std::pair<RSK, XEv>;
   using X2ES = VEPopulation<XES2>;

public:
   sref<Evaluator<S, XEv, XES>> evaluator;
   sref<DecoderRandomKeysNoEvaluation<S, KeyType>> decoderSol;

   BasicDecoderRandomKeys(sref<Evaluator<S, XEv, XES>> _evaluator,
                          sref<DecoderRandomKeysNoEvaluation<S, KeyType>> _decoderSol)
     : evaluator{ _evaluator }
     , decoderSol{ _decoderSol }
   {
   }

   virtual ~BasicDecoderRandomKeys()
   {
   }

   virtual pair<XEv, op<S>> decode(const RSK& rk, bool needsSolution) override
   {
      if (!needsSolution)
         if (Component::warning)
            std::cout << "WARNING: cannot disable solution decoding!" << std::endl;
      S s = decoderSol->decodeSolution(rk);
      XEv e = evaluator->evaluate(s);
      return pair<XEv, op<S>>{ e, op<S>{ s } };
   }

   virtual bool isMinimization() const
   {
      return evaluator->isMinimization();
   }

public:
   static std::string idComponent()
   {
      std::stringstream ss;
      ss << DecoderRandomKeys<XES, KeyType>::idComponent() << ":BasicDecoderRandomKeys";
      return ss.str();
   }

   virtual std::string id() const
   {
      return idComponent();
   }

   std::string toString() const override
   {
      return id();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class BasicDecoderRandomKeysBuilder : public ComponentBuilder<S, XEv, XES, X2ES>
{
   using KeyType = double;
   using RealS = std::vector<KeyType>;
   using RealXEv = Evaluation<>;
   using RealXES = std::pair<RealS, RealXEv>;

public:
   virtual ~BasicDecoderRandomKeysBuilder()
   {
   }

   virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      sptr<Evaluator<S, XEv, XES>> evaluator;
      std::string sid_0 = scanner.next();
      int id_0 = *scanner.nextInt();
      hf.assign(evaluator, id_0, sid_0);

      sptr<DecoderRandomKeysNoEvaluation<S, KeyType>> decoderSol;
      std::string sid_1 = scanner.next();
      int id_1 = *scanner.nextInt();
      hf.assign(decoderSol, id_1, sid_1);

      return new BasicDecoderRandomKeys<XES, double>(evaluator, decoderSol);
   }

   virtual vector<pair<string, string>> parameters() override
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(Evaluator<S, XEv, XES>::idComponent(), "evaluator"));
      params.push_back(make_pair(DecoderRandomKeysNoEvaluation<S, KeyType>::idComponent(), "decoder_no_eval"));

      return params;
   }

   virtual bool canBuild(string component) override
   {
      return component == BasicDecoderRandomKeys<RealXES, double>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << EA::family() << ":" << RK::family() << "BasicDecoderRandomKeysBuilder";
      return ss.str();
   }

   std::string toString() const override
   {
      return id();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /*OPTFRAME_BASIC_DECODER_RANDOM_KEYS_HPP_*/
