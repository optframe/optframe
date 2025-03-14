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

#ifndef OPTFRAME_ESContinous_HPP_
#define OPTFRAME_ESContinous_HPP_

#include <math.h>

#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Search/LocalSearch.hpp>
#include <OptFrame/Search/SingleObjSearch.hpp>
#include <OptFrame/Timer.hpp>
#include <vector>

#include "../../Constructive.hpp"
namespace optframe {

// ESTRUTURA DA ESTRATEGIA EVOLUTIVA
// CONSTITUIDA DE UMA MATRIX DE DESVIOS PADROES
template <class ESStruct>
struct ESContinuousStructure {
  ESStruct desvs;

  ESContinuousStructure(ESStruct _desvs) : desvs(_desvs) {}
};

// CADA INDIVIDUO EH UM PAR DE SOLUCAO E UMA TUPLE COM O PARAMETROS DA
// ESTRATEGIA template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class ESStruct
// = double>
//
// template<XRepresentation R, class ADS = OPTFRAME_DEFAULT_ADS, class ESStruct
// = double, XBaseSolution<R, ADS> S = CopySolution<R, ADS>, XEvaluation XEv =
// Evaluation<>, XESolution XES = pair<S, XEv>>
template <XRepresentation R, class ADS = OPTFRAME_DEFAULT_ADS,
          class ESStruct = double,
          XBaseSolution<R, ADS> S = CopySolution<R, ADS>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>

class ESContinous : public SingleObjSearch<XES> {
 private:
  S* sStar;
  XEv* eStar;
  Evaluator<S, XEv, XES>& eval;
  Constructive<S>& constructive;
  vector<NSSeq<XES>*> vNS;
  LocalSearch<XES>& ls;

  const int mi;
  const int lambda;
  const int gMax;
  int iterSemMelhora;
  int gAtual;

  typedef pair<S*, ESContinuousStructure<ESStruct>*> Individuo;

  typedef std::vector<std::pair<Individuo, double>> Populacao;

  static bool compara(pair<Individuo, double> p1, pair<Individuo, double> p2) {
    return p1.second < p2.second;
  }

  // FUNCAO UTILZIADA NO QuickSort

  virtual void changeParameters(ESContinuousStructure<ESStruct>* p) = 0;

  virtual void applyParameters(S* s, ESContinuousStructure<ESStruct>* p) = 0;

  virtual ESStruct generateInitialESStructure(S* s) = 0;

 public:
  ESContinous(Evaluator<S, XEv, XES>& _eval, Constructive<S>& _constructive,
              vector<NSSeq<XES>*> _vNS, LocalSearch<XES>& _ls, int _mi,
              int _lambda, int _gMax)
      : eval(_eval),
        constructive(_constructive),
        vNS(_vNS),
        ls(_ls),
        mi(_mi),
        lambda(_lambda),
        gMax(_gMax) {
    sStar = nullptr;
    eStar = nullptr;

    iterSemMelhora = 0;
    gAtual = 0;
  }

  virtual ~ESContinous() {}

  void applyLocalSearchBest(Populacao& p, int nBuscas) {
    bool aux[nBuscas];
    for (int i = 0; i < nBuscas; i++) aux[i] = false;

    // ORDECAO QuickSort
    std::vector<std::pair<Individuo, double>> v;

    for (int i = 0; i < (int)p.size(); i++) {
      Evaluation<>& e = eval.evaluate(*p[i].first);
      v.push_back(std::make_pair(p[i], e.evaluation()));
      delete &e;
    }

    sort(v.begin(), v.end(), compara);  // ordena com QuickSort

    int n = 0;

    while (n < nBuscas) {
      int ind;
      ind = rand() % nBuscas;

      if (aux[ind] == false) {
        S* filhoo = &ls.search(*p[ind].first);
        delete p[ind].first;
        p[ind].first = filhoo;

        aux[ind] = true;
        n++;
      }
    }
  }

