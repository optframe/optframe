/*
 * EFPESContinous.h
 *
 *  Created on: 01/09/2014
 *      Author: vitor
 */

#ifndef EFPESCONTINOUS_HPP_
#define EFPESCONTINOUS_HPP_

#include "../../OptFrame/Heuristics/EvolutionaryAlgorithms/ESContinuous.hpp"

namespace HFM
{

struct esParameters
{
	vector<vector<double> > singleParams; //single inputs relationships
	vector<vector<double> > averageParams; //single inputs relationships
	vector<vector<double> > derivativeParams; //single inputs relationships
	double alphaParam;
	vector<double> vAlphaParams;
	vector<double> vIndexAlphasParams;
};

class EFPESContinous: public ESContinous<RepHFM, OPTFRAME_DEFAULT_ADS, esParameters>
{
private:
	RandGen& rg;
	double initialDesv;
	double mutationDesv;


public:

	using ESContinous<RepHFM, OPTFRAME_DEFAULT_ADS,esParameters>::search; // prevents name hiding

	EFPESContinous(HFMEvaluator& _eval, Constructive<RepHFM>& _constructive, vector<NSSeq<RepHFM>*> _vNS, LocalSearch<RepHFM>& _ls, int _mi, int _lambda, int _gMax, RandGen& _rg, double _initialDesv, double _mutationDesv) :
			ESContinous<RepHFM, OPTFRAME_DEFAULT_ADS, esParameters>(_eval, _constructive, _vNS, _ls, _mi, _lambda, _gMax), rg(_rg), initialDesv(_initialDesv), mutationDesv(_mutationDesv)
	{

	}

	virtual ~EFPESContinous()
	{

	}

	void changeParameters(ESContinuousStructure<esParameters>* p)
	{

		int sizeSP = p->desvs.singleParams.size();
		int sizeMP = p->desvs.averageParams.size();
		int sizeDP = p->desvs.derivativeParams.size();
		for (int nSP = 0; nSP < sizeSP; nSP++)
		{
			for (int i = 0; i < 5; i++) //TODO Epsilon
				p->desvs.singleParams[nSP][i] += rg.randG(0, mutationDesv);
		}

		for (int nMP = 0; nMP < sizeMP; nMP++)
		{
			for (int i = 0; i < 5; i++)
				p->desvs.averageParams[nMP][i] += rg.randG(0, mutationDesv);
		}

		for (int nDP = 0; nDP < sizeDP; nDP++)
		{
			for (int i = 0; i < 5; i++)
				p->desvs.derivativeParams[nDP][i] += rg.randG(0, mutationDesv);
		}

		p->desvs.alphaParam += rg.randG(0, 1);

		int sizeAjustsParams = p->desvs.vAlphaParams.size();
		for (int aP = 0; aP < sizeAjustsParams; aP++)
		{
			p->desvs.vAlphaParams[aP] += rg.randG(0, 1);
		}

		/*int sizeIndexAlphasAjustsParams = p->desvs.vIndexAlphasParams.size();
		 for (int aP = 0; aP < sizeIndexAlphasAjustsParams; aP++)
		 {
		 p->desvs.vIndexAlphasParams[aP] += rg.randG(0, 1);
		 }*/

	}

