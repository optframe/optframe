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

#ifndef OPTFRAME_ADSMANAGER_HPP_
#define OPTFRAME_ADSMANAGER_HPP_

// C++
#include <cstdlib>
#include <iostream>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

// using namespace std;

namespace optframe {

//template<class R, class ADS, XBaseSolution<R,ADS> S = CopySolution<R,ADS>>
// force manual passing (for safety)
//
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <class R, class ADS, XBaseSolution<R, ADS> S>
#else 
template <class R, class ADS, typename S>
#endif
class ADSManager : public Component {
 public:
  ADSManager() {
  }

  virtual ~ADSManager() {
  }

  virtual void initializeADS(const R& rep, ADS& _ads) = 0;

  virtual void initializeADSNeighStructure(const R& rep, ADS& _ads) = 0;

  virtual void setNeighLocalOptimum(const R& rep, ADS& _ads, string str) = 0;

  virtual void setNeighLocalOptimum(S& s, string str) {
    setNeighLocalOptimum(s.getR(), s.getADS(), str);
  }

  virtual bool compareADS(const ADS& _ads1, const ADS& _ads2) = 0;

  virtual void printADS(const ADS& _ads) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << "ADSManager";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }

  std::string toString() const override {
    std::stringstream ss;
    ss << "ADSManager";
    return ss.str();
  }
};

}  // namespace optframe

#endif /* OPTFRAME_ADSMANAGER_HPP_ */
