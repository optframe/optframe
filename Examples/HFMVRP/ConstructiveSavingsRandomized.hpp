#ifndef HFMVRP_CONSTRUCTIVE_SavingsRandomized__HPP_
#define HFMVRP_CONSTRUCTIVE_SavingsRandomized_HPP_

#include "../../OptFrame/Constructive.h"
#include "../../OptFrame/Heuristics/GRASP/GRConstructive.h"
#include "../../OptFrame/Util/TestSolution.hpp"
#include "../../OptFrame/RandGen.hpp"
#include "ProblemInstance.hpp"
#include "Representation.h"
#include "Solution.h"
#include "Evaluator.hpp"

#include <list>
#include <algorithm>
#include <stdlib.h>

#define EXT1 4
#define EXT2 5

using namespace std;

namespace HFMVRP
{

class ConstructiveSavingsRandomized: public GRConstructive<RepHFMVRP, AdsHFMVRP>
{
private:

	ProblemInstance& pHFMVRP;
	RandGen& rg;
	vector<int> sumComp;
	HFMVRPADSManager& adsMan;

	// Your private vars

	//heurística construtiva Savings de Clarke and Wright
	vector<vector<int> >* savings(vector<int> & LC, Matrix<double>* dist, vector<int>* loads, double capVehicle)
	{

		/*
		 tabela auxiliar, mantem os seguintes dados:
		 # identificador da rota, custo, demanda atendida, # de clientes, extremo 1, extremo 2
		 */
		vector<vector<double> >* t; //tabela auxiliar por rota

		//lista de clientes que estão em cada uma das rotas
		vector<vector<int> > cs; // clientes por rota

		//ls:  vetor(valor da economia, [i,j] ) -> valor da economia ao ligar i com j
		vector<pair<double, pair<int, int> > > ls;

		//mapeamento que identifica em qual rota está cada cliente
		map<int, int> rotaCliente;

		vector<vector<int> >* retorno = new vector<vector<int> >();
		if (LC.size() == 0)
		{
			*retorno = cs;
			return retorno;
		}
		//criando a tabela auxiliar
		t = new vector<vector<double> >;
		//zerando o vetor de clientes
		cs.clear();
		//zerando as rotas associadas a cada cliente
		rotaCliente.clear();
		//zerando a lista de savings
		ls.clear();
		//vetor auxiliar para a criação da tabela
		vector<double> aux;
		//lista das rotas que não serão mais utilizadas
		vector<int> deleteList;

		//completando os dados da tabela auxiliar para cada uns dos clientes
		for (int c = 0; c < LC.size(); c++)
		{
			aux.clear();
			aux.push_back(c); // identificador da rota
			aux.push_back(2 * (*dist)(0, LC.at(c))); //custo de ir do depósito ao cliente e do cliente ao depósito
			aux.push_back(loads->at(LC.at(c))); // demanda do cliente
			aux.push_back(1); // número de clientes para essa rota
			aux.push_back(LC.at(c)); // primeiro extremo da rota
			aux.push_back(LC.at(c)); // segundo extremo da rota
			//adicionando os valores à tabela
			t->push_back(aux);
			//adicionando esse cliente a lista de clientes por rota
			vector<int> aux2;
			aux2.push_back(LC.at(c));
			cs.push_back(aux2);
			//adicionando em qual rota está cada cliente
			rotaCliente[LC.at(c)] = c;
		}
		//calculando a lista de savings
		double saving;
		pair<double, pair<int, int> > laux;
		pair<int, int> valor_aux;

		//caminhando por todas as rotas presentes na tabela auxiliar de rotas
		for (int r1 = 0; r1 < t->size() - 1; r1++)
		{
			for (int r2 = r1 + 1; r2 < t->size(); ++r2)
			{
				//calculo da economia
				saving = (*dist)(t->at(r1).at(EXT1), 0) + (*dist)(0, t->at(r2).at(EXT1)) + (*dist)(t->at(r1).at(EXT1), t->at(r2).at(EXT1));
				//adicionando esse valor a lista de savings
				valor_aux.first = t->at(r1).at(EXT1);
				valor_aux.second = t->at(r2).at(EXT1);
				laux.first = saving;
				laux.second = valor_aux;
				ls.push_back(laux);
			}

		}

		//ordenando a lista de savings
		sort(ls.begin(), ls.end(), comparacaoSavings);

		//fazendo os merges pela lista de savings
		//enquanto existir elementos na lista e o valor da economia for diferente de 0
		int c1, c2, rc1, rc2, extc1, extc2;
		while ((!ls.empty()) && ls.at(0).first >= 0)
		{
			//retirando a economia
			saving = ls.at(0).first;
			//retirando os clientes
			c1 = ls.at(0).second.first;
			c2 = ls.at(0).second.second;
			//rotas desses clientes
			rc1 = rotaCliente[c1];
			rc2 = rotaCliente[c2];

			//verificando se respeita a restrição de carga
			if (t->at(rc1).at(2) + t->at(rc2).at(2) <= capVehicle)
			{
				//verificando se os dois clientes são extremos
				if (clienteExtremo(c1, t) && clienteExtremo(c2, t))
				{
					//verificando se não estão na mesma rota
					if (rc1 != rc2)
					{

						//quatro casos de merge dependendo de qual extremo está cada cliente
						//extremo do cliente 1 - recebe o valor 1 se estiver no primeiro extremo, e 2 caso contrário
						extc1 = t->at(rc1).at(EXT1) == c1 ? 1 : 2;
						//extremo do cliente 2 - recebe o valor 1 se estiver no primeiro extremo, e 2 caso contrário
						extc2 = t->at(rc2).at(EXT1) == c2 ? 1 : 2;

						//primeiros dois casos - clientes como extremos diferentes
						if (extc1 != extc2)
						{
							if (extc1 == 2) //primeiro caso, o cliente 1 está no segundo extremo
							{
								//realizando o merge
								mergeSavings(saving, rc1, rc2, &rotaCliente, t, &cs, &deleteList);

							}
							else // segundo caso, o cliente 1 está no primeiro extremo, logo o cliente 2 está no segundo
							{
								//realizando o merge
								mergeSavings(saving, rc2, rc1, &rotaCliente, t, &cs, &deleteList);
							}
						}
						else //outros dois casos, clientes com mesmos extremos
						{
							if (extc1 == 1) //primeiro caso, caso ambos sejam o primeiro extremo
							{
								//realizando o merge
								//invertendo a rota do cliente 1, logo o extremo tb deve invertido
								reverse(cs.at(rc1).begin(), cs.at(rc1).end());
								int aux = t->at(rc1).at(EXT1);
								t->at(rc1).at(EXT1) = t->at(rc1).at(EXT2);
								t->at(rc1).at(EXT2) = aux;
								mergeSavings(saving, rc1, rc2, &rotaCliente, t, &cs, &deleteList);
							}
							else //segundo caso, ambos no segundo extremo
							{
								//realizando o merge
								// invertendo a rota do cliente 2, logo o extremo tb deve ser invertido
								reverse(cs.at(rc2).begin(), cs.at(rc2).end());
								int aux = t->at(rc2).at(EXT1);
								t->at(rc2).at(EXT1) = t->at(rc2).at(EXT2);
								t->at(rc2).at(EXT2) = aux;
								mergeSavings(saving, rc1, rc2, &rotaCliente, t, &cs, &deleteList);
							}

						}
					}

				}

			}

			//removendo o saving já processado, que é sempre o primeiro da lista
			ls.erase(ls.begin());
		}
		//removendo as rotas que não serão mais utilizadas
		sort(deleteList.begin(), deleteList.end());

		for (int i = 0; i < deleteList.size(); i++)
		{
			cs.erase(cs.begin() + deleteList.at(i) - i);
		}

		//adicionando as rotas geradas a representacao da solucao
		*retorno = cs;

		delete t;
		return retorno;
	}

