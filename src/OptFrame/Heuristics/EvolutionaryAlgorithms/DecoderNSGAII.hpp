// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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
#include "InitialMultiSolution.hpp"
#include "../../RandGen.hpp"
#include "../../NSSeq.hpp"
#include "../../ParetoDominance.hpp"
//#include "../../ParetoDominanceStrict.hpp"

#include "../../Decoder.hpp"

#include "../../Timer.hpp"

#include "../../Util/printable.h"

#include "NSGAII.hpp"

namespace optframe
{

template<class R> class NSGAIICrowdingComparison;

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS>
struct IndividualExtNSGAII
{
public:

	// direct parent of x-solution
	IndividualNSGAII<R>& s;

	// list of S-solutions that can decode this X-solution
	vector<IndividualNSGAII<R>*> ls;

	// list of twins
	vector<IndividualExtNSGAII<R, X, ADS>*> lx;

	Solution<X, ADS>& x;
	MultiEvaluation& mev;

	int rank;
	double distance;

	IndividualExtNSGAII(IndividualNSGAII<R>& _s, Solution<X, ADS>* _x, MultiEvaluation* _mev, int _rank = -1, double _distance = -1) :
			s(_s), x(*_x), mev(*_mev), rank(_rank), distance(_distance)
	{
		resetList();
	}

	IndividualExtNSGAII(IndividualNSGAII<R>& _s, vector<IndividualNSGAII<R>*>& _vs, vector<IndividualExtNSGAII<R, X, ADS>*>& _lx, Solution<X, ADS>* _x, MultiEvaluation* _mev, int _rank = -1, double _distance = -1) :
			s(_s), ls(_vs), lx(_lx), x(*_x), mev(*_mev), rank(_rank), distance(_distance)
	{
		// using pre-created 'ls' list!
	}

	void resetList()
	{
		ls.clear();
		ls.push_back(&s);
		lx.clear();
	}

	virtual ~IndividualExtNSGAII()
	{
		delete &x;
		delete &mev;
	}

	IndividualExtNSGAII<R, X, ADS>& clone() const
	{
		IndividualExtNSGAII<R, X, ADS>* ind = new IndividualExtNSGAII<R, X, ADS>(s, ls, lx, &x.clone(), &mev.clone(), rank, distance);
		return *ind;
	}

	static void zeroRanks(vector<IndividualNSGAII<R>*>& vs)
	{
		for(unsigned s = 0; s < vs.size(); s++)
			vs[s]->rank = 1000000; // INF
	}

	static void updateRanks(const vector<IndividualExtNSGAII<R, X, ADS>*>& vx)
	{
		// 'min' value
		for(unsigned x = 0; x < vx.size(); x++)
		{
			for(unsigned j = 0; j < vx[x]->ls.size(); j++)
			{
				if(vx[x]->rank < vx[x]->ls.at(j)->rank)
				{
					//cout << "update id " << vx[x]->ls.at(j)->id << ": " << vx[x]->ls.at(j)->rank;
					vx[x]->ls.at(j)->rank = vx[x]->rank;
					//cout << " => " << vx[x]->ls.at(j)->rank << endl;
				}
			}
			// spread to twins
			for(unsigned t = 0; t < vx[x]->lx.size(); t++)
				vx[x]->lx[t]->rank = vx[x]->rank;
		}
	}

	static void zeroDistances(vector<IndividualNSGAII<R>*>& vs)
	{
		for(unsigned s = 0; s < vs.size(); s++)
			vs[s]->distance = -1; // -INF
	}

	static void updateDistances(const vector<IndividualExtNSGAII<R, X, ADS>*>& vx)
	{
		// 'max' value
		for(unsigned x = 0; x < vx.size(); x++)
		{
			for(unsigned j = 0; j < vx[x]->ls.size(); j++)
				if(vx[x]->distance > vx[x]->ls.at(j)->distance)
					vx[x]->ls.at(j)->distance = vx[x]->distance;
			// spread to twins
			for(unsigned t = 0; t < vx[x]->lx.size(); t++)
				vx[x]->lx[t]->distance = vx[x]->distance;
		}
	}

	void print() const
	{
		cout << "IndExtNSGAII: rank=" << rank << "\tdist=" << distance << "\t";
		cout << "[ ";
		for(unsigned i=0; i<mev.size(); i++)
			cout << mev.at(i).evaluation() << " ; ";
		cout << "]\t";
		cout << "parents=" << ls.size() << "\tparent[0]=" << ls[0] << "\t";
		ls[0]->print();
	}

	static void print(const vector<IndividualExtNSGAII<R, X, ADS>*>& v)
	{
		cout << "Population of IndExtNSGAII (" << v.size() << "):" << endl;
		for(unsigned i = 0; i < v.size(); i++)
		{
			cout << i << ":\t";
			v[i]->print();
		}
	}

	static void print(const vector<IndividualNSGAII<R>*>& ps, const vector<IndividualExtNSGAII<R, X, ADS>*>& px)
	{
		cout << "Details of Population: |PS|=" << ps.size() << " |PX|=" << px.size() << endl;
		for(unsigned s=0; s<ps.size(); s++)
		{
			cout << "#" << s <<": ";
			ps[s]->print();
			for(unsigned j=0; j<px.size(); j++)
				if(&px[j]->s == ps[s])
				{
					cout << "\t";
					px[j]->print();
				}
 		}
	}


	static bool compareMev(MultiEvaluation& mev1, MultiEvaluation& mev2)
	{
		for(unsigned i = 0; i < mev1.size(); i++)
			if(mev1.at(i).evaluation() != mev2.at(i).evaluation())
				return false;
		return true;
	}

	static void mergeSameParents(vector<IndividualExtNSGAII<R, X, ADS>*>& vx)
	{
		vector<IndividualExtNSGAII<R, X, ADS>*> vReduced;
		for(unsigned i = 0; i < vx.size(); i++)
			if(vx[i]) // not null
			{
				// check if it's already on vReduced
				for(unsigned r = 0; r < vReduced.size(); r++)
					if(compareMev(vx[i]->mev, vReduced[r]->mev))
					{
						vReduced[r]->ls.insert(vReduced[r]->ls.end(), vx[i]->ls.begin(), vx[i]->ls.end());
						vx[i] = nullptr;
						break;
					}

				if(vx[i])
				{
					vReduced.push_back(vx[i]);
					vx[i] = nullptr;
				}
			}

		vx = vReduced;
	}

	static double min(const vector<double>& v)
	{
		int m = v[0];
		for(unsigned i = 1; i < v.size(); i++)
			if(v[i] < m)
				m = v[i];
		return m;
	}

	static double max(const vector<double>& v)
	{
		int m = v[0];
		for(unsigned i = 1; i < v.size(); i++)
			if(v[i] > m)
				m = v[i];
		return m;
	}

	static void printLimits(unsigned n_objs, const vector<IndividualExtNSGAII<R, X, ADS>*>& vx)
	{
		for(unsigned k = 0; k < n_objs; k++)
		{
			vector<double> c;
			for(unsigned i = 0; i < vx.size(); i++)
				c.push_back(vx[i]->mev.at(k).evaluation());

			cout << "OBJ " << k << ": MIN=" << min(c) << " MAX=" << max(c) << "\t";
		}
		cout << endl;
	}

