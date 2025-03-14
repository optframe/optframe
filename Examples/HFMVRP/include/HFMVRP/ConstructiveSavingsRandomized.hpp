// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef HFMVRP_CONSTRUCTIVE_SavingsRandomized__HPP_
#define HFMVRP_CONSTRUCTIVE_SavingsRandomized_HPP_

// C
#include <stdlib.h>

// C++
#include <algorithm>
#include <list>
//
// #include "../../OptFrame/Util/TestSolution.hpp"
#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Heuristics/GRASP/GRConstructive.hpp>
//
#include "Evaluator.hpp"
#include "ProblemInstance.hpp"
#include "Representation.h"
#include "Solution.h"

#define EXT1 4
#define EXT2 5

using namespace std;

namespace HFMVRP {

class ConstructiveSavingsRandomized : public GRConstructive<SolutionHFMVRP> {
 private:
  ProblemInstance& pHFMVRP;
  RandGen& rg;
  vector<int> sumComp;
  HFMVRPADSManager& adsMan;

  // Your private vars

  // heurística construtiva Savings de Clarke and Wright
  vector<vector<int> >* savings(std::vector<int>& LC, Matrix<double>* dist,
                                vector<int>* loads, double capVehicle) {
    /*
     tabela auxiliar, mantem os seguintes dados:
     # identificador da rota, custo, demanda atendida, # de clientes, extremo 1,
     extremo 2
     */
    vector<vector<double> >* t;  // tabela auxiliar por rota

    // lista de clientes que estão em cada uma das rotas
    vector<vector<int> > cs;  // clientes por rota

    // ls:  vetor(valor da economia, [i,j] ) -> valor da economia ao ligar i com
    // j
    std::vector<std::pair<double, pair<int, int> > > ls;

    // mapeamento que identifica em qual rota está cada cliente
    map<int, int> rotaCliente;

    vector<vector<int> >* retorno = new vector<vector<int> >();
    if (LC.size() == 0) {
      *retorno = cs;
      return retorno;
    }
    // criando a tabela auxiliar
    t = new vector<vector<double> >;
    // zerando o vetor de clientes
    cs.clear();
    // zerando as rotas associadas a cada cliente
    rotaCliente.clear();
    // zerando a lista de savings
    ls.clear();
    // vetor auxiliar para a criação da tabela
    vector<double> aux;
    // lista das rotas que não serão mais utilizadas
    vector<int> deleteList;

    // completando os dados da tabela auxiliar para cada uns dos clientes
    for (int c = 0; c < (int)LC.size(); c++) {
      aux.clear();
      aux.push_back(c);  // identificador da rota
      aux.push_back(2 *
                    (*dist)(0, LC.at(c)));  // custo de ir do depósito ao
                                            // cliente e do cliente ao depósito
      aux.push_back(loads->at(LC.at(c)));   // demanda do cliente
      aux.push_back(1);                     // número de clientes para essa rota
      aux.push_back(LC.at(c));              // primeiro extremo da rota
      aux.push_back(LC.at(c));              // segundo extremo da rota
      // adicionando os valores à tabela
      t->push_back(aux);
      // adicionando esse cliente a lista de clientes por rota
      vector<int> aux2;
      aux2.push_back(LC.at(c));
      cs.push_back(aux2);
      // adicionando em qual rota está cada cliente
      rotaCliente[LC.at(c)] = c;
    }
    // calculando a lista de savings
    double saving;
    pair<double, pair<int, int> > laux;
    pair<int, int> valor_aux;

    // caminhando por todas as rotas presentes na tabela auxiliar de rotas
    for (int r1 = 0; r1 < (int)t->size() - 1; r1++) {
      for (int r2 = r1 + 1; r2 < (int)t->size(); ++r2) {
        // calculo da economia
        saving = (*dist)(t->at(r1).at(EXT1), 0) +
                 (*dist)(0, t->at(r2).at(EXT1)) +
                 (*dist)(t->at(r1).at(EXT1), t->at(r2).at(EXT1));
        // adicionando esse valor a lista de savings
        valor_aux.first = t->at(r1).at(EXT1);
        valor_aux.second = t->at(r2).at(EXT1);
        laux.first = saving;
        laux.second = valor_aux;
        ls.push_back(laux);
      }
    }

    // ordenando a lista de savings
    sort(ls.begin(), ls.end(), comparacaoSavings);

    // fazendo os merges pela lista de savings
    // enquanto existir elementos na lista e o valor da economia for diferente
    // de 0
    int c1, c2, rc1, rc2, extc1, extc2;
    while ((!ls.empty()) && ls.at(0).first >= 0) {
      // retirando a economia
      saving = ls.at(0).first;
      // retirando os clientes
      c1 = ls.at(0).second.first;
      c2 = ls.at(0).second.second;
      // rotas desses clientes
      rc1 = rotaCliente[c1];
      rc2 = rotaCliente[c2];

      // verificando se respeita a restrição de carga
      if (t->at(rc1).at(2) + t->at(rc2).at(2) <= capVehicle) {
        // verificando se os dois clientes são extremos
        if (clienteExtremo(c1, t) && clienteExtremo(c2, t)) {
          // verificando se não estão na mesma rota
          if (rc1 != rc2) {
            // quatro casos de merge dependendo de qual extremo está cada
            // cliente extremo do cliente 1 - recebe o valor 1 se estiver no
            // primeiro extremo, e 2 caso contrário
            extc1 = t->at(rc1).at(EXT1) == c1 ? 1 : 2;
            // extremo do cliente 2 - recebe o valor 1 se estiver no primeiro
            // extremo, e 2 caso contrário
            extc2 = t->at(rc2).at(EXT1) == c2 ? 1 : 2;

            // primeiros dois casos - clientes como extremos diferentes
            if (extc1 != extc2) {
              if (extc1 ==
                  2)  // primeiro caso, o cliente 1 está no segundo extremo
              {
                // realizando o merge
                mergeSavings(saving, rc1, rc2, &rotaCliente, t, &cs,
                             &deleteList);

              } else  // segundo caso, o cliente 1 está no primeiro extremo,
                      // logo o cliente 2 está no segundo
              {
                // realizando o merge
                mergeSavings(saving, rc2, rc1, &rotaCliente, t, &cs,
                             &deleteList);
              }
            } else  // outros dois casos, clientes com mesmos extremos
            {
              if (extc1 ==
                  1)  // primeiro caso, caso ambos sejam o primeiro extremo
              {
                // realizando o merge
                // invertendo a rota do cliente 1, logo o extremo tb deve
                // invertido
                reverse(cs.at(rc1).begin(), cs.at(rc1).end());
                int aux = t->at(rc1).at(EXT1);
                t->at(rc1).at(EXT1) = t->at(rc1).at(EXT2);
                t->at(rc1).at(EXT2) = aux;
                mergeSavings(saving, rc1, rc2, &rotaCliente, t, &cs,
                             &deleteList);
              } else  // segundo caso, ambos no segundo extremo
              {
                // realizando o merge
                //  invertendo a rota do cliente 2, logo o extremo tb deve ser
                //  invertido
                reverse(cs.at(rc2).begin(), cs.at(rc2).end());
                int aux = t->at(rc2).at(EXT1);
                t->at(rc2).at(EXT1) = t->at(rc2).at(EXT2);
                t->at(rc2).at(EXT2) = aux;
                mergeSavings(saving, rc1, rc2, &rotaCliente, t, &cs,
                             &deleteList);
              }
            }
          }
        }
      }

      // removendo o saving já processado, que é sempre o primeiro da lista
      ls.erase(ls.begin());
    }
    // removendo as rotas que não serão mais utilizadas
    sort(deleteList.begin(), deleteList.end());

    for (int i = 0; i < (int)deleteList.size(); i++) {
      cs.erase(cs.begin() + deleteList.at(i) - i);
    }

    // adicionando as rotas geradas a representacao da solucao
    *retorno = cs;

    delete t;
    return retorno;
  }

