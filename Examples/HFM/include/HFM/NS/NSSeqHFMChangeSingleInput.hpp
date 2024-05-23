#ifndef HFM_NSSEQCHANGESINGLEINPUT_HPP_
#define HFM_NSSEQCHANGESINGLEINPUT_HPP_

// C++
#include <string>
#include <utility>
#include <vector>

// Framework includes
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/RandGen.hpp>

// Own includes
#include "../ProblemInstance.hpp"
#include "../Solution.h"

using namespace std;

namespace HFM {

template <XESolution hfmXES = ESolutionHFM, XEvaluation hfmXEv = EvaluationHFM>
class MoveHFMChangeSingleInput : public Move<hfmXES> {
 private:
  int rule;
  bool sign;
  int maxLag, maxUpperLag;
  int X;

 public:
  MoveHFMChangeSingleInput(int _rule, bool _sign, int _maxLag, int _maxUpperLag,
                           int _X)
      : rule(_rule),
        sign(_sign),
        maxLag(_maxLag),
        maxUpperLag(_maxUpperLag),
        X(_X) {}

  virtual ~MoveHFMChangeSingleInput() {}

  bool canBeApplied(const hfmXES& se) override {
    const RepHFM& rep = se.first.getR();
    bool minimumLag = false;
    bool maxLagCheck = false;
    bool notNull1 = false;
    bool notNull2 = false;
    if ((rule >= 0) && (rule < (int)rep.singleIndex.size())) {
      maxLagCheck = ((rep.singleIndex[rule].second + X) <= maxLag);
      minimumLag = ((rep.singleIndex[rule].second - X) >= maxUpperLag);
      if (rep.singleIndex[rule].first == 0) {
        notNull1 = ((rep.singleIndex[rule].second - X) != 0);
        notNull2 = ((rep.singleIndex[rule].second + X) != 0);
      } else {
        notNull1 = notNull2 =
            true;  // TODO - BIG PROBLEM, REMOVE THIS! Otherwise endogenous will
                   // always get sample
      }
    }

    return minimumLag && maxLagCheck && notNull1 && notNull2;
  }

  uptr<Move<hfmXES>> apply(hfmXES& se) override {
    RepHFM& rep = se.first.getR();
    if (!sign)
      rep.singleIndex[rule].second += X;
    else
      rep.singleIndex[rule].second -= X;

    // if (rep.singleIndex[rule].second > rep.earliestInput)
    //   rep.earliestInput = rep.singleIndex[rule].second;

    return uptr<Move<hfmXES>>(
        new MoveHFMChangeSingleInput(rule, !sign, maxLag, maxUpperLag, X));
  }

  virtual bool operator==(const Move<hfmXES>& _m) const {
    const MoveHFMChangeSingleInput& m = (const MoveHFMChangeSingleInput&)_m;
    return ((m.rule == rule) && (m.sign == sign) && (m.maxLag == maxLag) &&
            (m.maxUpperLag == maxUpperLag) && (m.X == X));
  }

  void print() const override {
    cout << "MoveNEIGHChangeSingleInput( vector:  rule " << rule
         << " <=>  sign " << sign << "\t X:" << X << "\t maxLag " << maxLag
         << "\t maxUpperLag " << maxUpperLag << " )";
    cout << endl;
  }
};

template <XESolution hfmXES = ESolutionHFM, XEvaluation hfmXEv = EvaluationHFM>
class NSIteratorHFMChangeSingleInput : public NSIterator<hfmXES> {
 private:
  // MoveHFMChangeSingleInput* m;
  uptr<Move<hfmXES>> m;
  // vector<uptr<MoveHFMChangeSingleInput>> moves;
  vector<uptr<Move<hfmXES>>> moves;  // general naming
  int index;
  const RepHFM& rep;
  vector<int> vMaxLag, vMaxUpperLag;

 public:
  NSIteratorHFMChangeSingleInput(const RepHFM& _rep, vector<int> _vMaxLag,
                                 vector<int> _vMaxUpperLag)
      : rep(_rep),
        vMaxLag(_vMaxLag),
        vMaxUpperLag(_vMaxUpperLag),
        index{0},
        m{nullptr} {}