	static void printLimits(unsigned n_objs, const vector<IndividualNSGAII<R>*>& vs)
	{
		for(unsigned k = 0; k < n_objs; k++)
		{
			vector<double> c;
			for(unsigned i = 0; i < vs.size(); i++)
			{
				if(vs[i]->mev)
					c.push_back(vs[i]->mev->at(k).evaluation());
				else
					cout << "WARNING::printLimits i=" << i << " IS nullptr!" << endl;
			}
			cout << "OBJ " << k << ": MIN=" << min(c) << " MAX=" << max(c) << "\t";
		}
		cout << endl;
	}

	static void mysort(vector<IndividualNSGAII<R>*>& vs, NSGAIICrowdingComparison<R>& comp)
	{
		//sort(vs.begin(), vs.end(), comp);
		sort(vs.begin(), vs.end(), simpleCompare);

		return;

		for(int i = 0; i < ((int) vs.size()) - 1; i++)
			for(int j = i + 1; j < ((int) vs.size()); j++)
				if(!comp(vs[i], vs[j]))
				{
					IndividualNSGAII<R>* tmp = vs[i];
					vs[i] = vs[j];
					vs[j] = tmp;
				}
	}

	static int countEquals(unsigned n_objs, const vector<IndividualExtNSGAII<R, X, ADS>*>& vx)
	{
		vector<vector<double> > list;
		for(unsigned k = 0; k < vx.size(); k++)
		{
			vector<double> vobjs;
			for(unsigned o = 0; o < n_objs; o++)
				vobjs.push_back(vx[k]->mev.at(o).evaluation());
			if(!in(vobjs, list))
				list.push_back(vobjs);
		}
		return list.size();
	}

	static int countCrowdingValues(const vector<IndividualExtNSGAII<R, X, ADS>*>& vx)
	{
		vector<double> list;
		for(unsigned k = 0; k < vx.size(); k++)
		{
			double cd = vx[k]->distance;
			if(!in(cd, list))
				list.push_back(cd);
		}
		return list.size();
	}

	static int countCrowdingValuesPS(const vector<IndividualNSGAII<R>*>& vx)
	{
		vector<double> list;
		for(unsigned k = 0; k < vx.size(); k++)
		{
			double cd = vx[k]->distance;
			if(!in(cd, list))
				list.push_back(cd);
		}
		return list.size();
	}

	template<class T>
	static bool in(T& t, const vector<T>& v)
	{
		for(unsigned i = 0; i < v.size(); i++)
			if(v[i] == t)
				return true;
		return false;
	}

	static bool simpleCompare(const IndividualNSGAII<R>* ind1, const IndividualNSGAII<R>* ind2)
	{
		if(ind1->rank < ind2->rank)
			return true;
		else if(ind1->rank == ind2->rank)
		{
			if(ind1->distance > ind2->distance)
				return true;
			else if(ind1->distance == ind2->distance) // TODO: FIX, IMPOSING MINIMIZATION!
				return ind1->mev->at(0).evaluation() < ind2->mev->at(0).evaluation();
		}

		return false; // default is false
	}

	static void printToDisk(const vector<IndividualNSGAII<R>*>& ps, const vector<IndividualExtNSGAII<R, X, ADS>*>& px)
	{
		if(ps[0]->mev->size() != 2)
		{
			cout << "ERROR: cannot plot more than 2 objectives!" << endl;
			return;
		}

		FILE* fscript = fopen("plot_current_population.p", "w");
		fprintf(fscript, "set autoscale\n");
		fprintf(fscript, "unset log\n");
		fprintf(fscript, "unset label\n");
		fprintf(fscript, "set xtic auto\n");
		fprintf(fscript, "set ytic auto\n");
		fprintf(fscript, "set title \"Population of size %d with %d decoded solutions\"\n", (int) ps.size(), (int) px.size());
		fprintf(fscript, "set xlabel \"total distance\"\n");
		fprintf(fscript, "set ylabel \"makespan\"\n");
		fprintf(fscript, "set terminal png enhanced font \"Helvetica,11\"\n");

		fprintf(fscript, "sind(n) = sprintf(\"s_individual_%%d.log\", n)\n");
		fprintf(fscript, "pareto(n) = sprintf(\"pareto_%%d.log\", n)\n");

		fprintf(fscript, "set output \"pngs/population-all.png\"\n");

		// example of circle!
		////fprintf(fscript, "set object 1 circle at  500, 70 size first 10.0 fc rgb \"navy\"\n");

		int out1 = system("rm pareto_*.log");
		int count_pareto = 0;
		for(unsigned x = 0; x < px.size(); x++)
		{
			int pn = px[x]->rank + 1;
			if(pn > count_pareto)
				count_pareto = pn;
			stringstream ss;
			ss << "pareto_" << pn << ".log";
			FILE* pareto = fopen(ss.str().c_str(), "a");

			fprintf(pareto, "%d\t%d\n", (int) px[x]->mev.at(0).evaluation(), (int) px[x]->mev.at(1).evaluation());
			fclose(pareto);
		}

		fprintf(fscript, "plot for [i=1:%d] sind(i) using 1:2 title \"individual \".i,\\\n", (int) ps.size());
		fprintf(fscript, "for [i=1:%d] pareto(i) using 1:2 smooth unique title \"pareto front \".i,\\\n", count_pareto);
		fprintf(fscript, "\"crowd_inf.log\" using 1:2 title 'infinite crowding distance' with circles linecolor rgb \"#A9A9A9\" lw 3\n");
		fprintf(fscript, "print \"ok generation all\"\n");
		fclose(fscript);

		for(unsigned s = 0; s < ps.size(); s++)
		{
			stringstream ss;
			ss << "s_individual_" << (s + 1) << ".log";
			FILE* find = fopen(ss.str().c_str(), "w");
			for(unsigned x = 0; x < px.size(); x++)
				if(&px[x]->s == ps[s])
					fprintf(find, "%d\t%d\n", (int) px[x]->mev.at(0).evaluation(), (int) px[x]->mev.at(1).evaluation());
			fclose(find);
		}

		FILE* cinf = fopen("crowd_inf.log", "w");
		for(unsigned x = 0; x < px.size(); x++)
			if(px[x]->distance > 100000) // INF
				fprintf(cinf, "%d\t%d\n", (int) px[x]->mev.at(0).evaluation(), (int) px[x]->mev.at(1).evaluation());
		fclose(cinf);

		cout << "FILE GENERATED!" << endl;

		int out2 = system("gnuplot plot_current_population.p");
		int out3 = system("eog pngs/population-all.png");
		if(out1 + out2 > 0)
		{
			cout << "WARNING: some error may have occured in system calls..." << endl;
		}
	}


};


// compare if 'ind1' is better than 'ind2'
template<class R>
class NSGAIICrowdingComparison
{
private:
	NSGAIICrowdingComparison<R>* userSpecific;

public:
	NSGAIICrowdingComparison(NSGAIICrowdingComparison<R>* _userSpecific = nullptr) :
			userSpecific(_userSpecific)
	{
	}

	virtual ~NSGAIICrowdingComparison()
	{
	}

	// only basic definition
	static bool compare(const IndividualNSGAII<R>* ind1, const IndividualNSGAII<R>* ind2)
	{
		if((ind1->rank) < (ind2->rank))
			return true;
		else if(ind1->rank == ind2->rank)
		{
			if(ind1->distance > ind2->distance)
				return true;
		}

		return false; // default is false
	}

