#ifndef HFM_NSSEQNEIGHMODIFYRULES_HPP_
#define HFM_NSSEQNEIGHMODIFYRULES_HPP_

// Framework includes
#include "../../../OptFrame/NSSeq.hpp"
#include "../../../OptFrame/RandGen.hpp"

// Own includes
#include "../ProblemInstance.hpp"
#include "../Solution.h"

using namespace std;
namespace HFM
{

class MoveHFMModifyRule: public Move<ESolutionHFM>
{
private:
	int r, o;
	double applyValue;
	bool sign;
	int vectorType;
public:

	MoveHFMModifyRule(int _r, int _o, double _applyValue, bool _sign, int _vectorType) :
			r(_r), o(_o), applyValue(_applyValue), sign(_sign), vectorType(_vectorType)
	{

	}

	virtual ~MoveHFMModifyRule()
	{
	}

	bool canBeApplied(const ESolutionHFM& se) override
	{
		return true;
	}

	uptr<Move<ESolutionHFM>> apply(ESolutionHFM& se) override
	{
      RepHFM& rep = se.first.getR();
		if (r == -1)
			return uptr<Move<ESolutionHFM>>(new MoveHFMModifyRule(-1, -1, -1, -1, -1));

		if (r == PERTINENCEFUNC)
		{
			if (vectorType == Single_Input)
				rep.singleFuzzyRS[o][r] = !rep.singleFuzzyRS[o][r];
			if (vectorType == Average_Inputs)
				rep.averageFuzzyRS[o][r] = !rep.averageFuzzyRS[o][r];
			if (vectorType == Derivative_Inputs)
				rep.derivativeFuzzyRS[o][r] = !rep.derivativeFuzzyRS[o][r];

			return uptr<Move<ESolutionHFM>>(new MoveHFMModifyRule(r, o, applyValue, !sign, vectorType));
		}

		if (vectorType == Single_Input)
		{

			if (!sign)
				rep.singleFuzzyRS[o][r] += applyValue;
			else
				rep.singleFuzzyRS[o][r] -= applyValue;
		}

		if (vectorType == Average_Inputs)
		{
			if (!sign)
				rep.averageFuzzyRS[o][r] += applyValue;
			else
				rep.averageFuzzyRS[o][r] -= applyValue;
		}

		if (vectorType == Derivative_Inputs)
		{
			if (!sign)
				rep.derivativeFuzzyRS.at(o).at(r) += applyValue;
			else
				rep.derivativeFuzzyRS[o][r] -= applyValue;
		}
		// return reverse move
		return uptr<Move<ESolutionHFM>>(new MoveHFMModifyRule(r, o, applyValue, !sign, vectorType));
	}

	virtual bool operator==(const Move<ESolutionHFM>& _m) const
	{
		const MoveHFMModifyRule& m = (const MoveHFMModifyRule&) _m;
		return ((m.r == r) && (m.o == o) && (m.sign == sign));
	}

	void print() const override
	{
		cout << "MoveNEIGHModifyRule( vector: " << r << " : ";
		cout << " option " << o << " <=>  sign " << sign << "vectorType " << vectorType << " )";
		cout << endl;
	}
}
;

class NSIteratorHFMModifyRules: public NSIterator<ESolutionHFM>
{
private:
	//MoveHFMModifyRule* m;
	//vector<uptr<MoveHFMModifyRule>> moves;
   uptr<Move<ESolutionHFM>> m; // general move
   vector<uptr<Move<ESolutionHFM>>> moves; // general moves
	int index;
	const RepHFM& rep;
	HFMProblemInstance& pEFP;
	vector<double>& vUpdateValues;
public:
	NSIteratorHFMModifyRules(const RepHFM& _rep, HFMProblemInstance& _pEFP, vector<double>& _vUpdateValues) :
			rep(_rep), pEFP(_pEFP), vUpdateValues(_vUpdateValues)
	{
		index = 0;
		m = nullptr;
	}

	virtual ~NSIteratorHFMModifyRules()
	{
		//for (int i = index + 1; i < (int) moves.size(); i++)
		//	delete moves[i];
	}

	virtual void first() override
	{
		int nShakes = vUpdateValues.size();
		int options = rep.singleFuzzyRS.size(); //rep.size() options

		for (int sign = 0; sign < 2; sign++)
			for (int r = 0; r < NCOLUMNATRIBUTES; r++)
				for (int o = 0; o < options; o++)
					for (int v = 0; v < nShakes; v++)
					{
						moves.push_back(uptr<Move<ESolutionHFM>>(new MoveHFMModifyRule(r, o, vUpdateValues[v], sign, 0)));
					}

		options = rep.averageFuzzyRS.size(); //rep.size() options

		for (int sign = 0; sign < 2; sign++)
			for (int r = 0; r < NCOLUMNATRIBUTES; r++)
				for (int o = 0; o < options; o++)
					for (int v = 0; v < nShakes; v++)
					{
						moves.push_back(uptr<Move<ESolutionHFM>>(new MoveHFMModifyRule(r, o, vUpdateValues[v], sign, 1)));
					}

		options = rep.derivativeFuzzyRS.size(); //rep.size() options

		for (int sign = 0; sign < 2; sign++)
			for (int r = 0; r < NCOLUMNATRIBUTES; r++)
				for (int o = 0; o < options; o++)
					for (int v = 0; v < (int) nShakes; v++)
					{
						moves.push_back(uptr<Move<ESolutionHFM>>(new MoveHFMModifyRule(r, o, vUpdateValues[v], sign, 2)));
					}

		if (moves.size() > 0)
			m = std::move(moves[index]); // stealing from vector... verify if this is correct! otherwise, must need clone() on Move
		else
			m = nullptr;
	}

