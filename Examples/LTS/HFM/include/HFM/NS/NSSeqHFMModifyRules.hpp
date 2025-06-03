#ifndef HFM_NSSEQNEIGHMODIFYRULES_HPP_
#define HFM_NSSEQNEIGHMODIFYRULES_HPP_

// Framework includes
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Core/RandGen.hpp>

// Own includes
#include "../ProblemInstance.hpp"
#include "../Solution.h"

using namespace std;
namespace HFM {

template <XESolution hfmXES = ESolutionHFM, XEvaluation hfmXEv = EvaluationHFM>
class MoveHFMModifyRule : public Move<hfmXES> {
 private:
  int r, o;
  double applyValue;
  bool sign;
  int vectorType;

 public:
  MoveHFMModifyRule(int _r, int _o, double _applyValue, bool _sign,
                    int _vectorType)
      : r(_r),
        o(_o),
        applyValue(_applyValue),
        sign(_sign),
        vectorType(_vectorType) {}

  virtual ~MoveHFMModifyRule() {}

  bool canBeApplied(const hfmXES& se) override { return true; }

  uptr<Move<hfmXES>> apply(hfmXES& se) override {
    RepHFM& rep = se.first.getR();
    if (r == -1)
      return uptr<Move<hfmXES>>(
          new MoveHFMModifyRule<hfmXES, hfmXEv>(-1, -1, -1, -1, -1));

    if (r == PERTINENCEFUNC) {
      if (vectorType == Single_Input)
        rep.singleFuzzyRS[o][r] = !rep.singleFuzzyRS[o][r];
      if (vectorType == Average_Inputs)
        rep.averageFuzzyRS[o][r] = !rep.averageFuzzyRS[o][r];
      if (vectorType == Derivative_Inputs)
        rep.derivativeFuzzyRS[o][r] = !rep.derivativeFuzzyRS[o][r];

      return uptr<Move<hfmXES>>(new MoveHFMModifyRule<hfmXES, hfmXEv>(
          r, o, applyValue, !sign, vectorType));
    }

    if (vectorType == Single_Input) {
      if (!sign)
        rep.singleFuzzyRS[o][r] += applyValue;
      else
        rep.singleFuzzyRS[o][r] -= applyValue;
    }

    if (vectorType == Average_Inputs) {
      if (!sign)
        rep.averageFuzzyRS[o][r] += applyValue;
      else
        rep.averageFuzzyRS[o][r] -= applyValue;
    }

    if (vectorType == Derivative_Inputs) {
      if (!sign)
        rep.derivativeFuzzyRS.at(o).at(r) += applyValue;
      else
        rep.derivativeFuzzyRS[o][r] -= applyValue;
    }
    // return reverse move
    return uptr<Move<hfmXES>>(new MoveHFMModifyRule<hfmXES, hfmXEv>(
        r, o, applyValue, !sign, vectorType));
  }

  virtual bool operator==(const Move<hfmXES>& _m) const {
    const MoveHFMModifyRule& m = (const MoveHFMModifyRule&)_m;
    return ((m.r == r) && (m.o == o) && (m.sign == sign));
  }

  void print() const override {
    std::cout << "MoveNEIGHModifyRule( vector: " << r << " : ";
    std::cout << " option " << o << " <=>  sign " << sign << "vectorType "
              << vectorType << " )";
    std::cout << std::endl;
  }
};

template <XESolution hfmXES = ESolutionHFM, XEvaluation hfmXEv = EvaluationHFM>
class NSIteratorHFMModifyRules : public NSIterator<hfmXES> {
 private:
  // MoveHFMModifyRule* m;
  // vector<uptr<MoveHFMModifyRule>> moves;
  uptr<Move<hfmXES>> m;              // general move
  vector<uptr<Move<hfmXES>>> moves;  // general moves
  int index;
  const RepHFM& rep;
  HFMProblemInstance& pEFP;
  vector<double>& vUpdateValues;

 public:
  NSIteratorHFMModifyRules(const RepHFM& _rep, HFMProblemInstance& _pEFP,
                           vector<double>& _vUpdateValues)
      : rep(_rep), pEFP(_pEFP), vUpdateValues(_vUpdateValues) {
    index = 0;
    m = nullptr;
  }

  virtual ~NSIteratorHFMModifyRules() {
    // for (int i = index + 1; i < (int) moves.size(); i++)
    //	delete moves[i];
  }

  virtual void first() override {
    int nShakes = vUpdateValues.size();
    int options = rep.singleFuzzyRS.size();  // rep.size() options

    for (int sign = 0; sign < 2; sign++)
      for (int r = 0; r < NCOLUMNATRIBUTES; r++)
        for (int o = 0; o < options; o++)
          for (int v = 0; v < nShakes; v++) {
            moves.push_back(
                uptr<Move<hfmXES>>(new MoveHFMModifyRule<hfmXES, hfmXEv>(
                    r, o, vUpdateValues[v], sign, 0)));
          }

    options = rep.averageFuzzyRS.size();  // rep.size() options

    for (int sign = 0; sign < 2; sign++)
      for (int r = 0; r < NCOLUMNATRIBUTES; r++)
        for (int o = 0; o < options; o++)
          for (int v = 0; v < nShakes; v++) {
            moves.push_back(
                uptr<Move<hfmXES>>(new MoveHFMModifyRule<hfmXES, hfmXEv>(
                    r, o, vUpdateValues[v], sign, 1)));
          }

    options = rep.derivativeFuzzyRS.size();  // rep.size() options

    for (int sign = 0; sign < 2; sign++)
      for (int r = 0; r < NCOLUMNATRIBUTES; r++)
        for (int o = 0; o < options; o++)
          for (int v = 0; v < (int)nShakes; v++) {
            moves.push_back(
                uptr<Move<hfmXES>>(new MoveHFMModifyRule<hfmXES, hfmXEv>(
                    r, o, vUpdateValues[v], sign, 2)));
          }

    if (moves.size() > 0)
      m = std::move(
          moves[index]);  // stealing from vector... verify if this is correct!
                          // otherwise, must need clone() on Move
    else
      m = nullptr;
  }

