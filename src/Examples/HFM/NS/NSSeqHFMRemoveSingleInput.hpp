#ifndef HFM_NSSEQREMOVESINGLEINPUT_HPP_
#define HFM_NSSEQREMOVESINGLEINPUT_HPP_

// Framework includes
#include "../../../OptFrame/NSSeq.hpp"
#include "../../../OptFrame/RandGen.hpp"

// Own includes
#include "../ProblemInstance.hpp"
#include "../Solution.h"

using namespace std;
namespace HFM
{

class MoveHFMRemoveSingleInput: public Move<RepHFM, OPTFRAME_DEFAULT_ADS>
{
private:
	int rule;
	bool reverse;
	pair<int, int> singleIndexOld;
	vector<double> singleFuzzyRSOld;

public:


	MoveHFMRemoveSingleInput(int _rule, bool _reverse, pair<int, int> _singleIndexOld, vector<double> _singleFuzzyRSOld) :
			rule(_rule), reverse(_reverse), singleIndexOld(_singleIndexOld), singleFuzzyRSOld(_singleFuzzyRSOld)
	{

	}

	virtual ~MoveHFMRemoveSingleInput()
	{
	}

	bool canBeApplied(const RepHFM& rep, const OPTFRAME_DEFAULT_ADS*)
	{
		return ((rule >= 0) && (rule < (int) rep.singleIndex.size()) && ((int) rep.singleIndex.size() > 1));
	}

	Move<RepHFM, OPTFRAME_DEFAULT_ADS>* apply(RepHFM& rep, OPTFRAME_DEFAULT_ADS*)
	{
		pair<int, int> tempSingleIndexOld;
		vector<double> tempSingleFuzzyRSOld;

		if (reverse == false)
		{
			if (rep.singleIndex.size() > 0)
			{
				tempSingleIndexOld = rep.singleIndex[rule];
				tempSingleFuzzyRSOld = rep.singleFuzzyRS[rule];

				rep.singleIndex.erase(rep.singleIndex.begin() + rule);
				rep.singleFuzzyRS.erase(rep.singleFuzzyRS.begin() + rule);
				return new MoveHFMRemoveSingleInput(rule, !reverse, tempSingleIndexOld, tempSingleFuzzyRSOld);
			}

		}
		else
		{
			rep.singleIndex.insert(rep.singleIndex.begin() + rule, singleIndexOld);
			rep.singleFuzzyRS.insert(rep.singleFuzzyRS.begin() + rule, singleFuzzyRSOld);
		}

		return new MoveHFMRemoveSingleInput(rule, !reverse, tempSingleIndexOld, tempSingleFuzzyRSOld);

	}

	virtual bool operator==(const Move<RepHFM, OPTFRAME_DEFAULT_ADS>& _m) const
	{
		const MoveHFMRemoveSingleInput& m = (const MoveHFMRemoveSingleInput&) _m;
		return ((m.rule == rule) && (m.reverse == reverse));
	}

	void print() const
	{
		cout << "MoveNEIGHRemoveSingleInput( vector:  rule " << rule << " <=>  reverse " << reverse << " )";
		cout << endl;
	}
}
;

class NSIteratorHFMRemoveSingleInput: public NSIterator<RepHFM, OPTFRAME_DEFAULT_ADS>
{
private:
	MoveHFMRemoveSingleInput* m;
	vector<MoveHFMRemoveSingleInput*> moves;
	int index;
	const RepHFM& rep;

public:
	NSIteratorHFMRemoveSingleInput(const RepHFM& _rep) :
			rep(_rep)
	{
		index = 0;
		m = nullptr;
	}

	virtual ~NSIteratorHFMRemoveSingleInput()
	{
		for (int i = index + 1; i < (int) moves.size(); i++)
			delete moves[i];
	}

	virtual void first()
	{
		pair<int, int> tempSingleIndexOld;
		vector<double> tempSingleFuzzyRSOld;

		for (int rule = 0; rule < (int) rep.singleIndex.size(); rule++)
		{
			moves.push_back(new MoveHFMRemoveSingleInput(rule, false, tempSingleIndexOld, tempSingleFuzzyRSOld));
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
			cout << "NSIteratorNEIGHRemoveSingleInput. Aborting." << endl;
			exit(1);
		}

		return m;
	}

};

class NSSeqHFMRemoveSingleInput: public NSSeq<RepHFM>
{
private:
	RandGen& rg;

public:


	NSSeqHFMRemoveSingleInput(RandGen& _rg) :
			rg(_rg)
	{
	}

	virtual ~NSSeqHFMRemoveSingleInput()
	{
	}

	virtual Move<RepHFM, OPTFRAME_DEFAULT_ADS>* randomMove(const RepHFM& rep, const OPTFRAME_DEFAULT_ADS*)
	{

		int rule = -1;
		if (rep.singleIndex.size() > 0)
			rule = rg.rand(rep.singleIndex.size());

		pair<int, int> tempSingleIndexOld;
		vector<double> tempSingleFuzzyRSOld;

		return new MoveHFMRemoveSingleInput(rule, false, tempSingleIndexOld, tempSingleFuzzyRSOld); // return a random move
	}

	virtual NSIterator<RepHFM, OPTFRAME_DEFAULT_ADS>* getIterator(const RepHFM& rep, const OPTFRAME_DEFAULT_ADS*)
	{
		return new NSIteratorHFMRemoveSingleInput(rep); // return an iterator to the neighbors of 'rep'
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqNEIGHRemoveSingleInput";
		return ss.str();
	}
};

}
#endif /*HFM_NSSEQREMOVESINGLEINPUT_HPP_*/

