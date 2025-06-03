#ifndef MODM_PROBLEMINSTANCE_H_
#define MODM_PROBLEMINSTANCE_H_

#include <iostream>

#include <OptFrame/Component.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>

using namespace std;
using namespace scannerpp;
using namespace optframe;

namespace MODM {

class ProblemInstance : public Component
{

private:
   // Your private vars

public:
   ProblemInstance(Scanner& scanner)
   {
      // Put here your code
      // You can read the input data from the 'scanner' object

      //Problem data
      std::cout << "========= THAYS Start Reading Problem Instance =========" << std::endl;
      //number of clients
      c = *scanner.nextInt();
      std::cout << "number of clients: ";
      std::cout << c << std::endl;

      //number of products
      n = *scanner.nextInt();
      std::cout << "number of products: ";
      std::cout << n << std::endl;

      //Hurdle rate
      std::cout << "hurdle rate: ";
      R = *scanner.nextDouble();
      std::cout << R << std::endl;

      cost = new Matrix<int>(c, n);
      revenue = new Matrix<int>(c, n);
      //reading cost, revenue and maxOffers
      std::cout << "cost(" << n << "), revenue(" << n << ") and maxOffers(1)" << std::endl;
      for (int client = 0; client < c; client++) {
         std::cout << "[";
         //cost
         for (int product = 0; product < n; product++) {
            int costProduct = *scanner.nextInt();
            std::cout << costProduct << ",";
            (*cost)(client, product) = costProduct;
         }

         //cost
         for (int product = 0; product < n; product++) {
            int revenueProduct = *scanner.nextInt();
            std::cout << revenueProduct << ",";
            (*revenue)(client, product) = revenueProduct;
         }

         int maxOffer = *scanner.nextInt();
         std::cout << maxOffer << "]" << std::endl;
         maxOffers.push_back(maxOffer);
      }

      //reading minClients to offer each product of the campain
      std::cout << "minClients of clients to offer each product during the campain" << std::endl;
      for (int products = 0; products < n; products++) {
         int nMinClients = *scanner.nextInt();
         std::cout << nMinClients << ",";
         minClients.push_back(nMinClients);
      }
      std::cout << std::endl;

      //reading budget to offer each product of the campain
      std::cout << "avaiable budge for each product" << std::endl;
      for (int products = 0; products < n; products++) {
         int vBudget = *scanner.nextInt();
         std::cout << vBudget << ",";
         budget.push_back(vBudget);
      }
      std::cout << std::endl;

      //reading fixedCost to offer each product of the campain
      std::cout << "fixedCost of each product" << std::endl;
      for (int products = 0; products < n; products++) {
         int vFixedCost = *scanner.nextInt();
         std::cout << vFixedCost << ",";
         fixedCost.push_back(vFixedCost);
      }
      std::cout << std::endl;
      std::cout << "========= Finish Reading Problem Instance =========" << std::endl;

      /*
		 std::cout<< R<<endl;
		 std::cout<< c<<endl;
		 std::cout<< n<<endl;
		 std::cout<< *cost<<endl;
		 std::cout<< *revenue<<endl;
		 std::cout<< maxOffers<<endl;
		 std::cout<< fixedCost<<endl;
		 std::cout<< budget<<endl;
		 std::cout<< minClients<<endl;
		 getchar();
		 */
   }

   int getNumberOfProducts()
   {
      return n;
   }

   int getNumberOfClients()
   {
      return c;
   }
   int getHurdleRate()
   {
      return R;
   }

   int getRevenue(int i, int j)
   {
      return (*revenue)(i, j);
   }

   int getCost(int i, int j)
   {
      return (*cost)(i, j);
   }

   int getProductFixedCost(int j)
   {
      return fixedCost[j];
   }

   int getProductBudget(int j)
   {
      return budget[j];
   }

   int getProductMinClients(int j)
   {
      return minClients[j];
   }

   int getClientMaxOffers(int i)
   {
      return maxOffers[i];
   }

   virtual ~ProblemInstance()
   {
   }

   std::string toString() const override
   {
      return "PROBLEM_MODM";
   }

   double R;
   int c;
   int n;
   Matrix<int>* cost;
   Matrix<int>* revenue;
   vector<int> maxOffers;
   vector<int> fixedCost;
   vector<int> budget;
   vector<int> minClients;
};

}

#endif /*MODM_PROBLEMINSTANCE_H_*/
