#ifndef SVRPDSP_SOLUTION_H_
#define SVRPDSP_SOLUTION_H_

#include "../../OptFrame/Solution.hpp"
#include "Representation.h"
#include "ADS.h"

#include "ProblemInstance.hpp"

#include "cgtype.h"

using namespace optframe;

namespace SVRPDSP
{
////static int mysolution_count = 0;

class MySolution : public Solution<RepSVRPDSP, AdsSVRPDSP>
{
private:
   static const bool verbose = false;

public:
   MySolution(RepSVRPDSP& r, AdsSVRPDSP& ads):
      Solution<RepSVRPDSP, AdsSVRPDSP>(r, ads)
   {
/*
       mysolution_count++;
       if(mysolution_count>1)
       {
           cout << "Exceeded 1 solutions = " << mysolution_count << endl;
           //getchar();
       }
       if(mysolution_count>20)
       {
           cout << "Exceeded 20 solutions!" << endl;
           exit(1);
       }
*/
   }

   explicit MySolution(RepSVRPDSP& r):
      Solution<RepSVRPDSP, AdsSVRPDSP>(r)
   {
/*
       mysolution_count++;
       if(mysolution_count>1)
       {
           cout << "Exceeded 1 solutions = " << mysolution_count << endl;
           //getchar();
       }
       if(mysolution_count>20)
       {
           cout << "Exceeded 20 solutions!" << endl;
           exit(1);
       }
*/
   }

   MySolution(const MySolution& sol):
      Solution<RepSVRPDSP, AdsSVRPDSP>(sol)
   {
/*
       mysolution_count++;
       if(mysolution_count>1)
       {
           cout << "Exceeded 1 solutions = " << mysolution_count << endl;
           //getchar();
       }
       if(mysolution_count>20)
       {
           cout << "Exceeded 20 solutions!" << endl;
           exit(1);
       }
*/

      //cout << "MyS: copy constructor" << endl;

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

   virtual ~MySolution()
   {
       ////mysolution_count--;
   }

   // syncronize ADS with R and check for errors
   bool syncADS(ProblemInstance& p)
   {
       return syncADS(p, getR(), getADS());
   }

   // syncronize ADS with R and check for errors
   static bool syncADS(ProblemInstance& p, const RepSVRPDSP& rep, AdsSVRPDSP& ads)
   {

      if(verbose)
      {
         cout << "MyS: syncADS" << endl;
         cout << "rep: " << rep << endl;
      }

      int N = p.N;

      int dn = p.dn;
      int n = p.n;

      if(rep.size() != (dn+2))
      {
         cout << "syncADS: missing numbers! it's " << rep.size() << " and should be " << (dn+2) << endl;
         return false;
      }

      if(rep.at(0) != 0) // no starting depot
      {
         cout << "syncADS: missing initial depot!" << endl;
         return false;
      }

      ads.load = vector<small_type>(rep.size(), 0);

      int* s = new int[rep.size()]; 
      small_type* q = new small_type[rep.size()];

      for(unsigned i=0; i<rep.size(); i++)
      {
         if(rep[i]==0)
         {
            ads.zero = i;
            if(verbose)
               cout << "new zero on " << i << endl;
         }

         s[i] = rep[i];
      }

      ads.load[0] = p.Q;
      if(verbose)
         cout << "load: ";
      for(unsigned i=1; i<ads.zero; i++)
      {
         ads.load[i] = ads.load[i-1] + p.dp.at(rep[i]) - p.dd.at(rep[i]);
         if(verbose)
            cout << i << "(" << rep[i] << "): +=" << (p.dp.at(rep[i]) - p.dd.at(rep[i])) << "=" << ads.load[i] << " ";
      }
      if(verbose)
         cout << endl;

      for(unsigned i=0; i<ads.load.size(); i++)
         q[i] = ads.load[i];

#ifdef ENABLE_GPU
      int* ds; // gpu solution
      small_type* dq; // gpu load vector

      if(!ads.gpuSol || !ads.gpuLoad)
      {
         initialize_gpu_solution(&ds, &dq, rep.size()); // max_size
         if(verbose)
            cout << "NEW GPU ADS! (" << ds << "," << dq << ")" << endl;
      }
      else
      {
         ds = ads.gpuSol;
         dq = ads.gpuLoad;
      }
      update_gpu_solution(rep.size(), s, q, ds, dq);

      ads.gpuSol  = ds;
      ads.gpuLoad = dq;

      if(verbose)
      {
         get_gpu_solution(rep.size(), ds, dq, s, q);
         for(unsigned i=0; i<rep.size(); i++)
            if(q[i] != ads.load[i])
            {
               cout << "GPU ERROR COPY LOAD: " << i << endl;
               exit(1);
            }
         cout << "GPU COPY OK!" << endl;
      }
#endif

      delete[] s;
      delete[] q;

      if(verbose)
         ads.print();

      return true;
   }

   void print() const
   {
      cout << "SVRPDSP Solution => ";
      Solution<RepSVRPDSP, AdsSVRPDSP>::print();
      //cout << "ADS: ";
      //getADS().print();
   }

   MySolution& operator= (const MySolution& s)
   {
      //cout << "MyS: MyS = MyS" << endl;
      if(&s == this) // auto ref check
         return *this;

      delete r;
      delete ads;
      r   = new RepSVRPDSP(s.getR());
      ads = new AdsSVRPDSP(s.getADS());

      return *this;
   }

   Solution<RepSVRPDSP, AdsSVRPDSP>& operator=(const Solution<RepSVRPDSP, AdsSVRPDSP>& s)
   {
      //cout << "MyS: S=S" << endl;
      if(&s == this) // auto ref check
         return *this;

      delete r;
      delete ads;
      r   = new RepSVRPDSP(s.getR());
      ads = new AdsSVRPDSP(s.getADS());

      return *this;
   }

   Solution<RepSVRPDSP, AdsSVRPDSP>& clone() const
   {
      //cout << "MyS: clone()" << endl;
      Solution<RepSVRPDSP, AdsSVRPDSP>* s = new MySolution(*this);
      return (*s);
   }

};

}

#endif /*SVRPDSP_SOLUTION_H_*/

