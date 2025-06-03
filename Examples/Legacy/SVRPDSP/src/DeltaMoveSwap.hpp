#ifndef SVRPDSP_DELTA_MOVE_SWAP_HPP_
#define SVRPDSP_DELTA_MOVE_SWAP_HPP_

#include <OptFrame/Util/NSAdapter/Moves/MoveTSPSwap.hpp>
#include <cmath>

#include "Delta.h"
#include "MySolution.hpp"

using namespace std;

namespace SVRPDSP {

class DeltaMoveSwap : public MoveTSPSwap<int, AdsSVRPDSP, MySolution> {
  typedef MoveTSPSwap<int, AdsSVRPDSP, MySolution> super;

 private:
  ProblemInstance* svrpdsp;

  static const bool verbose = false;

 public:
  using super::apply;
  using super::canBeApplied;

  DeltaMoveSwap(const super& s, ProblemInstance* _svrpdsp)
      : super(s.get_p1(), s.get_p2(), _svrpdsp), svrpdsp(_svrpdsp) {
    if (!svrpdsp) {
      std::cout << "Error: svrpdsp problem is NULL!" << std::endl;
      print();
      exit(1);
    }
  }

  DeltaMoveSwap(int p1, int p2, ProblemInstance* _svrpdsp)
      : super(p1, p2, _svrpdsp), svrpdsp(_svrpdsp) {
    if (!svrpdsp) {
      std::cout << "Error: svrpdsp problem is NULL!" << std::endl;
      print();
      exit(1);
    }
  }

  virtual ~DeltaMoveSwap() {}

  virtual bool canBeApplied(const RepSVRPDSP& rep,
                            const AdsSVRPDSP* _ads) override {
    const AdsSVRPDSP& ads = *_ads;
    if (!super::canBeApplied(rep, _ads)) return false;

    // do not move zeros!
    if ((p1 == 0) || (p1 == ads.zero) || (p2 == 0) || (p2 == ads.zero))
      return false;

    // do not swap after zero
    if ((p1 > ads.zero) && (p2 > ads.zero)) return false;

    // p1 always smaller than p2
    if (p1 > p2) return false;

    // do not swap delivery customers to after zero!
    if (svrpdsp->isDelivery(rep[p1]) && (p2 > ads.zero))  // p1 < p2
      return false;

    return true;
  }

  virtual Move<RepSVRPDSP, AdsSVRPDSP, MySolution>* apply(
      RepSVRPDSP& rep, AdsSVRPDSP* _ads) override {
    AdsSVRPDSP& ads = *_ads;
    // apply move
    Move<RepSVRPDSP, AdsSVRPDSP, MySolution>& rev = *super::apply(rep, _ads);
    MySolution::syncADS(*svrpdsp, rep, ads);  // syncronize ADS

    super& sRev = (super&)rev;
    DeltaMoveSwap* myMove = new DeltaMoveSwap(sRev, svrpdsp);
    delete &sRev;

    return myMove;
  }

  op<Evaluation<>> cost(const Evaluation<>& e, const RepSVRPDSP& rep,
                        const AdsSVRPDSP* ads, bool mayEstimate) override {
    return NULL;
  }

