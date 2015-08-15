#ifndef SVRPDSP_DELTA_MOVE_OROPTK_HPP_
#define SVRPDSP_DELTA_MOVE_OROPTK_HPP_

#include "../../OptFrame/Util/NeighborhoodStructures/Moves/MoveTSPOrOptk.hpp"

#include <cmath>

#include "MySolution.hpp"

#include "Delta.h"

using namespace std;

namespace SVRPDSP
{

class DeltaMoveOrOptk: public MoveTSPOrOptk<int, AdsSVRPDSP>
{
	typedef MoveTSPOrOptk<int, AdsSVRPDSP> super;

private:
    ProblemInstance* svrpdsp;

    static bool verbose;

public:

	using super::apply;
	using super::canBeApplied;

   DeltaMoveOrOptk(const super& s, ProblemInstance* _svrpdsp) :
      super(s.get_i(), s.get_j(), s.get_k(), _svrpdsp), svrpdsp(_svrpdsp)
   {
      if(!svrpdsp)
      {
         cout << "Error: svrpdsp problem is NULL!" << endl;
         print();
         exit(1);
      }
   }

	DeltaMoveOrOptk(int i, int j, int k, ProblemInstance* _svrpdsp) :
		 super(i, j, k, _svrpdsp), svrpdsp(_svrpdsp)
	{
		if(!svrpdsp)
		{
			cout << "Error: svrpdsp problem is NULL!" << endl;
			print();
			exit(1);
		}
	}

	virtual ~DeltaMoveOrOptk()
	{
	}

	int myabs(int x)
	{
		return std::abs(x);
	}


   virtual bool canBeApplied(const RepSVRPDSP& rep, const AdsSVRPDSP& ads)
   {
/*
      if((i==29) && (j==31))
         cout << "canBeApplied?i="<<i<<"j="<<j<<endl;
*/
      if(!super::canBeApplied(rep, ads))
         return false;

      // do not move last zero!
      if((i <= ads.zero) && ((i+k-1) >= ads.zero))
         return false;

      // do not move first zero!
      if((i == 0) || (j==0))
         return false;

      if((i > (ads.zero-k)) && (j > (ads.zero-k))) // all zero!
         return false;

      // do not move delivery customers to after zero!
      if((i < j) && (j > (ads.zero-k)))
         for(int ci=i; ci<=(i+k-1); ci++)
            if(svrpdsp->isDelivery(rep[ci]))
               return false;

/*
      if((i==29) && (j==31))
         cout << "canBeApplied=TRUE"<<endl;
*/

      return true;
   }


   virtual Move<RepSVRPDSP, AdsSVRPDSP>* apply(RepSVRPDSP& rep, AdsSVRPDSP& ads)
   {
      // apply move
      Move<RepSVRPDSP, AdsSVRPDSP>& rev = *super::apply(rep, ads);
      MySolution::syncADS(*svrpdsp, rep, ads); // syncronize ADS

      super& sRev = (super&)rev;
      DeltaMoveOrOptk* myMove = new DeltaMoveOrOptk(sRev, svrpdsp);
      delete& sRev;

      return myMove;
   }


	MoveCost* cost(const Evaluation& e, const RepSVRPDSP& rep, const AdsSVRPDSP& ads)
	{
		return NULL;
	}


