// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_DECODER_NONSORTINGGENETICALGORITHMII_HPP_
#define OPTFRAME_DECODER_NONSORTINGGENETICALGORITHMII_HPP_

#include <algorithm>

#include "../../ExtendedMultiObjSearch.hpp"
#include "../../Evaluator.hpp"
#include "../../Evaluation.hpp"
#include "../../Population.hpp"
#include "../../InitialPopulation.h"
#include "../../RandGen.hpp"
#include "../../NSSeq.hpp"
#include "../../ExtendedParetoDominance.hpp"

#include "../../Decoder.hpp"

#include "../../Timer.hpp"

#include "../../Util/printable.h"

#define INFINITO 1000000000

namespace optframe
{

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class DecoderNSGAII: public ExtendedMultiObjSearch<R, ADS, DS>
{
private:
	Decoder<R, X, ADS, DS>& decoder;
	unsigned nObjectives;

	InitialPopulation<R>& init_pop;
	int init_pop_size;

	ExtendedParetoDominance<R, X, ADS, DS> pDominance;
	int gMax;

	static bool compara(pair<double, int> p1, pair<double, int> p2)
	{
		return p1.first > p2.first;
	}

protected:
	RandGen& rg;
public:

	//using Heuristic<R, ADS, DS >::exec; // prevents name hiding

	DecoderNSGAII(Decoder<R, X, ADS, DS>& _decoder, vector<Direction<DS>*>& _v_d, InitialPopulation<R>& _init_pop, int _init_pop_size, int _gMax, RandGen& _rg) :
			decoder(_decoder), nObjectives(_v_d.size()), init_pop(_init_pop), init_pop_size(_init_pop_size), pDominance(ExtendedParetoDominance<R, X, ADS, DS>(_v_d, _decoder)), rg(_rg)
	{
		gMax = _gMax;
		//cout << "pDOMINANCE OBJ FUNCTIONS: " << pDominance.v_d.size() << endl;
	}

	virtual ~DecoderNSGAII()
	{
	}

	virtual void basicGeneticOperators(Population<R>& p) = 0;

	void printFronts(const vector<Population<R>*>& F)
	{
		cout << "Fronts(" << F.size() << ") => ";
		for(unsigned i=0; i<F.size(); i++)
			cout << i << ": " << F[i]->size() << " ";
		cout << endl;
	}

	virtual ExtendedPareto<R, ADS, DS>* search(double timelimit = 100000000, double target_f = 0, ExtendedPareto<R, ADS, DS>* _pf = NULL)
	{
		Timer tnow;

		cout << "search: DECODER version of Non Sorting Genetic Algorithm Search II" << endl;

		Population<R> p = init_pop.generatePopulation(init_pop_size);
		int N = p.size();

		Population<R> q = p;
		basicGeneticOperators(q);

		int g = 0;
		while((g <= gMax) && (tnow.now() < timelimit))
		{
			cout << "Generation = " << g << endl;

			Population<R> r = p;
			r.add(q);

			cout << "* p=" << p.size() << " q=" << q.size() << " r=" << r.size();
			cout << endl;

			//Start NonDominance Order by sets
			vector<Population<R>*> F;
			//cout << "calling fastNonDominanatedSort()" << endl;
			fastNonDominanatedSort(F, r);
			//cout << "finished fastNonDominanatedSort()" << endl;

			printFronts(F);

			Population<R> popTemp;
			int j = 0;

			vector<double> cD; //Crowding Distance

			while((popTemp.size() + F[j]->size()) < N)
			{
				crowdingDistanceOrder(cD, *F[j]);

				for(int i = 0; i < F[j]->size(); i++)
					popTemp.push_back(F[j]->at(i));
				j++;
			}

			vector<double> cDTemp;
			crowdingDistanceOrder(cDTemp, *F[j]);

			vector<pair<double, int> > cDOrdenated;
			for(int i = 0; i < cDTemp.size(); i++)
				cDOrdenated.push_back(make_pair(cDTemp[i], i));

			sort(cDOrdenated.begin(), cDOrdenated.end(), compara);

			int popTempSize = popTemp.size();
			for(int i = 0; i < (N - popTempSize); i++)
			{
				cD.push_back(cDOrdenated[i].first);
				popTemp.push_back(F[j]->at(cDOrdenated[i].second));
			}

			p.clear();
			p = popTemp;
			popTemp.clear();

			q.clear();
			q = basicSelection(p, cD);

			basicGeneticOperators(q);

			for(int i = 0; i < F.size(); i++)
				F[i]->clear();

			r.clear();

			g++;
		}

		// FINISH ALGORITHM!

		ExtendedPareto<R, ADS, DS>* pf = new ExtendedPareto<R, ADS, DS>;
		for(unsigned i = 0; i < p.size(); i++)
		{
			Solution<R>* s = &p.at(i);

			// CALL DECODER TO EVALUATE SOLUTIONS!

			pair<vector<Solution<X, ADS>*>, vector<MultiEvaluation<DS>*> > dec_vs = decoder.decode(*s);

			if(dec_vs.first.size() > 0)
			{
				cout << "DecoderNSGAII::error: not expecting generated solutions!" << endl;
				exit(1);
			}

			pf->push_back(*s, dec_vs.second);
		}

		return pf;
	}

	void crowdingDistanceOrder(vector<double>& CD, const Population<R>& Fj)
	{
		int N = Fj.size();
		if(N > 0)
		{
			int CDOldSize = CD.size();
			for(int i = 0; i < N; i++)
				CD.push_back(0);

			for(unsigned m = 0; m < nObjectives; m++)
			{
				vector<pair<double, int> > fitness;

				for(int i = 0; i < N; i++)
				{
					pair<vector<Solution<X, ADS>*>, vector<MultiEvaluation<DS>*> > dec_vs = decoder.decode(Fj.at(i));

					if(dec_vs.first.size() > 0)
					{
						cout << "DecoderNSGAII::error: crowding not expecting generated solutions!" << endl;
						exit(1);
					}

					double sum_fit = 0;
					for(unsigned k = 0; k < dec_vs.second.size(); k++)
					{
						sum_fit += dec_vs.second[k]->at(m).evaluation();
						delete dec_vs.second[k];
					}

					fitness.push_back(make_pair(sum_fit, i));
				}

				sort(fitness.begin(), fitness.end(), compara);

				CD[CDOldSize + fitness[0].second] = INFINITO;
				CD[CDOldSize + fitness[N - 1].second] = INFINITO;

				for(int i = (CDOldSize + 1); i < (CDOldSize + N - 1); i++)
				{
					if((fitness[0].first - fitness[N - 1].first) < 0.000001)
						CD[CDOldSize + fitness[i].second] = INFINITO;
					else
						CD[CDOldSize + fitness[i].second] = CD[CDOldSize + fitness[i].second] + (fitness[i - 1].first - fitness[i + 1].first) / (fitness[0].first - fitness[N - 1].first);

				}

			}
		}
	}

	void fastNonDominanatedSort(vector<Population<R>*>& F, const Population<R>& p)
	{
		//cout << "begin fastNonDominanatedSort" << endl;
		Population<R> pAtual = p;
		Population<R>* F0 = new Population<R>;
		F.push_back(F0);

		vector<int> v_nd;
		vector<int> deleteds;

		for(int i = 0; i < pAtual.size(); i++)
		{
			//cout << "i=" << i << "/" << pAtual.size();
			//cout << endl;

			int nd = 0;

			for(int j = 0; j < pAtual.size(); j++)
				if(j != i)
				{
					if(pDominance.dominates(pAtual.at(j), pAtual.at(i)))
					{
						//cout << j << " dominates " << i << endl;
						nd++;
					}
					else
					{
						//cout <<  "ELSE: " << j << " not dominates " << i << endl;
					}
				}

			if(nd == 0)
			{
				F[0]->push_back(pAtual.at(i));
				deleteds.push_back(i);
			}
		}

		int nMax = p.size() / 2;
		int nAtual = F[0]->size();

		//cout << "delete list: " << deleteds << endl;
		for(int i = 0; i < deleteds.size(); i++)
			delete &pAtual.remove(deleteds[i] - i);

		deleteds.clear();

		int k = 0;

		//cout << "entering main loop.." << endl;
		while((F[k]->size() != 0) && (nAtual <= nMax))
		{
			k++;

			Population<R>* uTemp = new Population<R>;
			F.push_back(uTemp);

			for(int i = 0; i < pAtual.size(); i++)
			{
				int nd = 0;

				for(int j = 0; j < pAtual.size(); j++)
				{
					if(j != i)
						if(pDominance.dominates(pAtual.at(j), pAtual.at(i)))
							nd++;
				}

				if(nd == 0)
				{
					F[k]->push_back(pAtual.at(i));
					deleteds.push_back(i);
				}
			}

			for(int i = 0; i < deleteds.size(); i++)
				delete &pAtual.remove(deleteds[i] - i);

			nAtual += F[k]->size();

			deleteds.clear();
		}

		//cout << "finish fastNonDominanatedSort" << endl;
	}

	virtual Population<R> basicSelection(const Population<R>& p, vector<double> cD)
	{
		Population<R> q;
		for(int i = 0; i < p.size(); i++)
		{
			int j = rg.rand(p.size());
			while(i == j)
				j = rg.rand(p.size());

			bool A = pDominance.dominates(p.at(i), p.at(j));
			if(A)
				q.push_back(p.at(i));

			bool B = pDominance.dominates(p.at(j), p.at(i));
			if(B)
				q.push_back(p.at(j));

			if(A == B)
			{
				if(cD[i] >= cD[j])
					q.push_back(p.at(i));
				else
					q.push_back(p.at(j));
			}
		}

		return q;
	}

};

}

#endif /*OPTFRAME_DECODER_NONSORTINGGENETICALGORITHMII_HPP_*/
