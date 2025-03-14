#ifndef MODM_CONSTRUCTIVE_BasicGreedyRandomized_H_
#define MODM_CONSTRUCTIVE_BasicGreedyRandomized_H_

#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Heuristics/GRASP/GRConstructive.hpp>
// #include "../../OptFrame/Util/TestSolution.hpp"
#include <OptFrame/Core/RandGen.hpp>

#include "Evaluator.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

// #include <list>

#include <stdlib.h>

#include <algorithm>

using namespace std;

using namespace optframe;

namespace MODM {

static bool comparacao(pair<double, int> a, pair<double, int> b) {
  return (a.first > b.first);
}

static bool comparacaoClients(int a, int b) { return (a > b); }

class ConstructiveBasicGreedyRandomized : public GRConstructive<SolutionMODM> {
 private:
  ProblemInstance& pMODM;
  RandGen& rg;
  MODMADSManager& adsMan;
  // Your private vars

  // Your private vars

 public:
  ConstructiveBasicGreedyRandomized(ProblemInstance& _pMODM, RandGen& _rg,
                                    MODMADSManager& _adsMan)
      :  // If necessary, add more parameters
        pMODM(_pMODM),
        rg(_rg),
        adsMan(_adsMan) {
    Component::verbose = false;
  }

  virtual ~ConstructiveBasicGreedyRandomized() {}

  std::optional<SolutionMODM> generateSolution(double timelimit) override {
    float alpha = 0.1;

    return generateGRSolution(alpha, timelimit);
  }

