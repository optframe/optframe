#ifndef MODM_NSSEQARProduct_H_
#define MODM_NSSEQARProduct_H_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"
#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.h"
#include "DeltaStructure.h"
#include "Solution.h"
#include "ConstructiveBasicGreedyRandomized.h"

using namespace std;

namespace MODM
{

class MoveARProduct: public Move<RepMODM, AdsMODM>
{
private:

	ProblemInstance* dmproblem;
	int yRemove; //produto
	int yNew; //produto
	vector<int> oldC;
	bool reverse;
	double tCost, tRevenue, productOffers;
	RandGen& rg;
	double alpha;

public:

	using Move<RepMODM, AdsMODM>::apply; // prevents name hiding
	using Move<RepMODM, AdsMODM>::canBeApplied; // prevents name hiding

	MoveARProduct(int _yRemove, int _yNew, vector<int> _oldC, double _tCost, double _tRevenue, double _productOffers, bool _reverse, ProblemInstance* _dmproblem, RandGen& _rg, double _alpha) :
			yRemove(_yRemove), yNew(_yNew), oldC(_oldC), tCost(_tCost), tRevenue(_tRevenue), productOffers(_productOffers), reverse(_reverse), dmproblem(_dmproblem), rg(_rg), alpha(_alpha)
	{

	}

	virtual ~MoveARProduct()
	{
	}

	bool canBeApplied(const RepMODM& rep, const AdsMODM& ads)
	{
		bool differentProducts = false;

		if (yRemove != yNew)
			differentProducts = true;

		bool yRemoveIsActive = false;

		if (ads.productOffers[yRemove] > 0)
			yRemoveIsActive = true;

		bool yNewIsNotActive = false;

		if (ads.productOffers[yNew] == 0)
			yNewIsNotActive = true;

		return differentProducts && yRemoveIsActive && yNewIsNotActive;
	}

	Move<RepMODM, AdsMODM>* apply(RepMODM& rep, AdsMODM& ads)
	{
		int nClients = dmproblem->getNumberOfClients();

		vector<int> revOldC;
		bool revReverse;
		double revTCost, revTRevenue, revProductOffers;

		if (reverse == false)
		{
			//Variables change for the reverse move
			revTCost = ads.totalCost[yRemove];
			revTRevenue = ads.totalRevenue[yRemove];
			revProductOffers = ads.productOffers[yRemove];
			revReverse = true;

			for (int c = 0; c < nClients; c++)
			{
				revOldC.push_back(rep[c][yRemove]);
				if (rep[c][yRemove] == 1)
					ads.clientOffers[c]--;

				rep[c][yRemove] = 0;
			}
			//update ADS
			ads.totalCost[yRemove] = 0;
			ads.totalRevenue[yRemove] = 0;
			ads.productOffers[yRemove] = 0;

			// =======================================
			//Add clients to the new product yNew

			vector<pair<double, int> > NPP;
			for (int c = 0; c < nClients; c++)
			{
				if (ads.clientOffers[c] < dmproblem->getClientMaxOffers(c))
				{
					int r = dmproblem->getRevenue(c, yNew);
					int cost = dmproblem->getCost(c, yNew);

					double valueNPP = (r - cost) / cost;

					NPP.push_back(make_pair(valueNPP, c));
				}
			}

			sort(NPP.begin(), NPP.end(), comparacao);

			int minSize = NPP.size();
			int oMin = dmproblem->getProductMinClients(yNew);

			ads.totalCost[yNew] = 0;
			ads.totalRevenue[yNew] = 0;
			ads.productOffers[yNew] = 0;
			if (minSize >= oMin)
			{
//				cout << ads.totalCost[yNew] << endl;
//				cout << ads.totalRevenue[yNew] << endl;
//				cout << ads.productOffers[yNew] << endl;

				double yNewTCost = 0;
				double yNewTRevenue = 0;
				int yNewPOffers = 0;
				vector<int> SC;
				for (int o = 0; o < oMin; o++)
				{
					int sizeLC = ceil(alpha * NPP.size());
					int greedyRand = rg.rand(sizeLC);
					int selectedClient = NPP[greedyRand].second;
					NPP.erase(NPP.begin() + greedyRand);

					//adding selected clients to the current product yNew
					rep[selectedClient][yNew] = 1;
					ads.clientOffers[selectedClient]++;
					yNewTCost += dmproblem->getCost(selectedClient, yNew);
					yNewTRevenue += dmproblem->getRevenue(selectedClient, yNew);
					yNewPOffers++;

					SC.push_back(selectedClient);
				}

				ads.totalCost[yNew] = yNewTCost;
				ads.totalRevenue[yNew] = yNewTRevenue;
				ads.productOffers[yNew] = yNewPOffers;
//				cout << "after:" << endl;
//				cout << ads.totalCost[yNew] << endl;
//				cout << ads.totalRevenue[yNew] << endl;
//				cout << ads.productOffers[yNew] << endl;
//				getchar();
			}
			// =======================================

		}
		else
		{
			//Move is Reverse
			//Return original values to the yRemove product

			for (int c = 0; c < nClients; c++)
			{
				rep[c][yRemove] = oldC[c];

				if (rep[c][yRemove] == 1)
					ads.clientOffers[c]++;

				//Clear yNew product
				if (rep[c][yNew] == 1)
					ads.clientOffers[c]--;

				rep[c][yNew] = 0;
			}
			//update ADS yRemove
			ads.totalCost[yRemove] = tCost;
			ads.totalRevenue[yRemove] = tRevenue;
			ads.productOffers[yRemove] = productOffers;

			//update ADS yNew
			ads.totalCost[yNew] = 0;
			ads.totalRevenue[yNew] = 0;
			ads.productOffers[yNew] = 0;

			//Variables change for the reverse move
			revTCost = 0;
			revTRevenue = 0;
			revProductOffers = 0;
			revReverse = false;
		}

		return new MoveARProduct(yRemove, yNew, revOldC, revTCost, revTRevenue, revProductOffers, revReverse, dmproblem, rg, alpha);
	}