	// also call 'specific' operator
	virtual bool operator()(const IndividualNSGAII<R>* ind1, const IndividualNSGAII<R>* ind2)
	{
                if(!ind1 || !ind2)
                {
                    cout << "CRZY ERROR!" << endl;
		    exit(1);
                }

		if(ind1 == ind2)
			return false;

		if((ind1->rank) < (ind2->rank))
			return true;
		else if(ind1->rank == ind2->rank)
		{
			if(ind1->distance > ind2->distance)
				return true;
			else if(ind1->distance == ind2->distance)
				return userSpecific && (*userSpecific)(ind1, ind2);
		}
		return false;
	}

	virtual void print() const
	{
		cout << "NSGAIICrowdingComparison DEFAULT" << endl;
	}
};


template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS>
class DecoderNSGAII: public ExtendedMultiObjSearch<R, X, ADS>
{
protected:
	Decoder<R, X, ADS>& decoder;
	vector<Direction*>& v_d;
	unsigned nObjectives;

	InitialMultiSolution<R>& init_pop;
	int pMin, pMax;
	int xTarget;
	bool adaptative_pop;

	NSGAIICrowdingComparison<R>* userSpecificComparison;

	static const bool LOG_POPULATIONS = false;
	static const bool DISPLAY_GENERAL = false;
	static const bool DISPLAY_IMPROVEMENT = true;

public:
	int N;  // 'N' may be estimated by 'pMin', 'pMax' and 'xTarget'

	double firstRatio;
	double lastRatio;

	double lastMin0; // TODO: remove!!

	double timeSort;
	double timeSTLSort;

	double timeDist;
	double timeLoop;
	double timeNewPop;
	int tMax;


protected:

     virtual void printToDisk(const vector<IndividualNSGAII<R>*>& ps, const vector<IndividualExtNSGAII<R, X, ADS>*>& px)
     {
     }


	ParetoDominance<R, ADS>& pDominance;
	int gMax;

	static bool compara(pair<double, int> p1, pair<double, int> p2)
	{
		return p1.first > p2.first;
	}

protected:
	RandGen& rg;

public:

	// constructor of class 'DecoderNSGAII'
	DecoderNSGAII(Decoder<R, X, ADS>& _decoder, vector<Direction*>& _v_d, InitialMultiSolution<R>& _init_pop, int _pMin, int _pMax, int _xTarget, int _gMax, RandGen& _rg, NSGAIICrowdingComparison<R>* uec = nullptr) :
			decoder(_decoder), v_d(_v_d), nObjectives(_v_d.size()), init_pop(_init_pop), pMin(_pMin), pMax(_pMax), xTarget(_xTarget), gMax(_gMax), pDominance(* new ParetoDominance<R, ADS>(_v_d)), rg(_rg), userSpecificComparison(uec)
	{
		N = -1;
		firstRatio = lastRatio = 0;
		adaptative_pop = true;
                lastMin0 = 0;
	}

	DecoderNSGAII(Decoder<R, X, ADS>& _decoder, vector<Direction*>& _v_d, InitialMultiSolution<R>& _init_pop, int _N, int _gMax, RandGen& _rg, NSGAIICrowdingComparison<R>* uec) :
			decoder(_decoder), v_d(_v_d), nObjectives(_v_d.size()), init_pop(_init_pop), N(_N), gMax(_gMax), pDominance(* new ParetoDominance<R, ADS>(_v_d)), rg(_rg), userSpecificComparison(uec)
	{
		pMin = pMax = N;
		xTarget = 0;
		firstRatio = lastRatio = 0;
		adaptative_pop = false;
                lastMin0 = 0;
	}

	// destructor of class 'DecoderNSGAII'
	virtual ~DecoderNSGAII()
	{
		if(userSpecificComparison)
			delete userSpecificComparison;
		delete& pDominance;
	}

	typedef vector<IndividualExtNSGAII<R, X, ADS>*> PX;
	typedef vector<IndividualNSGAII<R>*> PS;

	// base population 'p' and desired final population size 'p_size'
	////virtual PS basicGeneticOperators(const vector<IndividualNSGAII<R>*>& p, int p_size) = 0;

	// creates a brand new population based on population 'p'
	virtual pair<PS, PX> makeNewPopulation(const PS&, const PX&) = 0;

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
	PS* convert(Population<R>& p)
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
	PX decode(PS& ps)
	{
		vector<IndividualExtNSGAII<R, X, ADS>*> px;

		for(unsigned i = 0; i < ps.size(); i++)
		{
			pair<vector<Solution<X, ADS>*>, vector<MultiEvaluation*> > dx = decoder.decode(ps[i]->s);
			if(dx.first.size() != dx.second.size())
			{
				// TODO: change format?
				cout << "DecoderNSGAII::decode():error! Failed to get the same sizes for vx and vmev" << endl;
				exit(1);
			}

			for(unsigned x = 0; x < dx.first.size(); x++)
				px.push_back(new IndividualExtNSGAII<R, X, ADS>(*ps[i], dx.first[x], dx.second[x]));
		}
		return px;
	}


	pair<PS, PX> generateFirstPopulation(unsigned pSize)
	{
		// 1. create population 'p0' of size 'pMax' (will calculate 'N')
		Population<R> *p0 = &init_pop.generatePopulation(pSize);

		if(Component::information)
			cout << id() << ": population of " << p0->size() << " generated!" << endl;

		PS* ps_base = convert(*p0);
		p0->clearNoKill();
		delete p0;

		// will calculate 'N' population size, based on limits 'pMin', 'pMax', 'xTarget'

		PS ps(*ps_base);
		/*
		for(unsigned i = 0; i < ((unsigned) pMin); i++)
		{
			ps_base->at(i)->isRandom = true;
			ps.push_back(ps_base->at(i));
		}
		*/
		PX px = decode(ps);

		/*
		for(unsigned i = ps.size(); i < ps_base->size(); i++)
			if(px.size() > xTarget)
				break;
			else
			{
				vector<IndividualNSGAII<R>*> ns(1, ps_base->at(i));
				vector<IndividualExtNSGAII<R, X, ADS>*> nx = decode(ns);
				ps.push_back(ns[0]);
				px.insert(px.end(), nx.begin(), nx.end());
			}

		N = ps.size(); // keep the same population size

		// erase unused solutions from 'ps'
		for(unsigned i = ps.size(); i < ps_base->size(); i++)
			delete ps_base->at(i);
		*/
		delete ps_base;

		for(unsigned i = 0; i < ps.size(); i++)
			ps.at(i)->id = i;

		if(Component::information)
			cout << id() << ": first decode okay! generated " << ps.size() << " => " << px.size() << endl;

		// update values that may be used in later selection of individuals
		updateParentsObjectives(ps, px);

		return make_pair(ps, px);
	}

	pair<PS, PX> generateNextPopulation(const PS& ps, const PX& px)
	{
		pair<PS, PX> p = makeNewPopulation(ps, px);
		//PX qx = decode(qs);
		// update values that may be used in later selection of individuals
		updateParentsObjectives(p.first, p.second);

		return p;
	}

	// compare if 'ind1' is better than 'ind2'
	static bool crowdedComparisonX(const IndividualExtNSGAII<R, X, ADS>* ind1, const IndividualExtNSGAII<R, X, ADS>* ind2)
	{
		return ((ind1->rank) < (ind2->rank)) || ((ind1->rank == ind2->rank) && (ind1->distance > ind2->distance));
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

		return ((ind1.rank) < (ind2.rank)) || ((ind1.rank == ind2.rank) && (ind1.distance > ind2.distance));
	}