	void applyParameters(Solution<RepHFM>* s, ESContinuousStructure<esParameters>* p)
	{
		//cout<<p->desvs.singleParams<<endl;

		int sizeSP = s->getR().singleFuzzyRS.size();
		int sizeMP = s->getR().averageFuzzyRS.size();
		int sizeDP = s->getR().derivativeFuzzyRS.size();

		for (int nSP = 0; nSP < sizeSP; nSP++)
		{
			for (int i = 0; i < 5; i++)
			{
				double desv = p->desvs.singleParams[nSP][i];
				s->getR().singleFuzzyRS[nSP][i] += rg.randG(0, desv);
			}
		}

		for (int nMP = 0; nMP < sizeMP; nMP++)
		{
			for (int i = 0; i < 5; i++)
			{
				double desv = p->desvs.averageParams[nMP][i];
				s->getR().averageFuzzyRS[nMP][i] += rg.randG(0, desv);
			}
		}

		for (int nDP = 0; nDP < sizeDP; nDP++)
		{
			for (int i = 0; i < 5; i++)
			{
				double desv = p->desvs.derivativeParams[nDP][i];
				s->getR().derivativeFuzzyRS[nDP][i] += rg.randG(0, desv);
			}
		}

		s->getR().alpha += rg.randG(0, p->desvs.alphaParam);

		int sizeAjustsParams = s->getR().vAlpha.size();
		for (int aP = 0; aP < sizeAjustsParams; aP++)
		{
			s->getR().vAlpha[aP] += rg.randG(0, p->desvs.vAlphaParams[aP]);
		}

		/*
		 int sizeIndexAlphasAjustsParams = p->desvs.vIndexAlphasParams.size();
		 for (int aP = 0; aP < sizeIndexAlphasAjustsParams; aP++)
		 {
		 s->getR().vIndexAlphas[aP] += rg.randG(0, p->desvs.vIndexAlphasParams[aP]);

		 //This part is out of the classical Evolution Strategy, an adaptation
		 int changeIndex = rg.rand(3); //0 decrease, 1 no change and 2 increase
		 if (changeIndex == 0)
		 {
		 s->getR().vIndex[aP]--;
		 if (s->getR().vIndex[aP] < 1)
		 s->getR().vIndex[aP] = 1;
		 }
		 if (changeIndex == 2)
		 {
		 s->getR().vIndex[aP]++;
		 if (s->getR().vIndex[aP] > s->getR().notUsedForTest)
		 s->getR().vIndex[aP] = s->getR().notUsedForTest;
		 }
		 }*/

	}

	esParameters generateInitialESStructure(Solution<RepHFM>* s)
	{
		esParameters params;
		int sizeSP = s->getR().singleFuzzyRS.size();
		int sizeMP = s->getR().averageFuzzyRS.size();
		int sizeDP = s->getR().derivativeFuzzyRS.size();
		/*
		 cout << sizeSP << endl;
		 cout << sizeMP << endl;
		 cout << sizeDP << endl;
		 s->print();
		 getchar();*/

		vector<vector<double> > singleParams(sizeSP); //single inputs relationships
		vector<vector<double> > averageParams(sizeMP); //single inputs relationships
		vector<vector<double> > derivativeParams(sizeDP); //single inputs relationships

		for (int nSP = 0; nSP < sizeSP; nSP++)
		{
			for (int i = 0; i < 5; i++)
			{
				int initialDesvR = rg.rand(initialDesv) + 1;
				singleParams[nSP].push_back(initialDesvR);
			}
		}

		for (int nMP = 0; nMP < sizeMP; nMP++)
		{
			for (int i = 0; i < 5; i++)
			{
				int initialDesvR = rg.rand(initialDesv) + 1;
				averageParams[nMP].push_back(initialDesvR);
			}
		}

		for (int nDP = 0; nDP < sizeDP; nDP++)
		{
			for (int i = 0; i < 5; i++)
			{
				int initialDesvR = rg.rand(initialDesv) + 1;
				derivativeParams[nDP].push_back(initialDesvR);
			}
		}

		params.singleParams = singleParams;
		params.averageParams = averageParams;
		params.derivativeParams = derivativeParams;

		params.alphaParam = 1;

		int sizeAjustsParams = s->getR().vAlpha.size();
		params.vAlphaParams.resize(sizeAjustsParams, rg.rand01());

		//params.vIndexAlphasParams.resize(s->getR().vIndexAlphas.size(), 1);

		return params;
	}
};

}
#endif /* EFPESCONTINOUS_HPP_ */
