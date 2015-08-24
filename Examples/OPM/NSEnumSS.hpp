#ifndef OPM_NSENUMSS_HPP_
#define OPM_NSENEMSS_HPP_

// Framework includes
#include "../../OptFrame/NSEnum.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "Solution.h"

using namespace std;

namespace POLAD
{

class MoveSS: public Move<RepOPM>
{
private:
	int f1, f2;
	OPMProblemInstance& opm;

	vector<int>* vf1;
	vector<int>* vf2;

	MoveSS(int _f1, int _f2, OPMProblemInstance& _opm, vector<int>* _vf1, vector<int>* _vf2) :
		f1(_f1), f2(_f2), opm(_opm), vf1(_vf1), vf2(_vf2)
	{

	}

public:

	using Move<RepOPM>::apply; // prevents name hiding
	using Move<RepOPM>::canBeApplied; // prevents name hiding

	MoveSS(int _f1, int _f2, OPMProblemInstance& _opm) :
		f1(_f1), f2(_f2), opm(_opm)
	{
		vf1 = NULL;
		vf2 = NULL;
	}

	virtual ~MoveSS()
	{
		if (vf1)
			delete vf1;
		if (vf2)
			delete vf2;
	}

	bool canBeApplied(const RepOPM& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return f1 < f2;
	}

	Move<RepOPM>* apply(RepOPM& rep,  OPTFRAME_DEFAULT_ADS&)
	{
		int aux = rep.first[f1];
		rep.first[f1] = rep.first[f2];
		rep.first[f2] = aux;

		int car1 = rep.first[f1];
		int car2 = rep.first[f2];

		vector<int>* bckpF1 = NULL;
		vector<int>* bckpF2 = NULL;

		if (vf1 == NULL || vf2 == NULL)
		{
			bckpF1 = new vector<int> (opm.numCaminhoes);
			bckpF2 = new vector<int> (opm.numCaminhoes);

			for (unsigned c = 0; c < rep.second.getNumCols(); c++)
			{
				(*bckpF1)[c] = rep.second(f1, c);
				(*bckpF2)[c] = rep.second(f2, c);

				if ((car1 == -1) || (!opm.isCompatible(c, car1)))
					rep.second(f1, c) = 0;
				if ((car2 == -1) || (!opm.isCompatible(c, car2)))
					rep.second(f2, c) = 0;
			}
		}
		else
			for (unsigned int c = 0; c < rep.second.getNumCols(); c++)
			{
				rep.second(f1, c) = vf1->at(c);
				rep.second(f2, c) = vf2->at(c);
			}

		return new MoveSS(f1, f2, opm, bckpF1, bckpF2);
	}

	virtual bool operator==(const Move<RepOPM>& _m) const
	{
		const MoveSS& m = (const MoveSS&) _m;
		return ((f1 == m.f1) && (f2==m.f2));
	}

	void print() const
	{
		cout << "MoveSS(" << f1 << "," << f2 << ")" << endl;
	}
};

class NSEnumSS: public NSEnum<RepOPM>
{
private:
	OPMProblemInstance& opm;
public:

	using NSSeq<RepOPM>::move; // prevents name hiding

	NSEnumSS(OPMProblemInstance& _opm, RandGen& _rg) :
		NSEnum<RepOPM>(_rg), opm(_opm)
	{
	}

	virtual ~NSEnumSS()
	{
	}

	Move<RepOPM>& move(unsigned int m)
	{
		if (m > size())
		{
			cerr << "Neighborhood SS Error! Move " << m << " doesnt exist! Valid Interval from 0 to " << (size() - 1) << "." << endl;
			exit(1);
		}

		int num_f = opm.getNumFrentes();

		/* Implementação O(numFrentes²) *

		 for(int i=0;i<num_f;i++)
		 for(int j=0;j<num_f;j++)
		 if(k==0)
		 return new MovimentoCG(i,j);
		 else
		 k--; */

		/* Implementação O(1) */

		if (m < num_f * num_f)
		{
			int i = floor((double) m / num_f);
			int j = m % num_f;
			return *new MoveSS(i, j, opm);
		} /**/

		cerr << "Erro: estouro da vizinhanca SS. Nao existe o movimento " << m << ". Intervalo de 0.." << (size() - 1) << "." << endl;
		exit(1);

	}

	unsigned int size() const
	{
		return opm.getNumFrentes() * (opm.getNumFrentes() - 1) / 2;
	}

	void print()
	{
		cout << "NSEnum SS (" << size() << ")" << endl;
	}
};
}

#endif /*OPM_NSENUMSS_HPP_*/

