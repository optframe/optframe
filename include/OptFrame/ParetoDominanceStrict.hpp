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

/*
 * ParetoDominanceStrict.hpp
 *
 * LGPLv3
 *
 */

#ifndef OPTFRAME_PARETODOMINANCE_STRICT_HPP_
#define OPTFRAME_PARETODOMINANCE_STRICT_HPP_

#include <iostream>

#include "Direction.hpp"
#include "Evaluation.hpp"
#include "Evaluator.hpp"
#include "ParetoDominance.hpp"
#include "Solution.hpp"

// using namespace std;

template <XESolution XMES>
class ParetoDominanceStrict : public ParetoDominance<XMES> {
 public:
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  using XEv = typename XMEv::XEv;
  using XES = std::pair<S, XEv>;

  using ParetoDominance<XMES>::dominates;
  using ParetoDominance<XMES>::birelation;

  ParetoDominanceStrict(vector<Evaluator<S, XEv>*> _v_e)
      : ParetoDominance<XMES>(_v_e) {
  }

  ParetoDominanceStrict(vector<Direction<>*> _v_d)
      : ParetoDominance<XMES>(_v_d) {
  }

  ParetoDominanceStrict() {
  }

  virtual ~ParetoDominanceStrict() {
  }

  void insertEvaluators(vector<Evaluator<XES, XEv>*> _v_e) {
    ParetoDominance<XMES>::v_e = _v_e;
  }

  // true if 's1' weakly dominates 's2'
  virtual bool dominates(const vector<double>& v1, const vector<double>& v2) {
    vector<Evaluator<XES, XEv>*>& v_e = ParetoDominance<XMES>::v_e;

    if (!((v_e.size() == v1.size()) && (v1.size() == v2.size()))) {
      cout << "WARNING in ParetoDominanceStrict: different sizes." << endl;
      return false;
    }

    int better = 0;
    //int equals = 0;

    // TODO: make inheritance!
    if (v_e.size() == v1.size()) {
      for (int e = 0; e < v1.size(); e++) {
        if (v_e[e]->betterThan(v1[e], v2[e]))
          better++;
      }

    } else {
      for (int e = 0; e < v1.size(); e++) {
        if (this->v_d[e]->betterThan(v1[e], v2[e]))
          better++;
      }
    }

    return (better == v_e.size());
  }

  virtual pair<bool, bool> birelation(const vector<Evaluation<DS>*>& v1, const vector<Evaluation<DS>*>& v2) {
    return make_pair(dominates(v1, v2), dominates(v2, v1));
  }
};

#endif /*OPTFRAME_PARETODOMINANCE_STRICT_HPP_*/