  virtual void next() {
    index++;
    if (index < (int)moves.size())
      m = std::move(
          moves[index]);  // stealing from vector... verify if this is correct!
                          // otherwise, must need clone() on Move
    else
      m = nullptr;
  }

  virtual bool isDone() override { return m == nullptr; }

  virtual uptr<Move<hfmXES>> current() override {
    if (isDone()) {
      std::cout << "There isnt any current element!" << std::endl;
      std::cout << "NSIteratorNEIGHModifyRules. Aborting." << std::endl;
      exit(1);
    }

    uptr<Move<hfmXES>> m2 = std::move(m);
    m = nullptr;

    return m2;
  }
};

// This NSSeq can work for both Single or MultiObjective... should define space
// here. Defaults to SingleObj
template <XESolution hfmXES = ESolutionHFM, XEvaluation hfmXEv = EvaluationHFM>
class NSSeqHFMModifyRules : public NSSeq<hfmXES> {
 private:
  HFMProblemInstance& pEFP;
  RandGen& rg;
  vector<double> vUpdateValues;

 public:
  // A vector of values to be used for changing rules and weights can be passed
  // as parameter
  NSSeqHFMModifyRules(HFMProblemInstance& _pEFP, RandGen& _rg,
                      vector<double>* _vUpdateValues = nullptr)
      : pEFP(_pEFP), rg(_rg) {
    // TODO mean from the targetfile
    double mean = pEFP.getMean(0);

    if (_vUpdateValues) {
      std::cout << "Modify values given as input:" << *_vUpdateValues
                << std::endl;
      assert(_vUpdateValues->size() > 0);
      vUpdateValues = *_vUpdateValues;
    } else {
      vUpdateValues.push_back(0.01);
      vUpdateValues.push_back(0.1);
      vUpdateValues.push_back(1);
      vUpdateValues.push_back(mean / 30);
      vUpdateValues.push_back(mean / 15);
      vUpdateValues.push_back(mean / 6);
      vUpdateValues.push_back(mean / 2);
      vUpdateValues.push_back(mean);
      vUpdateValues.push_back(mean * 2);
      vUpdateValues.push_back(mean * 5);
      vUpdateValues.push_back(mean * 100);
    }
  }

  virtual ~NSSeqHFMModifyRules() {}

  // virtual uptr<Move<hfmXES>> randomMove(const ESolutionHFM& se)
  // override
  virtual uptr<Move<hfmXES>> randomMove(const hfmXES& se) override {
    const RepHFM& rep = se.first.getR();
    int vectorType = rg.rand(N_Inputs_Types);
    int o = -1;
    int r = -1;

    int maxTries = 1000;
    int tries = 1;
    while ((r == -1) && (tries < maxTries)) {
      vectorType = rg.rand(N_Inputs_Types);

      if (vectorType == Single_Input) {
        if (rep.singleFuzzyRS.size() > 0) {
          o = rg.rand(rep.singleFuzzyRS.size());  // rep.size() options
          r = rg.rand(NCOLUMNATRIBUTES);
        }
      }

      if (vectorType == Average_Inputs) {
        if (rep.averageFuzzyRS.size() > 0) {
          o = rg.rand(rep.averageFuzzyRS.size());  // rep.size() options
          r = rg.rand(NCOLUMNATRIBUTES);
        }
      }

      if (vectorType == Derivative_Inputs) {
        if (rep.derivativeFuzzyRS.size() > 0) {
          o = rg.rand(rep.derivativeFuzzyRS.size());  // rep.size() options
          r = rg.rand(NCOLUMNATRIBUTES);
        }
      }
      tries++;
    }

    if (tries == maxTries)
      return uptr<Move<hfmXES>>(new MoveHFMModifyRule<hfmXES, hfmXEv>(
          -1, -1, -1, -1, -1));  // return a random move

    int applyRand = rg.rand(vUpdateValues.size());
    double applyValue = vUpdateValues.at(applyRand);
    bool sign = rg.rand(2);

    return uptr<Move<hfmXES>>(new MoveHFMModifyRule<hfmXES, hfmXEv>(
        r, o, applyValue, sign, vectorType));  // return a random move
  }

  virtual uptr<NSIterator<hfmXES>> getIterator(const hfmXES& se) override {
    return uptr<NSIterator<hfmXES>>(
        new NSIteratorHFMModifyRules<hfmXES, hfmXEv>(
            se.first.getR(), pEFP,
            vUpdateValues));  // return an iterator to the neighbors of 'rep'
  }

  virtual string toString() const override {
    std::stringstream ss;
    ss << "NSSeqHFMModifyRules with move: ";
    return ss.str();
  }
};

}  // namespace HFM
#endif /*HFM_NSSEQNEIGHMODIFYRULES_HPP_*/