  Populacao& competition(Populacao& pais, Populacao& filhos) {
    std::vector<std::pair<Individuo, double>> v;

    for (int i = 0; i < (int)pais.size(); i++) {
      v.push_back(std::make_pair(pais[i].first, pais[i].second));
    }

    for (int i = 0; i < (int)filhos.size(); i++) {
      v.push_back(std::make_pair(filhos[i].first, filhos[i].second));
    }

    sort(v.begin(), v.end(), compara);  // ordena com QuickSort

    Populacao* p = new Populacao;

    double fo_pop = 0;

    for (int i = 0; i < (int)v.size(); i++)
      if (i < mi) {
        p->push_back(std::make_pair(v[i].first, v[i].second));
        fo_pop += v[i].second;
      } else {
        delete v[i].first.first;   // Solution
        delete v[i].first.second;  // vectors de mutacao e prob
      }

    fo_pop = fo_pop / mi;
    // std::cout << "Media Competicao, media: " << fo_pop << std::endl;

    // AVALIA MELHOR INDIVIDUO
    double fo = v[0].second;

    // for(int i = 0;i<v.size();i++)
    //	cout<<v[i].second<<endl;
    // getchar();

    Evaluation<> efo(fo);

    if (eval.betterThan(efo, *eStar)) {
      delete eStar;
      delete sStar;

      sStar = &v[0].first.first->clone();
      (*eStar) = eval.evaluate(*sStar);

      /*
                         std::cout << "vRealClone = " <<
         (&eval.evaluate(*v[0].first.first))->evaluation() << std::endl; std::cout <<
         "vRealClone = " << (&eval.evaluate(*v[0].first.first))->evaluation() <<
         std::endl; std::cout << "vRealClone = " <<
         (&eval.evaluate(*v[0].first.first))->evaluation() << std::endl; std::cout <<
         "vRealClone = " << (&eval.evaluate(*v[0].first.first))->evaluation() <<
         std::endl; std::cout << "vRealClone = " <<
         (&eval.evaluate(*v[0].first.first))->evaluation() << std::endl; std::cout <<
         "vRealClone = " << (&eval.evaluate(*v[0].first.first))->evaluation() <<
         std::endl; std::cout << "eStarClone = " <<
         (&eval.evaluate(v[0].first.first->clone()))->evaluation() << std::endl; std::cout
         << "eStarClone = " <<
         (&eval.evaluate(v[0].first.first->clone()))->evaluation() << std::endl; std::cout
         << "eStarClone = " <<
         (&eval.evaluate(v[0].first.first->clone()))->evaluation() << std::endl; std::cout
         << "eStarClone = " <<
         (&eval.evaluate(v[0].first.first->clone()))->evaluation() << std::endl; std::cout
         << "eStarClone = " << eStar->evaluation() << std::endl;

                         std::cout << v[0].first.first->getR() << std::endl;
                         std::cout << sStar->getR() << std::endl;
                         */
      std::cout << "Iter:" << gAtual << "\tWithoutImprovement: " << iterSemMelhora;
      std::cout << "\t Best: " << v[0].second;
      std::cout << "\t [";
      std::cout << "imprimirMatriz";
      std::cout << "]" << std::endl;

      /*
                         FILE* arquivo = fopen("logParam.txt", "a");
                         if (!arquivo)
                         {
                         std::cout << "ERRO: falha ao criar arquivo \"logParam.txt\""
         << std::endl;
                         }
                         else
                         {
                         fprintf(arquivo, "%d\t", gAtual);
                         for (int param = 0; param < nParam; param++)
                         {
                         //fprintf(arquivo, "%.4f\t%d\t",
         p->at(0).second->at(param).pr,p->at(0).second->at(param).nap);
                         }
                         fprintf(arquivo, "\n");
                         fclose(arquivo);
                         }*/

      iterSemMelhora = 0;

    } else
      iterSemMelhora++;

    return *p;
  }

  Populacao& lowSelectivePression(Populacao& pop, Populacao& pop_filhos) {
    // onlyOffsprings
    Populacao pop_nula;
    Populacao& pNova = competition(pop_nula, pop_filhos);

    for (int i = 0; i < pop.size(); i++) {
      delete pop[i].first.first;
      delete pop[i].first.second;
    }

    return pNova;
  }

  Populacao& highSelectivePression(Populacao& pop, Populacao& pop_filhos) {
    Populacao& pNova = competition(pop, pop_filhos);

    return pNova;
  }

  virtual void localSearch(pair<S, Evaluation<>>& se,
                           const StopCriteria<XEv>& stopCriteria) {
    ls.searchFrom(se, stopCriteria);
  }