  virtual ~NSIteratorHFMChangeSingleInput() {
    // for (int i = index + 1; i < (int) moves.size(); i++)
    //   delete moves[i];
  }

  void first() override {
    int X = 1;
    for (int rule = 0; rule < (int)rep.singleIndex.size(); rule++) {
      bool sign = false;
      int expVariable = rep.singleIndex[rule].first;
      int maxLag = vMaxLag[expVariable];
      int maxUpperLag = vMaxUpperLag[expVariable];
      moves.push_back(
          uptr<Move<hfmXES>>(new MoveHFMChangeSingleInput<hfmXES, hfmXEv>(
              rule, sign, maxLag, maxUpperLag, X)));
      moves.push_back(
          uptr<Move<hfmXES>>(new MoveHFMChangeSingleInput<hfmXES, hfmXEv>(
              rule, !sign, maxLag, maxUpperLag, X)));
    }

    if (moves.size() > 0)
      m = std::move(
          moves[index]);  // stealing from vector... verify if this is correct!
                          // otherwise, must need clone() on Move
    else
      m = nullptr;
  }

  void next() override {
    index++;
    if (index < (int)moves.size())
      m = std::move(
          moves[index]);  // stealing from vector... verify if this is correct!
                          // otherwise, must need clone() on Move
    else
      m = nullptr;
  }

  bool isDone() override { return m == nullptr; }

  uptr<Move<hfmXES>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSIteratorHFMChangeSingleInput. Aborting." << endl;
      exit(1);
    }

    uptr<Move<hfmXES>> m2 = std::move(m);
    m = nullptr;

    return m2;
  }
};

template <XESolution hfmXES = ESolutionHFM, XEvaluation hfmXEv = EvaluationHFM>
// class NSSeqHFMChangeSingleInput: public NSSeq<ESolutionHFM>
class NSSeqHFMChangeSingleInput : public NSSeq<hfmXES> {
 private:
  HFMProblemInstance& pEFP;
  RandGen& rg;
  vector<int> vMaxLag, vMaxUpperLag;

 public:
  NSSeqHFMChangeSingleInput(HFMProblemInstance& _pEFP, RandGen& _rg,
                            vector<int> _vMaxLag, vector<int> _vMaxUpperLag)
      : pEFP(_pEFP), rg(_rg), vMaxLag(_vMaxLag), vMaxUpperLag(_vMaxUpperLag) {}

  virtual ~NSSeqHFMChangeSingleInput() {}

  uptr<Move<hfmXES>> randomMove(const hfmXES& se) override {
    const RepHFM& rep = se.first.getR();
    int MAXCHANGE = 5;
    int X = rg.rand(MAXCHANGE) + 1;
    int rule = -1;
    if (rep.singleIndex.size() > 0)
      rule = rg.rand(rep.singleIndex.size());
    else
      return uptr<Move<hfmXES>>(
          new MoveHFMChangeSingleInput<hfmXES, hfmXEv>(-1, -1, -1, -1, -1));

    int sign = rg.rand(2);

    int expVariable = rep.singleIndex[rule].first;
    int maxLag = vMaxLag[expVariable];
    int maxUpperLag = vMaxUpperLag[expVariable];

    return uptr<Move<hfmXES>>(new MoveHFMChangeSingleInput<hfmXES, hfmXEv>(
        rule, sign, maxLag, maxUpperLag, X));  // return a random move
  }

  uptr<NSIterator<hfmXES>> getIterator(const hfmXES& se) override {
    return uptr<NSIterator<hfmXES>>(
        new NSIteratorHFMChangeSingleInput<hfmXES, hfmXEv>(
            se.first.getR(), vMaxLag,
            vMaxUpperLag));  // return an iterator to the neighbors of 'rep'
  }

  std::string toString() const override {
    stringstream ss;
    ss << "NSSeqHFMChangeSingleInput";
    return ss.str();
  }
};

}  // namespace HFM
#endif /*HFM_NSSEQCHANGESINGLEINPUT_HPP_*/