	//retorno se o cliente é um dos extremos no algoritmo Savings
	bool clienteExtremo(int c, vector<vector<double> >* t)
	{
		bool extremo = false;

		for (int i = 0; i < t->size() && !extremo; i++)
		{
			extremo = (t->at(i).at(EXT1) == c) || (t->at(i).at(EXT2) == c);
		}
		return extremo;
	}

	//metodo que une duas rotas já atualizando todos os valores da tabela auxiliar
	void mergeSavings(double saving, int rc1, int rc2, map<int, int> * rotaCliente, vector<vector<double> >* t, vector<vector<int> >*cs, vector<int>* deleteList)
	{
		cs->at(rc1).insert(cs->at(rc1).end(), cs->at(rc2).begin(), cs->at(rc2).end());
		//atualizando a tabela auxiliar
		//custo
		t->at(rc1).at(1) = t->at(rc1).at(1) + t->at(rc2).at(1) - saving;
		// demanda atendida
		t->at(rc1).at(2) += t->at(rc2).at(2);
		// número de clientes
		t->at(rc1).at(3) += t->at(rc2).at(2);
		// segundo extremo da rota do cliente 1, que deixa de ser esse segundo extremo
		t->at(rc1).at(EXT2) = t->at(rc2).at(EXT2);

		//alterando em qual rota está cada cliente
		for (int i = 0; i < cs->at(rc2).size(); i++)
		{
			rotaCliente->at(cs->at(rc2).at(i)) = rc1;
		}
		//removendo a rota que foi unida
		deleteList->push_back(rc2);
	}