  pair<double, double>* cost1(const Evaluation<>& e, const RepSVRPDSP& rep,
                              const AdsSVRPDSP& ads) {
    if (verbose) ads.print();

    // always p1 < p2

    unsigned bp1 = p1 - 1;  // before p1
    unsigned ap1 = p1 + 1;  // after p1

    unsigned bp2 = p2 - 1;  // before p2
    unsigned ap2 = p2 + 1;  // after p2

    int f = 0;

    if (p1 == p2) {
      f = 0;
      std::cout << "SWAP SHOULDN'T BE APPLIED! p1==p2" << std::endl;
      getchar();
    }

    if ((p1 > ads.zero) && (p2 > ads.zero))  // all zero!
    {
      f = 0;
      std::cout << "SWAP SHOULDN'T BE APPLIED! both after zero." << std::endl;
      getchar();
    }

    if ((p2 < ads.zero) &&
        ((p2 - p1) >= 2))  // inside zeros (p1 < p2) (distance of 2)
    {
      // std::cout << "CASE 1" << std::endl;
      f -= svrpdsp->dc(rep[bp1], rep[p1]);
      // std::cout << "-d(" << rep[bp1] << "," << rep[p1] << ") ";
      f -= svrpdsp->dc(rep[p1], rep[ap1]);
      // std::cout << "-d(" << rep[p1] << "," << rep[ap1] << ") ";
      f -= svrpdsp->dc(rep[bp2], rep[p2]);
      // std::cout << "-d(" << rep[bp2] << "," << rep[p2] << ") ";
      f -= svrpdsp->dc(rep[p2], rep[ap2]);
      // std::cout << "-d(" << rep[p2] << "," << rep[ap2] << ") ";

      f += svrpdsp->dc(rep[bp1], rep[p2]);
      // std::cout << "+d(" << rep[bp1] << "," << rep[p2] << ") ";
      f += svrpdsp->dc(rep[p2], rep[ap1]);
      // std::cout << "+d(" << rep[p2] << "," << rep[ap1] << ") ";
      f += svrpdsp->dc(rep[bp2], rep[p1]);
      // std::cout << "+d(" << rep[bp2] << "," << rep[p1] << ") ";
      f += svrpdsp->dc(rep[p1], rep[ap2]);
      // std::cout << "+d(" << rep[p1] << "," << rep[ap2] << ") ";
    } else if ((p2 < ads.zero) &&
               ((p2 - p1) == 1))  // inside zeros (p1 < p2) (distance of 1)
    {
      // std::cout << "CASE 2" << std::endl;
      f -= svrpdsp->dc(rep[bp1], rep[p1]);
      // std::cout << "-d(" << rep[bp1] << "," << rep[p1] << ") ";
      f -= svrpdsp->dc(rep[p1], rep[p2]);
      // std::cout << "-d(" << rep[p1] << "," << rep[p2] << ") ";
      f -= svrpdsp->dc(rep[p2], rep[ap2]);
      // std::cout << "-d(" << rep[p2] << "," << rep[ap2] << ") ";

      f += svrpdsp->dc(rep[bp1], rep[p2]);
      // std::cout << "+d(" << rep[bp1] << "," << rep[p2] << ") ";
      f += svrpdsp->dc(rep[p2], rep[p1]);
      // std::cout << "+d(" << rep[p2] << "," << rep[p1] << ") ";
      f += svrpdsp->dc(rep[p1], rep[ap2]);
      // std::cout << "+d(" << rep[p1] << "," << rep[ap2] << ") ";
    } else if (p2 > ads.zero)  // after zero (swap pickup with pickup)
    {
      f -= svrpdsp->dc(rep[bp1], rep[p1]);
      // std::cout << "-d(" << rep[bp1] << "," << rep[p1] << ") ";
      f -= svrpdsp->dc(rep[p1], rep[ap1]);
      // std::cout << "-d(" << rep[p1] << "," << rep[ap1] << ") ";

      f += svrpdsp->dc(rep[bp1], rep[p2]);
      // std::cout << "+d(" << rep[bp1] << "," << rep[p2] << ") ";
      f += svrpdsp->dc(rep[p2], rep[ap1]);
      // std::cout << "+d(" << rep[p2] << "," << rep[ap1] << ") ";

      // updating revenue
      f += svrpdsp->dr[rep[p1]];
      f -= svrpdsp->dr[rep[p2]];
    } else {
      std::cout << "DELTA SWAP DON'T KNOW!!" << std::endl;
      exit(1);
    }

    // std::cout << std::endl;

    // ==========
    // Infeasible
    // ==========

    int f_inv = 0;

    if (verbose) std::cout << "INF: ";

    int nQ = ads.load[0];  // FULL Q
    for (int c = 0; c < ads.zero; c++) {
      if (verbose)
        std::cout << "{" << nQ << "->" << f_inv << "}\t"
             << "c:" << c << " ";

      if (c == p1) nQ += svrpdsp->dp[rep[p2]] - svrpdsp->dd[rep[p2]];

      if (c == p2) nQ += svrpdsp->dp[rep[p1]] - svrpdsp->dd[rep[p1]];

      if ((c != p1) && (c != p2))
        nQ += svrpdsp->dp[rep[c]] - svrpdsp->dd[rep[c]];

      f_inv += infv(nQ, ads.load[0]);
      if (verbose) std::cout << "add_" << rep[c] << " ";
    }

    if (verbose) std::cout << std::endl;

    long long f_inv2 = ((long long)f_inv) * 100000 - e.getInfMeasure();

    if (verbose) {
      std::cout << "COST() inf was: " << e.getInfMeasure() << " => "
           << (((long long)f_inv) * 100000) << std::endl;
      std::cout << "COST() FINAL: " << f << " and " << f_inv2 << std::endl;
    }

    return new pair<double, double>(f, f_inv2);
  }

  // infeasible value
  int infv(int nv, int Q) { return max((nv - Q), 0); }

  static std::string idComponent() {
    string idComp = super::idComponent();
    idComp.append(":SVRPDSPDeltaMoveSwap");
    return idComp;
  }

  virtual bool operator==(
      const Move<RepSVRPDSP, AdsSVRPDSP, MySolution>& _m) const {
    const DeltaMoveSwap& m1 = (const DeltaMoveSwap&)_m;
    return (m1.p1 == p1) && (m1.p2 == p2);
  }

  void print() const override {
    std::cout << "SVRPDSPDeltaMoveSwap(" << p1 << ";" << p2 << ")" << std::endl;
  }
};

}  // namespace SVRPDSP

#endif /*SVRPDSP_DELTA_MOVE_SWAP_HPP_*/
