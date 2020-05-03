#ifndef SVRPDSP_PROBLEMINSTANCE_HPP_
#define SVRPDSP_PROBLEMINSTANCE_HPP_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"

using namespace std;
using namespace scannerpp;

namespace SVRPDSP
{

class ProblemInstance
{
private:
	//private:
public:
	int n, N;

#ifdef  ALL_INTEGER
	int Q;               // integer
	vector<int> d, p, r; // integer
	Matrix<int> c;       // integer
#endif
#ifndef ALL_INTEGER
	double Q;
	vector<double> d, p, r;
	Matrix<double> c;
#endif

        // ----------
        // duplicates
        // ----------

        int dN;
        int dn;

#ifdef  ALL_INTEGER
	vector<int> dd, dp, dr; // integer
        Matrix<int> dc;         // integer
#endif
#ifndef ALL_INTEGER
	vector<double> dd, dp, dr;
        Matrix<double> dc;
#endif


public:
   ProblemInstance(Scanner& scanner)
   {
      n = scanner.nextInt();
      N = n + 1;
      dN = 2 * n + 1;
      dn = 2 * n;
      cout << "Problem size: " << N << " => " << dN;
#ifdef  ALL_INTEGER
      cout << " (INTEGER)" << endl;
#else
      cout << " (DOUBLE)" << endl;
#endif

      Q = 0;

      d.clear();
      p.clear();
      r.clear();

      for (int i = 0; i < N; i++)
      {
         // drop number
         scanner.nextInt();

#ifdef  ALL_INTEGER
         d.push_back(scanner.nextDouble()*100); // integer
         p.push_back(scanner.nextDouble()*100); // integer
         r.push_back(scanner.nextDouble()*100); // integer
#else
         d.push_back(scanner.nextDouble());
         p.push_back(scanner.nextDouble());
         r.push_back(scanner.nextDouble());
#endif

         // update capacity
         Q += d[i];
      }

      // capacity
      cout << endl << "Q = " << Q << endl;

      // demands


		//cout << "d: " << d << endl;
		//cout << "p: " << p << endl;
		//cout << "r: " << r << endl;

                // duplicates

                for(int i = 0; i<N; i++)
                {
                    dd.push_back(d[i]);
                    dp.push_back(0);
                    dr.push_back(0);
                }

                for(int i = 1; i<N; i++)
                {
                    dd.push_back(0);
                    dp.push_back(p[i]);
                    dr.push_back(r[i]);
                }

                // duplicate demands

        cout << "N=" << N << " dN=" << dN << endl;
		//cout << "dd: " << dd << endl;
		//cout << "dp: " << dp << endl;
		//cout << "dr: " << dr << endl;
        
                // -----------
                // cost matrix
                // -----------

#ifdef  ALL_INTEGER
		c = Matrix<int> (N, N);                             // integer
		for (int i = 0; i < N; i++)                         // integer
			for (int j = 0; j < N; j++)                 // integer
				c(i, j) = scanner.nextDouble()*100; // integer
#endif
#ifndef ALL_INTEGER
		c = Matrix<double> (N, N);
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				c(i, j) = scanner.nextDouble();
#endif

        // CHECK SIMMETRY
        for(int i=0; i<N; i++)
           for(int j=0; j<N; j++)
               if(c(i,j) != c(j,i))
               {
                   cout << "Simmetry error at i=" <<i << " j=" << j << endl;
                   exit(1);
               }
        cout << "Matrix c checked for simmetry!" << endl;

                // duplicate

#ifdef  ALL_INTEGER
		dc = Matrix<int> (dN, dN);  // integer
#endif
#ifndef ALL_INTEGER
		dc = Matrix<double> (dN, dN);
#endif

		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
                        {
                            dc(i, j) = c(i, j);
                            dc(i, j+N-1) = c(i, j);
                            dc(i+N-1, j+N-1) = c(i, j);
                            dc(i+N-1, j) = c(i, j);
                        }

               for(int i=0; i<dN; i++)
                  for(int j=0; j<dN; j++)
                     if(dc(i,j) != dc(j,i))
                     {
                        cout << "erro em (" << i << "," << j << ")! matriz nao simetrica." << endl;
                        exit(1);
                     }


                //cout << "costs:" << endl;
		//cout << c << endl;

                //cout << "duplicate costs:" << endl;
		//cout << dc << endl;

		cout << "Problem read successfully!" << endl;
	}

   bool isDelivery(int c)
   {
      return (c>0) && (c <= n);
   }

   bool isPickup(int c)
   {
      return (c>n);
   }
	
	virtual ~ProblemInstance()
	{
		cout << "unloaded problem instance" << endl;
	}
};

}

#endif /*SVRPDSP_PROBLEMINSTANCE_HPP_*/

