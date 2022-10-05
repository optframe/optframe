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

#ifndef OPTFRAME_MULTI_OBJ_SEARCH_HPP_
#define OPTFRAME_MULTI_OBJ_SEARCH_HPP_

// C++
#include <cstring>
#include <iostream>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Direction.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/GlobalSearch.hpp>  // Base class
#include <OptFrame/Helper/MultiEvaluation.hpp>
#include <OptFrame/Helper/Population.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/MultiEvaluator.hpp>
#include <OptFrame/Pareto.hpp>
#include <OptFrame/ParetoDominance.hpp>
#include <OptFrame/ParetoDominanceWeak.hpp>

// using namespace std;

namespace optframe {

template <class Self, class XES>
concept
#if __cplusplus <= 201703L  // after c++20, not required 'bool'
    bool
#endif
        XMultiObjSearch = requires(Self a) {
  XESolution<XES>;
  X2ESolution<typename Self::BestType, XES>;
};

/*
// Multi Objective Stopping Criteria
// Must include GENERAL stopping criteria
// specific stopping criteria for metaheuristics can be included in their constructors
class MOSC: public Component
{
public:
	// maximum timelimit (seconds)
	double timelimit;
	//Hypervolume indicator
	//	double hv;
	//Pareto cardinality
	//	double cardinality;

	MOSC(double _timelimit = 100000000.0) :
			timelimit(_timelimit)
	{
	}

	virtual ~MOSC()
	{
	}

	virtual string id() const override
	{
		return "MOSC";
	}
};
*/

// This MultiObjSearch perspective inherits from Multi Solution Search,
// considering a X2ES space with Pareto structure

//template<XSolution S, XEvaluation XEv, X2ESolution<S, XEv> X2ES>
//
//template<XSolution S, XEvaluation XMEv = Evaluation<>, XESolution XMES = pair<S, XMEv>>
//
template <XESolution XMES, XESolution XMES2 = XMES, XSearch<XMES2> XMSH2 = XMES2>
class MultiObjSearch : public GlobalSearch<XMES, Pareto<XMES>>  // public Component
{
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  using XMSH = Pareto<XMES>;  // search space
 public:
  //
  //bool (*onParetoIncumbent)(GlobalSearch<XMES, Pareto<XMES>>& self, const XMSH2& incumbent) =
  //  [](GlobalSearch<XMES, Pareto<XMES>>& self, const XMSH2& incumbent) { return true; };

  // ========================================
  // THIS CLASS IS USELESS! WHAT'S THE POINT?
  // Best to just have XMultiObjSearch
  // ========================================

  MultiObjSearch() {
  }

  virtual ~MultiObjSearch() {
  }

  op<Pareto<XMES>>& getBestPareto() {
    return this->best;
  }

  //virtual Pareto<XMES>* search(MOSC& stopCriteria, Pareto<XMES>* _pf = nullptr) = 0;
  //
  //virtual SearchStatus search(std::optional<Pareto<XMES>>& p, const StopCriteria<XMEv>& stopCriteria) = 0;
  virtual SearchOutput<XMES, Pareto<XMES>> search(const StopCriteria<XMEv>& stopCriteria) override = 0;

  virtual string log() const {
    return "Empty heuristic log.";
  }

  virtual bool compatible(string s) {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << "MultiObjSearch:";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class MultiObjSearchBuilder : public ComponentBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~MultiObjSearchBuilder() {
  }

  virtual MultiObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") = 0;

  virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") {
    return build(scanner, hf, family);
  }

  virtual vector<pair<string, string>> parameters() = 0;

  virtual bool canBuild(string) = 0;

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "MultiObjSearch:";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif /* OPTFRAME_MULTI_OBJ_SEARCH_HPP_ */