	// count all 's' solutions parents in 'rx' that do not appear in 'rs'
	unsigned countNewParents(const vector<IndividualExtNSGAII<R, X, ADS>*>& rx, const vector<IndividualNSGAII<R>*>& rs)
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
	void addNewParents(const vector<IndividualExtNSGAII<R, X, ADS>*>& rx, vector<IndividualNSGAII<R>*>& rs)
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

	virtual void updateParentsObjectives(PS& ps, const PX& px)
	{
	}

	// main method for the class, 'search' method returns a pareto front of solutions
	virtual ExtendedPareto<R, X, ADS>* search(double timelimit = 100000000, double target_f = 0, ExtendedPareto<R, X, ADS>* _pf = nullptr)
	{
		Timer tnow;

		if(Component::information)
			cout << "search: DECODER version of Non Sorting Genetic Algorithm Search II" << endl;

		firstRatio = lastRatio = 0.0;
		bool fast = true;

		timeSort = 0;
		timeSTLSort = 0;
		timeDist = 0;
		timeLoop = 0;
		timeNewPop = 0;

		// 1. create population 'p0'
		pair<PS, PX> fPop = generateFirstPopulation(pMax);

		PS* ps = new PS(fPop.first);
		PX* px = new PX(fPop.second);

		N = ps->size(); // keep the same population size

		IndividualExtNSGAII<R, X, ADS>::zeroRanks(*ps); // update parents' ranks
		IndividualExtNSGAII<R, X, ADS>::zeroDistances(*ps); // update parents' distances

		firstRatio = px->size() / ((double) ps->size());

		vector<IndividualExtNSGAII<R, X, ADS>*> grouped_px(*px);
		IndividualExtNSGAII<R, X, ADS>::mergeSameParents(grouped_px);

		// 2. sort population by non domination
		vector<vector<IndividualNSGAII<R>*>*> F = fastNonDominatedSort(grouped_px, *ps);
		if(Component::information)
			cout << id() << ": first pareto front created with " << F.size() << " elements!" << endl;
		// also calculate crowding distance (for binary tournament!)
		Timer tDist;
		crowdingDistanceAssignment(*px, *ps);
		timeDist += tDist.now();
		if(Component::information)
			cout << id() << ": first crowding distance calculation applied!" << endl;

		//if(Component::information)
		//	IndividualExtNSGAII<R, X, ADS>::print(*px);
		logPopulation(*px, -1, tnow.now(), "first pop");

		// 3. create offspring of size 'N'
		if(Component::information)
			cout << id() << ": will generate first children!" << endl;

		pair<PS, PX> pChildren = generateNextPopulation(*ps, *px);
		PS qs = pChildren.first;
		PX qx = pChildren.second;

		//vector<IndividualNSGAII<R>*> qs = makeNewPopulation(*ps, N);
		//if(Component::information)
		//	cout << id() << ": will decode first children!" << endl;
		//vector<IndividualExtNSGAII<R, X, ADS>*> qx = decode(qs);
		//// update values that may be used in later selection of individuals
		//updateParentsObjectives(qs, qx);

		logPopulation(qx, -1, tnow.now(), "first pop qx");

		if(Component::information)
			cout << id() << ": main loop" << endl;

		vector<double> limits(nObjectives, 0.0);
		for(unsigned k = 0; k < nObjectives; k++)
		{
			vector<double> c;
			for(unsigned i=0; i<px->size(); i++)
				c.push_back(px->at(i)->mev.at(k).evaluation());
			if(v_d[k]->isMinimization())
				limits[k] = IndividualExtNSGAII<R, X, ADS>::min(c);
			else
				limits[k] = IndividualExtNSGAII<R, X, ADS>::max(c);
		}

		tMax = 1;
		int t = 1;

		while((t <= gMax) && (tnow.now() < timelimit))
		{
			if(DISPLAY_GENERAL)
			{
			//cout << endl << "===========================================" << endl;
			cout << endl << "Generation = " << t << " / " << gMax << " tMax=" << tMax << " time=" << tnow.now() << "s" << endl;
			cout << "N=" << N << " ps=" << ps->size() << " qs=" << qs.size() << " px=" << px->size() << " qx=" << qx.size() << endl;
			cout << "RATIO(|X|/|S|)=" << ((px->size() + qx.size()) / ((double) ps->size() + qs.size())) << endl;
			cout << "|F|=" << F.size() << " |F_0|=" << F[0]->size() << endl;
			double diff_cd = IndividualExtNSGAII<R, X, ADS>::countCrowdingValues(*px);
			cout << "UNIQUE CROWDING VALUES(PX): " << diff_cd << "/" << px->size() << " (" << (diff_cd * 100 / px->size()) << "%)" << endl;
			diff_cd = IndividualExtNSGAII<R, X, ADS>::countCrowdingValuesPS(*ps);
			cout << "UNIQUE CROWDING VALUES(PS): " << diff_cd << "/" << ps->size() << " (" << (diff_cd * 100 / ps->size()) << "%)" << endl;

			if(diff_cd == 1)
			{
				/*
				cout <<"WARNING: TOO LOW DIVERSITY (" << diff_cd << "), DEBUGGING: " << endl;
				IndividualExtNSGAII<R, X, ADS>::print(*ps, *px);
				printToDisk(*ps, *px);

				exit(1);
				*/
			}
			else
			{
				/*
				cout << "BETTER DIVERSITY!! " << diff_cd << endl;
				cout << "REALLY???" << endl;
				IndividualExtNSGAII<R, X, ADS>::print(*ps, *px);
				printToDisk(*ps, *px);
				exit(1);
				*/
			}


			cout << "PX LIMITS: ";
			IndividualExtNSGAII<R, X, ADS>::printLimits(nObjectives, *px);
			cout << "PS LIMITS: ";
			IndividualExtNSGAII<R, X, ADS>::printLimits(nObjectives, *ps);
			}

			for(unsigned k = 0; k < nObjectives; k++)
			{
				vector<double> c;
				for(unsigned i=0; i<px->size(); i++)
					c.push_back(px->at(i)->mev.at(k).evaluation());

				if(v_d[k]->isMinimization())
				{
					double dmin = IndividualExtNSGAII<R, X, ADS>::min(c);
					if(dmin < limits[k])
					{
						if(DISPLAY_IMPROVEMENT)
							cout << "N=" << N << " |QS|=" << qs.size() <<" \tt=" << t << "/" << gMax << "\t Improved OBJ. " << k << " to " << dmin << "\t time=" << tnow.now() << "s" << endl;
						if(DISPLAY_IMPROVEMENT)
							cout << "|PS|=" << ps->size() << " |PX|=" << px->size() << " RATIO(X/S)=" << (px->size()/(double)ps->size()) << endl;
						if(DISPLAY_IMPROVEMENT)
							IndividualNSGAII<R>::printResume(*ps);
						double tRest = tnow.now() - timeSort - timeDist - timeLoop - timeNewPop;
						cout << "times: sort=" << (timeSort*100/tnow.now()) << "%  dist=" << (timeDist*100/tnow.now()) << "% newPop=" << (timeNewPop*100/tnow.now()) << "% loop=" << (timeLoop*100/tnow.now()) << "% rest=" << (tRest*100/tnow.now()) << "%" << endl;
						limits[k] = dmin;
						if(t > tMax)
							tMax = t;
						t = 0;
						break;
					}


					if(dmin > limits[k])
					{
						cout << "ERROR: algorithm lost better solution for OBJ " << k << " '" << limits[k] << "' for '" << dmin << "'" << endl;
						exit(1);
					}

				}
				else
				{
					double dmax = IndividualExtNSGAII<R, X, ADS>::max(c);
					if(dmax < limits[k])
					{
						if(DISPLAY_IMPROVEMENT)
							cout << "t=" << t << " Improved OBJ. " << k << " to " << dmax << endl;
						limits[k] = dmax;
						if(t > tMax)
							tMax = t;
						t = 0;
						break;
					}

					if(dmax < limits[k])
					{
						cout << "ERROR: algorithm lost better solution for OBJ " << k << " '" << limits[k] << "' for '" << dmax << "'" << endl;
						exit(1);
					}
				}
			}

			for(unsigned i=0; i<ps->size(); i++)
				if(ps->at(i)->isChild)
				{
					ps->at(i)->isChild = false;
					//ps->at(i)->isCross = false;
					//ps->at(i)->isRandom = false;
					//ps->at(i)->isMutation = false;
				}

			if(LOG_POPULATIONS)
			{
				vector<MultiEvaluation*> mev_px;
				for(unsigned i = 0; i < px->size(); i++)
				{
					IndividualExtNSGAII<R, X, ADS>& ind = *px->at(i);
					mev_px.push_back(&ind.mev);
				}
				vector<MultiEvaluation*> nonDom = Pareto<R>::filterDominated(v_d, mev_px);
				stringstream ss1;
				ss1 << "generation_" << t << "_dist.log";
				FILE* f1 = fopen(ss1.str().c_str(), "w");
				for(unsigned i = 0; i < nonDom.size(); i++)
					fprintf(f1, "%d\n", ((int) nonDom[i]->at(0).evaluation()));
				fclose(f1);
				stringstream ss2;
				ss2 << "generation_" << t << "_make.log";
				FILE* f2 = fopen(ss2.str().c_str(), "w");
				for(unsigned i = 0; i < nonDom.size(); i++)
					fprintf(f2, "%d\n", ((int) nonDom[i]->at(1).evaluation()));
				fclose(f2);

				stringstream ss3;
				ss3 << "generation_rx.log";
				FILE* f3 = (t==1?fopen(ss3.str().c_str(), "w"):fopen(ss3.str().c_str(), "a"));
				fprintf(f3, "%d\n", ((int) (px->size()+qx.size())));
				fclose(f3);
				stringstream ss4;
				ss4 << "generation_px.log";
				FILE* f4 = (t==1?fopen(ss4.str().c_str(), "w"):fopen(ss4.str().c_str(), "a"));
				fprintf(f4, "%d\n", ((int) px->size()));
				fclose(f4);
				stringstream ss5;
				ss5 << "generation.log";
				FILE* f5 = (t==1?fopen(ss5.str().c_str(), "w"):fopen(ss5.str().c_str(), "a"));
				fprintf(f5, "%d\n", t);
				fclose(f5);

				Population<R>* prand = &init_pop.generatePopulation(pMax);
				vector<IndividualNSGAII<R>*>* ps_rand = convert(*prand);
				vector<IndividualExtNSGAII<R,X,ADS>*> dx = decode(*ps_rand);
				stringstream ss6;
				ss6 << "generation_rand.log";
				FILE* f6 = (t == 1 ? fopen(ss6.str().c_str(), "w") : fopen(ss6.str().c_str(), "a"));
				fprintf(f6, "%d\n", ((int) dx.size()));
				fclose(f6);
				for(unsigned i=0; i<dx.size(); i++)
					delete dx[i];
				for(unsigned i=0; i<ps_rand->size(); i++)
					delete ps_rand->at(i);

			}

			// ================
			Timer tgen;

			// 1. combine populations in 'Rt': Rt = Pt U Qt;
			if(Component::information)
			{
				cout << id() << ": joining population!" << endl;
			}
			vector<IndividualNSGAII<R>*>* rs = ps;
			ps = nullptr;
			vector<IndividualExtNSGAII<R, X, ADS>*>* rx = px;
			px = nullptr;
			rs->insert(rs->end(), qs.begin(), qs.end());
			rx->insert(rx->end(), qx.begin(), qx.end());

			IndividualExtNSGAII<R, X, ADS>::zeroRanks(*rs); // update parents' ranks
			IndividualExtNSGAII<R, X, ADS>::zeroDistances(*rs); // update parents' ranks

			for(unsigned i=0; i < rs->size(); i++)
				rs->at(i)->id = i;

			double diff = IndividualExtNSGAII<R, X, ADS>::countEquals(nObjectives, *rx);
			if(DISPLAY_GENERAL)
			cout << "UNIQUE ELEMENTS(RX): " << diff << "/" << rx->size() << " (" << (diff * 100 / rx->size()) << "%)" << endl;

			if(Component::information)
			{
				cout << id() << ": joint populations |rs|=" << rs->size() << " |rx|=" << rx->size() << endl;
				//IndividualExtNSGAII<R, X, ADS>::print(*rx);
			}

			if(Component::information)
				cout << id() << ": compressing elements" << endl;
			Timer tcompress;
			vector<IndividualExtNSGAII<R, X, ADS>*> grouped_rx(*rx);
			IndividualExtNSGAII<R, X, ADS>::mergeSameParents(grouped_rx);
			if(DISPLAY_GENERAL)
			cout << "|GROUPED|: " << grouped_rx.size() << " |RX|:" << rx->size() << ": compression of " << (((rx->size()-grouped_rx.size())*100.0)/rx->size()) << "% (took " << tcompress.inMilliSecs() << "ms)" << endl;

			Timer tsort;
			// 2. sort population by non domination
			if(Component::information)
				cout << id() << ": sorting by non domination" << endl;
			freeFronts(F);
			F = fastNonDominatedSort(grouped_rx, *rs);
			//printFronts(F);

			if(DISPLAY_GENERAL)
			cout << "sort time = " << tsort.inMilliSecs() << " ms for |rx|=" << grouped_rx.size() << endl;
			timeSort += tsort.now();
			logPopulation(*rx, t, tnow.now(), "rx sorted");


			// Apply crowding distance to full population, instead of part by part!
			// this is different from original algorithm, because in our case it's harder to compare if parents are equal after compression (grouped_rx),
			// so we apply the algorithm for everybody... (TODO analysis of tradeoff)
			crowdingDistanceAssignment(*rx, *rs);



/*
			for(unsigned i = 0; i < F.size(); i++)
			{
				cout << "S LIMITS FOR F[" << i << "/" << F.size() << "]: " << "|F[i]|=" << F[i]->size() << endl;
				IndividualExtNSGAII<R, X, ADS>::printLimits(nObjectives, *F[i]);
				for(unsigned j=0; j<F[i]->size(); j++)
					cout << "#" << F[i]->at(j)->id << " ";
                                cout << endl;
			}
*/


			// 3. Pt+1 = {}
			if(Component::information)
				cout << id() << ": creating new populations" << endl;
			vector<IndividualNSGAII<R>*>* nextPopS = new vector<IndividualNSGAII<R>*>;

			// 4. i=1
			int i = 0;

			Timer tloop;
			////int count = countNewParents(*F[i], *nextPopS);
			// while |Pt+1| + |Fi| <= N
			if(Component::information)
				cout << id() << ": while i=" << i << " (|S|:" << nextPopS->size() << " + |F[i]|:" << F[i]->size() << " <= N:" << N << ")" << endl;
			while((nextPopS->size() + F[i]->size()) <= N)
			{
				//cout << "WHILE: |S|:" << nextPopS->size() << " + |F_i|:" << F[i]->size() << " <= " << N << " for i=" << i << endl;
				// do crowding distance assignment (already done!)
				////crowdingDistanceAssignment(*F[i]);
				////IndividualExtNSGAII<R, X, ADS>::updateDistances(*F[i]); // update parents' ranks

				// Pt+1 = Pt+1 U Fi
				////addNewParents(*F[i], *nextPopS);
				nextPopS->insert(nextPopS->begin(), F[i]->begin(), F[i]->end());
				//nextPopX->insert(nextPopX->end(), F[i]->begin(), F[i]->end());

				i = i + 1;

				if(i == F.size())
				{
					cout << "ERROR! Elements from S still missing and can't reach front " << i << endl;
					exit(1);
				}
				////count = countNewParents(*F[i], *nextPopS);

				if(Component::information)
					cout << id() << ": while i=" << i << " (|S|:" << nextPopS->size() << " + |F[i]|:" << F[i]->size() << " <= N:" << N << ")" << endl;
			}

			if(nextPopS->size() < N)
			{
				if(i >= F.size())
				{
					cout << "ERROR in DecoderNSGAII! NOT EXPECTING i=" << i << " |F|=" << F.size() << endl;
					exit(1);
				}

				if(Component::information)
					cout << id() << ": will finish adding elements! i=" << i << " / |F|=" << F.size() << " |nextPopS|=" << nextPopS->size() << " < N=" << N << endl;

				if(Component::information)
					cout << id() << ": there are i=" << i << " |F_i|=" << F[i]->size() << " elements from S" << endl;

				// sort by last elements by crowding comparison (already sorted)
				////crowdingDistanceAssignment(*F[i]);
				////IndividualExtNSGAII<R, X, ADS>::updateDistances(*F[i]); // update parents' dists

				if(Component::information)
					cout << id() << ": sorting!" << endl;

				vector<IndividualNSGAII<R>*> toSort;
				for(unsigned j=0; j<F[i]->size(); j++)
					toSort.push_back(F[i]->at(j));
 
                                //NSGAIICrowdingComparison<R> comp(userSpecificComparison);

				sort(toSort.begin(), toSort.end(), IndividualExtNSGAII<R, X, ADS>::simpleCompare);
/*
				cout << "t=1 print sort!" << endl;
				for(unsigned z=0; z<toSort.size(); z++)
					toSort[z]->print();
				cout << "t=" << t << " PS LIMITS AFTER SORT:";
				IndividualExtNSGAII<R, X, ADS>::printLimits(nObjectives, toSort);
				cout << "t=" << t << " FIRST IN LIST:";
				toSort[0]->print();
*/
				//IndividualExtNSGAII<R, X, ADS>::mysort(toSort, comp);

				// Pt+1 = Pt+1 U Fi[1:(N - |Pt+1|) ]

				int DIFF = N - nextPopS->size();
				for(unsigned j=0; j < DIFF; j++)
				{
					if(j >= toSort.size())
					{
						cout << "ERROR in DecoderNSGAII: Didnt think about that..." << endl;
						exit(1);
					}

					nextPopS->push_back(toSort.at(j));
				}

				// add missing elements
				//addNewParents(toAdd, *nextPopS);
				//nextPopX->insert(nextPopX->end(), toAdd.begin(), toAdd.end());
			}

			if(DISPLAY_GENERAL)
			cout << "loop time = " << tloop.inMilliSecs() << " ms" << endl;
			timeLoop += tloop.now();


			if(nextPopS->size() < N)
			{
				cout << "ERROR: still missing elements in population! |nextPopS|=" << nextPopS->size() << " < N=" << N << endl;
				exit(1);
			}

			//cout << "Next pop PS LIMITS: ";
			//IndividualExtNSGAII<R, X, ADS>::printLimits(nObjectives, *nextPopS);

		// TEST ESTAGNATION!
		bool stagnated = true;
		for(unsigned i=0; i < ((int)nextPopS->size())-1; i++)
		for(unsigned j=i+1; j < nextPopS->size(); j++)
		{
			//cout << "i=" << i << " j=" << j << " size=" << nextPopS->size() << endl;
			/*			
			if(!nextPopS->at(i))
				continue;
			if(!nextPopS->at(j))
				continue;
			IndividualNSGAII<R>* si = nextPopS->at(i);
			IndividualNSGAII<R>* sj = nextPopS->at(j);
			si->print();
			sj->print();
			*/

				if(IndividualExtNSGAII<R, X, ADS>::simpleCompare(nextPopS->at(i), nextPopS->at(j)))
				{
					stagnated = false;
					break;
				}
				if(IndividualExtNSGAII<R, X, ADS>::simpleCompare(nextPopS->at(j), nextPopS->at(i)))
				{
					stagnated = false;
					break;
				}
		}


		if(stagnated && false)
		{
			cout << "STAGNATED AT t=" << t << "/" << gMax << " N=" << N << endl;
			//IndividualExtNSGAII<R, X, ADS>::printLimits(nObjectives, *nextPopS);
			
			while(nextPopS->size()>N/3)
				nextPopS->erase(nextPopS->begin()+(rand()%nextPopS->size()));

			cout << "REDUCED TO A THIRD: " << nextPopS->size() << endl;
			//IndividualExtNSGAII<R, X, ADS>::printLimits(nObjectives, *nextPopS);
			
		}


			vector<IndividualExtNSGAII<R, X, ADS>*>* nextPopX = new vector<IndividualExtNSGAII<R, X, ADS>*>;
			for(unsigned x = 0; x < rx->size(); x++)
			{
				IndividualExtNSGAII<R, X, ADS>* ind_x = rx->at(x);
				IndividualNSGAII<R>* ind_s = &ind_x->s;
				bool rank_elitism = (ind_x->rank == ind_x->s.rank) || true;
				if(in(*nextPopS, ind_s) && rank_elitism)
				{
					nextPopX->push_back(rx->at(x));
				}
				else
				{
					delete rx->at(x);
					rx->at(x) = nullptr;
				}
			}

			for(unsigned x=0; x<nextPopX->size(); x++)
				nextPopX->at(x)->resetList();



			//cout << "generation " << t << " final population sizes ";
			//cout << " nextPopS: " << nextPopS->size();
			//cout << " nextPopX: " << nextPopX->size();
			//cout << endl;

	
			//if(Component::information)
			//	IndividualExtNSGAII<R, X, ADS>::print(*nextPopX);
			logPopulation(*nextPopX, t, tnow.now(), "nextPopX final");

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

	/*
			vector<IndividualExtNSGAII<R, X, ADS>*> check_px = decode(*ps);
			if(check_px.size() != px->size())
			{
				cout << "ERROR! LOST 'X' INDIVIDUALS!" << endl;
				cout << "|check_px|=" << check_px.size() << endl;
				cout << "|px|=" << px->size() << endl;
				exit(1);
			}
			else
				cout << "SIZE OK FOR X POPULATION!" << endl;
	*/

			int count_child = 0;
			for(unsigned i=0; i<ps->size(); i++)
				if(ps->at(i)->isChild)
					count_child++;

			if(Component::information)
				cout << id() <<": will generate next population!" << endl;


			Timer tNewPop;
			pair<PS, PX> pChildren = generateNextPopulation(*ps, *px);
			qs = pChildren.first;
			qx = pChildren.second;
			timeNewPop += tNewPop.now();

			logPopulation(qx, t, tnow.now(), "new pop qx");

			if(DISPLAY_GENERAL)
			cout << "Finished Generation = " << t << " time=" << tgen.inMilliSecs() << "ms  grown_ups=" << count_child << endl;
			//cout << "===========================================" << endl << endl;
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

		lastRatio = px->size() / ((double) ps->size());

		// free all 'px' (not used anymore)
		for(unsigned k = 0; k < px->size(); k++)
			delete px->at(k);
		px->clear();
		delete px;

		if(Component::information)
			cout << id() << ": memory clean!" << endl;

		ExtendedPareto<R, X, ADS>* pf = new ExtendedPareto<R, X, ADS>;
		while(ps->size() > 0)
		{
			Solution<R>* s = &ps->at(0)->s.clone();
			delete ps->at(0);
			ps->erase(ps->begin() + 0);

			// CALL DECODER TO EVALUATE SOLUTIONS!

			pair<vector<Solution<X, ADS>*>, vector<MultiEvaluation*> > dec_vs = decoder.decode(*s);

			Population<X, ADS>* popx = new Population<X, ADS>;
			if(dec_vs.first.size() > 0)
			{
				// free unused solutions
				for(unsigned i = 0; i < dec_vs.first.size(); i++)
					popx->push_back(dec_vs.first[i]);
				dec_vs.first.clear();
			}

			pf->push_back(s, dec_vs.second, popx);
		}

		delete ps;

		if(Component::information)
			cout << id() << ": search finished!" << endl;

		return pf;
	}

	// calculates de crowding distance for each element of vector 'I', and stores in I[i].distance
	void crowdingDistanceAssignment(vector<IndividualExtNSGAII<R, X, ADS>*>& I, vector<IndividualNSGAII<R>*>& Ps)
	{
		int max_rank = 0;
		for(unsigned i = 0; i < I.size(); i++)
			if(I[i]->rank > max_rank)
				max_rank = I[i]->rank;
		unsigned nranks = max_rank + 1; // include 0

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
			for(unsigned r = 0; r < nranks; r++)
			{
				// I = sort(I, m)
				vector<pair<double, int> > fitness;  // (fitness, id)

				for(int i = 0; i < I.size(); i++)
					if(I[i]->rank == r)
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

				////cout << "ORDER: (rank=" << r << ") " << fitness << endl;

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
					I[i]->distance += ::abs(fitness[idx_after].first - fitness[idx_before].first) / ::abs(v_d[m]->max() - v_d[m]->min());
				}

			} // for each front
		} // for each objective

