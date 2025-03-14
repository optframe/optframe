#ifndef SVRPDSP_PROBLEMINSTANCE_HPP_
#define SVRPDSP_PROBLEMINSTANCE_HPP_

#include <iostream>

#include <OptFrame/Scanner++/Scanner.hpp>

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
      n = *scanner.nextInt();
      N = n + 1;
      dN = 2 * n + 1;
      dn = 2 * n;
      std::cout << "Problem size: " << N << " => " << dN;
#ifdef  ALL_INTEGER
      std::cout << " (INTEGER)" << std::endl;
#else
      std::cout << " (DOUBLE)" << std::endl;
#endif

      Q = 0;

      d.clear();
      p.clear();
      r.clear();

      for (int i = 0; i < N; i++)
      {
         // drop number
         *scanner.nextInt();

#ifdef  ALL_INTEGER
         d.push_back(*scanner.nextDouble()*100); // integer
         p.push_back(*scanner.nextDouble()*100); // integer
         r.push_back(*scanner.nextDouble()*100); // integer
#else
         d.push_back(*scanner.nextDouble());
         p.push_back(*scanner.nextDouble());
         r.push_back(*scanner.nextDouble());
#endif

         // update capacity
         Q += d[i];
      }

      // capacity
      std::cout << std::endl << "Q = " << Q << std::endl;

      // demands


		//cout << "d: " << d << std::endl;
		//cout << "p: " << p << std::endl;
		//cout << "r: " << r << std::endl;

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

        std::cout << "N=" << N << " dN=" << dN << std::endl;
		//cout << "dd: " << dd << std::endl;
		//cout << "dp: " << dp << std::endl;
		//cout << "dr: " << dr << std::endl;
        
                // -----------
                // cost matrix
                // -----------

#ifdef  ALL_INTEGER
		c = Matrix<int> (N, N);                             // integer
		for (int i = 0; i < N; i++)                         // integer
			for (int j = 0; j < N; j++)                 // integer
				c(i, j) = *scanner.nextDouble()*100; // integer
#endif
#ifndef ALL_INTEGER
		c = Matrix<double> (N, N);
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				c(i, j) = *scanner.nextDouble();
#endif

        // CHECK SIMMETRY
        for(int i=0; i<N; i++)
           for(int j=0; j<N; j++)
               if(c(i,j) != c(j,i))
               {
                   std::cout << "Simmetry error at i=" <<i << " j=" << j << std::endl;
                   exit(1);
               }
        std::cout << "Matrix c checked for simmetry!" << std::endl;

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
                        std::cout << "erro em (" << i << "," << j << ")! matriz nao simetrica." << std::endl;
                        exit(1);
                     }


                //cout << "costs:" << std::endl;
		//cout << c << std::endl;

                //cout << "duplicate costs:" << std::endl;
		//cout << dc << std::endl;

		cout << "Problem read successfully!" << std::endl;
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
		cout << "unloaded problem instance" << std::endl;
	}
};

}

#endif /*SVRPDSP_PROBLEMINSTANCE_HPP_*/

