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

#ifndef OPTFRAME_DECODER_POPULATION_BASED_MOS_HPP_
#define OPTFRAME_DECODER_POPULATION_BASED_MOS_HPP_

#include "../../MultiObjSearch.hpp"
#include "../../Timer.hpp"

// EXTENDED PARETO
#include "../../ExtendedMultiObjSearch.hpp"

#include "PopulationBasedMultiObjSearch.hpp"

#include "MOSExtIndividual.hpp"

#include "FitnessAssignment.hpp"
#include "DiversityManagement.hpp"
#include "Elitism.hpp"
#include "PopulationManagement.hpp"
#include "ExtMOSSelection.hpp"

namespace optframe
{

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class DecodedPopulationMultiObjSearch: public PopulationBasedMultiObjSearch<X>
{
protected:
	Decoder<R, X>& decoder;
	PopulationManagement<R>& popMan;
	FitnessAssignment<X>& fa;
	DiversityManagement<X>& dm;
	Elitism<X>& elitism;
	ExtMOSSelection<R, X>& sel;

	unsigned popSize;
	int maxIter;
	int maxGen;

public:
	DecodedPopulationMultiObjSearch(MultiDirection<>& mDir, Decoder<R, X>& _decoder, PopulationManagement<R>& _popMan, FitnessAssignment<X>& _fa, DiversityManagement<X>& _dm, Elitism<X>& _elitism, ExtMOSSelection<R, X>& _sel, unsigned _popSize, int _maxIter, int _maxGen = 100000000) :
			PopulationBasedMultiObjSearch<X>(mDir), decoder(_decoder), popMan(_popMan), fa(_fa), dm(_dm), elitism(_elitism), sel(_sel), popSize(_popSize), maxIter(_maxIter), maxGen(_maxGen)
	{
	}

	virtual ~DecodedPopulationMultiObjSearch()
	{
	}

	virtual vector<MOSExtIndividual<R, X>*>& decode(vector<MOSIndividual<R>*>& PS)
	{
		vector<MOSExtIndividual<R, X>*>* PX = new vector<MOSExtIndividual<R, X>*>;

		for(unsigned s = 0; s < PS.size(); s++)
		{
			pair<vector<Solution<X>*>, vector<MultiEvaluation<>*> > dx = decoder.decode(*PS.at(s)->s);

			for(unsigned x = 0; x < dx.first.size(); x++)
			{
				Solution<X>* ss = dx.first[x];
				MultiEvaluation<>* ee = dx.second[x];
				MOSIndividual<R>* ind = PS.at(s);
				MOSExtIndividual<R, X>* extind = new MOSExtIndividual<R, X>(ss, ee, ind);
				PX->push_back(extind);
			}
		}

		return *PX;
	}

	virtual void assignFitness(vector<MOSIndividual<X>*>& g, const vector<const MOSIndividual<X>*>& PX)
	{
		fa.assignFitnessGroup(g, PX);
	}

	inline void assignBestFitness(vector<MOSIndividual<R>*>& PS, vector<MOSExtIndividual<R, X>*>& PX)
	{
		// set worst value in 'PS'
		// get best from 'PX'
	}

	virtual void extendedAssignFitness(vector<MOSIndividual<R>*>& PS, vector<MOSExtIndividual<R, X>*>& PX)
	{
		// simple, use classic fitness assignment

		vector<MOSIndividual<X>*> PXind;
		vector<const MOSIndividual<X>*> PXconst;
		for(unsigned x = 0; x < PX.size(); x++)
		{
			PXind.push_back(PX[x]);
			PXconst.push_back(PX[x]);
		}
		assignFitness(PXind, PXconst);
		// get best
		assignBestFitness(PS, PX);
	}

	virtual void assignDiversity(vector<MOSIndividual<X>*>& g, const vector<const MOSIndividual<X>*>& PX)
	{
		vector<const MOSIndividual<X>*> PXconst(PX.begin(), PX.end());
		dm.assignDiversityGroup(g, PXconst);
	}

	virtual void extendedAssignDiversity(vector<MOSIndividual<R>*>& PS, vector<MOSExtIndividual<R, X>*>& PX)
	{
		// tricky, separate individuals and then apply classic diversity techniques
		for(unsigned x = 0; x < PX.size(); x++)
		{
			// filter solutions that come from same parent
			vector<const MOSIndividual<X>*> PXfilter;
			PXfilter.push_back(PX[x]);
			for(unsigned i = 0; i < PX.size(); i++)
				if((i != x) && (&PX[x]->parent != &PX[i]->parent))
					PXfilter.push_back(PX[i]);

			vector<MOSIndividual<X>*> v;
			v.push_back(PX[x]);
			assignDiversity(v, PXfilter);
		}

		// GET BEST!
		assignBestDiversity(PS, PX);
	}

	inline void assignBestDiversity(vector<MOSIndividual<R>*>& PS, vector<MOSExtIndividual<R, X>*>& PX)
	{
		// set worst value in 'PS'
		// get best from 'PX'
	}

	void updateArchive(const vector<const MOSIndividual<X>*>& PX, vector<MOSIndividual<X>*>& archiveX)
	{
		// GET 'PX'
		elitism.updateArchive(PX, archiveX);

		// UPDATE ARCHIVE IN 'S' TOO
	}

	virtual void extendedUpdateArchive(vector<MOSIndividual<R>*>& PS, vector<MOSExtIndividual<R, X>*>& PX, vector<MOSIndividual<R>*>& archiveS, vector<MOSExtIndividual<R, X>*>& archiveX)
	{
		vector<const MOSIndividual<X>*> vPX;
		for(unsigned x = 0; x < PX.size(); x++)
			vPX.push_back(PX[x]);

		vector<MOSIndividual<X>*> varchiveX;
		for(unsigned x = 0; x < varchiveX.size(); x++)
			varchiveX.push_back(archiveX[x]);

		// CLASSIC ARCHIVING IN 'X'
		updateArchive(vPX, varchiveX);

		// UPDATE ARCHIVE IN 'S' TOO
		// TODO: USE INFORMATION FROM PAPER
	}


	// free populations and update archives for the last time
	virtual void freePopulations(vector<MOSIndividual<R>*>& PS, vector<MOSExtIndividual<R, X>*>& PX, vector<MOSIndividual<R>*>& archiveS, vector<MOSExtIndividual<R, X>*>& archiveX)
	{
		extendedUpdateArchive(PS, PX, archiveS, archiveX);

		for(unsigned i = 0; i < PS.size(); i++)
			delete PS[i];
		for(unsigned i = 0; i < PX.size(); i++)
			delete PX[i];
	}

	vector<MOSIndividual<X>*> getBase(vector<MOSExtIndividual<R, X>*> v)
	{
		vector<MOSIndividual<X>*> vbase(v.size());
		for(unsigned i = 0; i < v.size(); i++)
			vbase[i] = v[i];
		return vbase;
	}

	Pareto<X>* search(double timelimit = 100000000, double target_f = 0, Pareto<X>* _pf = nullptr)
	{
		ExtendedPareto<R, X, ADS, DS>* extp = searchX(timelimit, target_f, nullptr);
		Pareto<X>* p = extp->getPareto();
		delete extp;
		return p;
	}

	ExtendedPareto<R, X, ADS, DS>* searchX(double timelimit = 100000000, double target_f = 0, ExtendedPareto<R, X, ADS, DS>* _pf = nullptr)
	{
		Timer timer;

		cout << "Population Based Multi Objective Search search()" << endl;

		vector<double> best = this->initializeBounds();

		// initialize archives
		vector<MOSIndividual<R>*> archiveS;
		vector<MOSExtIndividual<R, X>*> archiveX;

		// initilize populations
		vector<MOSIndividual<R>*>& PS = popMan.initialize(popSize);
		vector<MOSExtIndividual<R, X>*>& PX = decode(PS);

		extendedAssignFitness(PS, PX);
		extendedAssignDiversity(PS, PX);

		vector<MOSIndividual<X>*> PXbase = getBase(PX);
		this->updateBounds(PXbase, best);

		vector<const MOSIndividual<R>*> PSconst(PS.begin(), PS.end());
		vector<MOSIndividual<R>*>& QS = popMan.createNext(popSize, PSconst);
		vector<MOSExtIndividual<R, X>*>& QX = decode(QS);

		vector<MOSIndividual<X>*> QXbase = getBase(QX);
		this->updateBounds(QXbase, best);

		int t = 0;
		int tImp = 0;

		while((timer.now() < timelimit) && (t <= maxGen) && (tImp <= maxIter))
		{
			PS.insert(PS.end(), QS.begin(), QS.end());
			QS.clear();
			PX.insert(PX.end(), QX.begin(), QX.end());
			QX.clear();

			extendedAssignFitness(PS, PX);
			extendedAssignDiversity(PS, PX);

			// select and free unused individuals in PS and PX
			// can also use archive, but update will come next
			sel.extSelect(popSize, PS, PX, archiveX);

			extendedUpdateArchive(PS, PX, archiveS, archiveX);

			// generating next population
			vector<const MOSIndividual<R>*> PSconst(PS.begin(), PS.end());
			QS = popMan.createNext(popSize, PSconst);
			QX = decode(QS);

			vector<MOSIndividual<X>*> QXbase = getBase(QX);
			bool improved = this->updateBounds(QXbase, best);

			if(improved)
			{
				tImp = -1;
				cout << "t=" << t << " improved bounds: " << best << endl;
			}

			t++;
			tImp++;
		}

		// populations 'PS' and 'PX' will be deleted
		freePopulations(PS, PX, archiveS, archiveX);

		ExtendedPareto<R, X, ADS, DS>* pf = new ExtendedPareto<R, X, ADS, DS>;
		// TODO: update 'pf' properly with all information
		/*
		for(unsigned i = 0; i < archiveS.size(); i++)
		{
			Solution<R>* s = archiveS.at(i)->s;
			MultiEvaluation<>* mev = archiveS.at(i)->mev;
			pf->push_back(s, mev);
		}
		*/

		return pf;
	}

};

/*

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

 */

}

#endif /*OPTFRAME_DECODER_POPULATION_BASED_MOS_HPP_*/
