#ifndef HFM_NSSEQADDSINGLEINPUT_HPP_
#define HFM_NSSEQADDSINGLEINPUT_HPP_

// Framework includes
#include "../../../OptFrame/NSSeq.hpp"
#include "../../../OptFrame/RandGen.hpp"

// Own includes
#include "../ProblemInstance.hpp"
#include "../Solution.h"

using namespace std;
namespace HFM
{

class MoveNEIGHAddSingleInput: public Move<RepHFM, OPTFRAME_DEFAULT_ADS>
{
private:
	int file, K;
	vector<double> rulesAndWeights;
	bool reverse;

public:

	MoveNEIGHAddSingleInput(int _file, int _K, vector<double> _rulesAndWeights, bool _reverse) :
			file(_file), K(_K), rulesAndWeights(_rulesAndWeights), reverse(_reverse)
	{

	}

	virtual ~MoveNEIGHAddSingleInput()
	{
	}

	bool canBeApplied(const RepHFM& rep, const OPTFRAME_DEFAULT_ADS*)
	{
		bool currentSampleFromTarget = ( (file == 0) && (K == 0) );
		return !currentSampleFromTarget;
	}

	Move<RepHFM, OPTFRAME_DEFAULT_ADS>* apply(RepHFM& rep, OPTFRAME_DEFAULT_ADS*)
	{
		if (!reverse)
		{
			rep.singleIndex.push_back(make_pair(file, K));
			if (K > rep.earliestInput)
				rep.earliestInput = K;

			vector<double> fuzzyRules;
			fuzzyRules.resize(NCOLUMNATRIBUTES);
			fuzzyRules[GREATER] = rulesAndWeights[GREATER];
			fuzzyRules[GREATER_WEIGHT] = rulesAndWeights[GREATER_WEIGHT];
			fuzzyRules[LOWER] = rulesAndWeights[LOWER];
			fuzzyRules[LOWER_WEIGHT] = rulesAndWeights[LOWER_WEIGHT];
			fuzzyRules[EPSILON] = rulesAndWeights[EPSILON];
			fuzzyRules[PERTINENCEFUNC] = rulesAndWeights[PERTINENCEFUNC];

			rep.singleFuzzyRS.push_back(fuzzyRules);
		}
		else
		{
			rep.singleIndex.pop_back();
			rep.singleFuzzyRS.pop_back();
		}
		return new MoveNEIGHAddSingleInput(file, K, rulesAndWeights, !reverse);
	}

	virtual bool operator==(const Move<RepHFM, OPTFRAME_DEFAULT_ADS>& _m) const
	{
		const MoveNEIGHAddSingleInput& m = (const MoveNEIGHAddSingleInput&) _m;
		return ((m.file == file) && (m.K == K) && (m.rulesAndWeights == rulesAndWeights));
	}

	void print() const
	{
		cout << "MoveNEIGHAddSingleInput( vector:  explatonary variable " << file << " <=>  k " << K;
		cout << "rules and weights " << rulesAndWeights << " )";
		cout << endl;
	}
}
;

class NSIteratorNEIGHAddSingleInput: public NSIterator<RepHFM, OPTFRAME_DEFAULT_ADS>
{
private:
	const RepHFM& rep;
	vector<int> vMaxLag, vMaxUpperLag;
	HFMProblemInstance& pEFP;
	RandGen& rg;

	MoveNEIGHAddSingleInput* m;
	vector<MoveNEIGHAddSingleInput*> moves;
	int index;

public:
	NSIteratorNEIGHAddSingleInput(const RepHFM& _rep, vector<int> _vMaxLag, vector<int> _vMaxUpperLag, HFMProblemInstance& _pEFP, RandGen& _rg) :
			rep(_rep), vMaxLag(_vMaxLag), vMaxUpperLag(_vMaxUpperLag), pEFP(_pEFP), rg(_rg)
	{
		index = 0;
		m = nullptr;
	}

	virtual ~NSIteratorNEIGHAddSingleInput()
	{
		for (int i = index + 1; i < (int) moves.size(); i++)
			delete moves[i];
	}

