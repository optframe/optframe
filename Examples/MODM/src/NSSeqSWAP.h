#ifndef MODM_NSSEQSWAP_H_
#define MODM_NSSEQSWAP_H_

// Framework includes
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/RandGen.hpp>

// Own includes
#include "DeltaStructure.h"
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;

namespace MODM {

class MoveSWAP : public Move<ESolutionMODM> {
 private:
  ProblemInstance* dmproblem;
  int y;  // produto
  int c1, c2;

 public:
  MoveSWAP(int _y, int _c1, int _c2, ProblemInstance* _dmproblem)
      : y(_y), c1(_c1), c2(_c2), dmproblem(_dmproblem) {}

  virtual ~MoveSWAP() {}

  bool canBeApplied(const ESolutionMODM& se) override {
    const SolutionMODM& s = se.first;
    const RepMODM& rep = s.getR();
    const AdsMODM& ads = s.getADS();
    //
    bool usedProduct = ads.productOffers[y];

    bool differentOffers = false;

    if ((rep[c1][y] + rep[c2][y]) == 1) {
      differentOffers = true;
    }

    bool c1Saturado = true;
    // Verificar se o cliente c1 vai estourar
    if (rep[c2][y] - rep[c1][y] + ads.clientOffers[c1] <=
        dmproblem->getClientMaxOffers(c1))
      c1Saturado = false;

    bool c2Saturado = true;
    // Verificar se o cliente c1 vai estourar
    if (rep[c1][y] - rep[c2][y] + ads.clientOffers[c2] <=
        dmproblem->getClientMaxOffers(c2))
      c2Saturado = false;

    double costDiff = (rep[c2][y] * dmproblem->getCost(c1, y) +
                       rep[c1][y] * dmproblem->getCost(c2, y)) -
                      (rep[c1][y] * dmproblem->getCost(c1, y) +
                       rep[c2][y] * dmproblem->getCost(c2, y));
    // double revDiff = (rep[c1][y] * dmproblem->getRevenue(c1, y) + rep[c2][y]
    // * dmproblem->getRevenue(c2, y)) - (oldC1 * dmproblem->getRevenue(c1, y) +
    // oldC2 * dmproblem->getRevenue(c2, y));

    bool budget = true;
    if (ads.totalCost[y] + costDiff > dmproblem->getProductBudget(y))
      budget = false;

    // return differentOffers && (usedProduct > 0);
    return ((usedProduct > 0) && budget && differentOffers && !c1Saturado &&
            !c2Saturado);
  }

  op<Evaluation<>> cost(const pair<SolutionMODM, Evaluation<>>& se,
                        bool allowEstimate) override {
    const SolutionMODM& s = se.first;

    const RepMODM& rep = s.getR();
    const AdsMODM& ads = s.getADS();
    //
    double f = 0;

    double costDiff = (rep[c2][y] * dmproblem->getCost(c1, y) +
                       rep[c1][y] * dmproblem->getCost(c2, y)) -
                      (rep[c1][y] * dmproblem->getCost(c1, y) +
                       rep[c2][y] * dmproblem->getCost(c2, y));
    double revDiff = (rep[c2][y] * dmproblem->getRevenue(c1, y) +
                      rep[c1][y] * dmproblem->getRevenue(c2, y)) -
                     (rep[c1][y] * dmproblem->getRevenue(c1, y) +
                      rep[c2][y] * dmproblem->getRevenue(c2, y));

    f = revDiff - costDiff;

    double fInv = 0;
    double penMaxOffers = 100;
    int nOffersC1 = ads.clientOffers[c1] + rep[c2][y] - rep[c1][y];
    int nOffersC2 = ads.clientOffers[c2] + rep[c1][y] - rep[c2][y];

    if (nOffersC1 > dmproblem->getClientMaxOffers(c1) & rep[c2][y] == 1)
      fInv += -penMaxOffers;

    if (nOffersC1 > dmproblem->getClientMaxOffers(c1) & rep[c2][y] == 0)
      fInv += penMaxOffers;

    if (nOffersC2 > dmproblem->getClientMaxOffers(c2) & rep[c1][y] == 1)
      fInv += -penMaxOffers;

    if (nOffersC2 > dmproblem->getClientMaxOffers(c2) & rep[c1][y] == 0)
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

    // return new MoveCost<>(f, fInv + foInvBud * (-100000));
    return make_optional(Evaluation<>(f, fInv + foInvBud * (-100000)));
  }

