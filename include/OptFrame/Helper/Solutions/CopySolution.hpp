// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HELPER_SOLUTIONS_COPYSOLUTION_HPP_
#define OPTFRAME_HELPER_SOLUTIONS_COPYSOLUTION_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <assert.h>
//
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
//

// basic elements of an OptFrame Component
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
// #include "../Util/printable.h"

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

// will require 'R', thus should require basic printability here

// copy-based solution (NO POINTER IS USED HERE)
// requires copy-constructor and move-constructor (for R and ADS)
// ADS requires empty constructor (but not R)

namespace optframe {

// template<XRepresentation R, class ADS = nullptr_t>
template <XRepresentation R, class ADS = _ADS>
class CopySolution : public Component {
 public:
  // allow direct access to internals (no issue)
  R r;      // representation
  ADS ads;  // auxiliary data structure

  using typeR = R;
  using typeADS = ADS;

  // copy constructor for R (ADS requires empty constructor)
  explicit CopySolution(const R& _r) : r(_r), ads(ADS()) {}

  // copy constructor for R and ADS
  CopySolution(const R& _r, const ADS& _ads) : r(_r), ads(_ads) {}

  // move constructor for R and ADS
  CopySolution(R&& _r, ADS&& _ads) : r(_r), ads(_ads) {}

  // copy constructor
  CopySolution(const CopySolution<R, ADS>& s) : CopySolution(s.r, s.ads) {
#ifdef OPTFRAME_AC
    // copy listAC directly
    this->listAC = s.listAC;
#endif
  }

  // move constructor
  CopySolution(CopySolution<R, ADS>&& s) noexcept : CopySolution(s.r, s.ads) {
#ifdef OPTFRAME_AC
    // copy listAC directly
    this->listAC = s.listAC;
#endif
  }

  CopySolution<R, ADS>& operator=(const CopySolution<R, ADS>& s) {
    if (&s == this)  // auto ref check
      return *this;

    r = s.r;
    ads = s.ads;

#ifdef OPTFRAME_AC
    // copy listAC directly
    this->listAC = s.listAC;
#endif

    return *this;
  }

  CopySolution<R, ADS>& operator=(CopySolution<R, ADS>&& s) noexcept {
    r = std::move(s.r);
    ads = std::move(s.ads);

#ifdef OPTFRAME_AC
    // copy listAC directly
    this->listAC = s.listAC;
#endif

    return *this;
  }

  // -----------------------------------
  // getters and setters for R and ADS
  // according to XBaseSolution concept
  // -----------------------------------

  R& getR() { return r; }

  const R& getR() const { return r; }

  ADS& getADS() { return ads; }

  const ADS& getADS() const { return ads; }

  ADS* getADSptr() { return &ads; }

  const ADS* getADSptr() const { return &ads; }

  // ==================================================

  // destructor for Solution (must free R and ADS objects)
  virtual ~CopySolution() {}

  // ==================
  // end canonical part
  // ==================

  // shouldn't clone here!

#ifdef OPTFRAME_AC
  std::shared_ptr<Component> sharedClone() const override {
    std::shared_ptr<Component> sptr{&clone()};
    sptr->listAC = this->listAC;
    return sptr;
  }
#endif

  // TODO: remove!!!
  CopySolution<R, ADS>& clone() const {
    return *new CopySolution<R, ADS>(*this);
  }

  // =================
  // begin Object part
  // =================

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":CopySolution";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override {
    std::stringstream ss;
    ss << "CopySolution: " << r;
    // ss << "ADS: " << ads;DecoderRandomKeys
    return ss.str();
  }

  bool toStream(std::ostream& os) const override {
    /*
      if (&os == &optframe::cjson) {
         assert(false);
      }
      assert(false);
      return false;
      */
    // forward to operator<<
    os << (*this);
    return true;
  }

  friend std::ostream& operator<<(std::ostream& os,
                                  const CopySolution<R, ADS>& s) {
    // os << s.toString();
    if (&os == &optframe::cjson) {
      os << s.r;  // invoke json print from 'r'
    } else {
      os << s.toString();
    }
    return os;
  }
};

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS

template <XRepresentation R, class ADS = _ADS,
          XBaseSolution<R, ADS> S = CopySolution<R, ADS>>
struct _Testing {
  S s;
};

#endif

}  // namespace optframe

#endif  // OPTFRAME_HELPER_SOLUTIONS_COPYSOLUTION_HPP_
