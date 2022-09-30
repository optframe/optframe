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

#ifndef OPTFRAME_BASIC_INITIAL_EPOPULATION_RK_HPP_
#define OPTFRAME_BASIC_INITIAL_EPOPULATION_RK_HPP_

#include "ConstructiveRK.hpp"
#include "InitialEPopulationRK.hpp"

namespace optframe {

template<XESolution XES, class KeyType = double, X2ESolution<XES> X2ES = VEPopulation<XES>>
class BasicInitialEPopulationRK : public InitialEPopulationRK<XES, KeyType, X2ES>
{
   using S = typename XES::first_type;
   using XEv = typename XES::second_type;
   //
   // TODO: static_assert that S = std::vector<KeyType>

public:
   sref<ConstructiveRK<S>> constructiveRK;

   BasicInitialEPopulationRK(sref<ConstructiveRK<S>> _constructiveRK)
     : constructiveRK{ _constructiveRK }
   {
   }

   virtual ~BasicInitialEPopulationRK()
   {
   }

   // cannot evaluate here (return 'false')
   virtual bool canEvaluate() const override
   {
      return false;
   }

   virtual X2ES generatePopulation(unsigned populationSize, double timelimit) override
   {
      X2ES p;
      for (unsigned i = 0; i < populationSize; i++) {
         S s = constructiveRK->generateSolution(timelimit);
         p.push_back(XES{ s, XEv{} });
      }
      return p;
   }

   static string idComponent()
   {
      stringstream ss;
      ss << InitialEPopulationRK<XES, KeyType, X2ES>::idComponent() << ":BasicInitialEPopulationRK";
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

#endif /*OPTFRAME_BASIC_INITIAL_EPOPULATION_RK_HPP_*/
