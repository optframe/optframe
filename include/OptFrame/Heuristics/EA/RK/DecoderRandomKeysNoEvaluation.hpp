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

#ifndef OPTFRAME_DECODER_RANDOM_KEYS_NOEVALUATION_HPP_
#define OPTFRAME_DECODER_RANDOM_KEYS_NOEVALUATION_HPP_

#include "../../../Component.hpp"

namespace optframe {

template <XSolution S, optframe::comparability KeyType>
class DecoderRandomKeysNoEvaluation : public Component {
  using RSK = std::vector<KeyType>;

 public:
  virtual ~DecoderRandomKeysNoEvaluation() {
  }

  virtual S decodeSolution(const RSK& rk) = 0;

 public:
  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":" << EA::family() << ":" << RK::family() << "DecoderRandomKeysNoEvaluation";
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

#endif /*OPTFRAME_DECODER_RANDOM_KEYS_NOEVALUATION_HPP_*/
