#ifndef HFM_NSSEQVALPHA_HPP_
#define HFM_NSSEQVALPHA_HPP_

// Framework includes
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/RandGen.hpp>

// Own includes
#include "../ProblemInstance.hpp"
#include "../Solution.h"

using namespace std;
namespace HFM
{

class MoveNEIGHVAlpha: public Move<ESolutionHFM>
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

	bool canBeApplied(const ESolutionHFM& se) override
	{
		return true;
	}

	uptr<Move<ESolutionHFM>> apply(ESolutionHFM& se) override
	{
      RepHFM& rep = se.first.getR();
		if (sign == 0)
			rep.vAlpha[index] += applyValue;
		else
			rep.vAlpha[index] -= applyValue;
		// return reverse move
		return uptr<Move<ESolutionHFM>>(new MoveNEIGHVAlpha(index, applyValue, !sign));
	}

	virtual bool operator==(const Move<ESolutionHFM>& _m) const
	{
		const MoveNEIGHVAlpha& m = (const MoveNEIGHVAlpha&) _m;
		return ((m.applyValue == applyValue) && (m.index == index) && (m.sign == sign));
	}

	void print() const override
	{
		cout << "MoveNEIGHVAlpha( vector: " << applyValue << " : ";
		cout << " index " << index << " <=>  sign " << sign << " )";
		cout << endl;
	}
}
;

class NSIteratorNEIGHVAlpha: public NSIterator<ESolutionHFM>
{
private:
	//MoveNEIGHVAlpha* m;
	//vector<uptr<MoveNEIGHVAlpha>> moves;
   uptr<Move<ESolutionHFM>> m; // general move
   vector<uptr<Move<ESolutionHFM>>> moves; // general moves

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
		//for (int i = index + 1; i < (int) moves.size(); i++)
		//	delete moves[i];
	}

	virtual void first() override
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
					moves.push_back(uptr<Move<ESolutionHFM>>(new MoveNEIGHVAlpha(i, values[v], sign)));
				}

		if (moves.size() > 0)
		{
			m = std::move(moves[index]); // stealing from vector... verify if this is correct! otherwise, must need clone() on Move
		}
		else
			m = nullptr;
	}

	virtual void next() override
	{
		index++;
		if (index < (int) moves.size())
		{
			m = std::move(moves[index]); // stealing from vector... verify if this is correct! otherwise, must need clone() on Move
		}
		else
			m = nullptr;
	}

	virtual bool isDone() override
	{
		return m == nullptr;
	}

	virtual uptr<Move<ESolutionHFM>> current() override
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSIteratorNEIGHVAlpha. Aborting." << endl;
			exit(1);
		}

		uptr<Move<ESolutionHFM>> m2 = std::move(m);
      m = nullptr;
      return m2;
	}

};

//This NS is used for adapting weights of an approximation of the estimations
class NSSeqNEIGHVAlpha: public NSSeq<ESolutionHFM>
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

	virtual uptr<Move<ESolutionHFM>> randomMove(const ESolutionHFM& se) override
	{
      const RepHFM& rep = se.first.getR();
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

		return uptr<Move<ESolutionHFM>>(new MoveNEIGHVAlpha(i, applyValue, sign)); // return a random move
	}

	virtual uptr<NSIterator<ESolutionHFM>> getIterator(const ESolutionHFM& se) override
	{
		return uptr<NSIterator<ESolutionHFM>>(new NSIteratorNEIGHVAlpha(se.first.getR(), pEFP)); // return an iterator to the neighbors of 'rep'
	}

	virtual string toString() const override
	{
		stringstream ss;
		ss << "NSSeqHFMChangeFuzzyAproxVAlpha with move: ";
		return ss.str();
	}
};

}
#endif /*HFM_NSSEQVALPHA_HPP_*/

