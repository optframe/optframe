#ifndef MODM_NSSEQInvert_H_
#define MODM_NSSEQInvert_H_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"
#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.h"
#include "DeltaStructure.h"
#include "Solution.h"

using namespace std;

namespace MODM
{

class MoveInvert: public Move<RepMODM, AdsMODM>
{
private:

	ProblemInstance* dmproblem;
	int y; //produto
	int c;

public:

	using Move<RepMODM, AdsMODM>::apply; // prevents name hiding
	using Move<RepMODM, AdsMODM>::canBeApplied; // prevents name hiding

	MoveInvert(int _y, int _c, ProblemInstance* _dmproblem) :
			y(_y), c(_c), dmproblem(_dmproblem)
	{

	}

	virtual ~MoveInvert()
	{
	}

	bool canBeApplied(const RepMODM& rep, const AdsMODM& ads)
	{
		bool productOffers = (ads.productOffers[y] > 0);
		return productOffers;
	}

	MoveCost* cost(const Evaluation&, const RepMODM& rep, const AdsMODM& ads)
	{
		double f = 0;

		int newValue;
		if (rep[c][y] == 1)
			newValue = 0;
		else
			newValue = 1;

		double costDiff = (newValue - rep[c][y]) * dmproblem->getCost(c, y);
		double revDiff = (newValue - rep[c][y]) * dmproblem->getRevenue(c, y);

		f = revDiff - costDiff;

		double fInv = 0;
		double penMaxOffers = 100;
		int nOffers = ads.clientOffers[c] + newValue - rep[c][y];
		if (nOffers > dmproblem->getClientMaxOffers(c) && rep[c][y] == 0)
			fInv += -penMaxOffers;

		if (nOffers > dmproblem->getClientMaxOffers(c) && rep[c][y] == 1)
			fInv += penMaxOffers;

		double foInvBud = 0;

		double oldTotalCost = ads.totalCost[y];
		double newTotalCost = oldTotalCost + costDiff;
		double productBudgetLimit = dmproblem->getProductBudget(y);

		if (newTotalCost > productBudgetLimit)
		{
			foInvBud += newTotalCost - productBudgetLimit;
			if (oldTotalCost > productBudgetLimit)
			{
				foInvBud -= oldTotalCost - productBudgetLimit;
			}
		}

		return new MoveCost(f, fInv + foInvBud * (-1000));
	}

	Move<RepMODM, AdsMODM>* apply(RepMODM& rep, AdsMODM& ads)
	{
		int oldC = rep[c][y];
		if (oldC == 1)
			rep[c][y] = 0;
		else
			rep[c][y] = 1;
		//rep[c][y] = !rep[c][y];


		//update ADS

		ads.totalCost[y] += (rep[c][y] - oldC) * dmproblem->getCost(c, y);
		ads.totalRevenue[y] += (rep[c][y] - oldC) * dmproblem->getRevenue(c, y);
		ads.clientOffers[c] += rep[c][y] - oldC;
		ads.productOffers[y] += rep[c][y] - oldC;


		return new MoveInvert(y, c, dmproblem);
	}

	virtual bool operator==(const Move<RepMODM, AdsMODM>& _m) const
	{
		const MoveInvert& m = (const MoveInvert&) _m;
		return (m.y == y) && (m.c == c);
	}

	void print() const
	{
		cout << "MoveInvert( ";
		cout << y << ", " << c << " )";
		cout << endl;
	}
};

class NSIteratorInvert: public NSIterator<RepMODM, AdsMODM>
{
private:
	ProblemInstance* dmproblem;
	int y, c;
	int nClients, nProducts;
	const AdsMODM& ads;

public:
	NSIteratorInvert(const AdsMODM& _ads, ProblemInstance* _dmproblem) :
			ads(_ads), dmproblem(_dmproblem)
	{
		nClients = dmproblem->getNumberOfClients();
		nProducts = dmproblem->getNumberOfProducts();

	}

	virtual ~NSIteratorInvert()
	{
	}

	void first()
	{
		y = 0;
		c = 0;
	}

	void next()
	{
		c++;
		if (c >= (nClients))
		{
			y++;
			if (ads.productOffers[y] == 0)
				y++;
			c = 0;
		}
	}

	bool isDone()
	{
		return (y >= nProducts);
	}

	Move<RepMODM, AdsMODM>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqInvert. Aborting." << endl;
			exit(1);
		}

		return *new MoveInvert(y, c, dmproblem);
	}

};

class NSSeqInvert: public NSSeq<RepMODM, AdsMODM>
{
private:
	ProblemInstance* dmproblem;
	RandGen& rg;
public:

	using NSSeq<RepMODM, AdsMODM>::move; // prevents name hiding

	NSSeqInvert(RandGen& _rg, ProblemInstance* _dmproblem) :
			rg(_rg), dmproblem(_dmproblem)
	{
	}

	virtual ~NSSeqInvert()
	{
	}

	virtual Move<RepMODM, AdsMODM>& move(const RepMODM& rep, const AdsMODM& ads)
	{
		int nProduts = dmproblem->getNumberOfProducts();

		int y = rg.rand(nProduts);

		while (ads.productOffers[y] == 0)
		{
			y = rg.rand(nProduts);
		}

		int nClients = dmproblem->getNumberOfClients();
		int c = rg.rand(nClients);

		return *new MoveInvert(y, c, dmproblem); // return a random move
	}

	virtual NSIterator<RepMODM, AdsMODM>& getIterator(const RepMODM& rep, const AdsMODM& ads)
	{
		return *new NSIteratorInvert(ads, dmproblem); // return an iterator to the neighbors of 'rep'
	}

	static string idComponent()
	{
		stringstream ss;
		ss << NS<RepMODM, AdsMODM>::idComponent() << ":NSSeqInvert";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*MODM_NSSEQInvert_H_*/
