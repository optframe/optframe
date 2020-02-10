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

class MoveHFMRemoveSingleInput: public Move<SolutionHFM>
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

	bool canBeApplied(const SolutionHFM& s) override
	{
      const RepHFM& rep = s.getR();
		return ((rule >= 0) && (rule < (int) rep.singleIndex.size()) && ((int) rep.singleIndex.size() > 1));
	}

	Move<SolutionHFM>* apply(SolutionHFM& s) override
	{
      RepHFM& rep = s.getR();
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

	virtual bool operator==(const Move<SolutionHFM>& _m) const
	{
		const MoveHFMRemoveSingleInput& m = (const MoveHFMRemoveSingleInput&) _m;
		return ((m.rule == rule) && (m.reverse == reverse));
	}

	void print() const override
	{
		cout << "MoveNEIGHRemoveSingleInput( vector:  rule " << rule << " <=>  reverse " << reverse << " )";
		cout << endl;
	}
}
;

class NSIteratorHFMRemoveSingleInput: public NSIterator<SolutionHFM>
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

	virtual void first() override
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

	virtual bool isDone() override
	{
		return m == nullptr;
	}

	virtual Move<SolutionHFM>* current() override
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

class NSSeqHFMRemoveSingleInput: public NSSeq<SolutionHFM>
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

	virtual Move<SolutionHFM>* randomMove(const SolutionHFM& s)
	{
      const RepHFM& rep = s.getR();
		int rule = -1;
		if (rep.singleIndex.size() > 0)
			rule = rg.rand(rep.singleIndex.size());

		pair<int, int> tempSingleIndexOld;
		vector<double> tempSingleFuzzyRSOld;

		return new MoveHFMRemoveSingleInput(rule, false, tempSingleIndexOld, tempSingleFuzzyRSOld); // return a random move
	}

	virtual NSIterator<SolutionHFM>* getIterator(const SolutionHFM& s) override
	{
      const RepHFM& rep = s.getR();
		return new NSIteratorHFMRemoveSingleInput(rep); // return an iterator to the neighbors of 'rep'
	}

	virtual string toString() const override
	{
		stringstream ss;
		ss << "NSSeqNEIGHRemoveSingleInput";
		return ss.str();
	}
};

}
#endif /*HFM_NSSEQREMOVESINGLEINPUT_HPP_*/

