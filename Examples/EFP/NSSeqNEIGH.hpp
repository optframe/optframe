#ifndef EFP_NSSEQNEIGH_HPP_
#define EFP_NSSEQNEIGH_HPP_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"
#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "DeltaStructure.h"
#include "Solution.h"

using namespace std;
namespace EFP
{

class MoveNEIGH3: public Move<RepEFP, OPTFRAME_DEFAULT_ADS>
{
private:
	int r, o;
	bool sign;
	double applyValue;
	int vectorType;
public:

	using Move<RepEFP, OPTFRAME_DEFAULT_ADS>::apply; // prevents name hiding
	using Move<RepEFP, OPTFRAME_DEFAULT_ADS>::canBeApplied; // prevents name hiding

	MoveNEIGH3(int _r, int _o, double _applyValue, bool _sign, int _vectorType) :
			r(_r), o(_o), applyValue(_applyValue), sign(_sign), vectorType(_vectorType)
	{

	}

	virtual ~MoveNEIGH3()
	{
	}

	bool canBeApplied(const RepEFP& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		/*
		 if(sign == false)
		 return true;
		 else {
		 bool nonNegative = (rep[r][o] - applyValue2 >= 0);
		 return nonNegative;
		 }*/

		return true;

	}

	Move<RepEFP, OPTFRAME_DEFAULT_ADS>* apply(RepEFP& rep, OPTFRAME_DEFAULT_ADS&)
	{
		// apply this move to 'rep'

		//cout<<rep.singleFuzzyRS<<endl;
		//cout<<o<<"\t"<<r<<endl;
		//getchar();
		if (r == -1)
			return new MoveNEIGH3(-1, -1, -1, -1, -1);

		if (r == PERTINENCEFUNC)
		{
			if (vectorType == 0)
				rep.singleFuzzyRS[o][r] = !rep.singleFuzzyRS[o][r];
			if (vectorType == 1)
				rep.averageFuzzyRS[o][r] = !rep.averageFuzzyRS[o][r];
			if (vectorType == 2)
				rep.derivativeFuzzyRS[o][r] = !rep.derivativeFuzzyRS[o][r];

			return new MoveNEIGH3(r, o, applyValue, !sign, vectorType);
		}

		if (vectorType == 0)
		{

			if (sign == false)
				rep.singleFuzzyRS[o][r] += applyValue;
			else
				rep.singleFuzzyRS[o][r] -= applyValue;
		}

		if (vectorType == 1)
		{
			if (sign == false)
				rep.averageFuzzyRS[o][r] += applyValue;
			else
				rep.averageFuzzyRS[o][r] -= applyValue;
		}

		if (vectorType == 2)
		{
			if (sign == false)
			{
				rep.derivativeFuzzyRS.at(o).at(r) += applyValue;
			}
			else
				rep.derivativeFuzzyRS[o][r] -= applyValue;
		}
		// return reverse move
		return new MoveNEIGH3(r, o, applyValue, !sign, vectorType);
	}

	virtual bool operator==(const Move<RepEFP, OPTFRAME_DEFAULT_ADS>& _m) const
	{
		const MoveNEIGH3& m = (const MoveNEIGH3&) _m;
		return ((m.r == r) && (m.o == o) && (m.sign == sign));
	}

	void print() const
	{
		cout << "MoveNEIGH3( vector: " << r << " : ";
		cout << " option " << o << " <=>  sign " << sign << " )";
		cout << endl;
	}
}
;

class NSIteratorNEIGH3: public NSIterator<RepEFP, OPTFRAME_DEFAULT_ADS>
{
private:
	MoveNEIGH3* m;
	vector<MoveNEIGH3*> moves;
	int index;
	const RepEFP& rep;
	ProblemInstance& pEFP;
public:
	NSIteratorNEIGH3(const RepEFP& _rep, ProblemInstance& _pEFP) :
			rep(_rep), pEFP(_pEFP)
	{
		index = 0;
		m = NULL;
	}

	virtual ~NSIteratorNEIGH3()
	{
		for (int i = index + 1; i < moves.size(); i++)
			delete moves[i];

	}

