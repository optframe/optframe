#ifndef MODM_NSSEQSWAPInter_H_
#define MODM_NSSEQSWAPInter_H_

// Framework includes
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/RandGen.hpp>

// Own includes
#include "DeltaStructure.h"
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;

namespace MODM {

class MoveSWAPInter : public Move<ESolutionMODM> {
 private:
  ProblemInstance* dmproblem;
  int y1, y2;  // produto
  int c1, c2;

 public:
  MoveSWAPInter(int _y1, int _y2, int _c1, int _c2, ProblemInstance* _dmproblem)
      : y1(_y1), y2(_y2), c1(_c1), c2(_c2), dmproblem(_dmproblem) {}

  virtual ~MoveSWAPInter() {}

  bool canBeApplied(const ESolutionMODM& se) override {
    const SolutionMODM& s = se.first;
    const RepMODM& rep = s.getR();
    const AdsMODM& ads = s.getADS();
    // cout<<"canBeApplied!"<<endl;
    // cout<<"y1 = "<<y1<<endl;
    // cout<<"y2 = "<<y2<<endl;

    bool usedProduct1 = ads.productOffers[y1];
    bool usedProduct2 = ads.productOffers[y2];

    bool differentOffers = false;

    if ((rep[c1][y1] + rep[c2][y2]) == 1) {
      differentOffers = true;
    }

    bool differentProducts = false;

    if (y1 != y2) differentProducts = true;

    bool c1Saturado = true;
    // Verificar se o cliente c1 vai estourar
    if (rep[c2][y2] - rep[c1][y1] + ads.clientOffers[c1] <=
        dmproblem->getClientMaxOffers(c1))
      c1Saturado = false;

    bool c2Saturado = true;
    // Verificar se o cliente c1 vai estourar
    if (rep[c1][y1] - rep[c2][y2] + ads.clientOffers[c2] <=
        dmproblem->getClientMaxOffers(c2))
      c2Saturado = false;

    bool pOffers1 = true;
    bool pOffers2 = true;

    if ((ads.productOffers[y1] + rep[c2][y2] - rep[c1][y1]) <
        dmproblem->getProductMinClients(y1))
      pOffers1 = false;
    if ((ads.productOffers[y2] + rep[c1][y1] - rep[c2][y2]) <
        dmproblem->getProductMinClients(y2))
      pOffers2 = false;

    double costDiff1 =
        ((rep[c2][y2] - rep[c1][y1]) * dmproblem->getCost(c1, y1));
    double costDiff2 =
        ((rep[c1][y1] - rep[c2][y2]) * dmproblem->getCost(c2, y2));

    bool budget1 = true;
    bool budget2 = true;

    if (ads.totalCost[y1] + costDiff1 > dmproblem->getProductBudget(y1))
      budget1 = false;

    if (ads.totalCost[y2] + costDiff2 > dmproblem->getProductBudget(y2))
      budget2 = false;

    // cout<<"canBeApplied FINISH!"<<endl;
    // bool budget = true;
    // if (ads.totalCost[y1] + costDiff > dmproblem->getProductBudget(y1))
    //	budget = false;
    bool restr = budget1 && budget2 && pOffers1 && pOffers2;

    // cout<<"c1Saturado = "<<!c1Saturado <<endl;
    // cout<<"c2Saturado = "<<!c2Saturado <<endl <<endl;

    // return differentOffers && differentProducts && (usedProduct1 > 0) &&
    // (usedProduct2 > 0);
    return restr &&
           ((usedProduct1 > 0) && (usedProduct2 > 0) && differentProducts &&
            differentOffers && !c1Saturado && !c2Saturado);
  }