  // std::optional<pair<S, Evaluation<>>> search(StopCriteria<XEv>&
  // stopCriteria) override
  //
  // SearchStatus search(const StopCriteria<XEv>& stopCriteria) override
  SearchOutput<XES> searchBy(const StopCriteria<XEv>& stopCriteria,
                             std::optional<XES> best) override {
    // op<XES>& star = this->best;
    //  TODO: reimplement with SearchBy ...
    op<XES> star;
    std::cout << "ES search(" << stopCriteria.target_f << ","
         << stopCriteria.timelimit << ")" << std::endl;

    Timer tnow;

    Populacao pop(mi);

    double fo_Constructive = 0;

    // GERANDO VETOR DE POPULACAO INICIAL
    for (int i = 0; i < mi; i++) {
      std::optional<S> s =
          constructive.generateSolution(stopCriteria.timelimit);

      ESStruct a =
          generateInitialESStructure(&(*s));  // TODO: fix original function
      // std::cout << a;
      // getchar();

      ESContinuousStructure<ESStruct>* m =
          new ESContinuousStructure<ESStruct>(a);

      Evaluation<> e = eval.evaluate(*s);

      pop[i] = make_pair(
          make_pair(&(*s), m),
          e.evaluation());  // TODO: fix original function (or Population)

      fo_Constructive += e.evaluation();

      if (i == 0) {
        // First solution of the method
        eStar = &e.clone();
        sStar = &pop[i].first.first->clone();
        // std::cout<<"e.evaluation() = "<<(double)e.evaluation()<<endl;
        // std::cout<< " eStar = "<<(double)eStar->evaluation()<<endl;
        // getchar();

      } else {
        if (eval.betterThan(e, *eStar)) {
          delete eStar;
          delete sStar;
          eStar = &e.clone();
          sStar = &pop[i].first.first->clone();
        }
      }

      delete &e;
    }

    std::cout << "Mean initial population FO: " << fo_Constructive / mi << "\t";
    std::cout << "best:" << (double)eStar->evaluation() << std::endl;
    // ===============================

    iterSemMelhora = 0;

    // Timer tnowClone;
    // double sumClone = 0;
    // double sumEval = 0;
    // int counter = 1;

    while ((iterSemMelhora < gMax) && ((tnow.now()) < stopCriteria.timelimit) &&
           eval.betterThan(stopCriteria.target_f, *eStar)) {
      // std::cout << "gAtual = " << gAtual << std::endl;
      // getchar();

      Populacao pop_filhos;
      double fo_filhos = 0;

      // GERA OS OFFSPRINGS
      for (int l = 1; l <= lambda; l++) {
        int x = rand() % mi;

        // Cria Filho e Tuple de Parametros (pi,nap,vizinhança)
        // double tClone = tnowClone.now();
        S* filho = &pop[x].first.first->clone();
        // sumClone += tnowClone.now() - tClone;

        ESContinuousStructure<ESStruct>* vt =
            new ESContinuousStructure<ESStruct>(*pop[x].first.second);

        // Mutacao dos parametros l
        changeParameters(vt);

        // application dos parametros para gerar filho completo
        applyParameters(filho, vt);

        // ApplyLocal Search in each individual of the Population
        // TODO

        // Sem Busca Local
        S* filho_bl = filho;

        // double tEval = tnowClone.now();
        Evaluation<> e = eval.evaluate(*filho_bl);
        // sumEval += tnowClone.now() - tEval;
        // counter++;

        pop_filhos.push_back(
            make_pair(std::make_pair(filho_bl, vt), e.evaluation()));

        fo_filhos += e.evaluation();
      }

      // std::cout << "Valor Medio das FO's dos filhos: " << fo_filhos / mi << std::endl;

      // APLICA B.L VND EM 'nb' INDIVIDUOS DA POP_FILHOS
      // applyLocalSearchBest(pop_filhos, 2);
      // std::cout<<"Applying local Search ..."<<endl;

      // applyLocalSearch(pop_filhos, nb, lambda);

      // std::cout<<" local search finished!"<<endl;
      // getchar();
      //  ETAPA DE SELECAO - MI,LAMBDA ou MI + LAMBDA // ATUALIZA BEST
      // std::cout<<"Applying selection ..."<<endl;

      // Populacao& pNova = lowSelectivePression(pop, pop_filhos); //Estrategia
      // (Mi,Lamda)
      Populacao& pNova =
          highSelectivePression(pop, pop_filhos);  // Estrategia (Mi+Lamda)

      // std::cout<<"selection finished !"<<endl;
      pop.clear();
      pop_filhos.clear();

      pop = pNova;

      // INCREMENTA GERACAO
      gAtual++;

      delete &pNova;
    }

    /*cout << sumEval << std::endl;
                std::cout << sumClone << std::endl;
                std::cout << counter << std::endl;
                std::cout << sumEval / counter << std::endl;
                std::cout << sumClone / counter << std::endl;
                std::cout << tnow.now() << std::endl;
                getchar();*/

    std::cout << "tnow.now() = " << tnow.now()
         << " timelimit = " << stopCriteria.timelimit << std::endl;
    std::cout << "Acabou ES = iterSemMelhor = " << iterSemMelhora
         << " gMax = " << gMax << std::endl;
    std::cout << "target_f = " << stopCriteria.target_f
         << " eStar->evaluation() = " << (double)eStar->evaluation() << std::endl;
    // getchar();

    for (int i = 0; i < (int)pop.size(); i++) {
      delete pop[i].first.first;
      delete pop[i].first.second;
    }

    // S& s = *sStar;
    // Evaluation<>& e = *eStar;

    // std::cout<<s.getR();
    // getchar();
    // delete eStar;
    // delete sStar;

    // return new pair<S, Evaluation<>>(s, e);
    // return make_optional(std::make_pair(*sStar, *eStar)); // fix: leak
    star = make_optional(std::make_pair(*sStar, *eStar));  // fix: leak
    // this->best = star;
    return SearchOutput<XES>{SearchStatus::NO_REPORT, star};
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << "ESContinous";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe
#endif /* OPTFRAME_ESContinous_HPP_ */