	//método para realizar a ordenação dos dados da lista de Savings
	static bool comparacaoSavings(pair<double, pair<int, int> > a, pair<double, pair<int, int> > b)
	{
		return (a.first < b.first);
	}

	vector<int>& CriaLC(vector<vector<int> >& rep, vector<bool> allocated, int vType)
	{
		vector<int>* LC = new vector<int>;
		//int qAtualPP = 0; // capacidade atual do periodo, iniciado com 0
		//int qMaxPP = pHFMVRP.m * pHFMVRP.Q; //capacidade máxima que todas as rotas juntas podem ter

		vector<int> clientes;
		int vehicleCap = pHFMVRP.getVehicleTypeCap(vType);

		//adicionando todos os clientes em uma lista de clientes
		for (int c = 1; c < pHFMVRP.nodes; c++)
			if ((pHFMVRP.getComp(vType, c)) && (allocated[c] == false) && (pHFMVRP.demands[c] <= vehicleCap))
				clientes.push_back(c);

		*LC = clientes;

		return *LC;

	}

	void chooseRoutes(vector<vector<int> >& newRep, vector<vector<int> >& parcialRep, vector<bool>& allocated, int vType, double alpha)
	{
		int nVehicles = pHFMVRP.getVehicleNumber(vType);
		int nRoutes = parcialRep.size();
		//cout << "number of vehicles of type " << vType << " = " << nVehicles << endl;

		//Ordena as melhores rotas

		vector<double> routeGreedyCAP(nRoutes);
		for (int r = 0; r < nRoutes; r++)
		{
			routeGreedyCAP[r] = 0;
			vector<int> route = parcialRep[r]; // copia a rota
			for (int i = 0; i < route.size(); i++)
			{
				routeGreedyCAP[r] += pHFMVRP.demands[route[i]];

				//Force the allocation of the clients that can note be attended by many trucks
				if (sumComp[route[i]] <= 2)
					routeGreedyCAP[r] += 1000;
			}
		}

		vector<pair<int, double> > rankGreedyCAP;
		for (int i = 0; i < routeGreedyCAP.size(); i++)
			rankGreedyCAP.push_back(make_pair(i, routeGreedyCAP[i]));

		sort(rankGreedyCAP.begin(), rankGreedyCAP.end(), compare);

		//cout << "Melhores rotas" << endl;
		//cout << rankGreedyCAP << endl;
		//getchar();

		//Seleciona e transfere as melhores rotas para o vetor newRep
		for (int nv = 0; nv < min(nVehicles, nRoutes); nv++) //number of vehicles of type VType
		{

			int sizeLC = ceil(alpha * rankGreedyCAP.size());
			int greedyRand = rg.rand(sizeLC);
			int r = rankGreedyCAP[greedyRand].first;
			vector<int> route = parcialRep[r];

			rankGreedyCAP.erase(rankGreedyCAP.begin() + greedyRand);

			int vID = pHFMVRP.getVehicleID(vType);

			newRep[vID + nv] = route;

			//Mark all the allocated stores
			for (int c = 0; c < route.size(); c++)
				allocated[route[c]] = true;

		}

	}

