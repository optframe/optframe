#ifndef MODM_PROBLEMINSTANCE_H_
#define MODM_PROBLEMINSTANCE_H_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"
#include "../../OptFrame/Component.hpp"
#include "../../OptFrame/Util/Matrix.hpp"


using namespace std;
using namespace scannerpp;
using namespace optframe;

namespace MODM
{

class ProblemInstance: public Component
{


private:
	// Your private vars

public:
	ProblemInstance(Scanner& scanner)
	{
		// Put here your code
		// You can read the input data from the 'scanner' object

		//Problem data
		cout << "========= THAYS Start Reading Problem Instance =========" << endl;
		//number of clients
		c = scanner.nextInt();
		cout << "number of clients: ";
		cout << c << endl;

		//number of products
		n = scanner.nextInt();
		cout << "number of products: ";
		cout << n << endl;

		//Hurdle rate
		cout << "hurdle rate: ";
		R = scanner.nextDouble();
		cout << R << endl;

		cost = new Matrix<int> (c, n);
		revenue = new Matrix<int> (c, n);
		//reading cost, revenue and maxOffers
		cout << "cost(" << n << "), revenue(" << n << ") and maxOffers(1)" << endl;
		for (int client = 0; client < c; client++)
		{
			cout << "[";
			//cost
			for (int product = 0; product < n; product++)
			{
				int costProduct = scanner.nextInt();
				cout << costProduct << ",";
				(*cost)(client, product) = costProduct;
			}

			//cost
			for (int product = 0; product < n; product++)
			{
				int revenueProduct = scanner.nextInt();
				cout << revenueProduct << ",";
				(*revenue)(client, product) = revenueProduct;
			}

			int maxOffer = scanner.nextInt();
			cout << maxOffer << "]" << endl;
			maxOffers.push_back(maxOffer);
		}

		//reading minClients to offer each product of the campain
		cout << "minClients of clients to offer each product during the campain" << endl;
		for (int products = 0; products < n; products++)
		{
			int nMinClients = scanner.nextInt();
			cout << nMinClients << ",";
			minClients.push_back(nMinClients);

		}
		cout << endl;

		//reading budget to offer each product of the campain
		cout << "avaiable budge for each product" << endl;
		for (int products = 0; products < n; products++)
		{
			int vBudget = scanner.nextInt();
			cout << vBudget << ",";
			budget.push_back(vBudget);
		}
		cout << endl;

		//reading fixedCost to offer each product of the campain
		cout << "fixedCost of each product" << endl;
		for (int products = 0; products < n; products++)
		{
			int vFixedCost = scanner.nextInt();
			cout << vFixedCost << ",";
			fixedCost.push_back(vFixedCost);

		}
		cout << endl;
		cout << "========= Finish Reading Problem Instance =========" << endl;





		/*
		 cout<< R<<endl;
		 cout<< c<<endl;
		 cout<< n<<endl;
		 cout<< *cost<<endl;
		 cout<< *revenue<<endl;
		 cout<< maxOffers<<endl;
		 cout<< fixedCost<<endl;
		 cout<< budget<<endl;
		 cout<< minClients<<endl;
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

