#ifndef EFP_NSSEQVALPHA_HPP_
#define EFP_NSSEQVALPHA_HPP_

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

class MoveNEIGHVAlpha: public Move<RepEFP, OPTFRAME_DEFAULT_ADS>
{
private:
	int index;
	double applyValue;
	bool sign;
public:

	using Move<RepEFP, OPTFRAME_DEFAULT_ADS>::apply; // prevents name hiding
	using Move<RepEFP, OPTFRAME_DEFAULT_ADS>::canBeApplied; // prevents name hiding

	MoveNEIGHVAlpha(int _index, double _applyValue, bool _sign) :
			index(_index), applyValue(_applyValue), sign(_sign)
	{

	}

	virtual ~MoveNEIGHVAlpha()
	{
	}

	bool canBeApplied(const RepEFP& rep, const OPTFRAME_DEFAULT_ADS&)
	{

		return true;

	}

	Move<RepEFP, OPTFRAME_DEFAULT_ADS>* apply(RepEFP& rep, OPTFRAME_DEFAULT_ADS&)
	{
		if (sign == 0)
			rep.vAlpha[index] += applyValue;
		else
			rep.vAlpha[index] -= applyValue;
		// return reverse move
		return new MoveNEIGHVAlpha(index, applyValue, !sign);
	}

	virtual bool operator==(const Move<RepEFP, OPTFRAME_DEFAULT_ADS>& _m) const
	{
		const MoveNEIGHVAlpha& m = (const MoveNEIGHVAlpha&) _m;
		return ((m.applyValue == applyValue) && (m.index == index) && (m.sign == sign));
	}

	void print() const
	{
		cout << "MoveNEIGH3( vector: " << applyValue << " : ";
		cout << " index " << index << " <=>  sign " << sign << " )";
		cout << endl;
	}
}
;

class NSIteratorNEIGHVAlpha: public NSIterator<RepEFP, OPTFRAME_DEFAULT_ADS>
{
private:
	MoveNEIGHVAlpha* m;
	vector<MoveNEIGHVAlpha*> moves;
	int index;
	const RepEFP& rep;
	ProblemInstance& pEFP;
public:
	NSIteratorNEIGHVAlpha(const RepEFP& _rep, ProblemInstance& _pEFP) :
			rep(_rep), pEFP(_pEFP)
	{
		index = 0;
		m = NULL;
	}

	virtual ~NSIteratorNEIGHVAlpha()
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

		int nIndex = rep.vAlpha.size(); //rep.size() options

		for (int sign = 0; sign < 2; sign++)
			for (int i = 0; i < nIndex; i++)
				for (int v = 0; v < values.size(); v++)
				{
					moves.push_back(new MoveNEIGHVAlpha(i, values[v], sign));
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
			cout << "NSIteratorNEIGHVAlpha. Aborting." << endl;
			exit(1);
		}

		return *m;
	}

};

class NSSeqNEIGHVAlpha: public NSSeq<RepEFP>
{
private:
	RandGen& rg;
	ProblemInstance& pEFP;
	int nIntervals;
public:

	using NSSeq<RepEFP>::move; // prevents name hiding

	NSSeqNEIGHVAlpha(ProblemInstance& _pEFP, RandGen& _rg, int _nIntervals) :
			pEFP(_pEFP), rg(_rg), nIntervals(_nIntervals)
	{
	}

	virtual ~NSSeqNEIGHVAlpha()
	{
	}

	virtual Move<RepEFP, OPTFRAME_DEFAULT_ADS>& move(const RepEFP& rep, const OPTFRAME_DEFAULT_ADS&)
	{

		int i = rg.rand(rep.vAlpha.size());

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

		return *new MoveNEIGHVAlpha(i, applyValue, sign); // return a random move
	}

	virtual NSIterator<RepEFP, OPTFRAME_DEFAULT_ADS>& getIterator(const RepEFP& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return *new NSIteratorNEIGHVAlpha(rep, pEFP); // return an iterator to the neighbors of 'rep'
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqChangeFuzzyAproxVAlpha with move: ";
		return ss.str();
	}
};

}
#endif /*EFP_NSSEQVALPHA_HPP_*/