  // retorno se o cliente é um dos extremos no algoritmo Savings
  bool clienteExtremo(int c, vector<vector<double> >* t) {
    bool extremo = false;

    for (int i = 0; i < (int)t->size() && !extremo; i++) {
      extremo = (t->at(i).at(EXT1) == c) || (t->at(i).at(EXT2) == c);
    }
    return extremo;
  }

  // metodo que une duas rotas já atualizando todos os valores da tabela
  // auxiliar
  void mergeSavings(double saving, int rc1, int rc2, map<int, int>* rotaCliente,
                    vector<vector<double> >* t, vector<vector<int> >* cs,
                    vector<int>* deleteList) {
    cs->at(rc1).insert(cs->at(rc1).end(), cs->at(rc2).begin(),
                       cs->at(rc2).end());
    // atualizando a tabela auxiliar
    // custo
    t->at(rc1).at(1) = t->at(rc1).at(1) + t->at(rc2).at(1) - saving;
    // demanda atendida
    t->at(rc1).at(2) += t->at(rc2).at(2);
    // número de clientes
    t->at(rc1).at(3) += t->at(rc2).at(2);
    // segundo extremo da rota do cliente 1, que deixa de ser esse segundo
    // extremo
    t->at(rc1).at(EXT2) = t->at(rc2).at(EXT2);

    // alterando em qual rota está cada cliente
    for (int i = 0; i < (int)cs->at(rc2).size(); i++) {
      rotaCliente->at(cs->at(rc2).at(i)) = rc1;
    }
    // removendo a rota que foi unida
    deleteList->push_back(rc2);
  }

