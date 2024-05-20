// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef HFMVRP_CONSTRUCTIVE_Savings_HPP_
#define HFMVRP_CONSTRUCTIVE_Savings_HPP_

#include <stdlib.h>

// c++
#include <algorithm>
#include <list>
#include <utility>
//
// #include "../../OptFrame/Util/TestSolution.hpp"

#include <OptFrame/printable/printable.hpp>
//
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Constructive.hpp>
#include <OptFrame/RandGen.hpp>
//

#include "Evaluator.hpp"
#include "ProblemInstance.hpp"
#include "Representation.h"
#include "Solution.h"

#define EXT1 4
#define EXT2 5

using namespace std;

static_assert(optframe::XOStreamable<pair<int, double>>);
static_assert(optframe::XOStreamable<vector<int>>);
static_assert(optframe::XOStreamable<vector<pair<int, double>>>);

namespace HFMVRP {

class ConstructiveSavings : public Constructive<SolutionHFMVRP> {
 private:
  ProblemInstance& pHFMVRP;
  RandGen& rg;

  static bool compara(pair<int, double> p1, pair<int, double> p2) {
    return p1.second > p2.second;
  }
  // Your private vars

  // heurística construtiva Savings de Clarke and Wright
  vector<vector<int>>* savings(vector<int>& LC, Matrix<double>* dist,
                               vector<int>* loads, double capVehicle) {
    /*
     tabela auxiliar, mantem os seguintes dados:
     # identificador da rota, custo, demanda atendida, # de clientes, extremo 1,
     extremo 2
     */
    vector<vector<double>>* t;  // tabela auxiliar por rota

    // lista de clientes que estão em cada uma das rotas
    vector<vector<int>> cs;  // clientes por rota

    // ls:  vetor(valor da economia, [i,j] ) -> valor da economia ao ligar i com
    // j
    vector<pair<double, pair<int, int>>> ls;

    // mapeamento que identifica em qual rota está cada cliente
    map<int, int> rotaCliente;

    vector<vector<int>>* retorno = new vector<vector<int>>();
    if (LC.size() == 0) {
      *retorno = cs;
      return retorno;
    }
    // criando a tabela auxiliar
    t = new vector<vector<double>>;
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
      aux.push_back(1);         // número de clientes para essa rota
      aux.push_back(LC.at(c));  // primeiro extremo da rota
      aux.push_back(LC.at(c));  // segundo extremo da rota
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
    pair<double, pair<int, int>> laux;
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
  bool clienteExtremo(int c, vector<vector<double>>* t) {
    bool extremo = false;

    for (int i = 0; i < (int)t->size() && !extremo; i++) {
      extremo = (t->at(i).at(EXT1) == c) || (t->at(i).at(EXT2) == c);
    }
    return extremo;
  }

  // metodo que une duas rotas já atualizando todos os valores da tabela
  // auxiliar
  void mergeSavings(double saving, int rc1, int rc2, map<int, int>* rotaCliente,
                    vector<vector<double>>* t, vector<vector<int>>* cs,
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
  static bool comparacaoSavings(pair<double, pair<int, int>> a,
                                pair<double, pair<int, int>> b) {
    return (a.first < b.first);
  }

  vector<int>& CriaLC(vector<vector<int>>& rep, vector<bool> allocated,
                      int vType) {
    vector<int>* LC = new vector<int>;
    // int qAtualPP = 0; // capacidade atual do periodo, iniciado com 0
    // int qMaxPP = pHFMVRP.m * pHFMVRP.Q; //capacidade máxima que todas as
    // rotas juntas podem ter

    vector<int> clientes;

    // adicionando todos os clientes em uma lista de clientes
    for (int c = 1; c < pHFMVRP.nodes; c++) {
      if (pHFMVRP.getComp(vType, c) && allocated[c] == false)
        clientes.push_back(c);
    }

    *LC = clientes;

    return *LC;
  }

  void chooseRoutes(vector<vector<int>>& newRep,
                    vector<vector<int>>& parcialRep, vector<bool>& allocated,
                    int vType) {
    int nVehicles = pHFMVRP.getVehicleNumber(vType);
    int nRoutes = parcialRep.size();
    cout << "number of vehicles of type " << vType << " = " << nVehicles
         << endl;

    // Ordena as melhores rotas

    vector<double> routeCap(nRoutes);
    for (int r = 0; r < nRoutes; r++) {
      routeCap[r] = 0;
      vector<int> route = parcialRep[r];  // copia a rota
      for (int i = 0; i < (int)route.size(); i++)
        routeCap[r] += pHFMVRP.demands[route[i]];
    }

    vector<pair<int, double>> rank_Cap;
    for (int i = 0; i < (int)routeCap.size(); i++)
      rank_Cap.push_back(make_pair(i, routeCap[i]));

    sort(rank_Cap.begin(), rank_Cap.end(), compara);

    cout << "Melhores rotas" << endl;
    cout << rank_Cap << endl;
    // getchar();
    // Seleciona e transfere as melhores rotas para o vetor newRep
    for (int nv = 0; nv < min(nVehicles, nRoutes); nv++) {
      // number of vehicles of type VType
      int r = rank_Cap[nv].first;
      vector<int> route = parcialRep[r];
      int vID = pHFMVRP.getVehicleID(vType);

      for (int c = 0; c < (int)route.size(); c++) allocated[route[c]] = true;

      newRep[vID + nv] = route;
    }
  }

  void chooseRoutesMultiTrip(vector<vector<int>>& newRep,
                             vector<vector<int>>& parcialRep,
                             vector<bool>& allocated, int vType) {
    // Ordena as melhores rotas
    int nRoutes = parcialRep.size();

    vector<double> routeCap(nRoutes);
    for (int r = 0; r < nRoutes; r++) {
      routeCap[r] = 0;
      vector<int> route = parcialRep[r];  // copia a rota
      for (int i = 0; i < (int)route.size(); i++)
        routeCap[r] += pHFMVRP.demands[route[i]];
    }

    vector<pair<int, double>> rank_Cap;
    for (int i = 0; i < (int)routeCap.size(); i++)
      rank_Cap.push_back(make_pair(i, routeCap[i]));

    sort(rank_Cap.begin(), rank_Cap.end(), compara);

    cout << "Melhores rotas" << endl;
    cout << rank_Cap << endl;

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
  ConstructiveSavings(ProblemInstance& _pHFMVRP, RandGen& _rg)
      :  // If necessary, add more parameters
        pHFMVRP(_pHFMVRP),
        rg(_rg) {}

  virtual ~ConstructiveSavings() {}

  std::optional<SolutionHFMVRP> generateSolution(double timelimit) override {
    vector<vector<int>> newRep;
    bool feasibleSol = false;
    int nTentativas = 0;
    while (!feasibleSol) {
      cout << "Iteration = " << nTentativas << endl;
      vector<vector<int>> parcialRep;  // Keep the parcial solution
      vector<bool> allocated(
          pHFMVRP.nodes);  // mark the clients that has already been assigned

      for (int r = 0; r < pHFMVRP.nVehicles; r++)
        parcialRep.push_back(vector<int>());

      // gerando vetor de demandas dos clientes
      vector<int> demands;

      for (int c = 0; c < pHFMVRP.nodes; c++) {
        int demand = pHFMVRP.demands[c];
        if (demand == 0) allocated[c] = true;
        demands.push_back(demand);
      }

      for (int types = 0; types < pHFMVRP.typesVehicles; types++) {
        int r = pHFMVRP.typesVehicles - 1 - types;
        cout << "Generating solution for vehicle type = " << r << endl;
        vector<vector<int>>* vTypeRep;
        vTypeRep = savings(CriaLC(parcialRep, allocated, r), pHFMVRP.dist,
                           &demands, pHFMVRP.getVehicleTypeCap(r));
        cout << "Parcial Solution for this class of vehicles" << endl;
        cout << *vTypeRep << endl;

        chooseRoutes(parcialRep, *vTypeRep, allocated, r);

        cout << "REP" << endl;
        cout << parcialRep << endl;
        cout << "Allocated" << endl;
        cout << allocated << endl << endl;
        // getchar();
      }

      cout << "FINAL SOLUTION" << endl;
      cout << parcialRep << endl;

      int nonAllocated = 0;
      for (int i = 0; i < (int)allocated.size(); i++)
        if (allocated[i] == false) {
          nonAllocated++;
          cout << "cliente " << i << " demand = " << pHFMVRP.demands[i] << endl;
        }
      cout << "number of clientes that are not allocated" << nonAllocated
           << endl;
      getchar();

      //=============================================
      //============Multi-Trip Allocation============
      //=============================================
      if (nonAllocated != 0) {
        int r =
            2;  // Vehicle type 2 is allowed to do multi-trips (ONLY THIS ONE)
        cout << "Generating multi-trip solution for vehicle type = " << r
             << endl;
        vector<vector<int>>* vMultiTripRep;
        vMultiTripRep = savings(CriaLC(parcialRep, allocated, r), pHFMVRP.dist,
                                &demands, pHFMVRP.getVehicleTypeCap(r));
        cout << "Parcial MultiTrip Solution for this class of vehicles" << endl;
        cout << *vMultiTripRep << endl;

        chooseRoutesMultiTrip(parcialRep, *vMultiTripRep, allocated, r);
      }
      nonAllocated = 0;
      for (int i = 0; i < (int)allocated.size(); i++)
        if (allocated[i] == false) {
          cout << "BUGOUUUUUUUUUUUUU(EXIT SOLUTION)" << endl;
          nonAllocated++;
          cout << "cliente " << i << " demand = " << pHFMVRP.demands[i] << endl;
        }
      cout << "number of clientes that STILL NOT allocated" << nonAllocated
           << endl;
      if (nonAllocated == 0) {
        feasibleSol = true;
        newRep = parcialRep;
      }
      nTentativas++;
      //*newRep = parcialRep;
    }
    cout << "teste" << endl;
    getchar();
    return make_optional(SolutionHFMVRP(newRep));
  }
};

}  // namespace HFMVRP

#endif /*HFMVRP_CONSTRUCTIVE_Savings_HPP_*/
