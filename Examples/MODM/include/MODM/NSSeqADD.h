#ifndef MODM_NSSEQADD_H_
#define MODM_NSSEQADD_H_

// Framework includes
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/RandGen.hpp>

// Own includes
#include "ConstructiveBasicGreedyRandomized.h"
#include "DeltaStructure.h"
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;

namespace MODM {

class MoveADD : public Move<ESolutionMODM> {
 private:
  ProblemInstance* dmproblem;
  RandGen& rg;
  bool reverse;
  vector<int> vecProducts;
  vector<int> vecClients;

 public:
  MoveADD(bool _reverse, vector<int> _products, vector<int> _clients,
          ProblemInstance* _dmproblem, RandGen& _rg)
      : reverse(_reverse),
        vecProducts(_products),
        vecClients(_clients),
        dmproblem(_dmproblem),
        rg(_rg) {}

  virtual ~MoveADD() {}

  bool canBeApplied(const ESolutionMODM& se) override { return true; }

  uptr<Move<ESolutionMODM>> apply(ESolutionMODM& se) override {
    SolutionMODM& s = se.first;
    RepMODM& rep = s.getR();
    AdsMODM& ads = s.getADS();
    // cout<<"aplying..."<<endl;
    // cout<<reverse<<endl;
    vector<int> revProducts;
    vector<int> revClients;
    bool revReverse;
    int nClients = dmproblem->getNumberOfClients();
    int nProducts = dmproblem->getNumberOfProducts();

    if (reverse == false) {
      revReverse = true;
      vector<bool> whileProducts(nProducts, false);

      for (int rp = 0; rp < nProducts; rp++) {
        int p = rg.rand(nProducts);
        while (whileProducts[p] == true) p = rg.rand(nProducts);
        whileProducts[p] = true;

        vector<bool> whileClients(nClients, false);

        for (int rc = 0; rc < nClients; rc++) {
          int c = rg.rand(nClients);
          while (whileClients[c] == true) c = rg.rand(nClients);
          whileClients[c] = true;

          double rev = dmproblem->getRevenue(c, p);
          double cost = dmproblem->getCost(c, p);
          // cout << "(" << newRep[client] << "," <<
          // pMODM.getClientMaxOffers(client) << ")" << endl;
          if ((ads.productOffers[p] > 0) && (rep[c][p] == false))
            if (ads.clientOffers[c] <
                dmproblem->getClientMaxOffers(
                    c))  // Verifico se o client pode receber mais algum produto
              if ((rev > cost) &&
                  (cost + ads.totalCost[p]) <= dmproblem->getProductBudget(p)) {
                rep[c][p] = true;
                revProducts.push_back(p);
                revClients.push_back(c);

                // update ADS
                ads.clientOffers[c]++;
                ads.productOffers[p]++;
                ads.totalCost[p] += cost;
                ads.totalRevenue[p] += rev;
              }
        }
      }
    } else {
      revReverse = false;

      for (int i = 0; i < vecProducts.size(); i++) {
        int c = vecClients[i];
        int p = vecProducts[i];
        rep[c][p] = false;
        ads.clientOffers[c]--;
        ads.productOffers[p]--;
        ads.totalCost[p] -= dmproblem->getCost(c, p);
        ads.totalRevenue[p] -= dmproblem->getRevenue(c, p);
      }
    }

    // cout<<"applied..."<<endl;
    // getchar();
    return uptr<Move<ESolutionMODM>>(
        new MoveADD(revReverse, revProducts, revClients, dmproblem, rg));
  }

  virtual bool operator==(const Move<ESolutionMODM>& _m) const {
    const MoveADD& m = (const MoveADD&)_m;
    return (m.reverse == reverse);
  }

  void print() const override {
    cout << "MoveADD( ";
    cout << reverse << " )";
    cout << endl;
  }

  void toString() { cout << "MoveADD" << endl; }
};

class NSIteratorADD : public NSIterator<ESolutionMODM> {
 private:
  ProblemInstance* dmproblem;
  RandGen& rg;
  int i;

 public:
  NSIteratorADD(ProblemInstance* _dmproblem, RandGen& _rg)
      : dmproblem(_dmproblem), rg(_rg) {}

  virtual ~NSIteratorADD() {}

  void first() override { i = 1; }

  void next() override {
    i++;
    // cout << i << endl;
  }

  bool isDone() override { return (i == 2); }

  uptr<Move<ESolutionMODM>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSSeqADD. Aborting." << endl;
      exit(1);
    }
    vector<int> vazioP;
    vector<int> vazioC;

    return uptr<Move<ESolutionMODM>>(new MoveADD(
        false, vazioP, vazioC, dmproblem, rg));  // return a random move
  }
};

class NSSeqADD : public NSSeq<ESolutionMODM> {
 private:
  ProblemInstance* dmproblem;
  RandGen& rg;

 public:
  NSSeqADD(RandGen& _rg, ProblemInstance* _dmproblem)
      : rg(_rg), dmproblem(_dmproblem) {}

  virtual ~NSSeqADD() {}

  virtual uptr<NSIterator<ESolutionMODM>> getIterator(
      const ESolutionMODM& se) override {
    return uptr<NSIterator<ESolutionMODM>>(new NSIteratorADD(
        dmproblem, rg));  // return an iterator to the neighbors of 'rep'
  }

  virtual uptr<Move<ESolutionMODM>> randomMove(
      const ESolutionMODM& se) override {
    const SolutionMODM& s = se.first;
    const RepMODM& rep = s.getR();
    const AdsMODM* ads = &s.getADS();
    //
    vector<int> vazioP;
    vector<int> vazioC;
    /*cout<<"oi ADD NS"<<endl;
               getchar();*/
    return uptr<Move<ESolutionMODM>>(new MoveADD(
        false, vazioP, vazioC, dmproblem, rg));  // return a random move
  }

  static string idComponent() {
    stringstream ss;
    ss << NS<ESolutionMODM>::idComponent() << ":NSSeqADD";
    return ss.str();
  }

  virtual string id() const override { return idComponent(); }
};
}  // namespace MODM

#endif /*MODM_NSSEQADD_H_*/
