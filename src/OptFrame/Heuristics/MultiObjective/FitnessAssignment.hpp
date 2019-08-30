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

#ifndef OPTFRAME_FITNESS_ASSIGNMENT_HPP_
#define OPTFRAME_FITNESS_ASSIGNMENT_HPP_

#include "MOSIndividual.hpp"
#include "../../Component.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class FitnessAssignment: public Component
{
public:

	FitnessAssignment()
	{
	}

	virtual ~FitnessAssignment()
	{
	}

	// assign fitness to individual 's' according to population 'P'
	virtual void assignFitnessIndividual(MOSIndividual<R, ADS, DS>& s, const vector<const MOSIndividual<R, ADS, DS>*>& P)
	{
		vector<MOSIndividual<R, ADS, DS>*> v;
		v.push_back(&s);
		assignFitnessGroup(v, P);
	}

	// assign fitness to all individuals from population 'P'
	virtual void assignFitnessAll(vector<MOSIndividual<R, ADS, DS>*>& P)
	{
		vector<const MOSIndividual<R, ADS, DS>*> Pconst(P.begin(), P.end());
		assignFitnessGroup(P, Pconst);
	}

	// assign fitness to group of individuals 'g' according to population 'P'
	virtual void assignFitnessGroup(vector<MOSIndividual<R, ADS, DS>*>& g, const vector<const MOSIndividual<R, ADS, DS>*>& P) = 0;

	virtual void print() const
	{
		cout << "FitnessAssignment" << endl;
	}

	virtual string id() const
	{
		return "FitnessAssignment";
	}
};

template<class DS = OPTFRAME_DEFAULT_DS>
struct FitnessIndividual
{
	int idx;
	double fitness;
	MultiEvaluation<DS>* mev;

	FitnessIndividual()
	{
		idx = -1;
		fitness = 0;
		mev = nullptr;
	}