		IndividualExtNSGAII<R, X, ADS>::updateDistances(I); // update parents' ranks

/*
		specificDistanceAssignment(Ps);

		// guarantee that first is included!
		int fmin = 0;
		for(unsigned z=0; z<Ps.size(); z++)
			if(Ps[z]->mev->at(0).evaluation() < Ps[fmin]->mev->at(0).evaluation())
				fmin = z;

		Ps[fmin]->distance = 1000000; // INF
*/


	} // end function

	static bool sortByFirst(const IndividualExtNSGAII<R, X, ADS>* ind1, const IndividualExtNSGAII<R, X, ADS>* ind2)
	{
		return ind1->mev.at(0).evaluation() < ind2->mev.at(0).evaluation();
	}

	static bool sortBySecond(const IndividualExtNSGAII<R, X, ADS>* ind1, const IndividualExtNSGAII<R, X, ADS>* ind2)
	{
		return ind1->mev.at(1).evaluation() < ind2->mev.at(1).evaluation();
	}


	// calculates de ranking for each element of vector 'I', and updates in I[i].rank
	vector<vector<IndividualNSGAII<R>*>*> fastNonDominatedSort(vector<IndividualExtNSGAII<R, X, ADS>*>& P, vector<IndividualNSGAII<R>*>& Ps)
	{
		Timer tNonDomSort;

		checkUnique(P);

		vector<IndividualExtNSGAII<R, X, ADS>*> Px(P);

		sort(Px.begin(), Px.end(), sortBySecond); // any sort is good!
		stable_sort(Px.begin(), Px.end(), sortByFirst); // necessary to be stable!!
		// (otherwise, when first objective is equal then the second may be downgraded in future and destroy dominance)

		for(unsigned i=0; i<Px.size(); i++)
			Px[i]->rank = 10000000; // INF

		for(unsigned i=0; i<Px.size(); i++)
		{
			//cout << "i:" << i << " ";		
			//Px[i]->print();
		}

		unsigned count_x = 0;
		int rank = 0;
		while(count_x != Px.size())
		{
			// find first
			unsigned j = 0;
			for(j = 0; j < Px.size(); j++)
				if(Px[j]->rank == 10000000)
					break;
			Px[j]->rank = rank;
			//cout << "rank: " << rank << " to ";
			//Px[j]->print();
			int max_obj2 = Px[j]->mev.at(1).evaluation();
			int min_obj2 = max_obj2;
			//cout << "max_obj2: " << max_obj2 << " min_obj2: " << min_obj2 << endl;
			count_x++;
			vector<int> m;
			m.push_back(j);

			for(unsigned r=j+1; r<Px.size(); r++)
			if(Px[r]->rank == 10000000)
			{
				if(Px[r]->mev.at(1).evaluation() > max_obj2)
					continue; // discard element (is dominated!)

				if(Px[r]->mev.at(1).evaluation() < min_obj2) // add element (improves the best obj2!)
				{
					//cout << "r:" << r << " ";
					Px[r]->rank = rank;
					//cout << "rank: " << rank << " to ";
					//Px[r]->print();
					//cout << "min_obj2: " << min_obj2 << " => ";
					min_obj2 = Px[r]->mev.at(1).evaluation();
					//cout << min_obj2 << endl;
					count_x++;
					m.push_back(r);
					continue;
				}

				// otherwise, check on list
				bool nonDom = true;
				for(unsigned z=0; z<m.size(); z++)
					if(Px[m[z]]->mev.at(1).evaluation() <= Px[r]->mev.at(1).evaluation()) // <= is enough, because distance is better (and values are unique)
					{
						//cout << "r: " << r << " dominated by " << m[z] << " m=" << m << endl;
						nonDom = false;
						break;
					}

				if(nonDom)
				{
					//cout << "*r:" << r << " ";
					Px[r]->rank = rank;
					//cout << "rank: " << rank << " to ";
					//Px[r]->print();
					//cout << "m=" << m << endl;
					count_x++;
					m.push_back(r);
				}
				else
				{
					//cout << "r: " << r << " not selected! [" << Px[r]->mev.at(0).evaluation() << ";" << Px[r]->mev.at(1).evaluation() << "]" << endl;
				}
			}

			//cout << "FINISHED RANK " << rank << " WITH " << m << endl << endl;
			m.clear();		

			rank++;
		}

		IndividualExtNSGAII<R, X, ADS>::updateRanks(P); // update parents' ranks

		//cout << "WILL CHECK IMPROVED SORT!" << endl;
		//testNonDominationSort(P, rank);

/*
		// re-faire ranking
		vector<int> vRank(P.size(), -1);
		for(unsigned i=0; i<vRank.size(); i++)
			vRank[i] = P[i]->rank;

		IndividualExtNSGAII<R, X, ADS>::zeroRanks(Ps); // update parents' ranks

		vector<vector<IndividualNSGAII<R>*>*> F = fastNonDominatedSortClassical(P, Ps);
                freeFronts(F);

		cout << "WILL CHECK CLASSIC SORT!" << endl;
		testNonDominationSort(P, rank);


		for(unsigned i=0; i<vRank.size(); i++)
			if(vRank[i] != P[i]->rank)
			{
				cout << "ERROR IN RANKING " << i << endl;
				cout << "|vRank|=" << vRank.size() << " |P|=" << P.size() << endl;
				cout << "vRank: " << vRank[i] << " Px_i:" << P[i]->rank << endl;

				cout << "will print first!" << endl;
	                        IndividualExtNSGAII<R, X, ADS>::printToDisk(Ps, P);

				cout << "will return to last" << endl;
				for(unsigned z=0; z < P.size(); z++)
					P.at(z)->rank = vRank.at(z);

				cout << "will print second!" << endl;
	                        IndividualExtNSGAII<R, X, ADS>::printToDisk(Ps, P);
				exit(1);
			}	
*/

		vector<vector<IndividualNSGAII<R>*>*> fronts;
		unsigned count_check = 0;
		for(int r = 0; r < rank; r++)
		{
			fronts.push_back(new vector<IndividualNSGAII<R>*>);
			for(unsigned i = 0; i < Ps.size(); i++)
				if(Ps[i]->rank == r)
				{
					fronts[r]->push_back(Ps[i]);
					count_check++;
				}
		}

		if(count_check != Ps.size())
		{
			cout << "ERROR in nonDominatedSort! count_check=" << count_check << " |Ps|=" << Ps.size() << endl;
			exit(1);
		}

		return fronts;
	}


