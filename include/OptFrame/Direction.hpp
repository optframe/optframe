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

#ifndef OPTFRAME_OPTIMIZATION_DIRECTION_HPP_
#define OPTFRAME_OPTIMIZATION_DIRECTION_HPP_

#include <assert.h>
#include <float.h>

#include <iostream>
#include <limits>

//#include "ADSManager.hpp"
//#include "Action.hpp"
// Base concepts
#include <OptFrame/BaseConcepts.hpp>

#include "Component.hpp"
#include "Evaluation.hpp"
#include "Move.hpp"
#include "MoveCost.hpp"
//#include "Solution.hpp"

// using namespace std;
// using namespace scannerpp;

namespace optframe {

// Direction 'betterThan' depends on a 'totally_ordered' type
////template<optframe::totally_ordered ObjType = evtype, XEvaluation XEv = Evaluation<ObjType>>
template <XEvaluation XEv>  //= Evaluation<evtype>>  // default is 'evtype'
class Direction : public Component {
 public:
  ///MoveCost<evtype, Evaluation<>> nullCost;
  XEv nullCost;

  Direction()
      :                   ///nullCost(MoveCost<>(0))
        nullCost(XEv(0))  // requires cast from value zero, at least
  {
  }

  virtual ~Direction() {
  }

  // ============ betterThan ===========

  //! abstract method betterThan: true when a < b for minimization problems; and true when a > b for maximization problems.
  /*!
	 betterThan is the method in OptFrame used to guide the search methods into the solution space.
	 with betterThan the search methods are able to compare good and poor solutions, in one of the two directions: minimization and maximization.
	 It must be implemented by the final user in one of these ways:
	 - for minimization problems, returns a < b;
	 - for maximization problems, returns a > b.
	 */
  //virtual bool betterThan(evtype a, evtype b) = 0;

  // true if 'mc1' is better than 'mc2'
  virtual inline bool betterThan(const MoveCost<XEv>& mc1, const MoveCost<XEv>& mc2) {
    if (isMinimization())
      return (mc2.cost() - mc1.cost()) >= optframe::get_numeric_zero<typename XEv::objType>();
    else
      return (mc1.cost() - mc2.cost()) >= optframe::get_numeric_zero<typename XEv::objType>();
  }

  // true if 'e1' is better than 'e2'
  virtual inline bool betterThan(const XEv& e1, const XEv& e2) {
    //cout << "BETTER THAN! (isMin=" << isMinimization() << ")" << endl;
    //e1.print();
    //e2.print();

    //bool r = true;

    evtype diff;
    if (isMinimization()) {
      diff = e2.evaluation() - e1.evaluation();
      //r = (e2.evaluation() - e1.evaluation()) >= optframe::get_numeric_zero<evtype>();
    } else {
      diff = e1.evaluation() - e2.evaluation();
      //r = (e1.evaluation() - e2.evaluation()) >= optframe::get_numeric_zero<evtype>();
    }
    bool r = !(optframe::numeric_is_zero<typename XEv::objType>(diff)) && (diff > optframe::get_numeric_zero<typename XEv::objType>());

    //printf("r=%d e1=%.7f e2=%.7f zero=%.7f nzero=%.7f\n",r,e1.evaluation(), e2.evaluation(), optframe::get_numeric_zero<evtype>(), t);

    return r;
  }

  /*
	virtual inline bool betterThan(const vector<pair<evtype, evtype> >& altCosts1, const vector<pair<evtype, evtype> >& altCosts2)
	{
		if(altCosts1.size() != altCosts2.size())
			return false;
		for(unsigned i = 0; i < altCosts1.size(); i++)
			if(!betterThan(altCosts1[i].first + altCosts1[i].second, altCosts2[i].first + altCosts2[i].second))
				return false;
		return true;
	}
	*/

  // ============ betterOrEquals ===========

  /*
	inline bool betterOrEquals(const vector<pair<evtype, evtype> >& altCosts1, const vector<pair<evtype, evtype> >& altCosts2)
	{
		return betterThan(altCosts1, altCosts2) || equals(altCosts1, altCosts2);
	}
	*/

  // abolishing MoveCost
  /*
	inline bool betterOrEquals(const MoveCost<>& mc1, const MoveCost<>& mc2)
	{
		return betterThan(mc1, mc2) || equals(mc1, mc2);
	}
*/

  inline bool betterOrEquals(const XEv& e1, const XEv& e2) {
    return betterThan(e1, e2) || equals(e1, e2);
  }

  /*
	inline bool betterOrEquals(evtype a, evtype b)
	{
		return betterThan(a, b) || equals(a, b);
	}
	*/

  // ============ equals ============

  /*
	virtual inline bool equals(const evtype& t1, const evtype& t2, const vector<pair<evtype, evtype> >& altCosts1, const vector<pair<evtype, evtype> >& altCosts2)
	{
      if(optframe::numeric_is_zero<evtype>(t1 - t2))
		//if(EVALUATION_ABS(t1 - t2) <= optframe::get_numeric_zero<evtype>()) // deprecated
			return true;

		if(t1 != t2)
			return false;

		assert(altCosts1.size() == altCosts2.size());

		for(unsigned i = 0; i < altCosts1.size(); i++)
			if((altCosts1[i].first + altCosts1[i].second) != (altCosts2[i].first + altCosts2[i].second))
				return false;

		return true;
	}
*/