	virtual bool operator==(const Move<RepMODM, AdsMODM>& _m) const
	{
		const MoveARProduct& m = (const MoveARProduct&) _m;
		return (m.yRemove == yRemove) && (m.yNew == yNew) && (m.productOffers == productOffers) && (m.oldC == oldC) && (m.tCost == tCost) && (m.tRevenue == tRevenue) && (m.reverse == reverse);
	}

	void print() const
	{
		cout << "MoveARProduct( ";
		cout << yRemove << "," << yNew << "," << oldC << "," << tCost << "," << tRevenue << "," << productOffers << "," << reverse << " )";
		cout << endl;
	}

	void toString()
	{
		cout << "MoveARProduct" << endl;
	}
};

class NSIteratorARProduct: public NSIterator<RepMODM, AdsMODM>
{
private:
	ProblemInstance* dmproblem;
	int yRemove, yNew;
	int nClients, nProducts;
	const AdsMODM& ads;
	RandGen& rg;
	double alpha;

public:
	NSIteratorARProduct(const AdsMODM& _ads, ProblemInstance* _dmproblem, RandGen& _rg, double _alpha) :
			ads(_ads), dmproblem(_dmproblem), rg(_rg), alpha(_alpha)
	{
		nClients = dmproblem->getNumberOfClients();
		nProducts = dmproblem->getNumberOfProducts();

	}

	virtual ~NSIteratorARProduct()
	{
	}

	void first()
	{
		yRemove = 0;
		yNew = 1;
	}

	void next()
	{
		yNew++;
		if (yNew == nProducts)
		{
			yRemove++;
			yNew = 1;
		}
	}

	bool isDone()
	{
		return (yRemove == nProducts);
	}

	Move<RepMODM, AdsMODM>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqARProduct. Aborting." << endl;
			exit(1);
		}
		vector<int> vazio;
		return *new MoveARProduct(yRemove, yNew, vazio, 0, 0, 0, 0, dmproblem, rg, alpha);
	}

};

class NSSeqARProduct: public NSSeq<RepMODM, AdsMODM>
{
private:
	ProblemInstance* dmproblem;
	RandGen& rg;
	double alpha;
public:

	using NSSeq<RepMODM, AdsMODM>::move; // prevents name hiding

	NSSeqARProduct(RandGen& _rg, ProblemInstance* _dmproblem, double _alpha) :
			rg(_rg), dmproblem(_dmproblem), alpha(_alpha)
	{
		if (alpha == 0)
			alpha = 0.0001;
	}

	virtual ~NSSeqARProduct()
	{
	}

	virtual Move<RepMODM, AdsMODM>& move(const RepMODM& rep, const AdsMODM& ads)
	{
		int nProduts = dmproblem->getNumberOfProducts();

		int yRemove = rg.rand(nProduts);

		int yNew = rg.rand(nProduts);

		while (yNew == yRemove)
			yNew = rg.rand(nProduts);

		vector<int> vazio;
		return *new MoveARProduct(yRemove, yNew, vazio, 0, 0, 0, 0, dmproblem, rg, alpha); // return a random move
	}

	virtual NSIterator<RepMODM, AdsMODM>& getIterator(const RepMODM& rep, const AdsMODM& ads)
	{
		return *new NSIteratorARProduct(ads, dmproblem, rg, alpha); // return an iterator to the neighbors of 'rep'
	}

	static string idComponent()
	{
		stringstream ss;
		ss << NS<RepMODM, AdsMODM>::idComponent() << ":NSSeqARProduct";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

}

#endif /*MODM_NSSEQARProduct_H_*/
