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

#ifndef OPTFRAME_HEURISTICS_MULTIOBJECTIVE_MOELITISM_HPP_
#define OPTFRAME_HEURISTICS_MULTIOBJECTIVE_MOELITISM_HPP_

// C++
#include <vector>
//
#include "../../Component.hpp"
#include "MOSIndividual.hpp"

namespace optframe {

//template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
template <XESolution XMES2>
class MOElitism : Component {
 public:
  MOElitism() {
  }

  virtual ~MOElitism() {
  }

  // keep archive updated
  virtual void updateArchive(
      const vector<const MOSIndividual<XMES2>>& P,
      vector<MOSIndividual<XMES2>>& archive) = 0;

  virtual void print() const {
    cout << "MOElitism" << endl;
  }
};

// template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
template <XESolution XMES2>
class NoArchiving : public MOElitism<XMES2> {
 public:
  NoArchiving() {
  }

  virtual ~NoArchiving() {
  }

  void updateArchive(
      const vector<const MOSIndividual<XMES2>>& P,
      vector<MOSIndividual<XMES2>>& archive) {
    // DO ANYTHING! NSGA-II STYLE
  }

  virtual void print() const {
    cout << "NoArchiving" << endl;
  }
};

//template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
template <XESolution XMES2>
class BoundedArchiving : public MOElitism<XMES2> {
  using XMEv = typename XMES2::second_type;

 protected:
  unsigned limit;
  // STRANGE reference here...
  XMEv& mDir;

 public:
  BoundedArchiving(unsigned _limit, XMEv& _mDir)
      : limit(_limit), mDir(_mDir) {
  }

  virtual ~BoundedArchiving() {
  }

  virtual void updateArchive(
      const vector<const MOSIndividual<XMES2>>& P,
      vector<MOSIndividual<XMES2>>& archive) {
    // USE DOMINANCE TO KEEP ARCHIVING SUBJECT TO LIMIT
  }

  virtual void print() const {
    cout << "BoundedArchiving(" << limit << ")" << endl;
  }
};

//template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
template <XESolution XMES2>
class UnboundedArchiving : public MOElitism<XMES2> {
  using XMEv = typename XMES2::second_type;

 protected:
  // STRANGE reference here...
  XMEv& mDir;

 public:
  explicit UnboundedArchiving(XMEv& _mDir)
      : mDir(_mDir) {
  }

  virtual ~UnboundedArchiving() {
  }

  virtual void updateArchive(
      const vector<const MOSIndividual<XMES2>>& P,
      vector<MOSIndividual<XMES2>>& archive) {
    // USE DOMINANCE TO KEEP ARCHIVING
  }

  virtual void print() const {
    cout << "UnboundedArchiving" << endl;
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_MULTIOBJECTIVE_MOELITISM_HPP_