	void chooseRoutesMultiTrip(vector<vector<int> >& newRep, vector<vector<int> >& parcialRep, vector<bool>& allocated, int vType)
	{
		//Ordena as melhores rotas
		int nRoutes = parcialRep.size();
		//cout << "Number of Multi-Trips Routes = " << nRoutes << endl;
		//getchar();
		//Seleciona e transfere as melhores rotas para o vetor newRep
		for (int nv = 0; nv < nRoutes; nv++) //number of vehicles of type VType
		{
			vector<int> route = parcialRep[nv];

			for (int c = 0; c < route.size(); c++)
				allocated[route[c]] = true;

			newRep.push_back(route);
		}

	}

public:

	ConstructiveSavingsRandomized(ProblemInstance& _pHFMVRP, RandGen& _rg, HFMVRPADSManager& _adsMan) : // If necessary, add more parameters
			pHFMVRP(_pHFMVRP), rg(_rg), adsMan(_adsMan)
	{

		vector<int> _sumComp(pHFMVRP.nodes, 0);
		sumComp = _sumComp;

		for (int s = 0; s < pHFMVRP.nodes; s++)
			for (int v = 0; v < pHFMVRP.typesVehicles; v++)
				if (pHFMVRP.getComp(v, s))
					sumComp[s]++;

		/*
		 cout << sumComp << endl;
		 for (int s = 0; s < pHFMVRP.nodes; s++)
		 if (sumComp[s] == 2)
		 cout << s << endl;
		 */

	}

	virtual ~ConstructiveSavingsRandomized()
	{
	}

