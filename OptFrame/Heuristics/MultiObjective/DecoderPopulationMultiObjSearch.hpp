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

#ifndef OPTFRAME_DECODER_POPULATION_BASED_MOS_HPP_
#define OPTFRAME_DECODER_POPULATION_BASED_MOS_HPP_

#include "../../MultiObjSearch.hpp"
#include "../../Timer.hpp"

#include "MOSIndividual.hpp"
#include "FitnessAssignment.hpp"
#include "DiversityManagement.hpp"
#include "Elitism.hpp"
#include "PopulationManagement.hpp"

namespace optframe
{

// MultiObjSearch Extended Individual
template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MOSExtIndividual : public MOSIndividual<X, ADS, DS>
{
public:

	MOSIndividual<R>& parent;

	MOSExtIndividual(Solution<X, ADS>* s, MultiEvaluation<DS>* mev, MOSIndividual<R>* _parent) :
		MOSIndividual<X, ADS, DS>(s, mev), parent(*_parent)
	{
	}

	MOSExtIndividual(Solution<R, ADS>& s, MultiEvaluation<DS>& mev, MOSIndividual<R>* _parent) :
		MOSIndividual<X, ADS, DS>(s, mev), parent(*_parent)
	{
	}

	MOSExtIndividual(const MOSExtIndividual<R, X, ADS, DS>& ind) :
			s(ind.s.clone()), mev(ind.mev.clone()), parent(ind.parent)
	{
		fitness = ind.rank;
		diversity = ind.distance;

		id = ind.id;
	}

	virtual ~MOSExtIndividual()
	{
	}

	virtual void print() const
	{
		cout << "MOSExtIndividual: parent=" << &parent << " fitness=" << fitness << "\t diversity=" << diversity;
		cout << "\t[ ";
		for(unsigned e = 0; e < mev.size(); e++)
			cout << mev.at(e).evaluation() << (e == mev.size() - 1 ? " " : " ; ");
		cout << " ]";
		cout << endl;
	}

	virtual MOSIndividual<R, ADS>& clone() const
	{
		return *new MOSExtIndividual<R, ADS>(*this);
	}

};


template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class DecoderPopulationMultiObjSearch: public MultiObjSearch<R, ADS, DS>
{
protected:
	Decoder<R, X, ADS, DS>& decoder;

	vector<Direction<DS>*> vDir;

	FitnessAssignment<X, ADS>& fa;
	DiversityManagement<X, ADS>& dm;
	Elitism<X, ADS>& elitism;
	PopulationManagement<R>& popMan;

	unsigned popSize;

	// stopping criteria (-1: indicates unused)
	int maxGen;  // total number of generations
	int maxIter; // generations without improvement

public:
	DecoderPopulationMultiObjSearch(Decoder<R, X, ADS, DS>& _decoder, vector<Direction<DS>*> _vDir, FitnessAssignment<R, ADS, DS>& _fa, DiversityManagement<R, ADS, DS>& _dm, Elitism<R, ADS, DS>& _elitism, PopulationManagement<R, ADS, DS>& _popMan, unsigned _popSize, int _maxIter, int _maxGen = 100000000) :
			decoder(_decoder), vDir(_vDir), fa(_fa), dm(_dm), elitism(_elitism), popMan(_popMan), popSize(_popSize), maxGen(_maxGen), maxIter(_maxIter)
	{
	}

	virtual ~DecoderPopulationMultiObjSearch()
	{
	}

	inline vector<MOSExtIndividual<X, ADS, DS>*> decode(const vector<const MOSIndividual<R>*>& PS)
	{
		vector<MOSExtIndividual<X, ADS, DS>*> PX;
		for(unsigned s = 0; s < PS.size(); s++)
		{
			pair<vector<Solution<X, ADS>*>, vector<MultiEvaluation<DS>*> > dx = decoder.decode(PS[i]->s);

			for(unsigned x = 0; x < dx.first.size(); x++)
			{
				PX.push_back(new MOSExtIndividual<R, X, ADS, DS>(dx.first[x], dx.second[x], PS[i]));
			}
		}
		return PX;
	}

	void spreadBestFitness(vector<MOSIndividual<X, ADS, DS>*>& PS, const vector<MOSExtIndividual<X, ADS, DS>*>& PX)
	{
		if(PS.size() == 0)
			return;

		double worst = PS.at(0)->minFitness()?10000000:-10000000;
		for(unsigned s=0; s<PS.size(); s++)
			PS[s]->fitness = worst;

		for(unsigned x = 0; x < PX.size(); x++)
		{
			for(unsigned j = 0; j < PX[x]->ls.size(); j++)
			{
				if(PX[x]->rank < PX[x]->ls.at(j)->rank)
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

	static void updateDistances(const vector<IndividualExtNSGAII<R, X, ADS, DS>*>& vx)
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



	virtual Pareto<R, ADS, DS>* search(double timelimit = 100000000, double target_f = 0, Pareto<R, ADS, DS>* _pf = NULL)
	{
		Timer timer;

		cout << "Population Based Multi Objective Search search() with" << endl;
		cout << "FitnessAssignment: ";
		fa.print();
		cout << "DiversityManagement: ";
		dm.print();
		cout << "Elitism: ";
		elitism.print();
		cout << "|Population| = " << popSize << endl;
		cout << "PopulationManagement: ";
		popMan.print();

		vector<MOSIndividual<R>*> archiveS;
		vector<MOSIndividual<X>*> archiveX;

		vector<MOSIndividual<R>*> PS = popMan.initialize(popSize);
		vector<MOSExtIndividual<X, ADS, DS>*> PX = decode(PS);
		vector<MOSExtIndividual<X, ADS, DS>*> CX = PX;
		MOSIndividual<X, ADS, DS>::compress(CX);

		fa.assignFitness(CX);
		dm.assignDiversity(CX);

		vector<MOSIndividual<R>*> QS = popMan.createNext(popSize, PS);
		vector<MOSIndividual<X, ADS, DS>*> QX = decode(QS);

		int t = 0;
		int tImp = 0;
		vector<double> bestObj(vDir.size());
		for(unsigned i = 0; i < bestObj.size(); i++)
			bestObj[i] = vDir[i]->worst();

		while((timer.now() < timelimit) && (t <= maxGen) && (tImp <= maxIter))
		{
			P.insert(P.end(), Q.begin(), Q.end());

			fa.assignFitness(P);
			dm.assignDiversity(P);

			elitism.select(P, archive);

			// archive is updated
			// unused already free'd

			bool improved = false;
			for(unsigned s = 0; s < archive.size(); s++)
				for(unsigned i = 0; i < bestObj.size(); i++)
					if(vDir[i]->betterThan(archive[s]->mev.at(i).evaluation(), bestObj[i]))
					{
						// IMPROVEMENT IN ONE OBJECTIVE
						improved = true;
						bestObj[i] = archive[s]->mev.at(i).evaluation();
					}

			if(improved)
			{
				tImp = -1;
				cout << "t=" << t << " improved bounds: " << bestObj << endl;
			}

			// generating next population
			Q = popMan.createNext(popSize, P);
			t++;
			tImp++;
		}

		elitism.free(P, archive);

		Pareto<R, ADS, DS>* pf = new Pareto<R, ADS, DS>;
		for(unsigned i = 0; i < archive.size(); i++)
		{
			Solution<R, ADS>* s = &archive.at(i)->s;
			MultiEvaluation<DS>* mev = &archive.at(i)->mev;
			pf->push_back(s, mev);
		}
		return pf;
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class DecoderClassicNSGAII: public PopulationBasedMultiObjSearch<R, ADS, DS>
{
public:
	DecoderClassicNSGAII(vector<Direction<DS>*> vDir, MultiEvaluator<R, ADS, DS>& muev, InitialPopulation<R, ADS>& initPop, vector<NS<RepCARP>*> mutations, double mutationRate, vector<GeneralCrossover<RepCARP>*> crossovers, double renewRate, RandGen& rg, unsigned popSize, int maxIter, int maxGen = 100000000) :
			DecoderPopulationBasedMultiObjSearch<R, ADS, DS>(vDir, (vDir.size() == 2 ? *new BiObjNonDominatedSort<R, ADS, DS>(vDir) : *new NonDominatedSort<R, ADS, DS>(vDir)), *new CrowdingDistance<R, ADS, DS>(vDir), *new NSGAIISelection<R, ADS, DS>, *new BasicPopulationManagement<R, ADS, DS>(muev, initPop, mutations, mutationRate, crossovers, renewRate, rg), popSize, maxIter, maxGen)
	{
	}

	virtual ~DecoderClassicNSGAII()
	{
		delete &this->fa;
		delete &this->dm;
		delete &this->elitism;
		delete &this->popMan;
	}
};

}

#endif /*OPTFRAME_DECODER_POPULATION_BASED_MOS_HPP_*/