  // virtual S* generateGRSolution(float alpha, double timelimit) = 0;
  std::optional<SolutionMODM> generateGRSolution(float alpha,
                                                 double timelimit) override {
    if (alpha == 0) alpha = 0.00001;

    int n = pMODM.getNumberOfProducts();
    int c = pMODM.getNumberOfClients();

    // Inicializa nova solucao
    RepMODM newRep(c);
    for (int client = 0; client < c; client++) newRep[client].resize(n);

    // Inicializa new ADS
    AdsMODM newAds;

    // Vetor de Produtos - Para cada Produto tem-se um conjunto de clientes e
    // valor de NPP

    vector<bool> availableClients(c, true);
    vector<bool> availableProducts(n, true);
    vector<int> offers(c);
    double val = 0, exp = 0;

    bool ended = false;
    int iter = 0;
    int notAvailable = 0;

    vector<vector<pair<double, int>>> NPP(n);

    if (Component::verbose) std::cout << "COMPLETE NPPs:" << std::endl;

    for (int product = 0; product < n; product++) {
      for (int client = 0; client < c; client++) {
        int r = pMODM.getRevenue(client, product);
        int cost = pMODM.getCost(client, product);

        double clientRisk = 0;
        if (r >= 0 && r < 3) clientRisk = 0.01;

        if (r >= 3 && r < 5) clientRisk = 0.05;

        if (r >= 5 && r < 8) clientRisk = 0.1;

        if (r >= 8 && r < 11) clientRisk = 0.25;

        if (r >= 11 && r < 14) clientRisk = 0.4;

        if (r >= 14) clientRisk = 0.6;

        // std::cout << "clientRisk = " << clientRisk << std::endl;

        double valueNPP = (r - cost) / cost;

        NPP[product].push_back(std::make_pair(valueNPP, client));
      }

      sort(NPP[product].begin(), NPP[product].end(), comparacao);

      if (Component::verbose) std::cout << NPP[product] << std::endl;
    }

    while (!ended) {
      if (Component::verbose) std::cout << "\n iter = " << iter << std::endl;

      vector<double> PRj(n);
      vector<double> Cj(n);
      vector<double> Pj(n);
      vector<vector<int>> selectedClients(
          n);  // verificar se pode virar um booleano

      vector<vector<pair<double, int>>> NPPj(n);

      if (Component::verbose) std::cout << "PARCIAL NPPjs:" << std::endl;

      for (int product = 0; product < n; product++) {
        if (availableProducts[product])
          for (int client = 0; client < c; client++) {
            int clientID = NPP[product][client].second;
            if (availableClients[clientID]) {
              NPPj[product].push_back(NPP[product][client]);
            }
          }

        if (Component::verbose) std::cout << NPPj[product] << std::endl;
      }

      for (int product = 0; product < n; product++) {
        if (availableProducts[product] == true) {
          int minSize = NPPj[product].size();  // todo verificar se vai dar erro
          int Oj = pMODM.getProductMinClients(product);

          for (int o = 0; o < Oj; o++) {
            int sizeLC = ceil(alpha * NPPj[product].size());
            int greedyRand = rg.rand(sizeLC);
            int client = NPPj[product][greedyRand].second;

            if (greedyRand > sizeLC) {
              std::cout << "BUG!!!! ON CONSTRUTIVE" << std::endl;
              getchar();
            }

            NPPj[product].erase(NPPj[product].begin() + greedyRand);

            Cj[product] += pMODM.getCost(client, product);
            Pj[product] += pMODM.getRevenue(client, product);
            selectedClients[product].push_back(client);
          }

          PRj[product] =
              Pj[product] - Cj[product] - pMODM.getProductFixedCost(product);
        }
      }

      /*			cout << "Cj: " << std::endl;
                      std::cout << Cj << std::endl;
                      std::cout << "Pj: " << std::endl;
                      std::cout << Pj << std::endl;

                      getchar();*/

      if (Component::verbose) {
        std::cout << "Cj: " << std::endl;
        std::cout << Cj << std::endl;
        std::cout << "Pj: " << std::endl;
        std::cout << Pj << std::endl;
        std::cout << "PRj: " << std::endl;
        std::cout << PRj << std::endl;
        std::cout << "selectedClients: " << std::endl;
        std::cout << selectedClients << std::endl;
      }

      int bestj = -1;  // Melhor Produto a ser inserido
      int bestPRj = 0;

      for (int product = 0; product < n; product++)  // j = product
      {
        if (availableProducts[product] == true) {
          bool bugdet = (Cj[product] <= pMODM.getProductBudget(product));
          bool hurdle =
              ((val + Pj[product]) >=
               ((1 + pMODM.getHurdleRate()) *
                (exp + Cj[product] + pMODM.getProductFixedCost(product))));
          if (bugdet && hurdle)
            if (PRj[product] > bestPRj) {
              bestj = product;
              bestPRj = PRj[product];
              if (Component::verbose) std::cout << "bestPRj = " << bestPRj << std::endl;
            }
        }
      }

      int bugs = 0;
      /*
                      std::cout << "bestj" << bestj << std::endl;

                      sort(selectedClients[bestj].begin(),
         selectedClients[bestj].end(), comparacaoClients); std::cout <<
         selectedClients[bestj] << std::endl; for (int o = 0; o <
         pMODM.getProductMinClients(bestj) - 1; o++)
                      {
                      if (selectedClients[bestj][o] == selectedClients[bestj][o
         + 1])
                      {
                      std::cout << "Bug!!!!" << std::endl;
                      std::cout << selectedClients[bestj][o] << std::endl;
                      getchar();
                      }
                      }*/

      if (bestj != -1) {
        int minSize = selectedClients[bestj].size();
        int bestOj = min(pMODM.getProductMinClients(bestj), minSize);
        if (Component::verbose) {
          std::cout << "bestJ = " << bestj << std::endl;
          std::cout << "bestOj = " << bestOj << std::endl;
        }

        val += Pj[bestj];
        exp += Cj[bestj] + pMODM.getProductFixedCost(bestj);
        for (int o = 0; o < bestOj; o++) {
          availableProducts[bestj] = false;
          int client = selectedClients[bestj][o];

          if (newRep[client][bestj])  // TODO Excluir essa variavel
            bugs++;
          newRep[client][bestj] = true;
          offers[client]++;
          if (offers[client] == pMODM.getClientMaxOffers(client)) {
            availableClients[client] = false;
            notAvailable++;
          }
        }

        if (Component::verbose)
          std::cout << "notAvailable: " << notAvailable << std::endl;

        for (int product = 0; product < n; product++)  // j = product
          if (notAvailable > (c - pMODM.getProductMinClients(product)))
            availableProducts[product] = false;
        if (Component::verbose) std::cout << "bugs: " << bugs << std::endl;
      } else
        ended = true;

      // std::cout << newRep << std::endl;
      // getchar();
      iter++;
    }
    if (Component::verbose) {
      std::cout << "Solucao Final - 1 :" << std::endl;
      std::cout << newRep << std::endl;
    }

    adsMan.initializeADS(newRep, newAds);

    if (Component::verbose) {
      std::cout << "Total Revenue: ";
      std::cout << newAds.totalRevenue << std::endl;
      std::cout << "Total Cost: ";
      std::cout << newAds.totalCost << std::endl;
      // std::cout << availableProducts << std::endl;
      std::cout << "productOffers: ";
      std::cout << newAds.productOffers << std::endl;
      getchar();
    }

    //===================================================================================
    // Otimizando e melhorando a solução base:

    for (int product = 0; product < n; product++)
      for (int client = 0; client < c; client++) {
        // std::cout << "(" << newRep[client] << "," <<
        // pMODM.getClientMaxOffers(client) << ")" << std::endl;
        if ((newAds.productOffers[product] > 0) &&
            (newRep[client][product] == false))
          if (newAds.clientOffers[client] <
              pMODM.getClientMaxOffers(client))  // Verifico se o client pode
                                                 // receber mais algum produto
            if ((pMODM.getRevenue(client, product) >
                 pMODM.getCost(client, product)) &&
                (pMODM.getCost(client, product) + newAds.totalCost[product]) <=
                    pMODM.getProductBudget(product)) {
              // std::cout << "OK, VALE A PENA!!!" << std::endl;
              // std::cout << pMODM.getCost(client, product) << std::endl;
              // std::cout << pMODM.getRevenue(client, product) << std::endl;
              // std::cout << newAds.totalCost[product] << std::endl;
              // std::cout<<"Client: "<<client<<endl;
              // std::cout<<"Product: "<<product<<endl;
              newRep[client][product] = true;

              // update ADS
              newAds.clientOffers[client]++;
              newAds.productOffers[product]++;
              newAds.totalCost[product] += pMODM.getCost(client, product);
              newAds.totalRevenue[product] += pMODM.getRevenue(client, product);
              // std::cout << getchar();
            }
      }
    //===================================================================================

    if (Component::verbose) {
      std::cout << "Solucao Final - 2 :" << std::endl;
      std::cout << newRep << std::endl;
      std::cout << "Total Revenue: ";
      std::cout << newAds.totalRevenue << std::endl;
      std::cout << "Total Cost: ";
      std::cout << newAds.totalCost << std::endl;
      std::cout << "productOffers: ";
      std::cout << newAds.productOffers << std::endl;

      getchar();
    }

    // adsMan.initializeADS(newRep, newAds);

    if (Component::verbose) std::cout << "Fim Constructive!";

    // std::cout << "Tempo Atual: " << t.now() << std::endl;
    // getchar();
    return make_optional(SolutionMODM(newRep, newAds));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":" << GRASP::family()
       << ":GRConstructive"
       << ":MODM";
    return ss.str();
  }

  virtual std::string id() const override { return idComponent(); }
};
}  // namespace MODM

#endif /*MODM_CONTRUCTIVE_BasicGreedyRandomized_H_*/
