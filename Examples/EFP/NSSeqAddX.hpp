#ifndef EFP_NSSEQADDX_HPP_
#define EFP_NSSEQADDX_HPP_

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

class MoveNEIGHAddX: public Move<RepEFP, OPTFRAME_DEFAULT_ADS>
{
private:
	int r, o;
	bool sign;
	double applyValue;
	int vectorType;
public:

	using Move<RepEFP, OPTFRAME_DEFAULT_ADS>::apply; // prevents name hiding
	using Move<RepEFP, OPTFRAME_DEFAULT_ADS>::canBeApplied; // prevents name hiding

	MoveNEIGHAddX(int _r, int _o, double _applyValue, bool _sign, int _vectorType) :
			r(_r), o(_o), applyValue(_applyValue), sign(_sign), vectorType(_vectorType)
	{

	}

	virtual ~MoveNEIGHAddX()
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

			return new MoveNEIGHAddX(r, o, applyValue, !sign, vectorType);
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
		return new MoveNEIGHAddX(r, o, applyValue, !sign, vectorType);
	}

	virtual bool operator==(const Move<RepEFP, OPTFRAME_DEFAULT_ADS>& _m) const
	{
		const MoveNEIGHAddX& m = (const MoveNEIGHAddX&) _m;
		return ((m.r == r) && (m.o == o) && (m.sign == sign));
	}

	void print() const
	{
		cout << "MoveNEIGHAddX( vector: " << r << " : ";
		cout << " option " << o << " <=>  sign " << sign <<"\tapplyValue"<<applyValue <<" )";
		cout << endl;
	}
}
;

class NSIteratorNEIGHAddX: public NSIterator<RepEFP, OPTFRAME_DEFAULT_ADS>
{
private:
	MoveNEIGHAddX* m;
	vector<MoveNEIGHAddX*> moves;
	int index;
	const RepEFP& rep;
	ProblemInstance& pEFP;
	double applyValue;
public:
	NSIteratorNEIGHAddX(const RepEFP& _rep, ProblemInstance& _pEFP,double _applyValue) :
			rep(_rep), pEFP(_pEFP),applyValue(_applyValue)
	{
		index = 0;
		m = NULL;
	}

	virtual ~NSIteratorNEIGHAddX()
	{
		for (int i = index + 1; i < moves.size(); i++)
			delete moves[i];

	}

	virtual void first()
	{
		vector<double> values;
		values.push_back(applyValue);


		int options = rep.singleFuzzyRS.size(); //rep.size() options

		for (int sign = 0; sign < 2; sign++)
			for (int r = 0; r < NCOLUMNATRIBUTES; r++)
				for (int o = 0; o < options; o++)
					for (int v = 0; v < values.size(); v++)
					{
						moves.push_back(new MoveNEIGHAddX(r, o, values[v], sign, 0));
					}

		options = rep.averageFuzzyRS.size(); //rep.size() options

		for (int sign = 0; sign < 2; sign++)
			for (int r = 0; r < NCOLUMNATRIBUTES; r++)
				for (int o = 0; o < options; o++)
					for (int v = 0; v < values.size(); v++)
					{
						moves.push_back(new MoveNEIGHAddX(r, o, values[v], sign, 1));
					}

		options = rep.derivativeFuzzyRS.size(); //rep.size() options

		for (int sign = 0; sign < 2; sign++)
			for (int r = 0; r < NCOLUMNATRIBUTES; r++)
				for (int o = 0; o < options; o++)
					for (int v = 0; v < values.size(); v++)
					{
						moves.push_back(new MoveNEIGHAddX(r, o, values[v], sign, 2));
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

class NSSeqNEIGHAddX: public NSSeq<RepEFP>
{
private:
	RandGen& rg;
	ProblemInstance& pEFP;
	double applyValue;
public:

	using NSSeq<RepEFP>::move; // prevents name hiding

	NSSeqNEIGHAddX(ProblemInstance& _pEFP, RandGen& _rg, double _applyValue) :
			pEFP(_pEFP), rg(_rg), applyValue(_applyValue)
	{
	}

	virtual ~NSSeqNEIGHAddX()
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
			return *new MoveNEIGHAddX(-1, -1, -1, -1, -1); // return a random move
		}

		int construtive = 1;

		bool sign = rg.rand(2);

		return *new MoveNEIGHAddX(r, o, applyValue, sign, vectorType); // return a random move
	}

	virtual NSIterator<RepEFP, OPTFRAME_DEFAULT_ADS>& getIterator(const RepEFP& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return *new NSIteratorNEIGHAddX(rep, pEFP,applyValue); // return an iterator to the neighbors of 'rep'
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqAdd "<<applyValue<<" with move: ";
		return ss.str();
	}
};

}
#endif /*EFP_NSSEQADDX_HPP_*/