	virtual void first()
	{
		vector<double> values;
		double mean = pEFP.getMean(0); //mean from the targetfile

		values.push_back(0.01);
		values.push_back(0.1);
		values.push_back(0.5);
		values.push_back(mean / 30);
		values.push_back(mean / 15);
		values.push_back(mean / 6);
		values.push_back(mean);
		values.push_back(mean * 2);

		int options = rep.singleFuzzyRS.size(); //rep.size() options

		for (int sign = 0; sign < 2; sign++)
			for (int r = 0; r < NCOLUMNATRIBUTES; r++)
				for (int o = 0; o < options; o++)
					for (int v = 0; v < values.size(); v++)
					{
						moves.push_back(new MoveNEIGH3(r, o, values[v], sign, 0));
					}

		options = rep.averageFuzzyRS.size(); //rep.size() options

		for (int sign = 0; sign < 2; sign++)
			for (int r = 0; r < NCOLUMNATRIBUTES; r++)
				for (int o = 0; o < options; o++)
					for (int v = 0; v < values.size(); v++)
					{
						moves.push_back(new MoveNEIGH3(r, o, values[v], sign, 1));
					}

		options = rep.derivativeFuzzyRS.size(); //rep.size() options

		for (int sign = 0; sign < 2; sign++)
			for (int r = 0; r < NCOLUMNATRIBUTES; r++)
				for (int o = 0; o < options; o++)
					for (int v = 0; v < values.size(); v++)
					{
						moves.push_back(new MoveNEIGH3(r, o, values[v], sign, 2));
					}

		if (moves.size() > 0)
		{
			m = moves[index];
		}
		else
			m = NULL;
	}

	virtual void next()
	{
		index++;
		if (index < moves.size())
		{
			m = moves[index];
		}
		else
			m = NULL;
	}

	virtual bool isDone()
	{
		return m == NULL;
	}

	virtual Move<RepEFP, OPTFRAME_DEFAULT_ADS>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSIteratorNEIGH3. Aborting." << endl;
			exit(1);
		}

		return *m;
	}

};

class NSSeqNEIGH3: public NSSeq<RepEFP>
{
private:
	RandGen& rg;
	ProblemInstance& pEFP;
	int nIntervals;
public:

	using NSSeq<RepEFP>::move; // prevents name hiding

	NSSeqNEIGH3(ProblemInstance& _pEFP, RandGen& _rg, int _nIntervals) :
			pEFP(_pEFP), rg(_rg), nIntervals(_nIntervals)
	{
	}

	virtual ~NSSeqNEIGH3()
	{
	}

	virtual Move<RepEFP, OPTFRAME_DEFAULT_ADS>& move(const RepEFP& rep, const OPTFRAME_DEFAULT_ADS&)
	{

		int vectorType = rg.rand(3);
		int o = -1;
		int r = -1;

		int maxTries = 1000;
		int tries = 1;
		while ((r == -1) && (tries < maxTries))
		{
			vectorType = rg.rand(3);

			if (vectorType == 0)
			{

				if (rep.singleFuzzyRS.size() > 0)
				{
					o = rg.rand(rep.singleFuzzyRS.size()); //rep.size() options
					r = rg.rand(rep.singleFuzzyRS[0].size());
				}

			}

			if (vectorType == 1)
			{
				if (rep.averageFuzzyRS.size() > 0)
				{
					o = rg.rand(rep.averageFuzzyRS.size()); //rep.size() options
					r = rg.rand(rep.averageFuzzyRS[0].size());
				}
			}

			if (vectorType == 2)
			{
				if (rep.derivativeFuzzyRS.size() > 0)
				{
					o = rg.rand(rep.derivativeFuzzyRS.size()); //rep.size() options
					r = rg.rand(rep.derivativeFuzzyRS[0].size());
				}
			}

			tries++;
		}

		if (tries == maxTries)
		{
			return *new MoveNEIGH3(-1, -1, -1, -1, -1); // return a random move
		}

		int construtive = 1;

		bool sign = rg.rand(2);

		int applyRand = rg.rand(nIntervals + 3);
		double applyValue = 1;

		if (applyRand == 0)
			applyValue = 0.05;
		if (applyRand == 1)
			applyValue = 0.1;
		if (applyRand == 2)
			applyValue = 0.5;

		double mean = pEFP.getMean(0); //mean from the targetfile

		if (applyRand == 3)
			applyValue = mean / 30;
		if (applyRand == 4)
			applyValue = mean / 15;
		if (applyRand == 5)
			applyValue = mean / 6;
		if (applyRand == 6)
			applyValue = mean;
		if (applyRand == 7)
			applyValue = mean * 2;

		return *new MoveNEIGH3(r, o, applyValue, sign, vectorType); // return a random move
	}

	virtual NSIterator<RepEFP, OPTFRAME_DEFAULT_ADS>& getIterator(const RepEFP& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return *new NSIteratorNEIGH3(rep, pEFP); // return an iterator to the neighbors of 'rep'
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqChangeFuzzy with move: ";
		return ss.str();
	}
};

}
#endif /*EFP_NSSEQNEIGH_HPP_*/

