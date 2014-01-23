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

#include "NSGAII.hpp"

namespace optframe
{

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
struct IndividualExtNSGAII
{
public:
	IndividualNSGAII<R>& s;

	Solution<X, ADS>& x;
	MultiEvaluation<DS>& mev;

	int rank;
	double distance;

	IndividualExtNSGAII(IndividualNSGAII<R>& _s, Solution<X, ADS>* _x, MultiEvaluation<DS>* _mev, int _rank = -1, double _distance = -1) :
			s(_s), x(*_x), mev(*_mev), rank(_rank), distance(_distance)
	{
	}

	virtual ~IndividualExtNSGAII()
	{
		delete &x;
		delete &mev;
	}

	/*
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
	 */

	IndividualExtNSGAII<R, X, ADS, DS>& clone() const
	{
		IndividualExtNSGAII<R, X, ADS, DS>* ind = new IndividualExtNSGAII<R, X, ADS, DS>(s, &x.clone(), &mev.clone(), rank, distance);
		return *ind;
	}

	static void zeroRanks(vector<IndividualNSGAII<R>*>& vs)
	{
		for(unsigned s = 0; s < vs.size(); s++)
			vs[s]->rank = 1000000; // INF
	}

	static void updateRanks(const vector<IndividualExtNSGAII<R, X, ADS, DS>*>& vx)
	{
		// 'min' value
		for(unsigned x = 0; x < vx.size(); x++)
			if(vx[x]->rank < vx[x]->s.rank)
				vx[x]->s.rank = vx[x]->rank;
	}

	static void zeroDistances(vector<IndividualNSGAII<R>*>& vs)
	{
		for(unsigned s = 0; s < vs.size(); s++)
			vs[s]->distance = -1; // -INF
	}

	static void updateDistances(const vector<IndividualExtNSGAII<R, X, ADS, DS>*>& vx)
	{
		// 'max' value
		for(unsigned x = 0; x < vx.size(); x++)
		{
			if(vx[x]->distance > vx[x]->s.distance)
				vx[x]->s.distance = vx[x]->distance;
		}
	}

	void print() const
	{
		cout << "IndExtNSGAII: rank=" << rank << "\tdist=" << distance << "\tparent=" << &s << "\t";
		s.print();
	}