  // método para realizar a ordenação dos dados da lista de Savings
  static bool comparacaoSavings(pair<double, pair<int, int> > a,
                                pair<double, pair<int, int> > b) {
    return (a.first < b.first);
  }

  vector<int>& CriaLC(std::vector<vector<int> >& rep, vector<bool> allocated,
                      int vType) {
    vector<int>* LC = new vector<int>;
    // int qAtualPP = 0; // capacidade atual do periodo, iniciado com 0
    // int qMaxPP = pHFMVRP.m * pHFMVRP.Q; //capacidade máxima que todas as
    // rotas juntas podem ter

    vector<int> clientes;
    int vehicleCap = pHFMVRP.getVehicleTypeCap(vType);

    // adicionando todos os clientes em uma lista de clientes
    for (int c = 1; c < pHFMVRP.nodes; c++)
      if ((pHFMVRP.getComp(vType, c)) && (allocated[c] == false) &&
          (pHFMVRP.demands[c] <= vehicleCap))
        clientes.push_back(c);

    *LC = clientes;

    return *LC;
  }

  void chooseRoutes(std::vector<vector<int> >& newRep,
                    vector<vector<int> >& parcialRep, vector<bool>& allocated,
                    int vType, double alpha) {
    int nVehicles = pHFMVRP.getVehicleNumber(vType);
    int nRoutes = parcialRep.size();
    // std::cout << "number of vehicles of type " << vType << " = " << nVehicles <<
    // std::endl;

    // Ordena as melhores rotas

    vector<double> routeGreedyCAP(nRoutes);
    for (int r = 0; r < nRoutes; r++) {
      routeGreedyCAP[r] = 0;
      vector<int> route = parcialRep[r];  // copia a rota
      for (int i = 0; i < (int)route.size(); i++) {
        routeGreedyCAP[r] += pHFMVRP.demands[route[i]];

        // Force the allocation of the clients that can note be attended by many
        // trucks
        if (sumComp[route[i]] <= 2) routeGreedyCAP[r] += 1000;
      }
    }

    std::vector<std::pair<int, double> > rankGreedyCAP;
    for (int i = 0; i < (int)routeGreedyCAP.size(); i++)
      rankGreedyCAP.push_back(std::make_pair(i, routeGreedyCAP[i]));

    sort(rankGreedyCAP.begin(), rankGreedyCAP.end(), compare);

    // std::cout << "Melhores rotas" << std::endl;
    // std::cout << rankGreedyCAP << std::endl;
    // getchar();

    // Seleciona e transfere as melhores rotas para o vetor newRep
    for (int nv = 0; nv < min(nVehicles, nRoutes);
         nv++)  // number of vehicles of type VType
    {
      int sizeLC = ceil(alpha * rankGreedyCAP.size());
      int greedyRand = rg.rand(sizeLC);
      int r = rankGreedyCAP[greedyRand].first;
      vector<int> route = parcialRep[r];

      rankGreedyCAP.erase(rankGreedyCAP.begin() + greedyRand);

      int vID = pHFMVRP.getVehicleID(vType);

      newRep[vID + nv] = route;

      // Mark all the allocated stores
      for (int c = 0; c < (int)route.size(); c++) allocated[route[c]] = true;
    }
  }