	// calculates de ranking for each element of vector 'I', and updates in I[i].rank
	vector<vector<IndividualNSGAII<R>*>*> fastNonDominatedSortClassical(vector<IndividualExtNSGAII<R, X, ADS>*>& P, vector<IndividualNSGAII<R>*>& Ps)
	{
		Timer tNonDomSort;
		if(Component::information)
			cout << id() << "::fastNonDominatedSort |P|=" << P.size() << " begin" << endl;
		//for(unsigned i=0; i<P.size(); i++)
		//	P[i]->print();

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
			F.push_back(nullptr);
			F[i] = Q; // Fi = Q
			if(Component::debug)
				cout << "fastNonDominatedSort i=" << i << " |Q|=" << Q->size() << endl;
		}

		// update all rank information
		//cout << "printing ranks (" << Ps.size() << ") : ";
		//for(unsigned i=0; i<Ps.size(); i++)
		//	cout << "#" << Ps[i]->id << " " << Ps[i]->rank << " ";
		//cout << endl;

		IndividualExtNSGAII<R, X, ADS>::updateRanks(P); // update parents' ranks

		//cout << "printing ranks (" << Ps.size() << ") : ";
		//for(unsigned i=0; i<Ps.size(); i++)
		//	cout << "#" << Ps[i]->id << " " << Ps[i]->rank << " ";
		//cout << endl;

