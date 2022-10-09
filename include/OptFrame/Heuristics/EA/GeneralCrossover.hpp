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

#ifndef OPTFRAME_HEURISTICS_EA_GENERALCROSSOVER_HPP_
#define OPTFRAME_HEURISTICS_EA_GENERALCROSSOVER_HPP_

// C++
#include <string>
#include <utility>
//
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Helper/Solution.hpp>

namespace optframe {

// template <class R, class ADS = OPTFRAME_DEFAULT_ADS>
template <XSolution S>
class GeneralCrossover : public Component {
 public:
  virtual ~GeneralCrossover() {
  }

  virtual pair<std::optional<S>, std::optional<S>>
  cross(const S&, const S&) = 0;

 public:
  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":GeneralCrossover";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }

  std::string toString() const override {
    return id();
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_GENERALCROSSOVER_HPP_
