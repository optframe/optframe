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
#include "../../EPopulation.hpp"
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

   // unified constructors (explicit initPop)
   // to pass 'key_size' just use parameter "_initPop = RandomKeysInitPop(key_size)"
   BRKGA(sref<DecoderRandomKeys<S, XEv, KeyType>> _decoder, sref<InitialPopulation<XES2>> _initPop, unsigned _popSize, unsigned numGen, double fracTOP, double fracBOT, double _probElitism, sref<RandGen> _rg = new RandGen)
     : RKGA<XES, KeyType>(_decoder, _initPop, _popSize, numGen, fracTOP, fracBOT, _rg)
     , probElitism(_probElitism)
   {
      assert(probElitism > 0.5);
      assert(probElitism <= 1.0);
   }

   virtual ~BRKGA()
   {
   }

   virtual RSK& cross(const Population<XES2>& pop)
   {
      if (Component::debug)
         (*Component::logdata) << "BRKGA cross(|pop|=" << pop.size() << ")" << std::endl;
      //assert(this->key_size > 0); // In case of using InitPop, maybe must receive a Selection or Crossover object...

      const RSK& p1 = pop.at(this->rg->rand() % this->eliteSize);
      const RSK& p2 = pop.at(this->eliteSize + this->rg->rand() % (pop.size() - this->eliteSize));

      //random_keys* v = new random_keys(this->key_size, 0.0);
      //for (int i = 0; i < this->key_size; i++)
      Population<XES2> p_single = this->initPop->generatePopulation(1, 0.0); // implicit 'key_size'
      // TODO: should cache 'key_size' to prevent unused rands on generation
      random_keys* v = new random_keys(p_single.at(0)); // copy or 'move' ?
      std::fill(v->begin(), v->end(), 0);
      for (unsigned i = 0; i < v->size(); i++)
         if ((this->rg->rand() % 1000000) / 1000000.0 < probElitism)
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
