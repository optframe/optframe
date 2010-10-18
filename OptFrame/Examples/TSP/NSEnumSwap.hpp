#ifndef TSP_NSENUMSwap_HPP_
#define TSP_NSENUMSwap_HPP_

// Framework includes
#include "../../../OptFrame/Move.hpp"
#include "../../../OptFrame/NSEnum.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "Memory.h"
#include "Solution.h"

//#define MOV_Swap_DEBUG

using namespace std;

//============================================================================
//                           Swap MOVE
//============================================================================

class MoveSwap : public Move<RepTSP, MemTSP>
{
private:
	int c1, c2;

	// Your private vars

public:

	MoveSwap(int c1, int c2) // If necessary, add more parameters
	{
#ifdef  MOV_Swap_DEBUG
#define MOV_Swap_DEBUG

		printf("######## DEBUG MODE [ON] ########\n");
		// Use this (#ifdef..#endif) to put your debug codes

#endif


		this->c1 = c1;
		this->c2 = c2;

		// Put the rest of your code here
	}

	bool canBeApplied(RepTSP* rep)
	{
		// If there are some move "MoveSwap" that can't be applied, implement this method

		return true;
	}

	Move<RepTSP, MemTSP>* apply(RepTSP* rep)
	{
		// Specify how the move "MoveSwap" will be applied

		int aux = rep->at(c1);
		(*rep)[c1] = (*rep)[c2];
		(*rep)[c2] = aux;


		// return the reverse move
		return new MoveSwap(c2,c1); // If necessary, add more parameters
	}

	void print()
	{
		cout << "MoveSwap between "<<c1<<" and "<<c2<<endl;
	}

	bool equals(Move<RepTSP,MemTSP>* _m)
	{
		MoveSwap* m = (MoveSwap*) _m; // You can only compare if types are equal

		if( (c1==m->c1 && c2==m->c2) || (c1==m->c2 && c2==m->c1) )
			return true;
		else
			return false;
	}
};


//============================================================================
//                  Swap Neighborhood Structure
//============================================================================


class NSEnumSwap: public NSEnum< RepTSP,MemTSP >
{
private:
	TSPProblemInstance* pI;
	int n;

	// Your private vars

public:	

	NSEnumSwap(TSPProblemInstance* pI)
	{
		this->pI = pI;
		this->n = pI->n;
	}

	virtual Move<RepTSP,MemTSP>* kmove(int k)
	{
		if(k>size())
		{
			cerr << "Neighborhood Swap Error! Move " << k << " doesnt exist! Valid Interval from 0 to " << (size()-1) << "." << endl;
			exit(1);

			return NULL;
		}

		return busca(k, 1, 2*n);
	}


	int size()
	{
		return n*(n-1)/2;
	}

	virtual void print()
	{
		cout << "\nNSEnum Swap ("<< size() << ")\n";
	}



	// Auxiliar methods


	int corresp(int d)
	{
		return d - ((d - (n-1)) -1)*2;
	}


	int numElem(int d)
	{
		if(d <= n)
			return (d/2);
		else
			return numElem(corresp(d));
	}

	int comeca(int d)
	{

		if(d<=n)
		{
			int z = (d/2);

			int ant = z*(z-1);

			// Se impar, soma mais 'z'
			if(d%2==1)
				ant += z;

			return ant;
		}
		else
		{
			return 2*(comeca(n)) - comeca(corresp(d)+1) + numElem(n);
		}


	}

	int termina(int d)
	{
		return comeca(d) + numElem(d) - 1;
	}

	Move<RepTSP,MemTSP>* busca(int k, int a, int b)
	{
		int d = (a+b)/2;

		//cout << "busca "<<k<<" na diagonal "<<d<<"entre ["<<a<<","<<b<<"]"<<endl;

		int c = comeca(d);
		int t = termina(d);

		//cout <<"comeca em "<<c<<" e termina em "<<t<<endl;

		//int p;
		//cin >>p;

		if(k < c)
		{
			//cout << "k<c"<<endl;
			return busca(k, a, d);
		}

		if(k > t)
		{
			//cout << "k>t"<<endl;

			if(a==d)
				d++;

			return busca(k, d, b);
		}


		if( d<=n)
		{

			for(int i=0;i<numElem(d);i++)
				if(k == c+i)
					return new MoveSwap(i, d-i-1);
		}
		else
		{

			for(int i=0;i<numElem(d);i++)
				if(k == c+i)
				{
					int j = d-n;
					return new MoveSwap(i+j, d-i-1-j);
				}

		}

		return NULL;
	}


};


#endif /*TSP_NSENUMSwap_HPP_*/

