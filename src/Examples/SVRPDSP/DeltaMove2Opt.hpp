#ifndef SVRPDSP_DELTA_MOVE_2OPT_HPP_
#define SVRPDSP_DELTA_MOVE_2OPT_HPP_

#include "../../OptFrame/Util/NeighborhoodStructures/Moves/MoveTSP2Opt.hpp"

#include <cmath>

#include "MySolution.hpp"

#include "Delta.h"

using namespace std;

namespace SVRPDSP
{

class DeltaMove2Opt: public MoveTSP2Opt<int, AdsSVRPDSP >
{
	typedef MoveTSP2Opt<int, AdsSVRPDSP> super;

private:
	ProblemInstance* svrpdsp;

   static bool verbose;

public:
	using super::apply;
	using super::canBeApplied;

   DeltaMove2Opt(const super& s, ProblemInstance* _svrpdsp) :
      super(s.get_p1(), s.get_p2(), _svrpdsp), svrpdsp(_svrpdsp)
   {
      if(!svrpdsp)
      {
         cout << "Error: svrpdsp problem is NULL!" << endl;
         print();
         exit(1);
      }
   }

	DeltaMove2Opt(int p1, int p2, ProblemInstance* _svrpdsp) :
		 super(p1, p2, _svrpdsp), svrpdsp(_svrpdsp)
	{
		if(!svrpdsp)
		{
			cout << "Error: svrpdsp problem is NULL!" << endl;
			print();
			exit(1);
		}
	}

	virtual ~DeltaMove2Opt()
	{
	}

   virtual bool canBeApplied(const RepSVRPDSP& rep, const AdsSVRPDSP& ads)
   {
      if(!super::canBeApplied(rep, ads))
         return false;

      int i = p1;
      int j = p2-1;

      // cannot be neighbors
      if ((i == j) || (i == (j-1)))
         return false;

      // do not 2opt zeros!
      if((i == 0) || (i == ads.zero) || (j == 0) || (j == ads.zero))
         return false;

      // do not 2opt after zero
      if(j >= ads.zero)
         return false;

      // p1 always smaller than p2
      if(i >= j)
         return false;

      return true;
   }


   virtual Move<RepSVRPDSP, AdsSVRPDSP>* apply(RepSVRPDSP& rep, AdsSVRPDSP& ads)
   {
      // apply move
      Move<RepSVRPDSP, AdsSVRPDSP>& rev = *super::apply(rep, ads);
      MySolution::syncADS(*svrpdsp, rep, ads); // syncronize ADS

      super& sRev = (super&)rev;
      DeltaMove2Opt* myMove = new DeltaMove2Opt(sRev, svrpdsp);
      delete& sRev;

      return myMove;
   }

	MoveCost* cost(const Evaluation& e, const RepSVRPDSP& rep, const AdsSVRPDSP& ads)
	{
		return NULL;
	}


