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

#ifndef OPTFRAME_MOMETRICS_HPP_
#define OPTFRAME_MOMETRICS_HPP_

#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

//#include "Solution.hpp"
//#include "Population.hpp"
//#include "Evaluation.hpp"
//#include "Direction.hpp"

#include "MultiEvaluator.hpp"
#include "MultiEvaluation.hpp"

#include "Pareto.hpp"
#include "ParetoDominance.hpp"
#include "ParetoDominanceWeak.hpp"

namespace optframe
{


template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class MOMETRICS
{
protected:
//	vector<Direction*> v_d;
	ParetoDominance<R, ADS>* pDom;
	ParetoDominanceWeak<R, ADS>* pDomWeak;
	Pareto<R, ADS> p;

//	bool addSolution(vector<vector<double> >& p, vector<double>& s)
//	{
//		bool added = true;
//		for (int ind = 0; ind < p.size(); ind++)
//		{
//
//			if (weakDominates(p[ind], s))
//				return false;
//
//			if (dominates(s, p.at(ind)))
//			{
//				p.erase(p.begin() + ind); //Esta perdendo o individuo retornado,tem problema? todo
//				ind--;
//			}
//		}
//
//		if (added)
//			p.push_back(s);
//
//		return added;
//	}
//
//	// true if 's1' dominates 's2'
//	virtual bool weakDominates(const vector<double>& s1, const vector<double>& s2)
//	{
//
//		int better = 0;
//		int equals = 0;
//
//		for (int e = 0; e < s1.size(); e++)
//		{
//			if (v_e[e]->betterThan(s1[e], s2[e]))
//				better++;
//
//			if (abs(s1[e] - s2[e]) < 0.0001)
//				equals++;
//		}
//
//		return ((better + equals == v_e.size()));
//	}
//
//	// true if 's1' weak dominates 's2'
//	virtual bool dominates(const vector<double>& s1, const vector<double>& s2)
//	{
//
//		int better = 0;
//		int equals = 0;
//
//		for (int e = 0; e < s1.size(); e++)
//		{
//			if (v_e[e]->betterThan(s1[e], s2[e]))
//				better++;
//
//			if (abs(s1[e] - s2[e]) < 0.0001)
//				equals++;
//		}
//
//		return ((better + equals == v_e.size()) && (better > 0));
//	}

public:

	MOMETRICS(vector<Direction*> _v_d)
	{
		pDom = new ParetoDominance<R, ADS>(_v_d);
		pDomWeak = new ParetoDominanceWeak<R, ADS>(_v_d);
	}

	MOMETRICS(vector<Evaluator<R, ADS>*> _v_e)
	{
		pDom = new ParetoDominance<R, ADS>(_v_e);
		pDomWeak = new ParetoDominanceWeak<R, ADS>(_v_e);
	}

	MOMETRICS(int nObj)
	{
		cout << "Be careful, some methods of MOMETRICS might results in error! \n Direction is being created only as Minimization! " << endl;
		vector<Direction*> v_d;
		for (int o = 0; o < nObj; o++)
		{
			Minimization* m = new Minimization;
			v_d.push_back(m);
		}

		pDom = new ParetoDominance<R, ADS>(v_d);
		pDomWeak = new ParetoDominanceWeak<R, ADS>(v_d);

		//		cout << "Be careful, some methods of MOMETRICS might results in error! \n pDom and pDomWeak were not initialized." << endl;
	}

	File* createFile(string filename)
	{
		File* file;

		try
		{
			file = new File(filename);
		} catch (FileNotFound& f)
		{
			cout << "File '" << filename << "' not found" << endl;
			exit(1);
		}
		return file;
	}

	vector<vector<double> > readPF(string caminho, int nTests, int nOF)
	{
		vector<vector<double> > D;

		for (int e = 0; e < nTests; e++)
		{
			stringstream ss;
			ss << caminho;

			Scanner scanner(createFile(caminho));
			while (scanner.hasNext())
			{
				vector<double> ind;
				for (int o = 0; o < nOF; o++)
					ind.push_back(scanner.nextDouble());

				p.addSolution(*pDom, *pDomWeak, D, ind);
			}

		}

		return D;
	}

	void addSol(vector<vector<double> >& D, vector<double> ind)
	{
		p.addSolution(*pDom, *pDomWeak, D, ind);
	}

	vector<vector<double> > unionSets(vector<vector<double> > D1, vector<vector<double> > D2)
	{
		vector<vector<double> > ref = D1;

		for (int ind = 0; ind < D2.size(); ind++)
			p.addSolution(*pDom, *pDomWeak, ref, D2[ind]);

		return ref;
	}

	vector<vector<double> > getParetoEvaluations(Pareto<R, ADS>& pf, int nEv)
	{
		vector<MultiEvaluation*> vEval = pf.getParetoFront();
		int nObtainedParetoSol = vEval.size();

		vector<vector<double> > paretoDoubleEval;

		for (int i = 0; i < nObtainedParetoSol; i++)
		{
			int nObtainedParetoSol = vEval.size();
			vector<double> solEvaluations;
			for (int ev = 0; ev < nEv; ev++)
				solEvaluations.push_back(vEval[i]->at(ev).getObjFunction());

			paretoDoubleEval.push_back(solEvaluations);
		}
		return paretoDoubleEval;
	}

	vector<vector<double> > createParetoSet(vector<vector<double> > D2)
	{
		vector<vector<double> > ref;

		for (int ind = 0; ind < D2.size(); ind++)
			p.addSolution(*pDom, *pDomWeak, ref, D2[ind]);

		return ref;
	}

	int cardinalite(vector<vector<double> > D, vector<vector<double> > ref)
	{
		int card = 0;
		for (int i = 0; i < D.size(); i++)
		{
			for (int j = 0; j < ref.size(); j++)
			{

				if (pDomWeak->dominates(D[i], ref[j]))
				{
					card++;
					j = ref.size();
				}
			}
		}

		return card;
	}

	double setCoverage(vector<vector<double> > a, vector<vector<double> > b)
	{
		double cover = 0;

		for (int i = 0; i < b.size(); i++)
		{
			for (int j = 0; j < a.size(); j++)
			{
				bool wD = pDomWeak->dominates(a[j], b[i]);
				if (wD)
				{
					j = a.size();
					cover += 1;
					//cout<<" i = "<<i<<" j = "<<j<<endl;
					//getchar();
				}
			}
		}

		//cout<<" cover = "<<cover<<endl;

		double sCover = cover / b.size();

		return sCover;
	}

	//Delta Metric and Hipervolume are working requires Minimization problems
	double deltaMetric(vector<vector<double> > pareto, vector<double> utopicEval, bool minimization)
	{
		int nSol = pareto.size();
		int nObj = utopicEval.size();

		if (minimization == false)
		{
			for (int i = 0; i < nSol; i++)
				for (int ev = 0; ev < nObj; ev++)
					pareto[i][ev] *= -1;
		}

		if (pareto.size() == 1)
			return 1;

		vector<double> vDist;
		vector<double> minEval(nObj, 1000000);
		double dMean = 0;
		for (int nS = 0; nS < nSol; nS++)
		{
			//vector with the evaluation of solution nS
			vector<double> eval = pareto[nS];
			for (int e = 0; e < nObj; e++)
			{
				if (eval[e] < minEval[e])
					minEval[e] = eval[e];
			}

			double minDist = 10000000;
			for (int nS2 = 0; nS2 < pareto.size(); nS2++)
			{
				double dist = 0;
				if (nS2 != nS)
				{
					vector<double> eval2 = pareto[nS2];
					for (int e = 0; e < nObj; e++)
						dist += pow(eval[e] - eval2[e], 2);
					dist = sqrt(dist);
					if (dist < minDist)
						minDist = dist;
				}

			}

			vDist.push_back(minDist);
			dMean += minDist;

		}
		dMean /= nSol;

		double dUtopic = 0;
		for (int e = 0; e < nObj; e++)
		{
			dUtopic += pow(minEval[e] - utopicEval[e], 2);
			//cout<<minEval[e]<<endl;
			//cout<<utopicEval[e]<<endl;
		}

		dUtopic = sqrt(dUtopic);
		//cout<<dUtopic<<endl;
		//getchar();

		double sumDist = 0;
		for (int nS = 0; nS < nSol; nS++)
		{
			sumDist += vDist[nS] - dMean;
		}

		double delta = (dUtopic + sumDist) / (nSol * dMean + dUtopic);
		return delta;
		//cout << "delta = " << delta << endl;
		//getchar();

	}

	double spacing(vector<vector<double> > a)
	{
		double ss = 0;
		vector<double> distMin;
		int N = a.size();
		int m = a[0].size();
		for (int i = 0; i < a.size(); i++)
		{
			double di = 100000000;

			for (int j = 0; j < a.size(); j++)
			{
				if (i != j)
				{
					double diMin = 0;
					for (int eval = 0; eval < m; eval++)
						diMin += abs(a[i][eval] - a[j][eval]);

					if (diMin < di)
						di = diMin;
				}
			}
			distMin.push_back(di);
		}

		double dMean = 0;

		for (int i = 0; i < N; i++)
			dMean += distMin[i];

		dMean = dMean / N;

		for (int i = 0; i < N; i++)
			ss += pow(distMin[i] - dMean, 2);

		ss = ss / N;
		ss = sqrt(ss);

		return ss;
	}

	vector<double> spacingMultiplePF(vector<vector<vector<double> > > D)
	{
		vector<double> spacings;
		for (int frente = 0; frente < D.size(); frente++)
		{
			vector<vector<double> > a = D[frente];
			double ss = spacing(a);
			spacings.push_back(ss);
		}
		return spacings;
	}

	char* execCommand(const char* command)
	{

		FILE* fp;
		char* line = nullptr;
		// Following initialization is equivalent to char* result = ""; and just
		// initializes result to an empty string, only it works with
		// -Werror=write-strings and is so much less clear.
		char* result = (char*) calloc(1, 1);
		size_t len = 0;

		fflush(nullptr);
		fp = popen(command, "r");
		if (fp == nullptr)
		{
			printf("Cannot execute command:\n%s\n", command);
			return nullptr;
		}

		while (getline(&line, &len, fp) != -1)
		{
			// +1 below to allow room for null terminator.
			result = (char*) realloc(result, ::strlen(result) + strlen(line) + 1);
			// +1 below so we copy the final null terminator.
			strncpy(result + strlen(result), line, strlen(line) + 1);
			free(line);
			line = nullptr;
		}

		fflush(fp);
		if (pclose(fp) != 0)
		{
			perror("Cannot close stream.\n");
		}

		return result;
	}

	//Delta Metric and Hipervolume are working requires Minimization problems
	double hipervolumeWithExecRequested(vector<vector<double> > pareto, vector<double> refPoints, bool minimization)
	{
		int nSol = pareto.size();
		int nObj = pareto[0].size();

		if (minimization == false)
		{
			for (int i = 0; i < nSol; i++)
				for (int ev = 0; ev < nObj; ev++)
					pareto[i][ev] *= -1;
		}

		string tempFile = "tempFileHipervolueFunc";
		FILE* fTempHV = fopen(tempFile.c_str(), "w");

		for (int s = 0; s < nSol; s++)
		{
			for (int o = 0; o < nObj; o++)
			{
				fprintf(fTempHV, "%.7f\t", pareto[s][o]);
			}
			fprintf(fTempHV, "\n");
		}

		fclose(fTempHV);
		stringstream ss;
		ss << "./hv\t -r \"";
		for (int o = 0; o < nObj; o++)
			ss << refPoints[o] << " ";
		ss << "\" \t" << tempFile.c_str();
		string hvValueString = execCommand(ss.str().c_str());
		double hvValue = atof(hvValueString.c_str());
		return hvValue;
	}

	double hipervolumeWithExecRequested(string filename, vector<double> refPoints)
	{
		stringstream ss;
		ss << "./hv\t -r \"";
		for (int o = 0; o < refPoints.size(); o++)
			ss << refPoints[o] << " ";
		ss << "\" \t" << filename.c_str();
		string hvValueString = execCommand(ss.str().c_str());
		double hvValue = atof(hvValueString.c_str());
		return hvValue;
	}
};


}

#endif /* OPTFRAME_MOMETRICS_HPP_ */
