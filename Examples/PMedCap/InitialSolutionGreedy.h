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

#ifndef PCAP_INITIALSOLUTION_InitalSolutionGreedy_HPP_
#define PCAP_INITIALSOLUTION_InitalSolutionGreedy_HPP_

#include "../../OptFrame/Constructive.h"
#include "../../OptFrame/Util/TestSolution.hpp"
#include "../../OptFrame/RandGen.hpp"

#include "ProblemInstance.h"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.h"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;
using namespace optframe;

class PCAPInitialSolutionGreedy: public Constructive<RepPCAP>
{
private:
	PCAPProblemInstance& pPCAP;
	RandGen& rg;
	// Your private vars

public:

	static bool compara(pair<int, double> p1, pair<int, double> p2)
	{
		return p1.second < p2.second;
	}

	PCAPInitialSolutionGreedy(PCAPProblemInstance& _pPCAP, RandGen& _rg) : // If necessary, add more parameters
		pPCAP(_pPCAP), rg(_rg)
	{
	}


	SolutionPCAP& generateSolution()
	{
		RepPCAP newRep;

		//cout << "pPCAP.nCidades = " << pPCAP.nCidades << endl;
		vector<bool> medianasUtilizadas;
		for (int i = 0; i < pPCAP.nCidades; i++)
			medianasUtilizadas.push_back(false);

		for (int i = 0; i < pPCAP.nMedianas; i++)
		{
			int med = rg.rand(pPCAP.nCidades);
			while (medianasUtilizadas[med] == true)
				med = rg.rand(pPCAP.nCidades);
			newRep.first.push_back(med);
			medianasUtilizadas[med] = true;
		}

		vector<int> utilizacao;
		for (int i = 0; i < pPCAP.nMedianas; i++)
			utilizacao.push_back(0);

		for (int i = 0; i < pPCAP.nCidades; i++)
			newRep.second.push_back(-1);

		vector<bool> vertices;
		for (int i = 0; i < pPCAP.nCidades; i++)
			vertices.push_back(false);

		for (int i = 0; i < pPCAP.nMedianas; i++)
		{
			int cidade = newRep.first[i];
			newRep.second[cidade] = i;
			utilizacao[i] += pPCAP.vecCidades[cidade].demanda;
			vertices[cidade] = true;

			//cout<<"i = "<<i<<"  cidade "<<cidade<<" newRep.second[cidade] = "<<newRep.second[cidade]<<endl;
		}

		vector<bool> verticesAlocados;
		for (int i = 0; i < pPCAP.nCidades; i++)
			verticesAlocados.push_back(false);
		for (int k = 0; k < pPCAP.nCidades; k++)
		{
			int i = rg.rand(pPCAP.nCidades);
			while (verticesAlocados[i] == true)
				i = rg.rand(pPCAP.nCidades);

			verticesAlocados[i] = true;

			double x = pPCAP.vecCidades[i].x;
			double y = pPCAP.vecCidades[i].y;
			int lowerCost = -1;

			if (vertices[i] == false)
			{
				double distMin = 100000000;

				for (int j = 0; j < pPCAP.nMedianas; j++)
				{
					int med = newRep.first[j];
					double x2 = pPCAP.vecCidades[med].x;
					double y2 = pPCAP.vecCidades[med].y;
					double dist = sqrt(pow((x - x2), 2) + pow((y - y2), 2));

					if ((dist < distMin) && ((utilizacao[j] + pPCAP.vecCidades[i].demanda) < pPCAP.vecCidades[med].capacidade))
					{
						distMin = dist;
						lowerCost = j;
					}

				}

				if (lowerCost != -1)
				{
					newRep.second[i] = lowerCost;
					utilizacao[lowerCost] += pPCAP.vecCidades[i].demanda;
				} else
				{
					newRep.second[i] = 0;
					utilizacao[0] += pPCAP.vecCidades[i].demanda;
				}

			}
		}

		int ads;

		return *new TestSolution<RepPCAP> (newRep, ads);
	}

};

#endif /*PCAP_INITIALSOLUTION_InitalSolutionGreedy_HPP_*/
