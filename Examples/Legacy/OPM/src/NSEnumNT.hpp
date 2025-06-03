#ifndef OPM_NSENUMNT_HPP_
#define OPM_NSENUMNT_HPP_

// Framework includes
#include "../../OptFrame/NSEnum.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "Solution.h"

using namespace std;
namespace POLAD {

class MoveNT : public Move<RepOPM> {
 private:
  int i, j, inc_dec;
  OPMProblemInstance& opm;

 public:
  MoveNT(int _i, int _j, int _inc_dec, OPMProblemInstance& _opm)
      : i(_i), j(_j), inc_dec(_inc_dec), opm(_opm) {}

  virtual ~MoveNT() {}

  bool canBeApplied(const RepOPM& rep, const OPTFRAME_DEFAULT_ADS*) override {
    bool nao_negativo = !((rep.second(i, j) + inc_dec) < 0);
    return (nao_negativo && (rep.first.at(i) > -1) &&
            (opm.isCompatible(j, rep.first.at(i))));
  }

  Move<RepOPM>* apply(RepOPM& rep, OPTFRAME_DEFAULT_ADS*) override {
    rep.second(i, j) += inc_dec;

    if (rep.first[i] < 0) {
      std::cout << "Erro CARREGS!" << std::endl;
      this->print();
      exit(1);
    }

    if (rep.second(i, j) < 0) {
      std::cout << "Erro VIAGENS NEG!" << std::endl;
      this->print();
      exit(1);
    }

    if (!opm.isCompatible(j, rep.first[i])) {
      std::cout << "NT Erro Incompatibilidade!" << std::endl;
      this->print();
      exit(1);
    }

    return new MoveNT(i, j, -inc_dec, opm);
  }

  virtual bool operator==(const Move<RepOPM>& _m) const {
    const MoveNT& m = (const MoveNT&)_m;
    return false;
  }

  void print() const override {
    std::cout << "MoveNT(" << i << " , " << j << ") += " << inc_dec << "\n";
  }
};

class NSEnumNT : public NSEnum<RepOPM> {
 private:
  OPMProblemInstance& opm;

 public:
  NSEnumNT(OPMProblemInstance& _opm, RandGen& _rg)
      : NSEnum<RepOPM>(_rg), opm(_opm) {}

  virtual ~NSEnumNT() {}

  Move<RepOPM>* indexMove(unsigned int m) override {
    int num_i = opm.getNumFrentes();
    int num_j = opm.getNumCaminhoes();

    /* Implementação O(1) */

    if (m < num_i * num_j * 2) {
      int f = floor((double)(m / 2) / num_j);
      int c = ((int)floor((double)m / 2)) % num_j;
      int inc_dec = ((m % 2) == 0) ? -1 : 1;

      return new MoveNT(f, c, inc_dec, opm);
    } else {
      cerr << "Neighborhood NT Error! Move " << m
           << " doesnt exist! Valid Interval from 0 to " << (size() - 1) << "."
           << std::endl;
      exit(1);
    }
  }

  unsigned int size() const override {
    return opm.getNumFrentes() * opm.getNumCaminhoes() * 2;
  }

  void print() { std::cout << "NSEnum NT (" << size() << ")" << std::endl; }
};

}  // namespace POLAD
#endif /*OPM_NSENUMNT_HPP_*/