	virtual void first()
	{
		cout<<toString()<<" iterator is not working properly. Iterator should be improved"<<endl;
		exit(1);
		int nEXV = 0; //TODO
		int maxLag = vMaxLag[nEXV];

		for (int lag = 1; lag <= maxLag; lag++)
		{
			int mean = pEFP.getMean(nEXV);
			int stdDesv = pEFP.getStdDesv(nEXV);
			double meanWeight = pEFP.getMean(0); //File 0 is the target file
			double stdDesvWeight = pEFP.getStdDesv(0);

			double greater = rg.randG(mean, stdDesv);
			double lower = rg.randG(mean, stdDesv);
			double greaterWeight = rg.randG(meanWeight, stdDesvWeight);
			double lowerWeight = rg.randG(meanWeight, stdDesvWeight);
			double fuzzyFunction = rg.rand(N_Activation_Functions);
			double epsilon = 1; //forcing to 1, trapezoid functions - TODO

			vector<double> rulesAndWeights =
			{ greater, greaterWeight, lower, lowerWeight, epsilon, fuzzyFunction };

			moves.push_back(new MoveNEIGHAddSingleInput(0, lag, rulesAndWeights, false));
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
			cout << "NSIteratorNEIGHChangeSingleInput. Aborting." << endl;
			exit(1);
		}

		return m;
	}

};

class NSSeqNEIGHAddSingleInput: public NSSeq<RepHFM>
{
private:
	HFMProblemInstance& pEFP;
	RandGen& rg;

	vector<int> vMaxLag, vMaxUpperLag;

public:

	NSSeqNEIGHAddSingleInput(HFMProblemInstance& _pEFP, RandGen& _rg, vector<int> _vMaxLag, vector<int> _vMaxUpperLag) :
			pEFP(_pEFP), rg(_rg), vMaxLag(_vMaxLag), vMaxUpperLag(_vMaxUpperLag)
	{
	}

	virtual ~NSSeqNEIGHAddSingleInput()
	{
	}

	virtual Move<RepHFM, OPTFRAME_DEFAULT_ADS>* randomMove(const RepHFM& rep, const OPTFRAME_DEFAULT_ADS*)
	{
		//TODO - Check the possibility of add negative K values
		int nEXV = rg.rand(pEFP.getNumberExplanatoryVariables());
		int maxLag = vMaxLag[nEXV];

//		cout<<"nEXV:"<<nEXV<<endl;

		int K = rg.rand(maxLag);

		int mean = pEFP.getMean(nEXV);
		int stdDesv = pEFP.getStdDesv(nEXV);
		double meanWeight = pEFP.getMean(0); //File 0 is the target file
		double stdDesvWeight = pEFP.getStdDesv(0);

		double greater = rg.randG(mean, stdDesv);
		double lower = rg.randG(mean, stdDesv);
//		double greaterWeight = rg.randG(meanWeight, stdDesvWeight);
//		double lowerWeight = rg.randG(meanWeight, stdDesvWeight);
		double greaterWeight = rg.randG(stdDesvWeight, stdDesvWeight);
		double lowerWeight = rg.randG(stdDesvWeight, stdDesvWeight);
		double fuzzyFunction = rg.rand(N_Activation_Functions);
		double epsilon = 1; //forcing to 1, trapezoid functions - TODO

		vector<double> rulesAndWeights =
		{ greater, greaterWeight, lower, lowerWeight, epsilon, fuzzyFunction };
		return new MoveNEIGHAddSingleInput(nEXV, K, rulesAndWeights, false); // return a random move
	}

	virtual NSIterator<RepHFM, OPTFRAME_DEFAULT_ADS>* getIterator(const RepHFM& rep, const OPTFRAME_DEFAULT_ADS*)
	{
		return new NSIteratorNEIGHAddSingleInput(rep, vMaxLag, vMaxUpperLag, pEFP, rg); // return an iterator to the neighbors of 'rep'
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqNEIGHAddSingleInput";
		return ss.str();
	}
};

}
#endif /*HFM_NSSEQADDSINGLEINPUT_HPP_*/

