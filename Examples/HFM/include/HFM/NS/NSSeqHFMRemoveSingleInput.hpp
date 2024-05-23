#ifndef HFM_NSSEQREMOVESINGLEINPUT_HPP_
#define HFM_NSSEQREMOVESINGLEINPUT_HPP_

// Framework includes
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/RandGen.hpp>

// Own includes
#include "../ProblemInstance.hpp"
#include "../Solution.h"

using namespace std;
namespace HFM {

template <XESolution hfmXES = ESolutionHFM, XEvaluation hfmXEv = EvaluationHFM>
// class MoveHFMRemoveSingleInput: public Move<hfmXES>
class MoveHFMRemoveSingleInput : public Move<hfmXES> {
 private:
  int rule;
  bool reverse;
  pair<int, int> singleIndexOld;
  vector<double> singleFuzzyRSOld;

 public:
  MoveHFMRemoveSingleInput(int _rule, bool _reverse,
                           pair<int, int> _singleIndexOld,
                           vector<double> _singleFuzzyRSOld)
      : rule(_rule),
        reverse(_reverse),
        singleIndexOld(_singleIndexOld),
        singleFuzzyRSOld(_singleFuzzyRSOld) {}

  virtual ~MoveHFMRemoveSingleInput() {}

  bool canBeApplied(const hfmXES& se) override {
    const RepHFM& rep = se.first.getR();
    return ((rule >= 0) && (rule < (int)rep.singleIndex.size()) &&
            ((int)rep.singleIndex.size() > 1));
  }

  uptr<Move<hfmXES>> apply(hfmXES& se) override {
    RepHFM& rep = se.first.getR();
    pair<int, int> tempSingleIndexOld;
    vector<double> tempSingleFuzzyRSOld;

    if (reverse == false) {
      if (rep.singleIndex.size() > 0) {
        tempSingleIndexOld = rep.singleIndex[rule];
        tempSingleFuzzyRSOld = rep.singleFuzzyRS[rule];

        rep.singleIndex.erase(rep.singleIndex.begin() + rule);
        rep.singleFuzzyRS.erase(rep.singleFuzzyRS.begin() + rule);
        return uptr<Move<hfmXES>>(new MoveHFMRemoveSingleInput<hfmXES, hfmXEv>(
            rule, !reverse, tempSingleIndexOld, tempSingleFuzzyRSOld));
      }

    } else {
      rep.singleIndex.insert(rep.singleIndex.begin() + rule, singleIndexOld);
      rep.singleFuzzyRS.insert(rep.singleFuzzyRS.begin() + rule,
                               singleFuzzyRSOld);
    }

    return uptr<Move<hfmXES>>(new MoveHFMRemoveSingleInput<hfmXES, hfmXEv>(
        rule, !reverse, tempSingleIndexOld, tempSingleFuzzyRSOld));
  }

  virtual bool operator==(const Move<hfmXES>& _m) const {
    const MoveHFMRemoveSingleInput& m = (const MoveHFMRemoveSingleInput&)_m;
    return ((m.rule == rule) && (m.reverse == reverse));
  }

  void print() const override {
    cout << "MoveNEIGHRemoveSingleInput( vector:  rule " << rule
         << " <=>  reverse " << reverse << " )";
    cout << endl;
  }
};

template <XESolution hfmXES = ESolutionHFM, XEvaluation hfmXEv = EvaluationHFM>
// class NSIteratorHFMRemoveSingleInput: public NSIterator<ESolutionHFM>
class NSIteratorHFMRemoveSingleInput : public NSIterator<hfmXES> {
 private:
  // MoveHFMRemoveSingleInput* m;
  // vector<uptr<MoveHFMRemoveSingleInput>> moves;
  uptr<Move<hfmXES>> m;              // general move
  vector<uptr<Move<hfmXES>>> moves;  // general moves

  int index;
  const RepHFM& rep;

 public:
  NSIteratorHFMRemoveSingleInput(const RepHFM& _rep) : rep(_rep) {
    index = 0;
    m = nullptr;
  }

  virtual ~NSIteratorHFMRemoveSingleInput() {
    // for (int i = index + 1; i < (int) moves.size(); i++)
    //	delete moves[i];
  }

  virtual void first() override {
    pair<int, int> tempSingleIndexOld;
    vector<double> tempSingleFuzzyRSOld;

    for (int rule = 0; rule < (int)rep.singleIndex.size(); rule++) {
      moves.push_back(
          uptr<Move<hfmXES>>(new MoveHFMRemoveSingleInput<hfmXES, hfmXEv>(
              rule, false, tempSingleIndexOld, tempSingleFuzzyRSOld)));
    }

    if (moves.size() > 0) {
      m = std::move(
          moves[index]);  // stealing from vector... verify if this is correct!
                          // otherwise, must need clone() on Move
    } else
      m = nullptr;
  }

  virtual void next() {
    index++;
    if (index < (int)moves.size()) {
      m = std::move(
          moves[index]);  // stealing from vector... verify if this is correct!
                          // otherwise, must need clone() on Move
    } else
      m = nullptr;
  }

  virtual bool isDone() override { return m == nullptr; }

  virtual uptr<Move<hfmXES>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSIteratorNEIGHRemoveSingleInput. Aborting." << endl;
      exit(1);
    }

    uptr<Move<hfmXES>> m2 = std::move(m);
    m = nullptr;
    return m2;
  }
};

template <XESolution hfmXES = ESolutionHFM, XEvaluation hfmXEv = EvaluationHFM>
// class NSSeqHFMRemoveSingleInput: public NSSeq<ESolutionHFM>
class NSSeqHFMRemoveSingleInput : public NSSeq<hfmXES> {
 private:
  RandGen& rg;

 public:
  NSSeqHFMRemoveSingleInput(RandGen& _rg) : rg(_rg) {}

  virtual ~NSSeqHFMRemoveSingleInput() {}

  virtual uptr<Move<hfmXES>> randomMove(const hfmXES& se) {
    const RepHFM& rep = se.first.getR();
    int rule = -1;
    if (rep.singleIndex.size() > 0) rule = rg.rand(rep.singleIndex.size());

    pair<int, int> tempSingleIndexOld;
    vector<double> tempSingleFuzzyRSOld;

    return uptr<Move<hfmXES>>(new MoveHFMRemoveSingleInput<hfmXES, hfmXEv>(
        rule, false, tempSingleIndexOld,
        tempSingleFuzzyRSOld));  // return a random move
  }

  virtual uptr<NSIterator<hfmXES>> getIterator(const hfmXES& se) override {
    const RepHFM& rep = se.first.getR();
    return uptr<NSIterator<hfmXES>>(
        new NSIteratorHFMRemoveSingleInput<hfmXES, hfmXEv>(
            rep));  // return an iterator to the neighbors of 'rep'
  }

  virtual string toString() const override {
    stringstream ss;
    ss << "NSSeqNEIGHRemoveSingleInput";
    return ss.str();
  }
};

}  // namespace HFM
#endif /*HFM_NSSEQREMOVESINGLEINPUT_HPP_*/