		unsigned count_check = 0;
		vector<vector<IndividualNSGAII<R>*>*> fronts;
		for(unsigned i = 0; i < F.size(); i++)
		{
			vector<IndividualNSGAII<R>*>* f_i = new vector<IndividualNSGAII<R>*>;

			for(unsigned j = 0; j < Ps.size(); j++)
				if(Ps[j]->rank == i)
				{
					f_i->push_back(Ps[j]);
					count_check++;
				}

			if(f_i->size() > 0)
				fronts.push_back(f_i);
			else
				delete f_i;
		}

/*
		if(count_check != Ps.size())
		{
			cout << "ERROR in fastNonDominatedSort! count_check=" << count_check << " != |Ps|=" << Ps.size() << endl;
			exit(1);
		}
*/

		freeFronts(F);

		if(Component::information)
		{
			cout << id() << "::fastNonDominatedSort found " << fronts.size() << " fronts" << endl;
			printFronts(fronts);
			cout << id() << "::fastNonDominatedSort end" << endl;
		}

		//cout << "TOOK: " << tNonDomSort.now() << "s" << endl;
		//getchar();
		//getchar();

		return fronts;
	}

        // specific distance assignment may sum other values for distance metric
	virtual void specificDistanceAssignment(vector<IndividualNSGAII<R>*>& Ps)
	{
	}


	// function to log data from the population
	virtual void logPopulation(const vector<IndividualExtNSGAII<R, X, ADS>*>& px, int generation, double time, string other)
	{
	}

	virtual void checkUnique(const vector<IndividualExtNSGAII<R, X, ADS>*>& P)
	{
		for(unsigned i=0; i<P.size()-1; i++)
			for(unsigned j=i+1; j<P.size(); j++)
				if((P[i]->mev.at(0).evaluation()==P[j]->mev.at(0).evaluation()) && (P[i]->mev.at(1).evaluation()==P[j]->mev.at(1).evaluation()))
				{
					cout << "ERROR IN checkUnique! elements #" << i << " and #" << j << " are equal! " << endl;
					P[i]->print();
					P[j]->print();
					exit(1);
				}
	}

	virtual void testNonDominationSort(const vector<IndividualExtNSGAII<R, X, ADS>*>& P, int rank) // 'rank' is the number of ranks
	{
		// first test, elements from same front can't dominate each other
		for(int r=0; r<rank; r++)
			for(unsigned i=0; i<P.size(); i++)
				for(unsigned j=0; j<P.size(); j++)
					if((i != j) && (P[i]->rank==r) && (P[j]->rank==r))
					{
						pair<bool, bool> ret = pDominance.birelation(P[i]->mev, P[j]->mev);
						if(ret.first || ret.second)
						{
							cout << "Strange error: " << i << "(rank " << r << ") should not dominate " << j << " (rank " << P[j]->rank << ")" << endl;
							P[i]->print();
							P[j]->print();
							getchar();
							getchar();
						}

					}
		cout << "PASSED TEST 1" << endl;

		// second test, check if somebody dominate the element from a previous front
		for(int r=1; r<rank; r++)
			for(unsigned i=0; i<P.size(); i++)
			{
				if(P[i]->rank!=r)
					continue;

				bool dominate = false;
				for(unsigned j=0; j<P.size(); j++)
					if((i != j) && (P[j]->rank==(r-1)))
					{
						bool dom = pDominance.dominates(P[j]->mev, P[i]->mev);
						if(dom)
						{
							dominate = true;
							break;
						}
					}

				if(!dominate)
				{
					cout << "Strange error: #" << i << "(rank " << r << ") is not dominated by previous front " << (r-1) << endl;
					P[i]->print();
					cout << "FRONT " << (r-1) << endl;
					for(unsigned j=0; j<P.size(); j++)
						if(P[j]->rank == (r-1))
						{
							cout << "#" << j << " ";
							P[j]->print();
						}
					getchar();
					getchar();
				}
			}
		cout << "PASSED TEST 2" << endl;

		cout << "OKAY ALL TESTS!" << endl;
	}

	// returns id() of class
	string id() const
	{
		return "DecoderNSGAII";
	}

};


}

#endif /*OPTFRAME_DECODER_NONSORTINGGENETICALGORITHMII_HPP_*/