  virtual inline bool equals(const typename XEv::objType& t1, const typename XEv::objType& t2) {
    if (optframe::numeric_is_zero<typename XEv::objType>(t1 - t2))
      //if(EVALUATION_ABS(t1 - t2) <= optframe::get_numeric_zero<evtype>()) // deprecated
      return true;

    if (t1 != t2)
      return false;

    //assert(altCosts1.size() == altCosts2.size());

    //for(unsigned i = 0; i < altCosts1.size(); i++)
    //	if((altCosts1[i].first + altCosts1[i].second) != (altCosts2[i].first + altCosts2[i].second))
    //		return false;

    return true;
  }

  // Abolishing MoveCost
  /*
	virtual inline bool equals(const MoveCost<>& mc1, const MoveCost<>& mc2)
	{
		return equals(mc1.cost(), mc2.cost(), mc1.getAlternativeCosts(), mc2.getAlternativeCosts());
	}
*/
  virtual inline bool equals(const XEv& e1, const XEv& e2) {
    //return equals(e1.evaluation(), e2.evaluation(), e1.getAlternativeCosts(), e2.getAlternativeCosts());
    return equals(e1.evaluation(), e2.evaluation());
  }

  /*
	virtual inline bool equals(evtype a, evtype b)
	{
		return (::abs(a - b) < OPTFRAME_EPSILON);
	}
	*/

  // ============= improvement =============

  ///virtual bool isImprovement(const MoveCost<>& mc, const Evaluation<>& e1, const Evaluation<>& e2)
  // Analyse if '(mc + e1)' is an improvement over 'e2'
  virtual bool isImprovement(const XEv& mc, const XEv& e1, const XEv& e2) {
    //evtype ec1 = mc.cost() + e1.evaluation();
    evtype ec1 = mc.evaluation() + e1.evaluation();

    //if(isMinimization()  && (e2.evaluation() - ec1) >= EVALUATION_ZERO) // deprecated
    if (isMinimization() && (e2.evaluation() - ec1) >= optframe::get_numeric_zero<typename XEv::objType>())
      return true;

    //if(!isMinimization() && (ec1 - e2.evaluation()) >= EVALUATION_ZERO) // deprecated
    if (!isMinimization() && (ec1 - e2.evaluation()) >= optframe::get_numeric_zero<typename XEv::objType>())
      return true;

    return false;
  }

  ///virtual inline bool isImprovement(const MoveCost<>& mc)
  virtual inline bool isImprovement(const XEv& mc) {
    return betterThan(mc, nullCost);
  }

 public:
  // ============= direction ==============

  virtual bool isMinimization() const = 0;

  inline bool isMaximization() const {
    return !isMinimization();
  }

  // ============ estimation =============

  inline evtype ideal() {
    if (isMinimization())
      return min();
    else
      return max();
  }

  //inline evtype worst()
  inline evtype nadir() {
    if (isMinimization())
      return max();
    else
      return min();
  }

  // bad approximation!
  virtual inline typename XEv::objType min() {
    ////return -DBL_MAX;

    if (numeric_limits<evtype>::has_infinity)
      return -numeric_limits<typename XEv::objType>::infinity();
    else
      return -numeric_limits<typename XEv::objType>::max();
  }

  // bad approximation!
  virtual inline typename XEv::objType max() {
    ////return DBL_MAX;

    if (numeric_limits<typename XEv::objType>::has_infinity)
      return numeric_limits<typename XEv::objType>::infinity();
    else
      return numeric_limits<typename XEv::objType>::max();
  }

  // ============= Component ===============

  virtual bool compatible(string s) {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << "Direction";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }

  virtual string toString() const {
    if (isMinimization())
      return "Direction:MIN";
    else
      return "Direction:MAX";
  }

  virtual void print() const {
    cout << toString() << endl;
  }
};

//template<optframe::totally_ordered ObjType = evtype, XEvaluation XEv = Evaluation<ObjType>>
//class Minimization: public Direction<ObjType, XEv>
template <XEvaluation XEv = Evaluation<evtype>>  // default is 'evtype'
class Minimization : public Direction<XEv> {
 public:
  virtual ~Minimization() {
  }

  /*
	inline bool betterThan(evtype f1, evtype f2)
	{
		// MINIMIZATION
		return (f1 < (f2 - OPTFRAME_EPSILON));
	}*/

  inline bool isMinimization() const {
    return true;
  }
};

//template<optframe::totally_ordered ObjType = evtype, XEvaluation XEv = Evaluation<ObjType>>
//class Maximization: public Direction<ObjType, XEv>
template <XEvaluation XEv = Evaluation<evtype>>  // default is 'evtype'
class Maximization : public Direction<XEv> {
 public:
  virtual ~Maximization() {
  }

  /*
	inline bool betterThan(evtype f1, evtype f2)
	{
		// MAXIMIZATION
		return (f1 > (f2 + OPTFRAME_EPSILON));
	}*/

  inline bool isMinimization() const {
    return false;
  }
};

}  // namespace optframe

#endif /*OPTFRAME_OPTIMIZATION_DIRECTION_HPP_*/