  uptr<Move<ESolutionMODM>> apply(ESolutionMODM& se) override {
    SolutionMODM& s = se.first;
    RepMODM& rep = s.getR();
    AdsMODM& ads = s.getADS();
    // cout<<rep<<endl;
    // cout<<ads.clientOffers<<endl;
    // cout<<ads.productOffers<<endl;
    // cout<<ads.totalCost<<endl;
    // cout<<ads.totalRevenue<<endl;
    // getchar();

    int oldC1 = rep[c1][y];
    int oldC2 = rep[c2][y];
    rep[c1][y] = oldC2;
    rep[c2][y] = oldC1;

    // update ADS
    ads.totalCost[y] += (rep[c1][y] - oldC1) * dmproblem->getCost(c1, y) +
                        (rep[c2][y] - oldC2) * dmproblem->getCost(c2, y);
    ads.totalRevenue[y] += (rep[c1][y] - oldC1) * dmproblem->getRevenue(c1, y) +
                           (rep[c2][y] - oldC2) * dmproblem->getRevenue(c2, y);

    ads.clientOffers[c1] += oldC2 - oldC1;
    ads.clientOffers[c2] += oldC1 - oldC2;

    return uptr<Move<ESolutionMODM>>(new MoveSWAP(y, c1, c2, dmproblem));
  }

  virtual bool operator==(const Move<ESolutionMODM>& _m) const {
    const MoveSWAP& m = (const MoveSWAP&)_m;
    return (m.y == y) && (m.c1 == c1) && (m.c2 == c2);
  }

  void print() const override {
    cout << "MoveSWAP( ";
    cout << y << ", " << c1 << ", " << c2 << " )";
    cout << endl;
  }
};

class NSIteratorSWAP : public NSIterator<ESolutionMODM> {
 private:
  ProblemInstance* dmproblem;
  int y, c1, c2;
  int nClients, nProducts;
  const AdsMODM& ads;

 public:
  NSIteratorSWAP(const AdsMODM& _ads, ProblemInstance* _dmproblem)
      : ads(_ads), dmproblem(_dmproblem) {
    nClients = dmproblem->getNumberOfClients();
    nProducts = dmproblem->getNumberOfProducts();
  }

  virtual ~NSIteratorSWAP() {}

  void first() override {
    y = 0;
    c1 = 0;
    c2 = 1;
  }

  void next() override {
    c2++;
    if (c2 >= (nClients)) {
      c1++;
      c2 = c1 + 1;
      if (c1 >= (nClients - 1)) {
        y++;
        // cout << "y = " << y << endl;
        c1 = 0;
        c2 = c1 + 1;
      }
    }

    // cout<<"c1 = "<<c1<<endl;
    // cout<<"c2 = "<<c2<<endl;
  }

  bool isDone() override { return (y == nProducts); }

  uptr<Move<ESolutionMODM>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSSeqSWAP. Aborting." << endl;
      exit(1);
    }

    return uptr<Move<ESolutionMODM>>(new MoveSWAP(y, c1, c2, dmproblem));
  }
};

class NSSeqSWAP : public NSSeq<ESolutionMODM> {
 private:
  ProblemInstance* dmproblem;
  RandGen& rg;

 public:
  NSSeqSWAP(RandGen& _rg, ProblemInstance* _dmproblem)
      : rg(_rg), dmproblem(_dmproblem) {}

  virtual ~NSSeqSWAP() {}

  virtual uptr<Move<ESolutionMODM>> randomMove(
      const ESolutionMODM& se) override {
    const SolutionMODM& s = se.first;
    const RepMODM& rep = s.getR();
    const AdsMODM& ads = s.getADS();
    //
    int nProduts = dmproblem->getNumberOfProducts();

    int y = rg.rand(nProduts);

    while (ads.productOffers[y] == 0) {
      y = rg.rand(nProduts);
    }

    int nClients = dmproblem->getNumberOfClients();
    int c1 = rg.rand(nClients);
    int c2 = rg.rand(nClients);

    while (c1 == c2) {
      c2 = rg.rand(nClients);
    }

    return uptr<Move<ESolutionMODM>>(
        new MoveSWAP(y, c1, c2, dmproblem));  // return a random move
  }

  virtual uptr<NSIterator<ESolutionMODM>> getIterator(
      const ESolutionMODM& se) override {
    return uptr<NSIterator<ESolutionMODM>>(new NSIteratorSWAP(
        se.first.getADS(),
        dmproblem));  // return an iterator to the neighbors of 'rep'
  }

  static string idComponent() {
    stringstream ss;
    ss << NS<ESolutionMODM>::idComponent() << ":NSSeqSWAP";
    return ss.str();
  }

  virtual string id() const override { return idComponent(); }
};
}  // namespace MODM

#endif /*MODM_NSSEQSWAP_H_*/