  op<Evaluation<>> cost(const Evaluation<>&, const RepMODM& rep,
                        const AdsMODM* _ads, bool mayEstimate) {
    const AdsMODM& ads = *_ads;
    double f = 0;

    double diffCostC1Y1 =
        (rep[c2][y2] - rep[c1][y1]) * dmproblem->getCost(c1, y1);
    double diffCostC2Y2 =
        (rep[c1][y1] - rep[c2][y2]) * dmproblem->getCost(c2, y2);
    double diffRevC1Y1 =
        (rep[c2][y2] - rep[c1][y1]) * dmproblem->getRevenue(c1, y1);
    double diffRevC2Y2 =
        (rep[c1][y1] - rep[c2][y2]) * dmproblem->getRevenue(c2, y2);

    f = (diffRevC1Y1 + diffRevC2Y2) - (diffCostC1Y1 + diffCostC2Y2);

    double fInv = 0;
    double penMaxOffers = 100;
    int nOffersC1 = ads.clientOffers[c1] + rep[c2][y2] - rep[c1][y1];
    int nOffersC2 = ads.clientOffers[c2] + rep[c1][y1] - rep[c2][y2];

    if (c1 != c2) {
      if (nOffersC1 > dmproblem->getClientMaxOffers(c1) & rep[c2][y2] == 1)
        fInv += -penMaxOffers;

      if (nOffersC1 > dmproblem->getClientMaxOffers(c1) & rep[c2][y2] == 0)
        fInv += penMaxOffers;

      if (nOffersC2 > dmproblem->getClientMaxOffers(c2) & rep[c1][y1] == 1)
        fInv += -penMaxOffers;

      if (nOffersC2 > dmproblem->getClientMaxOffers(c2) & rep[c1][y1] == 0)
        fInv += penMaxOffers;
    }

    double foInvBud = 0;
    vector<int> vProducts;
    vector<int> vCostDiff;
    vProducts.push_back(y1);
    vProducts.push_back(y2);
    vCostDiff.push_back(diffCostC1Y1);
    vCostDiff.push_back(diffCostC2Y2);
    for (int i = 0; i <= 1; i++) {
      int y = vProducts[i];
      double oldTotalCost = ads.totalCost[y];
      double newTotalCost = oldTotalCost + vCostDiff[i];
      double productBudgetLimit = dmproblem->getProductBudget(y);

      if (newTotalCost > productBudgetLimit) {
        foInvBud += newTotalCost - productBudgetLimit;
        if (oldTotalCost > productBudgetLimit) {
          foInvBud -= oldTotalCost - productBudgetLimit;
        }
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

    int oldC1 = rep[c1][y1];
    int oldC2 = rep[c2][y2];
    rep[c1][y1] = oldC2;
    rep[c2][y2] = oldC1;

    // update ADS
    ads.totalCost[y1] += (rep[c1][y1] - oldC1) * dmproblem->getCost(c1, y1);
    ads.totalCost[y2] += (rep[c2][y2] - oldC2) * dmproblem->getCost(c2, y2);
    ads.totalRevenue[y1] +=
        (rep[c1][y1] - oldC1) * dmproblem->getRevenue(c1, y1);
    ads.totalRevenue[y2] +=
        (rep[c2][y2] - oldC2) * dmproblem->getRevenue(c2, y2);
    ads.productOffers[y1] += rep[c1][y1] - oldC1;
    ads.productOffers[y2] += rep[c2][y2] - oldC2;

    ads.clientOffers[c1] += rep[c1][y1] - oldC1;
    ads.clientOffers[c2] += rep[c2][y2] - oldC2;

    return uptr<Move<ESolutionMODM>>(
        new MoveSWAPInter(y1, y2, c1, c2, dmproblem));
  }

  virtual bool operator==(const Move<ESolutionMODM>& _m) const {
    const MoveSWAPInter& m = (const MoveSWAPInter&)_m;
    return (m.y1 == y1) && (m.y2 == y2) && (m.c1 == c1) && (m.c2 == c2);
  }

  void print() const override {
    cout << "MoveSWAPInter( ";
    cout << y1 << ", " << y2 << ", " << c1 << ", " << c2 << " )";
    cout << endl;
  }
};

class NSIteratorSWAPInter : public NSIterator<ESolutionMODM> {
 private:
  ProblemInstance* dmproblem;
  int y1, y2, c1, c2;
  int nClients, nProducts;

 public:
  NSIteratorSWAPInter(ProblemInstance* _dmproblem) : dmproblem(_dmproblem) {
    nClients = dmproblem->getNumberOfClients();
    nProducts = dmproblem->getNumberOfProducts();
  }

  virtual ~NSIteratorSWAPInter() {}

  void first() override {
    y1 = 0;
    y2 = 1;
    c1 = 0;
    c2 = 0;
  }

  void next() override {
    c2++;
    if (c2 >= nClients) {
      c1++;
      c2 = 0;
      if (c1 >= nClients) {
        y2++;
        // cout << "y = " << y << endl;
        c1 = 0;
        c2 = 0;
        if (y2 >= nProducts) {
          y1++;
          y2 = y1 + 1;
          //					cout<<"y1 = "<<y1<<endl;
          //					cout<<"y2 = "<<y2<<endl<<endl;
        }
      }
    }
  }

  bool isDone() override { return (y1 == nProducts - 1); }

  uptr<Move<ESolutionMODM>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSSeqSWAPDM. Aborting." << endl;
      exit(1);
    }

    return uptr<Move<ESolutionMODM>>(
        new MoveSWAPInter(y1, y2, c1, c2, dmproblem));
  }
};

class NSSeqSWAPInter : public NSSeq<ESolutionMODM> {
 private:
  ProblemInstance* dmproblem;
  RandGen& rg;

 public:
  NSSeqSWAPInter(RandGen& _rg, ProblemInstance* _dmproblem)
      : rg(_rg), dmproblem(_dmproblem) {}

  virtual ~NSSeqSWAPInter() {}

  virtual uptr<Move<ESolutionMODM>> randomMove(
      const ESolutionMODM& se) override {
    const SolutionMODM& s = se.first;
    const RepMODM& rep = s.getR();
    const AdsMODM& ads = s.getADS();
    //
    int nProduts = dmproblem->getNumberOfProducts();

    int y1 = rg.rand(nProduts);

    while (ads.productOffers[y1] == 0) {
      y1 = rg.rand(nProduts);
    }

    int y2 = rg.rand(nProduts);
    while (ads.productOffers[y2] == 0 && y1 == y2) {
      y2 = rg.rand(nProduts);
    }

    int nClients = dmproblem->getNumberOfClients();
    int c1 = rg.rand(nClients);
    int c2 = rg.rand(nClients);

    return uptr<Move<ESolutionMODM>>(
        new MoveSWAPInter(y1, y2, c1, c2, dmproblem));  // return a random move
  }

  virtual uptr<NSIterator<ESolutionMODM>> getIterator(
      const ESolutionMODM& se) override {
    return uptr<NSIterator<ESolutionMODM>>(new NSIteratorSWAPInter(
        dmproblem));  // return an iterator to the neighbors of 'rep'
  }

  static string idComponent() {
    stringstream ss;
    ss << NS<ESolutionMODM>::idComponent() << ":NSSeqSWAPInter";
    return ss.str();
  }

  virtual string id() const override { return idComponent(); }
};

}  // namespace MODM

#endif /*MODM_NSSEQSWAPInter_H_*/
