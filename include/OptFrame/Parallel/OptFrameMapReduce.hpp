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

#ifndef OptFrameMapReduce_HPP_
#define OptFrameMapReduce_HPP_

#include <OptFrame/Core/Evaluator.hpp>

#include "../HTrajectory.hpp"
#include "../NSEnum.hpp"
#include "../Util/TestSolution.hpp"
#include "MapReduce++/MaPI/MaPI.h"

// OptFrame MapReduce Types

// typedef R KeyA;				// initial solution // TODO
// memory for reevaluation typedef (int,(double,double)) A;		//
// search subspace index (timelimit,target_f) typedef int KeyB;
// // dummy unique key (integer -1) typedef pair<R, double> B;
// // best move in each subspace and its cost // TODO //typedef pair<Move<R,
// ADS, DS>*, double> B;	// best move in each subspace and its cost
// typedef R C;					// best solution among all
// subspaces

typedef pair<int, pair<double, double>> RankAndStop;  // A -> int double double

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_EMEMORY>
class MyMaPISerializer
    : public MaPI_Serializer<R, RankAndStop, int, pair<R, double>, R> {
 public:
  virtual R KeyA_fromString(string s) = 0;
  virtual string KeyA_toString(R o) = 0;

  virtual RankAndStop A_fromString(string s) = 0;
  virtual string A_toString(RankAndStop o) = 0;

  virtual int KeyB_fromString(string s) = 0;
  virtual string KeyB_toString(int o) = 0;

  virtual pair<R, double> B_fromString(string s) = 0;
  virtual string B_toString(pair<R, double> o) = 0;

  virtual R C_fromString(string s) = 0;
  virtual string C_toString(R o) = 0;
};

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_EMEMORY>
class MyMaPIMapper
    : public MaPI_Mapper<R, RankAndStop, int, pair<R, double>, R> {
 private:
  Evaluator<R, ADS, DS>& eval;
  HTrajectory<R, ADS, DS>* hmap;

 public:
  MyMaPIMapper(MaPI_MapReduce<R, RankAndStop, int, pair<R, double>, R>* mr,
               MaPI_Serializer<R, RankAndStop, int, pair<R, double>, R>* s,
               Evaluator<R, ADS, DS>& _eval)
      : MaPI_Mapper<R, RankAndStop, int, pair<R, double>, R>(mr, s),
        eval(_eval) {}

  void setHeuristic(HTrajectory<R, ADS, DS>* h) { hmap = h; }

  virtual vector<pair<int, pair<R, double>>> map(
      pair<R, RankAndStop> a)  // TODO
  {
    cout << "[MyMaPIMapper::map] begin " << a.second.first << endl;  // exit(1);
    vector<pair<int, pair<R, double>>> m;

    Solution<R, ADS> s(a.first);

    hmap->searchFrom(s, a.second.second.first, a.second.second.second);

    Evaluation<DS> e = eval.evaluate(s);

    R mapped = R(s.getR());

    m.push_back(make_pair(-1, make_pair(mapped, e.evaluation())));
    cout << "[MyMaPIMapper::map] end " << a.second << " " << m << endl;

    return m;
  }
};

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_EMEMORY>
class MyMaPIReducer
    : public MaPI_Reducer<R, RankAndStop, int, pair<R, double>, R> {
 private:
  Evaluator<R, ADS, DS>& eval;
  HTrajectory<R, ADS, DS>* hreduce;

 public:
  MyMaPIReducer(MaPI_MapReduce<R, RankAndStop, int, pair<R, double>, R>* mr,
                MaPI_Serializer<R, RankAndStop, int, pair<R, double>, R>* s,
                Evaluator<R, ADS, DS>& _eval)
      : MaPI_Reducer<R, RankAndStop, int, pair<R, double>, R>(mr, s),
        eval(_eval),
        hreduce(nullptr) {}

  void setHeuristic(HTrajectory<R, ADS, DS>* h) { hreduce = h; }

  virtual pair<int, R> reduce(pair<int, vector<pair<R, double>>> bs) {
    // cout << "[MyMaPIReducer::reduce] begin " << bs.first << endl;

    //

    R reduced;

    if (hreduce == nullptr) {  // reduction best solution

      int bestIndex = 0;
      double bestCost = bs.second[bestIndex].second;

      for (unsigned i = 0; i < bs.second.size(); i++) {
        if (eval.betterThan(bs.second[i].second, bestCost)) {
          bestIndex = i;
          bestCost = bs.second[i].second;
        }
      }

      reduced = bs.second[bestIndex].first; /**/
    } else {
      // reduction based on a heuristic method

      if (bs.second.size() == 0) return pair<int, R>(bs.first, R());

      Population<XES> pop;
      for (unsigned i = 0; i < bs.second.size(); i++)
        pop.push_back(*new TestSolution<R, ADS>(bs.second[i].first));

      // Population<XES> & popReduced =
      hreduce->searchFrom(pop, 60, 0);  // TODO considerar tempo

      if (pop.size() == 0) return pair<int, R>(bs.first, R());
      if (pop.size() == 1) return pair<int, R>(bs.first, pop.at(0).getR());

      Evaluation<DS>& ebest = eval.evaluate(pop.at(0));
      unsigned bestIndex = 0;

      for (unsigned i = 0; i < pop.size(); i++)  // TODO
      {
        Evaluation<DS>& e = eval.evaluate(pop.at(i).getR());

        if (eval.betterThan(e, ebest)) {
          bestIndex = i;
          ebest = e;
          delete &e;
        }
      }

      reduced = pop.at(bestIndex).getR();

      // for (unsigned i = 0 ; i < popReduced.size() ; i++) delete
      // &popReduced.at(i);
    }

    // cout << "[MyMaPIReducer::reduce] end " << bs.first << endl;

    return pair<int, R>(bs.first, reduced);
  }
};

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_EMEMORY>
class OptFrameMapReduce : public HTrajectory<R, ADS, DS> {
 private:
  Evaluator<R, ADS, DS>& evaluator;

