// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_DECODER_RANDOM_KEYS_NOEVALUATION_HPP_
#define OPTFRAME_DECODER_RANDOM_KEYS_NOEVALUATION_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <string>

#include "../../../Component.hpp"

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

MOD_EXPORT template <XSolution S, ConceptsComparability KeyType>
class DecoderRandomKeysNoEvaluation : public Component {
  using RSK = std::vector<KeyType>;

 public:
  virtual ~DecoderRandomKeysNoEvaluation() {}

  virtual S decodeSolution(const RSK& rk) = 0;

 public:
  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":" << EA::family() << ":" << RK::family()
       << "DecoderRandomKeysNoEvaluation";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
};

}  // namespace optframe

#endif /*OPTFRAME_DECODER_RANDOM_KEYS_NOEVALUATION_HPP_*/
