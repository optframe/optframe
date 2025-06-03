#ifndef OPM_NSENUMTT_HPP_
#define OPM_NSENUMTT_HPP_

// Framework includes
#include "../../OptFrame/NSEnum.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "Solution.h"

using namespace std;
namespace POLAD {

class MoveTT : public Move<RepOPM> {
 private:
  int cam, f1, f2;
  OPMProblemInstance& opm;

 public:
  MoveTT(int _f1, int _f2, int _cam, OPMProblemInstance& _opm)
      : f1(_f1), f2(_f2), cam(_cam), opm(_opm) {}

  virtual ~MoveTT() {}

  bool canBeApplied(const RepOPM& rep, const OPTFRAME_DEFAULT_ADS*) override {
    return ((f1 != f2) && (rep.second(f1, cam) > 0) &&
            (rep.first.at(f2) > -1) &&
            (opm.isCompatible(cam, rep.first.at(f2))));
  }

  Move<RepOPM>* apply(RepOPM& rep, OPTFRAME_DEFAULT_ADS*) override {
    rep.second(f1, cam)--;
    rep.second(f2, cam)++;

    if (!opm.isCompatible(cam, rep.first[f2])) {
      std::cout << "TT Erro Incompatibilidade!" << std::endl;
      this->print();
    }

    return new MoveTT(f2, f1, cam, opm);
  }

  virtual bool operator==(const Move<RepOPM>& _m) const {
    const MoveTT& m = (const MoveTT&)_m;
    return (cam == m.cam) && (f1 == m.f1) && (f2 == m.f2);
  }

  void print() const override {
    std::cout << "MoveTT(from " << f1 << " to " << f2 << " of truck " << cam << ")"
         << std::endl;
  }
};

class NSEnumTT : public NSEnum<RepOPM> {
 private:
  OPMProblemInstance& opm;

 public:
  NSEnumTT(OPMProblemInstance& _opm, RandGen& _rg)
      : NSEnum<RepOPM>(_rg), opm(_opm) {}

  virtual ~NSEnumTT() {}

  Move<RepOPM>* indexMove(unsigned int k) override {
    if (k > size()) {
      cerr << "Neighborhood TT Error! Move " << k
           << " doesnt exist! Valid Interval from 0 to " << (size() - 1) << "."
           << std::endl;
      exit(1);
    }

    int nf = opm.getNumFrentes();

    int f1, f2, cam, nf2;
    nf2 = nf * nf;
    cam = k / nf2;
    f2 = k % nf2 / nf;
    f1 = k % nf;

    return new MoveTT(f1, f2, cam, opm);
  }

  unsigned int size() const {
    return opm.getNumFrentes() * opm.getNumFrentes() * opm.getNumCaminhoes();
  }

  void print() { std::cout << "NSEnum TT (" << size() << ")\n"; }
};
}  // namespace POLAD
#endif /*OPM_NSENUMTT_HPP_*/
