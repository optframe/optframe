#ifndef HFM_NSSEQCHANGESINGLEINPUT_HPP_
#define HFM_NSSEQCHANGESINGLEINPUT_HPP_

// Framework includes
#include "../../../OptFrame/NSSeq.hpp"
#include "../../../OptFrame/RandGen.hpp"

// Own includes
#include "../ProblemInstance.hpp"
#include "../Solution.h"

using namespace std;
namespace HFM
{

class MoveHFMChangeSingleInput: public Move<RepHFM, OPTFRAME_DEFAULT_ADS>
{
private:
	int rule;
	bool sign;
	int maxLag, maxUpperLag;
	int X;

public:

	MoveHFMChangeSingleInput(int _rule, bool _sign, int _maxLag, int _maxUpperLag, int _X) :
			rule(_rule), sign(_sign), maxLag(_maxLag), maxUpperLag(_maxUpperLag), X(_X)
	{

	}

	virtual ~MoveHFMChangeSingleInput()
	{
	}

	bool canBeApplied(const RepHFM& rep, const OPTFRAME_DEFAULT_ADS*)
	{
		bool minimumLag = false;
		bool maxLagCheck = false;
		bool notNull1 = false;
		bool notNull2 = false;
		if ((rule >= 0) && (rule < (int) rep.singleIndex.size()))
		{
			maxLagCheck = ((rep.singleIndex[rule].second + X) <= maxLag);
			minimumLag = ((rep.singleIndex[rule].second - X) >= maxUpperLag);
			if (rep.singleIndex[rule].first == 0)
			{
				notNull1 = ((rep.singleIndex[rule].second - X) != 0);
				notNull2 = ((rep.singleIndex[rule].second + X) != 0);
			}
			else
			{
				notNull1 = notNull2 = true; //TODO - BIG PROBLEM, REMOVE THIS! Otherwise endogenous will always get sample
			}
		}

		return minimumLag && maxLagCheck && notNull1 && notNull2;
	}

	Move<RepHFM, OPTFRAME_DEFAULT_ADS>* apply(RepHFM& rep, OPTFRAME_DEFAULT_ADS*)
	{

		if (!sign)
			rep.singleIndex[rule].second += X;
		else
			rep.singleIndex[rule].second -= X;

//		if (rep.singleIndex[rule].second > rep.earliestInput)
//			rep.earliestInput = rep.singleIndex[rule].second;

		return new MoveHFMChangeSingleInput(rule, !sign, maxLag, maxUpperLag, X);
	}

	virtual bool operator==(const Move<RepHFM, OPTFRAME_DEFAULT_ADS>& _m) const
	{
		const MoveHFMChangeSingleInput& m = (const MoveHFMChangeSingleInput&) _m;
		return ((m.rule == rule) && (m.sign == sign) && (m.maxLag == maxLag) && (m.maxUpperLag == maxUpperLag) && (m.X == X));
	}

	void print() const
	{
		cout << "MoveNEIGHChangeSingleInput( vector:  rule " << rule << " <=>  sign " << sign << "\t X:" << X << "\t maxLag " << maxLag << "\t maxUpperLag " << maxUpperLag << " )";
		cout << endl;
	}
}
;

class NSIteratorHFMChangeSingleInput: public NSIterator<RepHFM, OPTFRAME_DEFAULT_ADS>
{
private:
	MoveHFMChangeSingleInput* m;
	vector<MoveHFMChangeSingleInput*> moves;
	int index;
	const RepHFM& rep;
	vector<int> vMaxLag, vMaxUpperLag;

public:
	NSIteratorHFMChangeSingleInput(const RepHFM& _rep, vector<int> _vMaxLag, vector<int> _vMaxUpperLag) :
			rep(_rep), vMaxLag(_vMaxLag), vMaxUpperLag(_vMaxUpperLag)
	{
		index = 0;
		m = nullptr;
	}

	virtual ~NSIteratorHFMChangeSingleInput()
	{
		for (int i = index + 1; i < (int) moves.size(); i++)
			delete moves[i];
	}

	virtual void first()
	{
		int X = 1;
		for (int rule = 0; rule < (int) rep.singleIndex.size(); rule++)
		{
			bool sign = false;
			int expVariable = rep.singleIndex[rule].first;
			int maxLag = vMaxLag[expVariable];
			int maxUpperLag = vMaxUpperLag[expVariable];
			moves.push_back(new MoveHFMChangeSingleInput(rule, sign, maxLag, maxUpperLag, X));
			moves.push_back(new MoveHFMChangeSingleInput(rule, !sign, maxLag, maxUpperLag, X));
		}

		if (moves.size() > 0)
			m = moves[index];
		else
			m = nullptr;
	}

	virtual void next()
	{
		index++;
		if (index < (int) moves.size())
			m = moves[index];
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
			cout << "NSIteratorHFMChangeSingleInput. Aborting." << endl;
			exit(1);
		}

		return m;
	}

};

class NSSeqHFMChangeSingleInput: public NSSeq<RepHFM>
{
private:
	HFMProblemInstance& pEFP;
	RandGen& rg;
	vector<int> vMaxLag, vMaxUpperLag;

public:

	NSSeqHFMChangeSingleInput(HFMProblemInstance& _pEFP, RandGen& _rg, vector<int> _vMaxLag, vector<int> _vMaxUpperLag) :
			pEFP(_pEFP), rg(_rg), vMaxLag(_vMaxLag), vMaxUpperLag(_vMaxUpperLag)
	{
	}

	virtual ~NSSeqHFMChangeSingleInput()
	{
	}

	virtual Move<RepHFM, OPTFRAME_DEFAULT_ADS>* randomMove(const RepHFM& rep, const OPTFRAME_DEFAULT_ADS*)
	{
		int MAXCHANGE = 5;
		int X = rg.rand(MAXCHANGE) + 1;
		int rule = -1;
		if (rep.singleIndex.size() > 0)
			rule = rg.rand(rep.singleIndex.size());
		else
			return new MoveHFMChangeSingleInput(-1, -1, -1, -1, -1);

		int sign = rg.rand(2);

		int expVariable = rep.singleIndex[rule].first;
		int maxLag = vMaxLag[expVariable];
		int maxUpperLag = vMaxUpperLag[expVariable];

		return new MoveHFMChangeSingleInput(rule, sign, maxLag, maxUpperLag, X); // return a random move
	}

	virtual NSIterator<RepHFM, OPTFRAME_DEFAULT_ADS>* getIterator(const RepHFM& rep, const OPTFRAME_DEFAULT_ADS*)
	{
		return new NSIteratorHFMChangeSingleInput(rep, vMaxLag, vMaxUpperLag); // return an iterator to the neighbors of 'rep'
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqHFMChangeSingleInput";
		return ss.str();
	}
};

}
#endif /*HFM_NSSEQCHANGESINGLEINPUT_HPP_*/