	static void print(const vector<IndividualExtNSGAII<R, X, ADS, DS>*>& v)
	{
		cout << "Population of IndExtNSGAII (" << v.size() << "):" << endl;
		for(unsigned i = 0; i < v.size(); i++)
		{
			cout << i << ":\t";
			v[i]->print();
		}
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

	static bool compara(pair<double, int> p1, pair<double, int> p2)
	{
		return p1.first > p2.first;
	}

protected:
	RandGen& rg;

public:

	// constructor of class 'DecoderNSGAII'
	DecoderNSGAII(Decoder<R, X, ADS, DS>& _decoder, vector<Direction<DS>*>& _v_d, InitialPopulation<R>& _init_pop, int _init_pop_size, int _gMax, RandGen& _rg) :
			decoder(_decoder), v_d(_v_d), nObjectives(_v_d.size()), init_pop(_init_pop), N(_init_pop_size), pDominance(ParetoDominance<R, ADS, DS>(_v_d)), rg(_rg)
	{
		gMax = _gMax;
		//cout << "pDOMINANCE OBJ FUNCTIONS: " << pDominance.v_d.size() << endl;
	}

	// destructor of class 'DecoderNSGAII'
	virtual ~DecoderNSGAII()
	{
	}

	// base population 'p' and desired final population size 'p_size'
	virtual vector<IndividualNSGAII<R>*> basicGeneticOperators(const vector<IndividualNSGAII<R>*>& p, int p_size) = 0;

	// free pareto front 'F'
	template<class T>
	void freeFronts(vector<vector<T>*>& F)
	{
		for(unsigned i = 0; i < F.size(); i++)
			delete F[i];
		F.clear();
	}

	// print counting information of pareto front 'F'
	template<class T>
	void printFronts(const vector<vector<T>*>& F)
	{
		cout << "Fronts(" << F.size() << ") => ";
		for(unsigned i = 0; i < F.size(); i++)
			cout << i << ": " << F[i]->size() << " ";
		cout << endl;
	}

	// convert a population 'p' of Solution to a vector of 'IndividualNSGAII'
	vector<IndividualNSGAII<R>*>* convert(Population<R>& p)
	{
		vector<IndividualNSGAII<R>*>* vp = new vector<IndividualNSGAII<R>*>;
		for(unsigned i = 0; i < p.size(); i++)
		{
			Solution<R>& s = p.at(i);
			vp->push_back(new IndividualNSGAII<R>(s));
		}
		return vp;
	}

	// decode each element from 'ps' and create a list of 'IndividualExtNSGAII'
	vector<IndividualExtNSGAII<R, X, ADS, DS>*> decode(vector<IndividualNSGAII<R>*>& ps)
	{
		vector<IndividualExtNSGAII<R, X, ADS, DS>*> px;

		for(unsigned i = 0; i < ps.size(); i++)
		{
			pair<vector<Solution<X, ADS>*>, vector<MultiEvaluation<DS>*> > dx = decoder.decode(ps[i]->s);
			if(dx.first.size() != dx.second.size())
			{
				// TODO: change format?
				cout << "DecoderNSGAII::decode():error! Failed to get the same sizes for vx and vmev" << endl;
				exit(1);
			}

			for(unsigned x = 0; x < dx.first.size(); x++)
				px.push_back(new IndividualExtNSGAII<R, X, ADS, DS>(*ps[i], dx.first[x], dx.second[x]));
		}
		return px;
	}

	// compare if 'ind1' is better than 'ind2'
	static bool crowdedComparison(const IndividualExtNSGAII<R, X, ADS, DS>* ind1, const IndividualExtNSGAII<R, X, ADS, DS>* ind2)
	{
		return (ind1->rank < ind2->rank) || ((ind1->rank == ind2->rank) && (ind1->distance > ind2->distance));
	}

	// compare if 'ind1' is better than 'ind2'
	static bool crowdedComparisonParents(const IndividualNSGAII<R>& ind1, const IndividualNSGAII<R>& ind2)
	{
		if((ind1.rank == -1) || (ind2.rank == -1))
		{
			cout << "crowdedComparisonParents()::error! uninitialized rank!" << endl;
			exit(1);
		}

		if((ind1.distance == -1) || (ind2.distance == -1))
		{
			cout << "crowdedComparisonParents()::error! uninitialized distance!" << endl;
			exit(1);
		}

		return (ind1.rank < ind2.rank) || ((ind1.rank == ind2.rank) && (ind1.distance > ind2.distance));
	}

	// count all 's' solutions parents in 'rx' that do not appear in 'rs'
	unsigned countNewParents(const vector<IndividualExtNSGAII<R, X, ADS, DS>*>& rx, const vector<IndividualNSGAII<R>*>& rs)
	{
		vector<IndividualNSGAII<R>*> vs = rs; // copy
		unsigned count = 0;
		for(unsigned i = 0; i < rx.size(); i++)
			if(!in(vs, &rx[i]->s))
			{
				vs.push_back(&rx[i]->s);
				count++;
			}

		if(Component::debug)
			cout <<"countNewParents found " << count << " S elements from " << rx.size() << " X elements!" << endl;

		return count;
	}

	// add to list 'rs' all 's' solutions parents in 'rx' that do not appear in 'rs'
	void addNewParents(const vector<IndividualExtNSGAII<R, X, ADS, DS>*>& rx, vector<IndividualNSGAII<R>*>& rs)
	{
		for(unsigned i = 0; i < rx.size(); i++)
			if(!in(rs, &rx[i]->s))
				rs.push_back(&rx[i]->s);
	}

	// returns 'true' if value 't' is in list 'v'; and 'false' otherwise
	template<class T>
	bool in(vector<T>& v, T t)
	{
		for(unsigned i = 0; i < v.size(); i++)
			if(v[i] == t)
				return true;
		return false;
	}

	// main method for the class, 'search' method returns a pareto front of solutions
	virtual ExtendedPareto<R, ADS, DS>* search(double timelimit = 100000000, double target_f = 0, ExtendedPareto<R, ADS, DS>* _pf = NULL)
	{
		Timer tnow;

		if(Component::information)
			cout << "search: DECODER version of Non Sorting Genetic Algorithm Search II" << endl;

		// 1. create population 'p0' of size 'N'
		Population<R>* p0 = &init_pop.generatePopulation(N);

		if(Component::information)
			cout << id() << ": population of " << p0->size() << " generated!" << endl;

		vector<IndividualNSGAII<R>*>* ps = convert(*p0);
		p0->clearNoKill();
		delete p0;

		vector<IndividualExtNSGAII<R, X, ADS, DS>*> _px = decode(*ps);
		vector<IndividualExtNSGAII<R, X, ADS, DS>*>* px = new vector<IndividualExtNSGAII<R, X, ADS, DS>*>(_px);

		IndividualExtNSGAII<R, X, ADS, DS>::zeroRanks(*ps); // update parents' ranks
		IndividualExtNSGAII<R, X, ADS, DS>::zeroDistances(*ps); // update parents' distances

		if(Component::information)
			cout << id() << ": first decode okay! generated " << ps->size() << " => " << px->size() << endl;

		// 2. sort population by non domination
		vector<vector<IndividualExtNSGAII<R, X, ADS, DS>*>*> F = fastNonDominatedSort(*px);
		IndividualExtNSGAII<R, X, ADS, DS>::updateRanks(*px); // update parents' ranks
		if(Component::information)
			cout << id() << ": first pareto front created with " << F.size() << " elements!" << endl;
		// also calculate crowding distance (for binary tournament!)
		crowdingDistanceAssignment(*px);
		IndividualExtNSGAII<R, X, ADS, DS>::updateDistances(*px); // update parents' distances
		if(Component::information)
			cout << id() << ": first crowding distance calculation applied!" << endl;

		if(Component::information)
			IndividualExtNSGAII<R, X, ADS, DS>::print(*px);

		// 3. create offspring of size 'N'
		if(Component::information)
			cout << id() << ": will generate first children!" << endl;
		vector<IndividualNSGAII<R>*> qs = makeNewPopulation(*ps);
		if(Component::information)
			cout << id() << ": will decode first children!" << endl;
		vector<IndividualExtNSGAII<R, X, ADS, DS>*> qx = decode(qs);

		if(Component::information)
			cout << id() << ": main loop" << endl;
		int t = 0;
		while((t <= gMax) && (tnow.now() < timelimit))
		{
			cout << endl << "===========================================" << endl;
			cout << "Generation = " << t << endl;

			cout << "N=" << N << " ps=" << ps->size() << " qs=" << qs.size() << " px=" << px->size() << " qx=" << qx.size();
			cout << endl;

			// 1. combine populations in 'Rt': Rt = Pt U Qt;
			if(Component::information)
			{
				cout << id() << ": joining population!" << endl;
			}
			vector<IndividualNSGAII<R>*>* rs = ps;
			ps = NULL;
			vector<IndividualExtNSGAII<R, X, ADS, DS>*>* rx = px;
			px = NULL;
			rs->insert(rs->end(), qs.begin(), qs.end());
			rx->insert(rx->end(), qx.begin(), qx.end());

			IndividualExtNSGAII<R, X, ADS, DS>::zeroRanks(*rs); // update parents' ranks
			IndividualExtNSGAII<R, X, ADS, DS>::zeroDistances(*rs); // update parents' ranks

			if(Component::information)
			{
				cout << id() << ": joint populations |rs|=" << rs->size() << " |rx|=" << rx->size() << endl;
				//IndividualExtNSGAII<R, X, ADS, DS>::print(*rx);
			}


			// 2. sort population by non domination
			if(Component::information)
				cout << id() << ": sorting by non domination" << endl;
			freeFronts(F);
			F = fastNonDominatedSort(*rx);
			IndividualExtNSGAII<R, X, ADS, DS>::updateRanks(*rx); // update parents' ranks

			// 3. Pt+1 = {}
			if(Component::information)
				cout << id() << ": creating new populations" << endl;
			vector<IndividualNSGAII<R>*>* nextPopS = new vector<IndividualNSGAII<R>*>;

			// 4. i=1
			int i = 0;

			int count = countNewParents(*F[i], *nextPopS);
			// while |Pt+1| + |Fi| <= N
			if(Component::information)
				cout << id() << ": while i=" << i << " (|S|:" << nextPopS->size() << " + count:" << count << " <= N:" << N << ")" << endl;
			while((nextPopS->size() + count) <= N)
			{
				// do crowding distance assignment
				crowdingDistanceAssignment(*F[i]);
				IndividualExtNSGAII<R, X, ADS, DS>::updateDistances(*F[i]); // update parents' ranks

				// Pt+1 = Pt+1 U Fi
				addNewParents(*F[i], *nextPopS);
				//nextPopX->insert(nextPopX->end(), F[i]->begin(), F[i]->end());

				i = i + 1;

				if(i == F.size())
				{
					cout << "ERROR! Elements from S still missing and can't reach front " << i << endl;
					exit(1);
				}
				count = countNewParents(*F[i], *nextPopS);

				if(Component::information)
					cout << id() << ": while i=" << i << " (|S|:" << nextPopS->size() << " + count:" << count << " <= N:" << N << ")" << endl;
			}

			if((nextPopS->size() < N) && (i < F.size()) && (F[i]->size() > 0))
			{
				if(Component::information)
					cout << id() << ": will finish adding elements! i=" << i << " / |F|=" << F.size() << " |nextPopS|=" << nextPopS->size() << " < N=" << N << endl;

				if(Component::information)
					cout << id() << ": there are i=" << i << " |F_i|=" << F[i]->size() << " elements from X" << endl;

				// sort by last elements by crowding comparison
				crowdingDistanceAssignment(*F[i]);
				IndividualExtNSGAII<R, X, ADS, DS>::updateDistances(*F[i]); // update parents' dists

				if(Component::information)
					cout << id() << ": sorting!" << endl;

				sort(F[i]->begin(), F[i]->end(), crowdedComparison);

				// Pt+1 = Pt+1 U Fi[1:(N - |Pt+1|) ]
				vector<IndividualExtNSGAII<R, X, ADS, DS>*> toAdd;
				int j = 0;
				toAdd.push_back(F[i]->at(j));
				count = countNewParents(toAdd, *nextPopS);
				while(nextPopS->size() + count != N)
				{
					j++;
					if(j == F[i]->size())
					{
						cout << "ERROR: Element " << j << " not available in front " << i << "!" << endl;
						exit(1);
					}
					toAdd.push_back(F[i]->at(j));
					count = countNewParents(toAdd, *nextPopS);
				}

				// add missing elements
				addNewParents(toAdd, *nextPopS);
				//nextPopX->insert(nextPopX->end(), toAdd.begin(), toAdd.end());
			}

			if(nextPopS->size() < N)
			{
				cout << "ERROR: still missing elements in population! |nextPopS|=" << nextPopS->size() << " < N=" << N << endl;
				exit(1);
			}

			vector<IndividualExtNSGAII<R, X, ADS, DS>*>* nextPopX = new vector<IndividualExtNSGAII<R, X, ADS, DS>*>;
			for(unsigned x = 0; x < rx->size(); x++)
			{
				IndividualExtNSGAII<R, X, ADS, DS>* ind_x = rx->at(x);
				IndividualNSGAII<R>* ind_s = &ind_x->s;
				if(in(*nextPopS, ind_s))
					nextPopX->push_back(rx->at(x));
				else
				{
					delete rx->at(x);
					rx->at(x) = NULL;
				}
			}

			cout << "generation " << t << " final population sizes ";
			cout << " nextPopS: " << nextPopS->size();
			cout << " nextPopX: " << nextPopX->size();
			cout << endl;

			//if(Component::information)
			//	IndividualExtNSGAII<R, X, ADS, DS>::print(*nextPopX);

			// delete unused elements (quadratic)
			for(unsigned k = 0; k < rs->size(); k++)
				if(!in(*nextPopS, rs->at(k)))
					delete rs->at(k);

			// or... do other magic stuff to update all pointers, maybe? (TODO)

			delete rs;
			////delete ps; // 'rs' is pointing to 'ps'
			ps = nextPopS;
			delete rx;
			////delete px; // 'rx' is pointing to 'px'
			px = nextPopX;

			if(Component::information)
				cout << id() <<": will generate next population!" << endl;

			qs = makeNewPopulation(*ps);
			qx = decode(qs);

			cout << "Finished Generation = " << t << endl;
			cout << "===========================================" << endl << endl;
			t++;
		}

		freeFronts(F);

		// FINISH ALGORITHM!
		if(Component::information)
			cout << id() << ": cleaning memory to finish!" << endl;

		// free all 'q'
		for(unsigned k = 0; k < qs.size(); k++)
			delete qs[k];
		qs.clear();

		for(unsigned k = 0; k < qx.size(); k++)
			delete qx[k];
		qx.clear();

		// free all 'px' (not used anymore)
		for(unsigned k = 0; k < px->size(); k++)
			delete px->at(k);
		px->clear();
		delete px;

		if(Component::information)
			cout << id() << ": memory clean!" << endl;

		ExtendedPareto<R, ADS, DS>* pf = new ExtendedPareto<R, ADS, DS>;
		while(ps->size() > 0)
		{
			Solution<R>* s = &ps->at(0)->s.clone();
			delete ps->at(0);
			ps->erase(ps->begin() + 0);

			// CALL DECODER TO EVALUATE SOLUTIONS!

			pair<vector<Solution<X, ADS>*>, vector<MultiEvaluation<DS>*> > dec_vs = decoder.decode(*s);

			if(dec_vs.first.size() > 0)
			{
				// free unused solutions
				for(unsigned i = 0; i < dec_vs.first.size(); i++)
					delete dec_vs.first[i];
				dec_vs.first.clear();
			}

			pf->push_back(s, dec_vs.second);
		}

		delete ps;

		if(Component::information)
			cout << id() << ": search finished!" << endl;

		return pf;
	}

	// calculates de crowding distance for each element of vector 'I', and stores in I[i].distance
	void crowdingDistanceAssignment(vector<IndividualExtNSGAII<R, X, ADS, DS>*>& I)
	{
		int l = I.size();
		if(l == 0)
			return;

		// for each 'i', ...
		for(unsigned i = 0; i < I.size(); i++)
		{
			// ... set I[i].distance = 0
			I[i]->distance = 0;
		}

		// for each objective 'm'
		for(unsigned m = 0; m < nObjectives; m++)
		{
			// I = sort(I, m)
			vector<pair<double, int> > fitness;  // (fitness, id)

			for(int i = 0; i < I.size(); i++)
			{
				double fit = I[i]->mev.at(m).evaluation();
				fitness.push_back(make_pair(fit, i));
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
				int i = fitness[k].second;

				int idx_before = -1;
				for(int e = k - 1; e >= 0; e--)
					if(&I[i]->s != &I[fitness[e].second]->s)
					{
						idx_before = e;
						break;
					}

				if(idx_before == -1)
				{
					I[i]->distance += numeric_limits<double>::infinity();
					continue;
				}

				int idx_after = -1;
				for(int e = k + 1; e < fitness.size(); e++)
					if(&I[i]->s != &I[fitness[e].second]->s)
					{
						idx_after = e;
						break;
					}

				if(idx_after == -1)
				{
					I[i]->distance += numeric_limits<double>::infinity();
					continue;
				}

				// I[i] dist += (I[i+1].m - I[i-1].m)/(fmax_m - fmin_m)
				// ADAPTATION
				I[i]->distance += (fitness[idx_after].first - fitness[idx_before].first) / (v_d[m]->max() - v_d[m]->min());
			}
		}
	}

	// calculates de ranking for each element of vector 'I', and updates in I[i].rank
	vector<vector<IndividualExtNSGAII<R, X, ADS, DS>*>*> fastNonDominatedSort(vector<IndividualExtNSGAII<R, X, ADS, DS>*>& P)
	{
		if(Component::information)
			cout << id() << "::fastNonDominatedSort |P|=" << P.size() << " begin" << endl;
		vector<vector<int>*> F;
		F.push_back(new vector<int>);
		vector<vector<int> > S(P.size());
		vector<int> n(P.size());

		// for each 'p' in 'P'
		double sum_dom_time = 0.0;
		int count_dom_times = 0;
		for(unsigned p = 0; p < P.size(); p++)
		{
			S[p].clear(); // Sp = {}
			n[p] = 0;     // np = 0

			// for each 'q' in 'P'
			for(unsigned q = 0; q < P.size(); q++)
				if(p != q)
				{
					Timer timer;
					pair<bool, bool> v = pDominance.birelation(P.at(p)->mev, P.at(q)->mev);
					sum_dom_time += timer.inMilliSecs();
					count_dom_times++;
					// if (p << q)
					if(v.first)
						S[p].push_back(q); // Sp = Sp U {q}
					// else if (q << p)
					else if(v.second)
						n[p]++; // np = np + 1
				}

			// if n_p = 0, p belongs to the first front
			if(n[p] == 0)
			{
				P[p]->rank = 0; // p rank = 1
				F[0]->push_back(p);  // F_1 = F_1 U {p}
			}
		}

		if(Component::information)
		{
			cout << "count_dom_times=" << count_dom_times << endl;
			cout << "sum_dom_time=" << sum_dom_time << "ms" << endl;
			cout << "average: " << sum_dom_time / count_dom_times << "ms" << endl;
		}

		int i = 0; // i=1
		while(F[i]->size() != 0) // while Fi != {}
		{
			if(Component::debug)
				cout << "fastNonDominatedSort start i=" << i << " |F[i]|=" << F[i]->size() << endl;
			vector<int>* Q = new vector<int>;  // Q = {}

			//for each 'p' in 'Fi'
			for(unsigned k = 0; k < F[i]->size(); k++)
			{
				int p = F[i]->at(k);

				// for each 'q' in 'Sp'
				for(unsigned j = 0; j < S[p].size(); j++)
				{
					int q = S[p][j];

					// nq = nq - 1
					n[q]--;

					// if nq = 0
					if(n[q] == 0)
					{
						P[q]->rank = i + 1;  // q rank = i+1
						Q->push_back(q);    // Q = Q U {q}
					}
				}

			}

			i++; // i = i + 1
			F.push_back(NULL);
			F[i] = Q; // Fi = Q
			if(Component::debug)
				cout << "fastNonDominatedSort i=" << i << " |Q|=" << Q->size() << endl;
		}

		// TODO: do this automatically before! not a big cost, but...
		unsigned count_check = 0;
		vector<vector<IndividualExtNSGAII<R, X, ADS, DS>*>*> fronts;
		for(unsigned i = 0; i < F.size(); i++)
			if(F[i]->size() > 0)
			{
				count_check += F[i]->size();
				fronts.push_back(new vector<IndividualExtNSGAII<R, X, ADS, DS>*>);
				for(unsigned j = 0; j < F[i]->size(); j++)
					fronts[i]->push_back(P[F[i]->at(j)]);
			}

		if(count_check != P.size())
		{
			cout << "ERROR in fastNonDominatedSort! count_check=" << count_check << " != |P|=" << P.size() << endl;
			exit(1);
		}

		freeFronts(F);

		if(Component::information)
		{
			cout << id() << "::fastNonDominatedSort found " << fronts.size() << " fronts" << endl;
			printFronts(fronts);
			cout << id() << "::fastNonDominatedSort end" << endl;
		}
		return fronts;
	}

	// creates a brand new population based on population 'p'
	virtual vector<IndividualNSGAII<R>*> makeNewPopulation(const vector<IndividualNSGAII<R>*>& p) = 0;

	// returns id() of class
	string id() const
	{
		return "DecoderNSGAII";
	}

};

}

#endif /*OPTFRAME_DECODER_NONSORTINGGENETICALGORITHMII_HPP_*/
