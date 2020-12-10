// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OPTFRAME_BRKGA_HPP_
#define OPTFRAME_BRKGA_HPP_

#include <algorithm>

#include "../../InitialPopulation.h"
#include "../../Population.hpp"
#include "../../SingleObjSearch.hpp"

#include "RKGA.hpp"

// BRKGA - Biased-Random Key Genetic Algorithms

namespace optframe {

// It seems that R is connected to S here... S<R> is rule perhaps?
// maybe better to not provide XRS directly here (used to be = = RSolution<R>)
// RKeys is the representation of random keys
//
//template<XRepresentation R, XRSolution<R> XRS, XEvaluation XEv, XRepresentation RKeys = optframe::random_keys>
//
// Assuming XSolution allows "raw representation" such as vector<int>...
// Will require XSolution instead of XRepresentation, since legacy Evaluation already deals with that
// User may also want to "decode" the ADS from keys, if necessary...
//
template<
  XESolution XES,
  optframe::comparability KeyType = double,
  XESolution XES2 = std::pair<std::vector<KeyType>, typename XES::second_type>,
  XSearch<XES2> XSH2 = Population<XES2>>
class BRKGA : public RKGA<XES, KeyType, XES2, XSH2>
{
   using S = typename XES::first_type;
   using XEv = typename XES::second_type;
   //using RSK = RSolution<RKeys>;
   using RSK = typename XES2::first_type;

protected:
   double probElitism;

public:
   BRKGA(DecoderRandomKeys<S, XEv, KeyType>& _decoder, InitialPopulation<XES2>& _initPop, int _key_size, unsigned numGen, unsigned _popSize, double fracTOP, double fracBOT, double _probElitism, RandGen& _rg)
     : RKGA<XES, KeyType>(_decoder, _initPop, _key_size, numGen, _popSize, fracTOP, fracBOT, _rg)
     , probElitism(_probElitism)
   {
      assert(probElitism > 0.5);
      assert(probElitism <= 1.0);
   }

   BRKGA(DecoderRandomKeys<S, XEv, KeyType>& _decoder, int key_size, unsigned numGen, unsigned popSize, double fracTOP, double fracBOT, double _probElitism, RandGen& _rg)
     : RKGA<XES, KeyType>(_decoder, key_size, numGen, popSize, fracTOP, fracBOT, _rg)
     , probElitism(_probElitism)
   {
      assert(probElitism > 0.5);
      assert(probElitism <= 1.0);
   }

   BRKGA(Evaluator<S, XEv>& _evaluator, InitialPopulation<XES2>& _initPop, int _key_size, unsigned numGen, unsigned _popSize, double fracTOP, double fracBOT, double _probElitism, RandGen& _rg)
     : RKGA<XES, KeyType>(_evaluator, _initPop, _key_size, numGen, _popSize, fracTOP, fracBOT, _rg)
     , probElitism(_probElitism)
   {
      assert(probElitism > 0.5);
      assert(probElitism <= 1.0);
   }

   BRKGA(Evaluator<S, XEv>& _evaluator, int key_size, unsigned numGen, unsigned _popSize, double fracTOP, double fracBOT, double _probElitism, RandGen& _rg)
     : RKGA<XES, KeyType>(_evaluator, key_size, numGen, _popSize, fracTOP, fracBOT, _rg)
     , probElitism(_probElitism)
   {
      assert(probElitism > 0.5);
      assert(probElitism <= 1.0);
   }

   virtual ~BRKGA()
   {
   }

   virtual RSK& cross(const Population<XES2>& pop) const
   {
      if (Component::debug)
         (*Component::logdata) << "BRKGA cross(|pop|=" << pop.size() << ")" << std::endl;
      assert(this->key_size > 0); // In case of using InitPop, maybe must receive a Selection or Crossover object...

      const RSK& p1 = pop.at(this->rg.rand() % this->eliteSize);
      const RSK& p2 = pop.at(this->eliteSize + this->rg.rand() % (pop.size() - this->eliteSize));

      random_keys* v = new random_keys(this->key_size, 0.0);
      for (int i = 0; i < this->key_size; i++)
         if ((this->rg.rand() % 1000000) / 1000000.0 < probElitism)
            //v->at(i) = p1.getR()[i];
            v->at(i) = p1[i];
         else
            //v->at(i) = p2.getR()[i];
            v->at(i) = p2[i];
      //return *new RSK(v);
      return *v; // TODO: pass by std::move() or unique_ptr
   }

   virtual bool setVerboseR() override
   {
      this->setVerbose();
      return RKGA<XES, KeyType>::setVerboseR();
   }
}; // class BRKGA

} // namespace optframe

#endif /*OPTFRAME_BRKGA_HPP_*/
