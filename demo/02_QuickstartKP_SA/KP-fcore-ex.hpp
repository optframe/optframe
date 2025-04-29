// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

// OptFrame Demo 0-1 Knapsack Problem + Simulated Annealing
// File: KP-fcore-ex.hpp
#pragma once

#define NO_CXX_MODULES 1

// C++
#include <algorithm>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>
//
#include <OptFrame/printable/printable.hpp>
using namespace optframe;
//
#include <OptFCore/FCore.hpp>
#include <OptFCore/FCoreAll.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT

// namespace para o problema da mochila
// namespace KP_fcore {

// Solução para o problema da mochila, e elemento de avaliação

// ================================
//    Definição do 'ESolution'
// Par: (Representação, Avaliação)
// ================================

using ESolutionKP = std::pair<std::vector<bool>,  // (representation)
                              Evaluation<int>     // (objective value)
                              >;
class ProblemContext {
 public:
  int n{0};       // numero de itens
  int Q{-1};      // capacidade máxima da mochila
  vector<int> p;  // lucro 'p' de cada item
  vector<int> w;  // peso 'w' de cada item
  //
  // leitor de dados "estilo Java" (Scanner)
  void load(Scanner& scanner) {
    n = *scanner.nextInt();  // leitura do numero de itens na mochila
    Q = *scanner.nextInt();  // leitura da capacidade da mochila
    p = vector<int>(n);      // realoca espaço
    w = vector<int>(n);      // realoca espaço
    //
    std::cout << "n=" << n << " Q=" << Q << std::endl;
    //
    // leitura do lucro do item i
    for (int i = 0; i < n; i++) {
      p[i] =
          *scanner.nextInt();  // '*' é usado pq saída do 'nextInt' é 'optional'
      std::cout << "p[" << i << "]=" << p[i] << " ";
    }
    std::cout << std::endl;
    //
    // leitura do peso do item i
    for (int i = 0; i < n; i++) {
      w[i] =
          *scanner.nextInt();  // '*' é usado pq saída do 'nextInt' é 'optional'
      std::cout << "w[" << i << "]=" << w[i] << " ";
    }
    std::cout << std::endl;
  }
};
// Instanciar um problema da mochila pKP
// ProblemContext pKP; // NOLINT

std::vector<bool> frandom(sref<ProblemContext> pKP) {
  vector<bool> v(pKP->n, false);  // começa sem nenhum item escolhido
  for (unsigned i = 0; i < v.size(); i++)
    v[i] = rand() % 2;  // sorteia se leva item ou não (resultado 0 ou 1)
  // retorna solução
  return v;
}

// Gerador de solução inicial (método construtivo)
// FConstructive<std::vector<bool>, ProblemContext> randomConstructive{pKP,
// frandom};

// ===========================
//    Função de avaliação
// ===========================

// função de avaliação da mochila
Evaluation<int> fevaluate(sref<ProblemContext> pKP,
                          const std::vector<bool>& s) {
  int f = 0;
  int somaPeso = 0;
  for (int i = 0; i < pKP->n; i++)
    if (s[i])  // se elemento foi escolhido
    {
      f += pKP->p[i];         // acumula lucro do elemento i
      somaPeso += pKP->w[i];  // acumula peso do elemento i
    }
  // verifica capacidade excedida
  if (somaPeso >= pKP->Q)
    f -= 1000000 *
         (somaPeso - pKP->Q);  // punição proporcional ao excesso de peso
  //
  return Evaluation<int>{f};
}

// Evaluate (false -> maximização)
// FEvaluator<ESolutionKP, MAXIMIZE> evalKP{fevaluate};
//
class MoveBitFlip : public Move<ESolutionKP> {
 public:
  int k;  // MoveBitFlip (moveData = 'int' (k))

  explicit MoveBitFlip(int _k) : k{_k} {}

  uptr<Move<ESolutionKP>> apply(ESolutionKP& se) override {
    se.first[k] = !se.first[k];                          // reverts element 'k'
    return uptr<Move<ESolutionKP>>(new MoveBitFlip{k});  // returns reverse move
  }
};

uptr<Move<ESolutionKP>> makeMoveBitFlip(int k) {
  return uptr<Move<ESolutionKP>>(new MoveBitFlip{k});
}
// gerador de movimentos aleatórios do tipo BitFlip
uptr<Move<ESolutionKP>> fRandomFlip(sref<ProblemContext> pKP,
                                    const ESolutionKP& se) {
  int k = ::rand() % pKP->n;  // sorteia um item entre [0..n-1]
  // cria um "movimento" de flip, na posição k, com operação 'fApplyFlip'
  return uptr<Move<ESolutionKP>>(makeMoveBitFlip(k));
}

// Estrutura de Vizinhança para BitFlip (NS)
// FNS<ESolutionKP> nsFlip{fRandomFlip};

// ================== EVERYTHING TOGETHER ====================

class OptFrameDemoKP {
 public:
  FConstructive<std::vector<bool>, ProblemContext> randomConstructive;
  FEvaluator<ESolutionKP, MAXIMIZE, ProblemContext> evalKP;
  FNS<ESolutionKP, ProblemContext> nsFlip;

  explicit OptFrameDemoKP(sref<ProblemContext> p)
      : randomConstructive{p, frandom},
        evalKP{FEvaluator<ESolutionKP, MAXIMIZE, ProblemContext>{p, fevaluate}},
        nsFlip{FNS<ESolutionKP, ProblemContext>{p, fRandomFlip}} {}
};