	virtual void next()
	{
		index++;
		if (index < (int) moves.size())
			m = std::move(moves[index]); // stealing from vector... verify if this is correct! otherwise, must need clone() on Move
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
			cout << "NSIteratorNEIGHModifyRules. Aborting." << endl;
			exit(1);
		}

      uptr<Move<ESolutionHFM>> m2 = std::move(m);
      m = nullptr;

		return m2;
	}

};

class NSSeqHFMModifyRules: public NSSeq<ESolutionHFM>
{
private:
	HFMProblemInstance& pEFP;
	RandGen& rg;
	vector<double> vUpdateValues;
public:

	//A vector of values to be used for changing rules and weights can be passed as parameter
	NSSeqHFMModifyRules(HFMProblemInstance& _pEFP, RandGen& _rg, vector<double>* _vUpdateValues = nullptr) :
			pEFP(_pEFP), rg(_rg)
	{
		//TODO mean from the targetfile
		double mean = pEFP.getMean(0);

		if (_vUpdateValues)
		{
			cout << "Modify values given as input:" << *_vUpdateValues << endl;
			assert(_vUpdateValues->size() > 0);
			vUpdateValues = *_vUpdateValues;
		}
		else
		{
			vUpdateValues.push_back(0.01);
			vUpdateValues.push_back(0.1);
			vUpdateValues.push_back(1);
			vUpdateValues.push_back(mean / 30);
			vUpdateValues.push_back(mean / 15);
			vUpdateValues.push_back(mean / 6);
			vUpdateValues.push_back(mean / 2);
			vUpdateValues.push_back(mean);
			vUpdateValues.push_back(mean * 2);
			vUpdateValues.push_back(mean * 5);
			vUpdateValues.push_back(mean * 100);
		}
	}

	virtual ~NSSeqHFMModifyRules()
	{
	}

	virtual uptr<Move<ESolutionHFM>> randomMove(const ESolutionHFM& se) override
	{
      const RepHFM& rep = se.first.getR();
		int vectorType = rg.rand(N_Inputs_Types);
		int o = -1;
		int r = -1;

		int maxTries = 1000;
		int tries = 1;
		while ((r == -1) && (tries < maxTries))
		{
			vectorType = rg.rand(N_Inputs_Types);

			if (vectorType == Single_Input)
			{
				if (rep.singleFuzzyRS.size() > 0)
				{
					o = rg.rand(rep.singleFuzzyRS.size()); //rep.size() options
					r = rg.rand(NCOLUMNATRIBUTES);
				}
			}

			if (vectorType == Average_Inputs)
			{
				if (rep.averageFuzzyRS.size() > 0)
				{
					o = rg.rand(rep.averageFuzzyRS.size()); //rep.size() options
					r = rg.rand(NCOLUMNATRIBUTES);
				}
			}

			if (vectorType == Derivative_Inputs)
			{
				if (rep.derivativeFuzzyRS.size() > 0)
				{
					o = rg.rand(rep.derivativeFuzzyRS.size()); //rep.size() options
					r = rg.rand(NCOLUMNATRIBUTES);
				}
			}
			tries++;
		}

		if (tries == maxTries)
			return uptr<Move<ESolutionHFM>>(new MoveHFMModifyRule(-1, -1, -1, -1, -1)); // return a random move

		int applyRand = rg.rand(vUpdateValues.size());
		double applyValue = vUpdateValues.at(applyRand);
		bool sign = rg.rand(2);

		return uptr<Move<ESolutionHFM>>(new MoveHFMModifyRule(r, o, applyValue, sign, vectorType)); // return a random move
	}

	virtual uptr<NSIterator<ESolutionHFM>> getIterator(const ESolutionHFM& se) override
	{
		return uptr<NSIterator<ESolutionHFM>>(new NSIteratorHFMModifyRules(se.first.getR(), pEFP, vUpdateValues)); // return an iterator to the neighbors of 'rep'
	}

	virtual string toString() const override
	{
		stringstream ss;
		ss << "NSSeqHFMModifyRules with move: ";
		return ss.str();
	}
};

}
#endif /*HFM_NSSEQNEIGHMODIFYRULES_HPP_*/

