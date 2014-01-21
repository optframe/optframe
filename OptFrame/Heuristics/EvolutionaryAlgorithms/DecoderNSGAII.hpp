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
#include "../../ParetoDominance.hpp"

#include "../../Decoder.hpp"

#include "../../Timer.hpp"

#include "../../Util/printable.h"

#define INFINITO 1000000000

namespace optframe
{

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class IndividualExtNSGAII
{
public:
	Solution<R>* s;
	int rank;
	double distance;

	vector<Solution<X, ADS>*> genSols;
	vector<MultiEvaluation<DS>*> genSolsEval;
	vector<double> vDist; // crowding distance for each part of evaluation
	vector<int> vRank;    // ranking for each part of evaluation

	IndividualExtNSGAII(Solution<R>* _s) :
			s(_s)
	{
		rank = -1;
		distance = -1;
		if(s == NULL)
		{
			cout << "IndividualExtNSGAII error: not expecting NULL solution (and not using references for now)!" << endl;
			exit(1);
		}
	}

	virtual ~IndividualExtNSGAII()
	{
	}

	void updateDist()
	{
		// find 'max' of parts
		double max = vDist[0];
		for(unsigned i=1; i<vDist.size(); i++)
			if(vDist[i]>max)
				max = vDist[i];
		distance = max;
	}

	void updateRank()
	{
		// find 'min' of parts
		double min = vRank[0];
		for(unsigned i=1; i<vRank.size(); i++)
			if(vRank[i]< min)
				min = vRank[i];
		rank = min;
	}

	IndividualExtNSGAII<R, X, ADS, DS>& clone() const
	{
		IndividualExtNSGAII<R, X, ADS, DS>* ind = new IndividualExtNSGAII<R, X, ADS, DS>(&s->clone());
		ind->rank = rank;
		ind->distance = distance;
		for(unsigned i = 0; i < genSols.size(); i++)
			ind->genSols.push_back(&genSols[i]->clone());
		for(unsigned i = 0; i < genSolsEval.size(); i++)
			ind->genSolsEval.push_back(&genSolsEval[i]->clone());

		return *ind;
	}

};

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class DecoderNSGAII: public ExtendedMultiObjSearch<R, ADS, DS>
{
private:
	Decoder<R, X, ADS, DS>& decoder;
	vector<Direction<DS>*>& v_d;
	unsigned nObjectives;

	InitialPopulation<R>& init_pop;
	int N;

	ParetoDominance<R, ADS, DS> pDominance;
	int gMax;

	static bool compara(pair<double, pair<int, int> > p1, pair<double, pair<int, int> > p2)
	{
		return p1.first > p2.first;
	}

protected:
	RandGen& rg;
public:

	//using Heuristic<R, ADS, DS >::exec; // prevents name hiding

	DecoderNSGAII(Decoder<R, X, ADS, DS>& _decoder, vector<Direction<DS>*>& _v_d, InitialPopulation<R>& _init_pop, int _init_pop_size, int _gMax, RandGen& _rg) :
			decoder(_decoder), v_d(_v_d), nObjectives(_v_d.size()), init_pop(_init_pop), N(_init_pop_size), pDominance(ParetoDominance<R, ADS, DS>(_v_d)), rg(_rg)
	{
		gMax = _gMax;
		//cout << "pDOMINANCE OBJ FUNCTIONS: " << pDominance.v_d.size() << endl;
	}

	virtual ~DecoderNSGAII()
	{
	}

	// base population 'p' and desired final population size 'p_size'
	virtual vector<IndividualExtNSGAII<R, X, ADS, DS>*> basicGeneticOperators(const vector<IndividualExtNSGAII<R, X, ADS, DS>*>& p, int p_size) = 0;

	void freeFronts(vector<vector<int>*>& F)
	{
		for(unsigned i = 0; i < F.size(); i++)
			delete F[i];
		F.clear();
	}

	void printFronts(const vector<vector<int>*>& F)
	{
		cout << "Fronts(" << F.size() << ") => ";
		for(unsigned i = 0; i < F.size(); i++)
			cout << i << ": " << F[i]->size() << " ";
		cout << endl;
	}

	vector<IndividualExtNSGAII<R, X, ADS, DS>*>* convert(Population<R>& p)
	{
		vector<IndividualExtNSGAII<R, X, ADS, DS>*>* vp = new vector<IndividualExtNSGAII<R, X, ADS, DS>*>;
		for(unsigned i = 0; i < p.size(); i++)
		{
			Solution<R>* s = &p.at(i);
			vp->push_back(new IndividualExtNSGAII<R, X, ADS, DS>(s));
		}
		return vp;
	}

	// compare if 'ind1' is better than 'ind2'
	static bool crowdedComparison(const IndividualExtNSGAII<R, X, ADS, DS>& ind1, const IndividualExtNSGAII<R, X, ADS, DS>& ind2)
	{
		return (ind1.rank < ind2.rank) || ((ind1.rank == ind2.rank) && (ind1.distance > ind2.distance));
	}

	virtual ExtendedPareto<R, ADS, DS>* search(double timelimit = 100000000, double target_f = 0, ExtendedPareto<R, ADS, DS>* _pf = NULL)
	{
		Timer tnow;

		cout << "search: DECODER version of Non Sorting Genetic Algorithm Search II" << endl;

		// 1. create population 'p0' of size 'N'
		Population<R>* p0 = &init_pop.generatePopulation(N);

		vector<IndividualExtNSGAII<R, X, ADS, DS>*>* p = convert(*p0);
		p0->clearNoKill();
		delete p0;

		// 2. sort population by non domination
		vector<vector<int>*> F = fastNonDominatedSort(*p);

		// 3. update archive (how??)

		// 4. create offspring of size 'N'
		vector<IndividualExtNSGAII<R, X, ADS, DS>*> q = makeNewPopulation(*p);

		int t = 0;
		while((t <= gMax) && (tnow.now() < timelimit))
		{
			cout << "Generation = " << t << endl;

			cout << "N=" << N << " p.size()=" << p->size() << " q.size()=" << q.size();
			cout << endl;

			// 1. combine populations in 'Rt': Rt = Pt U Qt;
			vector<IndividualExtNSGAII<R, X, ADS, DS>*>* r = p;
			r->insert(r->end(), q.begin(), q.end());

			// 2. sort population by non domination
			freeFronts(F);
			F = fastNonDominatedSort(*r);
			printFronts(F);

			// 3. Pt+1 = {}
			vector<IndividualExtNSGAII<R, X, ADS, DS>*>* nextPop = new vector<IndividualExtNSGAII<R, X, ADS, DS>*>;

			// 4. i=1
			int i = 0;

			//vector<double> cD;
			while((nextPop->size() + F[i]->size()) <= N)
			{
				crowdingDistanceAssignment(*F[i], *r);

				for(int j = 0; j < F[i]->size(); j++)
					nextPop->push_back(F[i]->at(j));
				i = i + 1;
			}

			vector<double> cDTemp;
			crowdingDistanceOrder(cDTemp, *F[i]);

			vector<pair<double, int> > cDOrdenated;
			for(int j = 0; j < cDTemp.size(); j++)
				cDOrdenated.push_back(make_pair(cDTemp[j], j));

			sort(cDOrdenated.begin(), cDOrdenated.end(), compara);

			int popTempSize = nextPop->size();
			for(int j = 0; j < (N - popTempSize); j++)
			{
				cD.push_back(cDOrdenated[j].first);
				nextPop->push_back(F[i]->at(cDOrdenated[j].second)); //todo: leak
			}

			delete r; // also deleted 'p'
			r = NULL;

			p = nextPop;

			q.clear();

			q = *p;
			basicGeneticOperators(q);

			// TODO: fixing..
			//q =
			vector<IndividualExtNSGAII<R, X, ADS, DS>*> q1 = basicSelection(*p1);

			for(int k = 0; k < F.size(); k++)
				delete F[k];

			//r.clear();

			g++;
		}

		// FINISH ALGORITHM!

		ExtendedPareto<R, ADS, DS>* pf = new ExtendedPareto<R, ADS, DS>;
		while(p->size() > 0)
		{
			Solution<R>* s = &p->remove(0);

			// CALL DECODER TO EVALUATE SOLUTIONS!

			pair<vector<Solution<X, ADS>*>, vector<MultiEvaluation<DS>*> > dec_vs = decoder.decode(*s);

			if(dec_vs.first.size() > 0)
			{
				cout << "DecoderNSGAII::error: not expecting generated solutions!" << endl;
				exit(1);
			}

			pf->push_back(*s, dec_vs.second);
		}

		delete p;

		return pf;
	}

	void crowdingDistanceAssignment(const vector<int>& I, vector<IndividualExtNSGAII<R, X, ADS, DS>*>& r)
	{
		int l = I.size();
		if(l == 0)
			return;

		// for each 'i', ...
		for(unsigned k = 0; k < I.size(); k++)
		{
			int i = I[k];
			// ... set I[i].distance = 0
			r[i]->distance = 0;
			for(unsigned z = 0; z < r[i]->vDist.size(); z++)
				r[i]->vDist[z] = 0;
		}

		// for each objective 'm'
		for(unsigned m = 0; m < nObjectives; m++)
		{
			// I = sort(I, m)
			vector<pair<double, pair<int, int> > > fitness;  // (fitness, (id_s, id_x))

			for(int z = 0; z < I.size(); z++)
			{
				int i = I[z];

				for(unsigned k = 0; k < r[i]->genSolsEval.size(); k++)
				{
					double fit = r[i]->genSolsEval[k]->at(m).evaluation();
					fitness.push_back(make_pair(fit, make_pair(i, k)));
				}
			}

			sort(fitness.begin(), fitness.end(), compara);

			// I[1] dist = I[l] dist = 'infinity'
			// ADAPTATION
			/*
			 r[fitness[0].second]->distance = numeric_limits<double>::infinity();
			 r[fitness[l - 1].second]->distance = numeric_limits<double>::infinity();
			 */

			// for i=2 to l-1
			// ADAPTATION WITH ANOTHER LOOP
			for(int k = 0; k < fitness.size(); k++)
			{
				int i = fitness[k].second.first;
				int i_part = fitness[k].second.second;

				int idx_before = -1;
				for(int e = k - 1; e >= 0; e--)
					if(fitness[e].second.first != i)
					{
						idx_before = e;
						break;
					}

				if(idx_before == -1)
				{
					r[i]->vDist[i_part] += numeric_limits<double>::infinity();
					continue;
				}

				int idx_after = -1;
				for(int e = k + 1; e < fitness.size(); e++)
					if(fitness[e].second.first != i)
					{
						idx_after = e;
						break;
					}

				if(idx_after == -1)
				{
					r[i]->vDist[i_part] += numeric_limits<double>::infinity();
					continue;
				}

				// I[i] dist += (I[i+1].m - I[i-1].m)/(fmax_m - fmin_m)
				// ADAPTATION
				r[i]->vDist[i_part] += (fitness[idx_after].first - fitness[idx_before].first) / (v_d[m]->max() - v_d[m]->min());
			}


			// ADAPTATION (getBest)
			// for each 'i', ...
			for(unsigned k = 0; k < I.size(); k++)
			{
				int i = I[k];
				// ... set I[i].distance = max of parts
				r[i]->updateDist();
			}
		}
	}


	vector<vector<int>*> fastNonDominatedSort(vector<IndividualExtNSGAII<R, X, ADS, DS>*>& Pop)
	{
		// create a map for solutions
		vector< pair<int,int> > P;

		for(unsigned i=0; i<P.size(); i++)
			for(unsigned j=0; j<Pop[i]->genSols.size(); j++)
				P.push_back(make_pair(i, j));

		vector<vector<int>*> F;
		F.push_back(new vector<int>);
		vector<vector<int> > S(P.size());
		vector<int> n(P.size());

		// for each 'p' in 'P'
		for(unsigned p = 0; p < P.size(); p++)
		{
			S[p].clear(); // Sp = {}
			n[p] = 0;     // np = 0

			// for each 'q' in 'P'
			for(unsigned q = 0; q < P.size(); q++)
				if(p != q)
				{
					int p_i = P[p].first;
					int p_j = P[p].second;

					int q_i = P[q].first;
					int q_j = P[q].second;

					// if (p << q)
					if(pDominance.dominates(*Pop.at(p_i)->genSolsEval[p_j], *Pop.at(q_i)->genSolsEval[q_j]))
						S[p].push_back(q); // Sp = Sp U {q}
					// else if (q << p)
					else if(pDominance.dominates(*Pop.at(q_i)->genSolsEval[q_j], *Pop.at(p_i)->genSolsEval[p_j]))
						n[p]++; // np = np + 1

					if(n[p] == 0)
					{
						Pop[p_i]->vRank[p_j] = 0; // p rank = 1
						F[0]->push_back(p);  // F1 = F1 U {p}
					}
				}
		}

		int i = 0; // i=1
		while(F[i]->size() != 0) // while Fi != {}
		{
			vector<int>* Q = new vector<int>;  // Q = {}

			//for each 'p' in 'Fi'
			for(unsigned k = 0; k < F[i]->size(); k++)
			{
				int p = F[i]->at(k);
				int p_i = P[p].first;
				int p_j = P[p].second;

				// for each 'q' in 'Sp'
				for(unsigned j = 0; j < S[p].size(); j++)
				{
					int q = S[p][j];
					int q_i = P[q].first;
					int q_j = P[q].second;

					// nq = nq - 1
					n[q]--;

					// if nq = 0
					if(n[q] == 0)
					{
						Pop[q_i]->vRank[q_j] = i + 1;  // q rank = i+1
						Q->push_back(q);    // Q = Q U {q}
					}
				}

			}

			i++; // i = i + 1
			F[i] = Q; // Fi = Q
		}

		// update all ranks
		for(unsigned p = 0; p < Pop.size(); p++)
			Pop[p]->updateRank();

		return F;
	}

	virtual vector<IndividualExtNSGAII<R, X, ADS, DS>*> makeNewPopulation(const vector<IndividualExtNSGAII<R, X, ADS, DS>*>& p) = 0;

};

}

#endif /*OPTFRAME_DECODER_NONSORTINGGENETICALGORITHMII_HPP_*/
