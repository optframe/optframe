#ifndef SVRPDSP_SOLUTION_H_
#define SVRPDSP_SOLUTION_H_

#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

#include "ADS.h"
#include "ProblemInstance.hpp"
#include "Representation.h"
#include "cgtype.h"

using namespace optframe;

namespace SVRPDSP {
////static int mysolution_count = 0;

class MySolution : public CopySolution<RepSVRPDSP, AdsSVRPDSP> {
 private:
  static const bool verbose = false;

 public:
  MySolution(RepSVRPDSP& r, AdsSVRPDSP& ads)
      : CopySolution<RepSVRPDSP, AdsSVRPDSP>(r, ads) {
    /*
       mysolution_count++;
       if(mysolution_count>1)
       {
           std::cout << "Exceeded 1 solutions = " << mysolution_count << std::endl;
           //getchar();
       }
       if(mysolution_count>20)
       {
           std::cout << "Exceeded 20 solutions!" << std::endl;
           exit(1);
       }
*/
  }

  explicit MySolution(RepSVRPDSP& r) : CopySolution<RepSVRPDSP, AdsSVRPDSP>(r) {
    /*
       mysolution_count++;
       if(mysolution_count>1)
       {
           std::cout << "Exceeded 1 solutions = " << mysolution_count << std::endl;
           //getchar();
       }
       if(mysolution_count>20)
       {
           std::cout << "Exceeded 20 solutions!" << std::endl;
           exit(1);
       }
*/
  }

  MySolution(const MySolution& sol)
      : CopySolution<RepSVRPDSP, AdsSVRPDSP>(sol) {
    /*
       mysolution_count++;
       if(mysolution_count>1)
       {
           std::cout << "Exceeded 1 solutions = " << mysolution_count << std::endl;
           //getchar();
       }
       if(mysolution_count>20)
       {
           std::cout << "Exceeded 20 solutions!" << std::endl;
           exit(1);
       }
*/

    // std::cout << "MyS: copy constructor" << std::endl;

    /*
      int* s = new int[getR().size()];
      small_type* q = new small_type[getR().size()];

      for(unsigned i=0; i<getR().size(); i++)
      {
         s[i] = getR()[i];
         q[i] = getADS().load[i];
      }

#ifdef ENABLE_GPU
      int* ds;
      small_type* dq;

      initialize_gpu_solution(&ds, &dq, getR().size()); // max_size
      update_gpu_solution(getR().size(), s, q, ds, dq);

      getADS().gpuSol  = ds;
      getADS().gpuLoad = dq;
#endif

      delete[] s;
      delete[] q;
*/
  }

  virtual ~MySolution() {
    ////mysolution_count--;
  }

  // syncronize ADS with R and check for errors
  bool syncADS(ProblemInstance& p) { return syncADS(p, getR(), getADS()); }

  // syncronize ADS with R and check for errors
  static bool syncADS(ProblemInstance& p, const RepSVRPDSP& rep,
                      AdsSVRPDSP& ads) {
    if (verbose) {
      std::cout << "MyS: syncADS" << std::endl;
      std::cout << "rep: " << rep << std::endl;
    }

    int N = p.N;

    int dn = p.dn;
    int n = p.n;

    if (rep.size() != (dn + 2)) {
      std::cout << "syncADS: missing numbers! it's " << rep.size()
           << " and should be " << (dn + 2) << std::endl;
      return false;
    }

    if (rep.at(0) != 0)  // no starting depot
    {
      std::cout << "syncADS: missing initial depot!" << std::endl;
      return false;
    }

    ads.load = vector<small_type>(rep.size(), 0);

    int* s = new int[rep.size()];
    small_type* q = new small_type[rep.size()];

    for (unsigned i = 0; i < rep.size(); i++) {
      if (rep[i] == 0) {
        ads.zero = i;
        if (verbose) std::cout << "new zero on " << i << std::endl;
      }

      s[i] = rep[i];
    }

    ads.load[0] = p.Q;
    if (verbose) std::cout << "load: ";
    for (unsigned i = 1; i < ads.zero; i++) {
      ads.load[i] = ads.load[i - 1] + p.dp.at(rep[i]) - p.dd.at(rep[i]);
      if (verbose)
        std::cout << i << "(" << rep[i]
             << "): +=" << (p.dp.at(rep[i]) - p.dd.at(rep[i])) << "="
             << ads.load[i] << " ";
    }
    if (verbose) std::cout << std::endl;

    for (unsigned i = 0; i < ads.load.size(); i++) q[i] = ads.load[i];

#ifdef ENABLE_GPU
    int* ds;         // gpu solution
    small_type* dq;  // gpu load vector

    if (!ads.gpuSol || !ads.gpuLoad) {
      initialize_gpu_solution(&ds, &dq, rep.size());  // max_size
      if (verbose) std::cout << "NEW GPU ADS! (" << ds << "," << dq << ")" << std::endl;
    } else {
      ds = ads.gpuSol;
      dq = ads.gpuLoad;
    }
    update_gpu_solution(rep.size(), s, q, ds, dq);

    ads.gpuSol = ds;
    ads.gpuLoad = dq;

    if (verbose) {
      get_gpu_solution(rep.size(), ds, dq, s, q);
      for (unsigned i = 0; i < rep.size(); i++)
        if (q[i] != ads.load[i]) {
          std::cout << "GPU ERROR COPY LOAD: " << i << std::endl;
          exit(1);
        }
      std::cout << "GPU COPY OK!" << std::endl;
    }
#endif

    delete[] s;
    delete[] q;

    if (verbose) ads.print();

    return true;
  }

  void print() const override {
    std::cout << "SVRPDSP Solution => ";
    CopySolution<RepSVRPDSP, AdsSVRPDSP>::print();
    // std::cout << "ADS: ";
    // getADS().print();
  }

  MySolution& operator=(const MySolution& s) {
    // std::cout << "MyS: MyS = MyS" << std::endl;
    if (&s == this)  // auto ref check
      return *this;

    delete r;
    delete ads;
    r = new RepSVRPDSP(s.getR());
    ads = new AdsSVRPDSP(s.getADS());

    return *this;
  }

  CopySolution<RepSVRPDSP, AdsSVRPDSP>& operator=(
      const CopySolution<RepSVRPDSP, AdsSVRPDSP>& s) {
    // std::cout << "MyS: S=S" << std::endl;
    if (&s == this)  // auto ref check
      return *this;

    delete r;
    delete ads;
    r = new RepSVRPDSP(s.getR());
    ads = new AdsSVRPDSP(s.getADS());

    return *this;
  }

  CopySolution<RepSVRPDSP, AdsSVRPDSP>& clone() const {
    // std::cout << "MyS: clone()" << std::endl;
    CopySolution<RepSVRPDSP, AdsSVRPDSP>* s = new MySolution(*this);
    return (*s);
  }
};

}  // namespace SVRPDSP

#endif /*SVRPDSP_SOLUTION_H_*/
