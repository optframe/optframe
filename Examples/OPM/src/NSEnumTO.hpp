#ifndef OPM_NSENUMTO_HPP_
#define OPM_NSENUMTO_HPP_

// Framework includes
#include "../../OptFrame/NSEnum.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "Solution.h"

using namespace std;

namespace POLAD {

class MoveTO : public Move<RepOPM> {
 private:
  int f, c;
  int valor_alvo;

  MoveTO(int _f, int _c, int _valor_alvo)
      : f(_f), c(_c), valor_alvo(_valor_alvo) {}

 public:
  MoveTO(int _f, int _c) : f(_f), c(_c) { valor_alvo = 0; }

  virtual ~MoveTO() {}

  bool canBeApplied(const RepOPM& rep, const OPTFRAME_DEFAULT_ADS*) override {
    return true;
  }

  Move<RepOPM>* apply(RepOPM& rep, OPTFRAME_DEFAULT_ADS*) override {
    int valor_ant = rep.second(f, c);

    rep.second(f, c) = valor_alvo;

    return new MoveTO(f, c, valor_ant);
  }

  virtual bool operator==(const Move<RepOPM>& _m) const {
    const MoveTO& m = (const MoveTO&)_m;
    return (f == m.f) && (c == m.c);
  }

  void print() const override {
    std::cout << "MoveTO(" << f << "," << c << ")" << std::endl;
  }
};

class NSEnumTO : public NSEnum<RepOPM> {
 private:
  OPMProblemInstance& opm;

 public:
  NSEnumTO(OPMProblemInstance& _opm, RandGen& _rg)
      : NSEnum<RepOPM>(_rg), opm(_opm) {}

  virtual ~NSEnumTO() {}

  Move<RepOPM>* indexMove(unsigned int k) override {
    if (k > size()) {
      cerr << "Neighborhood TO Error! Move " << k
           << " doesnt exist! Valid Interval from 0 to " << (size() - 1) << "."
           << std::endl;
      exit(1);
    }

    int nc = opm.getNumCaminhoes();

    int f = k / nc;
    int c = k % nc;

    return new MoveTO(f, c);
  }

  unsigned int size() const override {
    return opm.getNumFrentes() * opm.getNumCaminhoes();
  }

  void print() { std::cout << "NSEnum TO (" << size() << ")\n"; }
};

}  // namespace POLAD
#endif /*OPM_NSENUMTO_HPP_*/
