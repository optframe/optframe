#ifndef MODM_NSSEQADD_H_
#define MODM_NSSEQADD_H_

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

class MoveADD: public Move<RepMODM, AdsMODM>
{
private:

	ProblemInstance* dmproblem;
	RandGen& rg;
	bool reverse;
	vector<int> vecProducts;
	vector<int> vecClients;

public:

	using Move<RepMODM, AdsMODM>::apply; // prevents name hiding
	using Move<RepMODM, AdsMODM>::canBeApplied; // prevents name hiding

	MoveADD(bool _reverse, vector<int> _products, vector<int> _clients, ProblemInstance* _dmproblem, RandGen& _rg) :
			reverse(_reverse), vecProducts(_products), vecClients(_clients), dmproblem(_dmproblem), rg(_rg)
	{

	}

	virtual ~MoveADD()
	{
	}

	bool canBeApplied(const RepMODM& rep, const AdsMODM& ads)
	{
		return true;
	}

	Move<RepMODM, AdsMODM>* apply(RepMODM& rep, AdsMODM& ads)
	{
		//cout<<"aplying..."<<endl;
		//cout<<reverse<<endl;
		vector<int> revProducts;
		vector<int> revClients;
		bool revReverse;
		int nClients = dmproblem->getNumberOfClients();
		int nProducts = dmproblem->getNumberOfProducts();

		if (reverse == false)
		{
			revReverse = true;
			vector<bool> whileProducts(nProducts, false);

			for (int rp = 0; rp < nProducts; rp++)
			{
				int p = rg.rand(nProducts);
				while (whileProducts[p] == true)
					p = rg.rand(nProducts);
				whileProducts[p] = true;

				vector<bool> whileClients(nClients, false);

				for (int rc = 0; rc < nClients; rc++)
				{
					int c = rg.rand(nClients);
					while (whileClients[c] == true)
						c = rg.rand(nClients);
					whileClients[c] = true;

					double rev = dmproblem->getRevenue(c, p);
					double cost = dmproblem->getCost(c, p);
					//cout << "(" << newRep[client] << "," << pMODM.getClientMaxOffers(client) << ")" << endl;
					if ((ads.productOffers[p] > 0) && (rep[c][p] == false))
						if (ads.clientOffers[c] < dmproblem->getClientMaxOffers(c))	//Verifico se o client pode receber mais algum produto
							if ((rev > cost) && (cost + ads.totalCost[p]) <= dmproblem->getProductBudget(p))
							{
								rep[c][p] = true;
								revProducts.push_back(p);
								revClients.push_back(c);

								//update ADS
								ads.clientOffers[c]++;
								ads.productOffers[p]++;
								ads.totalCost[p] += cost;
								ads.totalRevenue[p] += rev;

							}
				}
			}
		}
		else
		{
			revReverse = false;

			for (int i = 0; i < vecProducts.size(); i++)
			{
				int c = vecClients[i];
				int p = vecProducts[i];
				rep[c][p] = false;
				ads.clientOffers[c]--;
				ads.productOffers[p]--;
				ads.totalCost[p] -= dmproblem->getCost(c, p);
				ads.totalRevenue[p] -= dmproblem->getRevenue(c, p);
			}

		}

		//cout<<"applied..."<<endl;
		//getchar();
		return new MoveADD(revReverse, revProducts, revClients, dmproblem, rg);
	}

	virtual bool operator==(const Move<RepMODM, AdsMODM>& _m) const
	{
		const MoveADD& m = (const MoveADD&) _m;
		return (m.reverse == reverse);
	}

	void print() const
	{
		cout << "MoveADD( ";
		cout << reverse << " )";
		cout << endl;
	}

	void toString()
	{
		cout << "MoveADD" << endl;
	}
};

class NSIteratorADD: public NSIterator<RepMODM, AdsMODM>
{
private:
	ProblemInstance* dmproblem;
	RandGen& rg;
	int i;

public:
	NSIteratorADD(ProblemInstance* _dmproblem, RandGen& _rg) :
			dmproblem(_dmproblem), rg(_rg)
	{

	}

	virtual ~NSIteratorADD()
	{
	}

	void first()
	{
		i = 1;
	}

	void next()
	{
		i++;
		//cout << i << endl;
	}

	bool isDone()
	{
		return (i == 2);
	}

	Move<RepMODM, AdsMODM>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqADD. Aborting." << endl;
			exit(1);
		}
		vector<int> vazioP;
		vector<int> vazioC;

		return *new MoveADD(false, vazioP, vazioC, dmproblem, rg); // return a random move
	}

};

class NSSeqADD: public NSSeq<RepMODM, AdsMODM>
{
private:
	ProblemInstance* dmproblem;
	RandGen& rg;

public:

	using NSSeq<RepMODM, AdsMODM>::move; // prevents name hiding

	NSSeqADD(RandGen& _rg, ProblemInstance* _dmproblem) :
			rg(_rg), dmproblem(_dmproblem)
	{

	}

	virtual ~NSSeqADD()
	{
	}

	virtual NSIterator<RepMODM, AdsMODM>& getIterator(const RepMODM& rep, const AdsMODM& ads)
	{
		return *new NSIteratorADD(dmproblem, rg); // return an iterator to the neighbors of 'rep'
	}

	virtual Move<RepMODM, AdsMODM>& move(const RepMODM& rep, const AdsMODM& ads)
	{
		vector<int> vazioP;
		vector<int> vazioC;
		/*cout<<"oi ADD NS"<<endl;
		 getchar();*/
		return *new MoveADD(false, vazioP, vazioC, dmproblem, rg); // return a random move
	}

	static string idComponent()
	{
		stringstream ss;
		ss << NS<RepMODM, AdsMODM>::idComponent() << ":NSSeqADD";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

}

#endif /*MODM_NSSEQADD_H_*/