  void chooseRoutesMultiTrip(std::vector<vector<int> >& newRep,
                             vector<vector<int> >& parcialRep,
                             vector<bool>& allocated, int vType) {
    // Ordena as melhores rotas
    int nRoutes = parcialRep.size();
    // std::cout << "Number of Multi-Trips Routes = " << nRoutes << std::endl;
    // getchar();
    // Seleciona e transfere as melhores rotas para o vetor newRep
    for (int nv = 0; nv < nRoutes; nv++) {
      // number of vehicles of type VType
      vector<int> route = parcialRep[nv];

      for (int c = 0; c < (int)route.size(); c++) allocated[route[c]] = true;

      newRep.push_back(route);
    }
  }

 public:
  ConstructiveSavingsRandomized(ProblemInstance& _pHFMVRP, RandGen& _rg,
                                HFMVRPADSManager& _adsMan)
      :  // If necessary, add more parameters
        pHFMVRP(_pHFMVRP),
        rg(_rg),
        adsMan(_adsMan) {
    vector<int> _sumComp(pHFMVRP.nodes, 0);
    sumComp = _sumComp;

    for (int s = 0; s < pHFMVRP.nodes; s++)
      for (int v = 0; v < pHFMVRP.typesVehicles; v++)
        if (pHFMVRP.getComp(v, s)) sumComp[s]++;

    /*
     std::cout << sumComp << std::endl;
     for (int s = 0; s < pHFMVRP.nodes; s++)
     if (sumComp[s] == 2)
     std::cout << s << std::endl;
     */
  }

  virtual ~ConstructiveSavingsRandomized() {}

  std::optional<SolutionHFMVRP> generateSolution(double timelimit) override {
    return generateGRSolution(0, timelimit);
  }