	FitnessIndividual(int _idx, double _fitness, MultiEvaluation<DS>* _mev) :
			idx(_idx), fitness(_fitness), mev(_mev)
	{
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class BiObjNonDominatedSort: public FitnessAssignment<R, ADS, DS>
{
public:
	vector<Direction<DS>*> vDir;

	BiObjNonDominatedSort(vector<Direction<DS>*>& _vDir) :
			vDir(_vDir)
	{
		assert(vDir.size() == 2);
	}

	virtual ~BiObjNonDominatedSort()
	{
	}

	static bool sortByFirst(const FitnessIndividual<DS>& ind1, const FitnessIndividual<DS>& ind2)
	{
		return ind1.mev->at(0).evaluation() < ind2.mev->at(0).evaluation();
	}

	static bool sortBySecond(const FitnessIndividual<DS>& ind1, const FitnessIndividual<DS>& ind2)
	{
		return ind1.mev->at(1).evaluation() < ind1.mev->at(1).evaluation();
	}

	virtual void assignFitnessGroup(vector<MOSIndividual<R, ADS, DS>*>& g, const vector<const MOSIndividual<R, ADS, DS>*>& Pop)
	{
		// ASSUMES UNIQUE ELEMENTS IN 'Pop'

		const int INF = 10000000;

		vector<FitnessIndividual<DS> > P(Pop.size());
		for(unsigned s = 0; s < P.size(); s++)
			P[s] = FitnessIndividual<DS>(s, INF, Pop[s]->mev);

		sort(P.begin(), P.end(), sortBySecond); // any sort is good!
		stable_sort(P.begin(), P.end(), sortByFirst); // necessary to be stable!!
		// (otherwise, when first objective is equal then the second may be downgraded in future and destroy dominance)

		unsigned count_x = 0;
		int rank = 0;
		while(count_x != P.size())
		{
			// find first
			unsigned j = 0;
			for(j = 0; j < P.size(); j++)
				if(P[j].fitness == INF)
					break;
			P[j].fitness = rank;
			//cout << "rank: " << rank << " to ";
			//P[j]->print();
			int max_obj2 = P[j].mev->at(1).evaluation();
			int min_obj2 = max_obj2;
			//cout << "max_obj2: " << max_obj2 << " min_obj2: " << min_obj2 << endl;
			count_x++;
			vector<int> m;
			m.push_back(j);

			for(unsigned r = j + 1; r < P.size(); r++)
				if(P[r].fitness == INF)
				{
					if(P[r].mev->at(1).evaluation() > max_obj2)
						continue; // discard element (is dominated!)

					if(P[r].mev->at(1).evaluation() < min_obj2) // add element (improves the best obj2!)
					{
						//cout << "r:" << r << " ";
						P[r].fitness = rank;
						//cout << "rank: " << rank << " to ";
						//P[r]->print();
						//cout << "min_obj2: " << min_obj2 << " => ";
						min_obj2 = P[r].mev->at(1).evaluation();
						//cout << min_obj2 << endl;
						count_x++;
						m.push_back(r);
						continue;
					}

					// otherwise, check on list
					bool nonDom = true;
					for(unsigned z = 0; z < m.size(); z++)
						if(P[m[z]].mev->at(1).evaluation() <= P[r].mev->at(1).evaluation()) // <= is enough, because distance is better (and values are unique)
						{
							//cout << "r: " << r << " dominated by " << m[z] << " m=" << m << endl;
							nonDom = false;
							break;
						}

					if(nonDom)
					{
						//cout << "*r:" << r << " ";
						P[r].fitness = rank;
						//cout << "rank: " << rank << " to ";
						//P[r]->print();
						//cout << "m=" << m << endl;
						count_x++;
						m.push_back(r);
					}
					else
					{
						//cout << "r: " << r << " not selected! [" << P[r]->mev.at(0).evaluation() << ";" << P[r]->mev.at(1).evaluation() << "]" << endl;
					}
				}

			//cout << "FINISHED RANK " << rank << " WITH " << m << endl << endl;
			m.clear();

			rank++;
		}

		for(unsigned s = 0; s < g.size(); s++)
			for(unsigned k = 0; k < P.size(); k++)
				if(g[s]->id == Pop[P[k].idx]->id)
				{
					g[s]->fitness = P[k].fitness;
					break;
				}
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NonDominatedSort: public FitnessAssignment<R, ADS, DS>
{
public:

	vector<Direction<DS>*> vDir;

	NonDominatedSort(vector<Direction<DS>*>& _vDir) :
			vDir(_vDir)
	{
	}

	virtual ~NonDominatedSort()
	{
	}

	virtual void assignFitnessGroup(vector<MOSIndividual<R, ADS, DS>*>& g, const vector<const MOSIndividual<R, ADS, DS>*>& Pop)
	{
		ParetoDominance<R, ADS, DS> pDominance(vDir);

		vector<FitnessIndividual<DS> > P(Pop.size());
		for(unsigned s = 0; s < P.size(); s++)
			P[s] = FitnessIndividual<DS>(s, -1, Pop[s]->mev);

		if(Component::information)
			cout << this->id() << "::fastNonDominatedSort |P|=" << P.size() << " begin" << endl;

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
					pair<bool, bool> v = pDominance.birelation(*P.at(p).mev, *P.at(q).mev);

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
				P[p].fitness = 0; // p rank = 1
				F[0]->push_back(p);  // F_1 = F_1 U {p}
			}
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
						P[q].fitness = i + 1;  // q rank = i+1
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

		// finished, P is updated, update group 'g'
		for(unsigned s = 0; s < g.size(); s++)
			for(unsigned k = 0; k < P.size(); k++)
				if(g[s]->id == Pop[P[k].idx]->id)
				{
					g[s]->fitness = P[k].fitness;
					break;
				}

	}
};

}

#endif /*OPTFRAME_FITNESS_ASSIGNMENT_HPP_*/
