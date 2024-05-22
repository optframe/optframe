#ifndef MODM_NSSEQInvert_H_
#define MODM_NSSEQInvert_H_

// Framework includes
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/RandGen.hpp>

// Own includes
#include "DeltaStructure.h"
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;

namespace MODM {

class MoveInvert : public Move<ESolutionMODM> {
 private:
  ProblemInstance* dmproblem;
  int y;  // produto
  int c;

 public:
  MoveInvert(int _y, int _c, ProblemInstance* _dmproblem)
      : y(_y), c(_c), dmproblem(_dmproblem) {}

  virtual ~MoveInvert() {}

  bool canBeApplied(const ESolutionMODM& se) override {
    const SolutionMODM& s = se.first;
    const AdsMODM& ads = s.getADS();
    bool productOffers = (ads.productOffers[y] > 0);
    return productOffers;
  }

  op<Evaluation<>> cost(const pair<SolutionMODM, Evaluation<>>& se,
                        bool allowEstimate) override {
    const SolutionMODM& s = se.first;

    const RepMODM& rep = s.getR();
    const AdsMODM& ads = s.getADS();
    double f = 0;

    int newValue;
    if (rep[c][y] == 1)
      newValue = 0;
    else
      newValue = 1;

    double costDiff = (newValue - rep[c][y]) * dmproblem->getCost(c, y);
    double revDiff = (newValue - rep[c][y]) * dmproblem->getRevenue(c, y);

    f = revDiff - costDiff;

    double fInv = 0;
    double penMaxOffers = 100;
    int nOffers = ads.clientOffers[c] + newValue - rep[c][y];
    if (nOffers > dmproblem->getClientMaxOffers(c) && rep[c][y] == 0)
      fInv += -penMaxOffers;

    if (nOffers > dmproblem->getClientMaxOffers(c) && rep[c][y] == 1)
      fInv += penMaxOffers;

    double foInvBud = 0;

    double oldTotalCost = ads.totalCost[y];
    double newTotalCost = oldTotalCost + costDiff;
    double productBudgetLimit = dmproblem->getProductBudget(y);

    if (newTotalCost > productBudgetLimit) {
      foInvBud += newTotalCost - productBudgetLimit;
      if (oldTotalCost > productBudgetLimit) {
        foInvBud -= oldTotalCost - productBudgetLimit;
      }
    }

    // return new MoveCost<>(f, fInv + foInvBud * (-1000));
    return make_optional(Evaluation<>(f, fInv + foInvBud * (-1000)));
  }

  uptr<Move<ESolutionMODM>> apply(ESolutionMODM& se) override {
    SolutionMODM& s = se.first;
    RepMODM& rep = s.getR();
    AdsMODM& ads = s.getADS();
    int oldC = rep[c][y];
    if (oldC == 1)
      rep[c][y] = 0;
    else
      rep[c][y] = 1;
    // rep[c][y] = !rep[c][y];

    // update ADS

    ads.totalCost[y] += (rep[c][y] - oldC) * dmproblem->getCost(c, y);
    ads.totalRevenue[y] += (rep[c][y] - oldC) * dmproblem->getRevenue(c, y);
    ads.clientOffers[c] += rep[c][y] - oldC;
    ads.productOffers[y] += rep[c][y] - oldC;

    return uptr<Move<ESolutionMODM>>(new MoveInvert(y, c, dmproblem));
  }

  virtual bool operator==(const Move<ESolutionMODM>& _m) const {
    const MoveInvert& m = (const MoveInvert&)_m;
    return (m.y == y) && (m.c == c);
  }

  void print() const override {
    cout << "MoveInvert( ";
    cout << y << ", " << c << " )";
    cout << endl;
  }
};

class NSIteratorInvert : public NSIterator<ESolutionMODM> {
 private:
  ProblemInstance* dmproblem;
  int y, c;
  int nClients, nProducts;
  const AdsMODM& ads;

 public:
  NSIteratorInvert(const AdsMODM& _ads, ProblemInstance* _dmproblem)
      : ads(_ads), dmproblem(_dmproblem) {
    nClients = dmproblem->getNumberOfClients();
    nProducts = dmproblem->getNumberOfProducts();
  }

  virtual ~NSIteratorInvert() {}

  void first() override {
    y = 0;
    c = 0;
  }

  void next() override {
    c++;
    if (c >= (nClients)) {
      y++;
      if (ads.productOffers[y] == 0) y++;
      c = 0;
    }
  }

  bool isDone() override { return (y >= nProducts); }

  uptr<Move<ESolutionMODM>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSSeqInvert. Aborting." << endl;
      exit(1);
    }

    return uptr<Move<ESolutionMODM>>(new MoveInvert(y, c, dmproblem));
  }
};

class NSSeqInvert : public NSSeq<ESolutionMODM> {
 private:
  ProblemInstance* dmproblem;
  RandGen& rg;

 public:
  NSSeqInvert(RandGen& _rg, ProblemInstance* _dmproblem)
      : rg(_rg), dmproblem(_dmproblem) {}

  virtual ~NSSeqInvert() {}

  virtual uptr<Move<ESolutionMODM>> randomMove(
      const ESolutionMODM& se) override {
    const SolutionMODM& s = se.first;
    const RepMODM& rep = s.getR();
    const AdsMODM& ads = s.getADS();
    int nProduts = dmproblem->getNumberOfProducts();

    int y = rg.rand(nProduts);

    while (ads.productOffers[y] == 0) {
      y = rg.rand(nProduts);
    }

    int nClients = dmproblem->getNumberOfClients();
    int c = rg.rand(nClients);

    return uptr<Move<ESolutionMODM>>(
        new MoveInvert(y, c, dmproblem));  // return a random move
  }

  virtual uptr<NSIterator<ESolutionMODM>> getIterator(
      const ESolutionMODM& se) override {
    const AdsMODM* ads = &se.first.getADS();
    return uptr<NSIterator<ESolutionMODM>>(new NSIteratorInvert(
        *ads, dmproblem));  // return an iterator to the neighbors of 'rep'
  }

  static string idComponent() {
    stringstream ss;
    ss << NS<ESolutionMODM>::idComponent() << ":NSSeqInvert";
    return ss.str();
  }

  virtual string id() const override { return idComponent(); }
};
}  // namespace MODM

#endif /*MODM_NSSEQInvert_H_*/