  // GRConstructive
  std::optional<SolutionHFMVRP> generateGRSolution(float alpha,
                                                   double timelimit) override {
    if (alpha == 0) alpha = 0.00001;

    std::cout << "ConstructiveSavingsRandomized..." << std::endl;
    vector<vector<int> > newRep;

    // TODO resize for multi-trip case
    AdsHFMVRP newAds;

    bool feasibleSol = false;
    int nTentativas = 0;

    while (!feasibleSol) {
      std::cout << "Iteration = " << nTentativas << std::endl;
      // getchar();
      vector<vector<int> > parcialRep;  // Keep the parcial solution
      vector<bool> allocated(
          pHFMVRP.nodes);  // mark the clients that has already been assigned

      for (int r = 0; r < pHFMVRP.nVehicles; r++)
        parcialRep.push_back(std::vector<int>());

      // gerando vetor de demandas dos clientes
      vector<int> demands;

      for (int c = 0; c < pHFMVRP.nodes; c++) {
        int demand = pHFMVRP.demands[c];
        if (demand == 0) allocated[c] = true;
        demands.push_back(demand);
      }

      int nTypes = pHFMVRP.typesVehicles;
      vector<bool> typesAllocated(nTypes);

      for (int types = 0; types < nTypes; types++) {
        int v = rg.rand(nTypes);
        while (typesAllocated[v] == true) v = rg.rand(nTypes);
        typesAllocated[v] = true;
        v = types;

        int vehiclesCap = pHFMVRP.getVehicleTypeCap(v);
        // std::cout << "Generating solution for vehicle type = " << v << "\t
        // vehiclesCap = " << vehiclesCap << std::endl;
        vector<vector<int> >* vTypeRep;
        vTypeRep = savings(CriaLC(parcialRep, allocated, v), pHFMVRP.dist,
                           &demands, vehiclesCap);
        // std::cout << "Parcial Solution for class of vehicles v = " << v << std::endl;
        // std::cout << *vTypeRep << std::endl;
        // getchar();
        chooseRoutes(parcialRep, *vTypeRep, allocated, v, alpha);

        // std::cout << "REP" << std::endl;
        // std::cout << parcialRep << std::endl;
        // std::cout << "Allocated" << std::endl;
        // std::cout << allocated << std::endl << std::endl;
        // getchar();
      }

      // std::cout << "PARCIAL SOLUTION:" << std::endl;
      // std::cout << parcialRep << std::endl;

      // =============================================
      // ============Multi-Trip Allocation============
      // =============================================

      // CHECKING STORES THAT ARE GOING TO BE INCLUDE IN MULTI-TRIP VEHICLES
      int nonAllocated = 0;
      for (int i = 0; i < (int)allocated.size(); i++)
        if (allocated[i] == false) {
          nonAllocated++;
          // std::cout << "cliente " << i << " demand = " << pHFMVRP.demands[i] <<
          // std::endl;
        }
      std::cout << "Stores not allocated: " << nonAllocated << std::endl;
      // getchar();

      if (nonAllocated != 0) {
        // std::cout << "Multi-Trip Allocation" << std::endl;
        int r =
            2;  // Vehicle type 2 is allowed to do multi-trips (ONLY THIS ONE)
        // std::cout << "Generating multi-trip solution for vehicle type = " << r <<
        // std::endl;
        vector<vector<int> >* vMultiTripRep;
        vMultiTripRep = savings(CriaLC(parcialRep, allocated, r), pHFMVRP.dist,
                                &demands, pHFMVRP.getVehicleTypeCap(r));
        // std::cout << "Parcial MultiTrip Solution for this class of vehicles" <<
        // std::endl; std::cout << *vMultiTripRep << std::endl;

        chooseRoutesMultiTrip(parcialRep, *vMultiTripRep, allocated, r);
      }

      // CHECKING STORES TO BE ALLOCATED IN DUMMY VEHICLES - ``ERROR'' TODO
      nonAllocated = 0;
      for (int i = 0; i < (int)allocated.size(); i++)
        if (allocated[i] == false) {
          nonAllocated++;
          std::cout << "BUG (EXIT-SOL) - Cliente " << i
               << "\tID = " << pHFMVRP.getStoreID(i)
               << "\tdemand = " << pHFMVRP.demands[i] << std::endl;
          std::cout << "pHFMVRP.getDist(" << i << ",0) = " << pHFMVRP.getDist(i, 0)
               << std::endl;
        }
      std::cout << "Stores not allocated after Multi trips: " << nonAllocated << std::endl
           << std::endl;

      if (nonAllocated == 0) {
        feasibleSol = true;
        newRep.clear();
        newRep = parcialRep;
      } else
        nTentativas++;
    }

    // add depot in the beginning and end of the routes
    for (int r = 0; r < (int)newRep.size(); r++) {
      newRep[r].push_back(0);
      newRep[r].insert(newRep[r].begin(), 0);
    }

    adsMan.initializeADS(newRep, newAds);
    adsMan.initializeADSNeighStructure(
        newRep,
        newAds);  // general initialization according to your neighborhoods

    /*
     std::cout << "newAds.cumulativeDemand" << newAds.cumulativeDemand << std::endl;
     std::cout << "newAds.maxDemand" << newAds.maxDemand << std::endl;
     std::cout << "newAds.minDemand" << newAds.minDemand << std::endl;
     std::cout << "newAds.maxPairDemand" << newAds.maxPairDemand << std::endl;
     std::cout << "newAds.minPairDemand" << newAds.minPairDemand << std::endl;
     std::cout << "newAds.sumDemand" << newAds.sumDemand << std::endl;
     getchar();

     std::cout << "FINAL SOLUTION" << std::endl;
     std::cout << newRep << std::endl;
     std::cout << "newAds.cumulativeDemand" << newAds.cumulativeDemand << std::endl;
     std::cout << "newAds.maxDemand" << newAds.maxDemand << std::endl;
     std::cout << "newAds.minDemand" << newAds.minDemand << std::endl;
     std::cout << "newAds.maxPairDemand" << newAds.maxPairDemand << std::endl;
     std::cout << "newAds.minPairDemand" << newAds.minPairDemand << std::endl;
     std::cout << "newAds.sumDemand" << newAds.sumDemand << std::endl;
     */
    return make_optional(SolutionHFMVRP(newRep, newAds));
  }
};

}  // namespace HFMVRP

#endif /*HFMVRP_CONSTRUCTIVE_SavingsRandomized_HPP_*/
