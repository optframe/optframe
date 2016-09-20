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

/*
 * MultObjectiveMetrics.hpp
 *
 * LGPLv3
 *
 */

#ifndef OPTFRAME_MOMETRICS_HPP_
#define OPTFRAME_MOMETRICS_HPP_

#include "../Solution.hpp"
#include "../Evaluation.hpp"
#include "../Evaluator.hpp"
#include "../Move.hpp"
#include "../Population.hpp"

#include <iostream>
#include <cmath>

using namespace std;

template<class R, class DS = OPTFRAME_DEFAULT_MEMORY>
class UnionNDSets
{
protected:
	vector<Evaluator<R, DS >*> v_e;

	bool addSolution(vector<vector<double> >& p, vector<double>& s)
	{
		bool added = true;
		for (int ind = 0; ind < p.size(); ind++)
		{

			if (weakDominates(p[ind], s))
				return false;

			if (dominates(s, p.at(ind)))
				p.erase(p.begin() + ind); //Esta perdendo o individuo retornado,tem problema? todo
		}

		if (added == true)
			p.push_back(s);

		return added;
	}

	// true if 's1' dominates 's2'
	virtual bool weakDominates(vector<double>& s1, const vector<double>& s2)
	{

		int better = 0;
		int equals = 0;

		for (int e = 0; e < s1.size(); e++)
		{
			if (v_e[e]->betterThan(s1[e], s2[e]))
				better++;

			if (abs(s1[e] - s2[e]) < 0.0001)
				equals++;
		}

		return ((better + equals == v_e.size()));
	}

	// true if 's1' weak dominates 's2'
	virtual bool dominates(vector<double>& s1, const vector<double>& s2)
	{

		int better = 0;
		int equals = 0;

		for (int e = 0; e < s1.size(); e++)
		{
			if (v_e[e]->betterThan(s1[e], s2[e]))
				better++;

			if (abs(s1[e] - s2[e]) < 0.0001)
				equals++;
		}

		return ((better + equals == v_e.size()) && (better > 0));
	}

public:

	UnionNDSets(vector<Evaluator<R, DS >*> _v_e) :
		v_e(_v_e)
	{

	}

	UnionNDSets()
	{

	}

	virtual ~UnionNDSets()
	{
	}

	vector<vector<double> > unionSets(string caminho, int nTests)
	{

		vector<vector<double> > D;

		for (int e = 0; e < nTests; e++)
		{
			stringstream ss;
			ss << caminho;


			Scanner scanner(new File(ss.str()));
			while (scanner.hasNext())
			{
				double eval1 = scanner.nextDouble();
				double eval2 = scanner.nextDouble();
				double eval3 = scanner.nextDouble();
				vector<double> ind;
				ind.push_back(eval1);
				ind.push_back(eval2);
				ind.push_back(eval3);

				addSolution(D, ind);
			}

		}

		return D;
	}

	vector<vector<double> > unionSets(vector<vector<double> > D1, vector<vector<double> > D2)
	{

		vector<vector<double> > ref = D1;

		for (int ind = 0; ind < D2.size(); ind++)
			addSolution(ref, D2[ind]);

		return ref;
	}

	int cardinalite(vector<vector<double> > D, vector<vector<double> > ref)
	{
		int card = 0;
		for (int i = 0; i < D.size(); i++)
		{
			for (int j = 0; j < ref.size(); j++)
			{
				int equal = 0;
				for (int eval = 0; eval < D[0].size(); eval++)
				{
					if (D[i][eval] == ref[j][eval])
						equal++;
				}
				if (equal == D[0].size())
					card++;
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
				bool wD = weakDominates(a[j], b[i]);
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

};

#endif /*OPTFRAME_MOMETRICS_HPP_*/