    pair<double, double>* cost1(const Evaluation& e, const RepSVRPDSP& rep, const AdsSVRPDSP& ads)
    {
        if(verbose)
            ads.print();

/*
        if((i==5) && (j==30))
        {
            verbose = true;
            cout << "DEBUGGING MoveOrOpt(" << k << ") i=" <<i << " j=" << j << endl;
        //getchar();
        }
*/



      unsigned bi = i - 1; // before i

      unsigned bj = j - 1; // before j 
      unsigned aj = j + 1; // after j

      unsigned nj = j + k -1; // (ajusted j)

      unsigned bnj = nj - 1; // before j (ajusted j)
      unsigned anj = nj + 1; // after j  (ajusted j)

      // last i+k
      unsigned lik = i + k - 1;

      // after i+k
      unsigned aik = i + k;

      int f = 0;

      if(i == j)
      {
         f = 0;
         cout << "OROPTK SHOULDN'T BE APPLIED! I==J" << endl;
         getchar();
      }

        if((i > (ads.zero-k)) && (j > (ads.zero-k))) // all zero!
        {
         f = 0;
         cout << "OROPTK SHOULDN'T BE APPLIED!" << endl;
         getchar();
        }
        else if((i < j) && (i <= (ads.zero-k)) && (j <= (ads.zero-k))) // inside zeros (I < J)
        {
            if(verbose)
                cout << "CASE 1" << endl;
         f -= svrpdsp->dc(rep[bi], rep[i]);
         //cout << "-d(" << rep[bi] << "," << rep[i] << ") ";
         f -= svrpdsp->dc(rep[lik], rep[aik]);
         //cout << "-d(" << rep[lik] << "," << rep[aik] << ") ";
         f -= svrpdsp->dc(rep[nj], rep[anj]);
         //cout << "-d(" << rep[nj] << "," << rep[aj] << ") ";

         f += svrpdsp->dc(rep[bi], rep[aik]);
         //cout << "+d(" << rep[bi] << "," << rep[aik] << ") ";
         f += svrpdsp->dc(rep[nj], rep[i]);
         //cout << "+d(" << rep[j] << "," << rep[i] << ") ";
         f += svrpdsp->dc(rep[lik], rep[anj]);
         //cout << "+d(" << rep[lik] << "," << rep[aj] << ") ";
        }
        else if((i > j) && (i <= (ads.zero-k)) && (j <= (ads.zero-k))) // inside zeros (I > J)
        {
            if(verbose)
                cout << "CASE 2" << endl;
            f -= svrpdsp->dc(rep[bi], rep[i]);
            if(verbose)
                cout << "-d(" << rep[bi] << "," << rep[i] << ")=" << -svrpdsp->dc(rep[bi], rep[i]) << " ";
            f -= svrpdsp->dc(rep[lik], rep[aik]);
            if(verbose)
                cout << "-d(" << rep[lik] << "," << rep[aik] << ")=" << -svrpdsp->dc(rep[lik], rep[aik]) << " ";
            f -= svrpdsp->dc(rep[bj], rep[j]);
            if(verbose)
                cout << "-d(" << rep[bj] << "," << rep[j] << ")=" << -svrpdsp->dc(rep[bj], rep[j]) << " ";

            f += svrpdsp->dc(rep[bi], rep[aik]);
            if(verbose)
                cout << "+d(" << rep[bi] << "," << rep[aik] << ")=" << svrpdsp->dc(rep[bi], rep[aik]) << " ";
            f += svrpdsp->dc(rep[bj], rep[i]);
            if(verbose)
                cout << "+d(" << rep[bj] << "," << rep[i] << ")=" << svrpdsp->dc(rep[bj], rep[i]) << " ";
            f += svrpdsp->dc(rep[lik], rep[j]);
            if(verbose)
                cout << "+d(" << rep[lik] << "," << rep[j] << ")=" << svrpdsp->dc(rep[lik], rep[j]) << " ";
        }
        else if((i < j) && (j > (ads.zero-k))) // after zeros REMOVING PICKUP
        {
         //cout << "AFTER ZERO REMOVING PICKUP!" << endl;
            if(verbose)
                cout << "CASE 3" << endl;
            f -= svrpdsp->dc(rep[bi], rep[i]);
            if(verbose)
                cout << "-d(" << rep[bi] << "," << rep[i] << ") ";
            f -= svrpdsp->dc(rep[lik], rep[aik]);
            if(verbose)
                cout << "-d(" << rep[lik] << "," << rep[aik] << ") ";

            f += svrpdsp->dc(rep[bi], rep[aik]);
            if(verbose)
                cout << "+d(" << rep[bi] << "," << rep[aik] << ") ";

            // REMOVING PICKUP
            for(int ci=i; ci<=(i+k-1); ci++)
            {
                f += svrpdsp->dr[rep[ci]]; // revenue is reverse!
                if(verbose)
                    cout << "+r(" << rep[ci] << ")=" << svrpdsp->dr[rep[ci]] << " ";

                if((ci+1) <= (i+k-1))
                {
                    f -= svrpdsp->dc(rep[ci], rep[ci+1]);
                    if(verbose)
                        cout << "f-=d(" << rep[ci] << "," << rep[ci+1] << ") ";
                }
            }
        }
        else if((i > j) && (i > (ads.zero-k))) // after zeros ADDING PICKUP
        {
         //cout << "After zero!************" << endl;
            if(verbose)
                cout << "CASE 4" << endl;
            f -= svrpdsp->dc(rep[bj], rep[j]);
            if(verbose)
                cout << "-d(" << rep[bj] << "," << rep[j] << ")=" << -svrpdsp->dc(rep[bj], rep[j])<< " ";

            f += svrpdsp->dc(rep[bj], rep[i]);
            if(verbose)
                cout << "+d(" << rep[bj] << "," << rep[i] << ")=" << svrpdsp->dc(rep[bj], rep[i]) << " ";
            f += svrpdsp->dc(rep[lik], rep[j]);
            if(verbose)
                cout << "+d(" << rep[lik] << "," << rep[j] << ")=" << svrpdsp->dc(rep[lik], rep[j]) << " ";

            //cout << "DDIST=" << f << endl;

            //cout << "ADDING PICKUP" << endl;
            for(int ci=i; ci<=(i+k-1); ci++)
            {
                //cout << rep[ci] << endl;
                f -= svrpdsp->dr[rep[ci]]; // revenue is reverse!
                if(verbose)
                    cout << "-r(" << rep[ci] << ")=" << -svrpdsp->dr[rep[ci]] << " ";
                if((ci+1) <= (i+k-1))
                {
                    f += svrpdsp->dc(rep[ci], rep[ci+1]);
                    if(verbose)
                        cout << "+d(" << rep[ci] << "," << rep[ci+1] << ") ";
                }
            }
        }
        else
        {
            cout << "OR OPTK DONT KNOW!!" << endl;
            exit(1);
        }

        if(verbose)
        {
            cout << "delta=" << f << endl;
        }

      //cout << endl;

      // ==========
      // Infeasible
      // ==========

      int f_inv = 0;

      if(verbose)
         cout << "INF: ";

      int nQ = ads.load[0]; // FULL Q
      for(int c = 0; c < ads.zero; c++)
      {
         if(verbose)
            cout <<"{" << nQ << "->" << f_inv <<"}\t" << "c:" << c << " ";

         if((c>=i) && (c<i+k)) // not there anymore
            nQ += 0;

         if(c == j) // sum all new values
         {

            if(i < j) // add 'j' before
            {
               //for(int nc=(j-k+1); nc<=j; nc++)  // 2,8
               for(int nc=j; nc<j+k; nc++) // 9,20
               {
                  nQ += svrpdsp->dp[rep[nc]] - svrpdsp->dd[rep[nc]]; // sum 'j' real value
                  f_inv += (rep[nc]!=0) * infv(nQ, ads.load[0]);
                  if(verbose)
                     cout << "inf_j_before_" << rep[nc] << "+=" << ((rep[nc]!=0) * infv(nQ, ads.load[0])) << " ";
               }
            }

            if(j <= ads.zero-k)
            for(int nc=i; nc<i+k; nc++)
            {
               nQ += svrpdsp->dp[rep[nc]] - svrpdsp->dd[rep[nc]];
               f_inv += infv(nQ, ads.load[0]);
               if(verbose)
                  cout << "addY_" << rep[nc] << "+=" << infv(nQ, ads.load[0]) << " ";
            }

            if(i > j) // add 'j' after
            {
               for(int nc=j; nc<j+k; nc++) // 9,20
               {
                  nQ += svrpdsp->dp[rep[nc]] - svrpdsp->dd[rep[nc]]; // sum 'j' real value
                  f_inv +=  infv(nQ, ads.load[0]);
                  if(verbose)
                     cout << "inf_j_after_" << rep[nc] << "+=" << infv(nQ, ads.load[0]) << " ";
               }
            }

         }

         // ELSE
         if( !((c>=i) && (c<i+k)) && ((c < j) || (c>(j+k-1))) )
         {
            nQ += svrpdsp->dp[rep[c]] - svrpdsp->dd[rep[c]];
            f_inv += infv(nQ, ads.load[0]);

            if(verbose)
               cout << "addX_" << rep[c] << "+=" << infv(nQ, ads.load[0]) << " ";
         }

      }

      if(verbose)
         cout << endl << "END {" << nQ << "->" << f_inv <<"}(" << e.getInfMeasure() << ")" << endl;

      if(verbose)
         cout << endl;

      long long f_inv2 = ((long long)f_inv)*100000 - e.getInfMeasure();

      if(verbose)
      {
         cout << "COST() inf was: " << e.getInfMeasure() << " => " << (((long long)f_inv)*100000) << endl;
         cout << "COST() FINAL: " << f << " and " << f_inv2 << endl;
      }

/*
        if((i==5) && (j==30))
        {
            verbose = false;
            cout << endl;
        }
*/

        return new pair<double, double>(f, f_inv2);
    }


    // ================
    // infeasible value
    // ================

    int infv(int nv, int Q)
    {
        return max((nv - Q), 0);
    }

	static string idComponent()
	{
		string idComp = super::idComponent();
		idComp.append(":SVRPDSPDeltaMoveOrOptk");
		return idComp;
	}

	virtual bool operator==(const Move<RepSVRPDSP, AdsSVRPDSP >& _m) const
	{
		const DeltaMoveOrOptk& m1 = (const DeltaMoveOrOptk&) _m;
		return (m1.i == i) && (m1.j == j) && (m1.k == k);
	}

	virtual void print() const
	{
		cout << "SVRPDSPDeltaMoveOrOpt{K=" << k << "}";
		cout << "(" << i << ";" << j << ")" << endl;
	}
};

bool DeltaMoveOrOptk::verbose = false;

}

#endif /*SVRPDSP_DELTA_MOVE_OROPTK_HPP_*/
