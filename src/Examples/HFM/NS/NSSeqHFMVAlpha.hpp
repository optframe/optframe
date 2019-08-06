#ifndef HFM_NSSEQVALPHA_HPP_
#define HFM_NSSEQVALPHA_HPP_

// Framework includes
#include "../../../OptFrame/NSSeq.hpp"
#include "../../../OptFrame/RandGen.hpp"

// Own includes
#include "../ProblemInstance.hpp"
#include "../Solution.h"

using namespace std;
namespace HFM
{

class MoveNEIGHVAlpha: public Move<RepHFM, OPTFRAME_DEFAULT_ADS>
{
private:
	int index;
	double applyValue;
	bool sign;
public:


	MoveNEIGHVAlpha(int _index, double _applyValue, bool _sign) :
			index(_index), applyValue(_applyValue), sign(_sign)
	{

	}

	virtual ~MoveNEIGHVAlpha()
	{
	}

	bool canBeApplied(const RepHFM& rep, const OPTFRAME_DEFAULT_ADS*)
	{
		return true;
	}

	Move<RepHFM, OPTFRAME_DEFAULT_ADS>* apply(RepHFM& rep, OPTFRAME_DEFAULT_ADS*)
	{
		if (sign == 0)
			rep.vAlpha[index] += applyValue;
		else
			rep.vAlpha[index] -= applyValue;
		// return reverse move
		return new MoveNEIGHVAlpha(index, applyValue, !sign);
	}

	virtual bool operator==(const Move<RepHFM, OPTFRAME_DEFAULT_ADS>& _m) const
	{
		const MoveNEIGHVAlpha& m = (const MoveNEIGHVAlpha&) _m;
		return ((m.applyValue == applyValue) && (m.index == index) && (m.sign == sign));
	}

	void print() const
	{
		cout << "MoveNEIGHVAlpha( vector: " << applyValue << " : ";
		cout << " index " << index << " <=>  sign " << sign << " )";
		cout << endl;
	}
}
;

class NSIteratorNEIGHVAlpha: public NSIterator<RepHFM, OPTFRAME_DEFAULT_ADS>
{
private:
	MoveNEIGHVAlpha* m;
	vector<MoveNEIGHVAlpha*> moves;
	int index;
	const RepHFM& rep;
	HFMProblemInstance& pEFP;
public:
	NSIteratorNEIGHVAlpha(const RepHFM& _rep, HFMProblemInstance& _pEFP) :
			rep(_rep), pEFP(_pEFP)
	{
		index = 0;
		m = nullptr;
	}

	virtual ~NSIteratorNEIGHVAlpha()
	{
		for (int i = index + 1; i < (int) moves.size(); i++)
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
				for (int v = 0; v < (int) values.size(); v++)
				{
					moves.push_back(new MoveNEIGHVAlpha(i, values[v], sign));
				}

		if (moves.size() > 0)
		{
			m = moves[index];
		}
		else
			m = nullptr;
	}

	virtual void next()
	{
		index++;
		if (index < (int) moves.size())
		{
			m = moves[index];
		}
		else
			m = nullptr;
	}

	virtual bool isDone()
	{
		return m == nullptr;
	}

	virtual Move<RepHFM, OPTFRAME_DEFAULT_ADS>* current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSIteratorNEIGHVAlpha. Aborting." << endl;
			exit(1);
		}

		return m;
	}

};

//This NS is used for adapting weights of an approximation of the estimations
class NSSeqNEIGHVAlpha: public NSSeq<RepHFM>
{
private:
	HFMProblemInstance& pEFP;
	RandGen& rg;
	int nIntervals;
public:


	NSSeqNEIGHVAlpha(HFMProblemInstance& _pEFP, RandGen& _rg, int _nIntervals) :
			pEFP(_pEFP), rg(_rg), nIntervals(_nIntervals)
	{
	}

	virtual ~NSSeqNEIGHVAlpha()
	{
	}

	virtual Move<RepHFM, OPTFRAME_DEFAULT_ADS>* randomMove(const RepHFM& rep, const OPTFRAME_DEFAULT_ADS*)
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

		return new MoveNEIGHVAlpha(i, applyValue, sign); // return a random move
	}

	virtual NSIterator<RepHFM, OPTFRAME_DEFAULT_ADS>& getIterator(const RepHFM& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return *new NSIteratorNEIGHVAlpha(rep, pEFP); // return an iterator to the neighbors of 'rep'
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqHFMChangeFuzzyAproxVAlpha with move: ";
		return ss.str();
	}
};

}
#endif /*HFM_NSSEQVALPHA_HPP_*/