	Solution<RepHFMVRP, AdsHFMVRP>& generateSolution()
	{
		return generateSolution(0);
	}
	Solution<RepHFMVRP, AdsHFMVRP>& generateSolution(float alpha)
	{
		if (alpha == 0)
			alpha = 0.00001;

		cout << "ConstructiveSavingsRandomized..." << endl;
		vector<vector<int> > newRep;

		//TODO resize for multi-trip case
		AdsHFMVRP newAds;

		bool feasibleSol = false;
		int nTentativas = 0;

		while (!feasibleSol)
		{
			cout << "Iteration = " << nTentativas << endl;
			//getchar();
			vector<vector<int> > parcialRep; // Keep the parcial solution
			vector<bool> allocated(pHFMVRP.nodes); // mark the clients that has already been assigned

			for (int r = 0; r < pHFMVRP.nVehicles; r++)
				parcialRep.push_back(vector<int>());

			//gerando vetor de demandas dos clientes
			vector<int> demands;

			for (int c = 0; c < pHFMVRP.nodes; c++)
			{
				int demand = pHFMVRP.demands[c];
				if (demand == 0)
					allocated[c] = true;
				demands.push_back(demand);
			}

			int nTypes = pHFMVRP.typesVehicles;
			vector<bool> typesAllocated(nTypes);

			for (int types = 0; types < nTypes; types++)
			{

				int v = rg.rand(nTypes);
				while (typesAllocated[v] == true)
					v = rg.rand(nTypes);
				typesAllocated[v] = true;
				v = types;

				int vehiclesCap = pHFMVRP.getVehicleTypeCap(v);
				//cout << "Generating solution for vehicle type = " << v << "\t vehiclesCap = " << vehiclesCap << endl;
				vector<vector<int> >* vTypeRep;
				vTypeRep = savings(CriaLC(parcialRep, allocated, v), pHFMVRP.dist, &demands, vehiclesCap);
				//cout << "Parcial Solution for class of vehicles v = " << v << endl;
				//cout << *vTypeRep << endl;
				//getchar();
				chooseRoutes(parcialRep, *vTypeRep, allocated, v,alpha);

				//cout << "REP" << endl;
				//cout << parcialRep << endl;
				//cout << "Allocated" << endl;
				//cout << allocated << endl << endl;
				//getchar();
			}

			//cout << "PARCIAL SOLUTION:" << endl;
			//cout << parcialRep << endl;

			//=============================================
			//============Multi-Trip Allocation============
			//=============================================

			//CHECKING STORES THAT ARE GOING TO BE INCLUDE IN MULTI-TRIP VEHICLES
			int nonAllocated = 0;
			for (int i = 0; i < allocated.size(); i++)
				if (allocated[i] == false)
				{
					nonAllocated++;
					//cout << "cliente " << i << " demand = " << pHFMVRP.demands[i] << endl;
				}
			cout << "Stores not allocated: " << nonAllocated << endl;
			//getchar();

			if (nonAllocated != 0)
			{
				//cout << "Multi-Trip Allocation" << endl;
				int r = 2; //Vehicle type 2 is allowed to do multi-trips (ONLY THIS ONE)
				//cout << "Generating multi-trip solution for vehicle type = " << r << endl;
				vector<vector<int> >* vMultiTripRep;
				vMultiTripRep = savings(CriaLC(parcialRep, allocated, r), pHFMVRP.dist, &demands, pHFMVRP.getVehicleTypeCap(r));
				//cout << "Parcial MultiTrip Solution for this class of vehicles" << endl;
				//cout << *vMultiTripRep << endl;

				chooseRoutesMultiTrip(parcialRep, *vMultiTripRep, allocated, r);
			}

			//CHECKING STORES TO BE ALLOCATED IN DUMMY VEHICLES - ``ERROR'' TODO
			nonAllocated = 0;
			for (int i = 0; i < allocated.size(); i++)
				if (allocated[i] == false)
				{
					nonAllocated++;
					cout << "BUG (EXIT-SOL) - Cliente " << i << "\tID = " << pHFMVRP.getStoreID(i) << "\tdemand = " << pHFMVRP.demands[i] << endl;
					cout << "pHFMVRP.getDist(" << i << ",0) = " << pHFMVRP.getDist(i, 0) << endl;
				}
			cout << "Stores not allocated after Multi trips: " << nonAllocated << endl << endl;

			if (nonAllocated == 0)
			{
				feasibleSol = true;
				newRep.clear();
				newRep = parcialRep;
			}
			else
				nTentativas++;
		}

		//add depot in the beginning and end of the routes
		for (int r = 0; r < newRep.size(); r++)
		{
			newRep[r].push_back(0);
			newRep[r].insert(newRep[r].begin(), 0);

		}

		//cout << "Initialize ADS..." << endl;
		//getchar();
		adsMan.initializeADS(newRep, newAds);
		adsMan.initializeADSNeighStructure(newRep, newAds); //general initialization according to your neighborhoods
		//cout << "OK Initialize ADS...." << endl;
		//getchar();
		/*
		 cout << "newAds.cumulativeDemand" << newAds.cumulativeDemand << endl;
		 cout << "newAds.maxDemand" << newAds.maxDemand << endl;
		 cout << "newAds.minDemand" << newAds.minDemand << endl;
		 cout << "newAds.maxPairDemand" << newAds.maxPairDemand << endl;
		 cout << "newAds.minPairDemand" << newAds.minPairDemand << endl;
		 cout << "newAds.sumDemand" << newAds.sumDemand << endl;
		 getchar();

		 cout << "FINAL SOLUTION" << endl;
		 cout << newRep << endl;
		 cout << "newAds.cumulativeDemand" << newAds.cumulativeDemand << endl;
		 cout << "newAds.maxDemand" << newAds.maxDemand << endl;
		 cout << "newAds.minDemand" << newAds.minDemand << endl;
		 cout << "newAds.maxPairDemand" << newAds.maxPairDemand << endl;
		 cout << "newAds.minPairDemand" << newAds.minPairDemand << endl;
		 cout << "newAds.sumDemand" << newAds.sumDemand << endl;
		 */
		return *new TestSolution<RepHFMVRP, AdsHFMVRP>(newRep, newAds);
	}

};

}

#endif /*HFMVRP_CONSTRUCTIVE_SavingsRandomized_HPP_*/
