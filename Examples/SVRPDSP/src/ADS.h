#ifndef SVRPDSP_ADS_H_
#define SVRPDSP_ADS_H_

//#include <ostream>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>
#include <vector>

#include "cgtype.h"

using namespace std;

struct AdsSVRPDSP {
  AdsSVRPDSP() {
    gpuSol = 0;
    gpuLoad = 0;
    zero = 0;
  }

  virtual ~AdsSVRPDSP() {}

  AdsSVRPDSP(const AdsSVRPDSP& ads) {
    // std::cout << "ADS::()" << std::endl;
    zero = ads.zero;
    load = ads.load;
  }

  vector<small_type> load;

  unsigned zero;
  int* gpuSol;
  small_type* gpuLoad;

  void print() const {
    std::cout << "ADS gpuPtr(" << gpuSol << "," << gpuLoad << ") zero=" << zero
         << " load: " << load << std::endl;

    if (gpuSol && gpuLoad) {
      int* s = new int[load.size()];
      small_type* q = new small_type[load.size()];

#ifdef ENABLE_GPU
      get_gpu_solution(load.size(), gpuSol, gpuLoad, s, q);
      std::cout << "gpuSol: ";
      printv(s, load.size());
      std::cout << "gpuLoad: ";
      printv2(q, load.size());

      for (unsigned i = 0; i <= zero; i++)
        if (load[i] != q[i]) {
          std::cout << "ERROR IN GPU ADS AT POS " << i << " CPU is " << load[i]
               << " and GPU is " << q[i] << std::endl;
          exit(1);
        }
#endif

      delete[] s;
      delete[] q;
    }
  }

  AdsSVRPDSP& operator=(const AdsSVRPDSP& ads) {
    std::cout << "ADS::op=" << std::endl;
    exit(1);
    // std::cout << "MyS: S=S" << std::endl;
    if (&ads == this)  // auto ref check
      return *this;
    /*
    delete r;
    delete ads;
    r   = new RepSVRPDSP(s.getR());
    ads = new AdsSVRPDSP(s.getADS());
*/
    return *this;
  }
};

// ostream& operator<< (ostream & s, AdsSVRPDSP){}

#endif /*SVRPDSP_ADS_H_*/