   pair<double, double>* cost1(const Evaluation& e, const RepSVRPDSP& rep, const AdsSVRPDSP& ads)
   {
/*
      if((p1==5) && (p2==20))
      {
         verbose = true;
         //getchar();
      }
*/

      if(verbose)
      {
         //cout << "Cost() of rep: " << rep << endl;
         //ads.print();
      }

      // always p1 < p2

      p2--;

      unsigned bp1 = p1 - 1; // before p1
      unsigned ap1 = p1 + 1; // after p1

      unsigned bp2 = p2 - 1; // before p2
      unsigned ap2 = p2 + 1; // after p2

      long long f = 0;

      if(p1 == p2)
      {
         f = 0;
         cout << "2OPT SHOULDN'T BE APPLIED! p1==p2" << endl;
         getchar();
      }

      if((p1 > ads.zero) && (p2 > ads.zero)) // all zero!
      {
         f = 0;
         cout << "2OPT SHOULDN'T BE APPLIED! both after zero." << endl;
         getchar();
      }

      if(p2 < ads.zero) // inside zeros (p1 < p2) (distance of 2)
      {
         //cout << "CASE 1" << endl;
         f -= svrpdsp->dc(rep[bp1], rep[p1]);
         if(verbose) cout << "-d(" << rep[bp1] << "," << rep[p1] << ") ";
         f -= svrpdsp->dc(rep[p2], rep[ap2]);
         if(verbose) cout << "-d(" << rep[p2] << "," << rep[ap2] << ") ";

         f += svrpdsp->dc(rep[bp1], rep[p2]);
         if(verbose) cout << "+d(" << rep[bp1] << "," << rep[p2] << ") ";
         f += svrpdsp->dc(rep[p1], rep[ap2]);
         if(verbose) cout << "+d(" << rep[p1] << "," << rep[ap2] << ") ";
      }
      else
      {
         cout << "DELTA 2OPT DON'T KNOW!!" << endl;
         exit(1);
      }

      //cout << endl;
      if(verbose)
          cout << "f is: " << f << endl;

      // ==========
      // Infeasible
      // ==========

      long long f_inv = 0;

      if(verbose)
         cout << endl << "INF C++:";

      int nQ = svrpdsp->Q; // FULL Q
      for(int c = 0; c < ads.zero; c++)
      {
         if(verbose)
            cout <<"{" << nQ << "->" << f_inv <<"}\t" << "c:" << c << " ";

         if((c>=p1) && (c<=p2))
         {
            int diff = p2 - p1 + 1;
            int nc = diff - (c-p1) + p1-1;
            nQ += svrpdsp->dp[rep[nc]] - svrpdsp->dd[rep[nc]];
            f_inv += infv(nQ, ads.load[0]);
            if(verbose)
               cout << "add_" << rep[c] << "_rev_" << rep[nc] << " ";
         }

         if((c<p1) || (c>p2))
         {
            nQ += svrpdsp->dp[rep[c]] - svrpdsp->dd[rep[c]];
            f_inv += infv(nQ, ads.load[0]);
            if(verbose)
               cout << "add_" << rep[c] << " ";
         }

      }

      if(verbose)
         cout << endl;

      if(verbose)
          cout << "finv is: " << f_inv << endl;

      long long f_inv2 = ((long long)f_inv)*100000 - e.getInfMeasure();

      if(verbose)
      {
          cout << "finv2 is: " << f_inv << "*100000 - " << e.getInfMeasure() << endl;
          cout << "finv2 is: " << f_inv2 << endl;
          getchar();
      }


      if(verbose)
      {
         printf("ads.zero=%d\n", ads.zero);
         printf("COST() objective func: was %.2f and became %lld\n", e.getObjFunction(), ((long long)e.getObjFunction()+f) );
         printf("COST() infeasibility: was %.2f and became %lld\n", e.getInfMeasure(), (((long long)f_inv)*100000) );
         printf("COST() FINAL: %lld + %lld = %lld\n", f, f_inv2, (f+f_inv2));
      }

      p2++;

      verbose = false;

/*
      if((p1==30) && (p2==33))
      {
         verbose = false;
         //getchar();
      }
*/

      return new pair<double, double>(f, f_inv2);
   }

   // infeasible value
   int infv(int nv, int Q)
   {
      return max((nv - Q), 0);
   }

	static string idComponent()
	{
		string idComp = super::idComponent();
		idComp.append(":DeltaMove2Opt");
		return idComp;
	}

	virtual bool operator==(const Move<RepSVRPDSP, AdsSVRPDSP>& _m) const
	{
		const DeltaMove2Opt& m1 = (const DeltaMove2Opt&) _m;
		return ((m1.p1 == p1) && (m1.p2 == p2));
	}

	virtual void print() const
	{
		cout << "DeltaMove2Opt( ";
		cout << " edge " << p1 << " <=>  edge " << p2 << " )";
		cout << endl;
	}
};

bool DeltaMove2Opt::verbose = false;

}

#endif /*SVRPDSP_DELTA_MOVE_2OPT_HPP_*/
