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

#ifndef OPTFRAME_HEURISTICS_EA_RK_INITIALEPOPULATIONRK_HPP_
#define OPTFRAME_HEURISTICS_EA_RK_INITIALEPOPULATIONRK_HPP_

#include <OptFrame/Helper/VEPopulation.hpp>
#include <OptFrame/InitialPopulation.hpp>

#include "ConstructiveRK.hpp"

namespace optframe {

template <XESolution XES, class KeyType = double, X2ESolution<XES> X2ES = VEPopulation<XES>>
class InitialEPopulationRK : public InitialEPopulation<XES, X2ES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  virtual ~InitialEPopulationRK() {
  }

  // By default, this generator will evaluate ESolution pairs here,
  // but user can opt-out of this, leaving evaluations for the future
  // See BRKGA on a implementation over this concept.
  bool canEvaluate() const override {
    return true;
  }

  X2ES generateEPopulation(unsigned populationSize, double timelimit) override = 0;

  static string idComponent() {
    stringstream ss;
    ss << InitialEPopulation<XES>::idComponent() << ":" << EA::family() << ":" << RK::family() << "InitialEPopulationRK";
    return ss.str();
  }

  std::string toString() const override {
    return id();
  }

  std::string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_RK_INITIALEPOPULATIONRK_HPP_