  MyMaPISerializer<R, ADS, DS>& serializer;
  MaPI_MapReduce<R, RankAndStop, int, pair<R, double>, R>& mapReduce;
  MyMaPIMapper<R, ADS, DS>& mapper;
  MyMaPIReducer<R, ADS, DS>& reducer;

 public:
  using HTrajectory<R, ADS, DS>::exec;  // prevents name hiding

  OptFrameMapReduce(
      MyMaPISerializer<R, ADS, DS>& _serializer,
      MaPI_MapReduce<R, RankAndStop, int, pair<R, double>, R>& _mapReduce,
      MyMaPIMapper<R, ADS, DS>& _mapper, MyMaPIReducer<R, ADS, DS>& _reducer,
      Evaluator<R, ADS, DS>& _eval)
      : serializer(_serializer),
        mapReduce(_mapReduce),
        mapper(_mapper),
        reducer(_reducer),
        evaluator(_eval) {}

  virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f) {
    Evaluation<DS>& e = evaluator.evaluate(s.getR());
    exec(s, e, timelimit, target_f);
    delete &e;
  }

  virtual void exec(Solution<R, ADS>& s, Evaluation<DS>& e, double timelimit,
                    double target_f) {
    vector<pair<R, RankAndStop>> input;
    for (int i = 0; i < mapReduce.getMPISize() - 1; i++)
      input.push_back(
          make_pair(s.getR(), make_pair(i, make_pair(timelimit, target_f))));

    vector<pair<int, R>> output = mapReduce.run(mapper, reducer, input);

    Solution<R, ADS>& s1 = s.clone();
    if (output.size() > 0) s1.setR(output[0].second);

    Evaluation<DS>& e1 = evaluator.evaluate(s1);

    if (evaluator.betterThan(e1, e)) {
      s = s1;
      e = e1;
      e.print();
    }

    delete &s1;
    delete &e1;
  }
};

#endif /*OptFrameMapReduce_HPP_*/
