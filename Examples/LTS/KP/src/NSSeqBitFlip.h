#ifndef KP_NSSEQBitFlip_H_
#define KP_NSSEQBitFlip_H_

// Framework includes
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Core/RandGen.hpp>

// Own includes
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;

namespace KP {

class MoveBitFlip : public Move<ESolutionKP> {
 private:
  int item;

 public:
  MoveBitFlip(int _item) : item(_item) {}

  virtual ~MoveBitFlip() {}

  void print() const override {
    std::cout << id() << " with params: '"
              << "item=" << item << "'" << std::endl;
  }

  std::string id() const override {
    return Move<ESolutionKP>::idMoveComponent().append(":MoveBitFlip");
  }

  bool operator==(const Move<ESolutionKP>& _m) const override {
    auto& m = (const MoveBitFlip&)_m;
    return item == m.item;
  }

  // Implement these methods in the .cpp file

  bool canBeApplied(const ESolutionKP& s) override;

  uptr<Move<ESolutionKP>> apply(ESolutionKP& s) override;

  op<Evaluation<>> cost(const pair<SolutionKP, Evaluation<>>& se,
                        bool mayEstimate) override;
};

class NSIteratorBitFlip : public NSIterator<ESolutionKP> {
 private:
  int N, item;

 public:
  NSIteratorBitFlip(int _N) : N(_N) {}

  virtual ~NSIteratorBitFlip() {}

  // Implement these methods in the .cpp file

  void first() override;
  void next() override;
  bool isDone() override;
  uptr<Move<ESolutionKP>> current() override;
};

class NSSeqBitFlip : public NSSeq<ESolutionKP> {
 private:
  // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
  ProblemInstance& pKP;  // problem instance data
  RandGen& rg;           // random number generator

 public:
  // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
  NSSeqBitFlip(ProblemInstance& _pKP, RandGen& _rg) : pKP(_pKP), rg(_rg) {}

  virtual ~NSSeqBitFlip() {}

  void print() const override { std::cout << "NSSeqBitFlip" << std::endl; }

  std::string id() const override {
    return NSSeq<ESolutionKP>::idComponent().append(":NSSeqBitFlip");
  }

  uptr<NSIterator<ESolutionKP>> getIterator(const ESolutionKP& s) override {
    // return an iterator to the neighbors of 'rep'
    return uptr<NSIterator<ESolutionKP>>(new NSIteratorBitFlip(pKP.N));
  }

  // Implement this method in the .cpp file

  uptr<Move<ESolutionKP>> randomMove(const ESolutionKP& s) override;
};
}  // namespace KP

#endif /*KP_NSSEQBitFlip_H_*/
