#ifndef OPM_INITIALSOLUTION_Random_HPP_
#define OPM_INITIALSOLUTION_Random_HPP_

#include "../OptFrame/Constructive.h"
#include "../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.hpp"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.hpp"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

namespace POLAD
{
class RandomInitialSolutionOPM: public Constructive<RepOPM,OPTFRAME_DEFAULT_ADS>
{
private:
	OPMProblemInstance* polad;

	// Your private vars

public:

	RandomInitialSolutionOPM(OPMProblemInstance* opm) // If necessary, add more parameters
	{
		this->polad = opm;

		// Put the rest of your code here
	};

	SolutionOPM& generateSolution()
	{
		// Inicialmente sem nenhuma carregadeira alocada
		vector<int> frentes(polad->getNumFrentes(),-1);

		vector<bool> alocadas(polad->getNumCarregs(),false);

		int num_alocadas = 0;

		// Aloca todas as carregadeiras.
		// Funciona apenas se o numero de carregadeiras for inferior
		// ao numero total de frentes.

		while(num_alocadas < polad->getNumCarregs())
		{
			int f = rand()%polad->getNumFrentes();

			// Se frente ja tem carregadeira, tente denovo.
			if( frentes.at(f) > -1 )
				continue;

			int k = rand()%polad->getNumCarregs();

			// Se carregadeira ja foi utilizada, tente denovo.
			if( alocadas.at(k) )
				continue;

			// Frente e carregadeira disponiveis!
			frentes[f] = k;
			alocadas[k] = true;

			num_alocadas++;
		}

		// Constroi a matriz de viagens.

		Matrix<int> viagens (polad->getNumFrentes(), polad->getNumCaminhoes());

		// Preenche a matriz com zeros.
		viagens.fill(0);

	/*
		(*viagens)(0,5) = 6;
		(*viagens)(0,6) = 1;

		(*viagens)(15,3) = 4;
		(*viagens)(15,20) = 3;
	*/

		// Constroi a representacao da solucao.
		RepOPM* rep = new pair<vector<int> , Matrix<int> >(frentes, viagens);

		// Retorna a nova solucao.
		return new TestSolution<RepOPM>(rep);
	}

};
}
#endif /*OPM_INITIALSOLUTION_Random_HPP_*/
