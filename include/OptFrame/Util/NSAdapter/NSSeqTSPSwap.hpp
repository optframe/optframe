// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_NSSEQTSPSWAP_HPP_
#define OPTFRAME_NSSEQTSPSWAP_HPP_

#include "../../Helper/Solution.hpp"
#include "../../Move.hpp"
#include "../../NSSeq.hpp"
#include "BaseSolutionTSP.hpp"
#include "Moves/MoveTSPSwap.hpp"
#include "NSIterators/IteratorTSPSwap.hpp"

using namespace std;

//! \english NSSeq class for the classic Traveling Salesman Problem (TSP)
//! neighborhood of Swap. \endenglish \portuguese Classe NSSeq para a estrutura
//! de vizinhança clássica de troca (Swap) do Problema do Caixeiro Viajante.
//! \endportuguese

/*!
 \english
 Working structure: vector<T>.

 Classical Problem: Traveling Salesman Problem

 The Classic Swap Neighborhood Structure.

 Article:

 Swap is applied for any problem that representation is like a vector<T>, where
 T is the type of the vector.

 e.g: vector<T> where type of T is int

 s means Solution

 initial s: 1 2 3 4 5 6 7 8

 s' after apply MoveTSPSwap(2,5) in s

 s': 1 2 6 4 5 3 7 8
 \endenglish

 \portuguese
 Estrutura alvo: vector<T>.

 Problema clássico: Problema do Caixeiro Viajante (em inglês TSP - Traveling
 Salesman Problem)

 Estrutura de vizinhança clássica de troca.

 Artigo referência:

 Swap é aplicável em todo tipo de problema no qual a representação é um
 vector<T>, onde T é o tipo do vector.

 Exemplo: vector<T> onde o tipo de T é int

 s significa solução

 inicial s: 1 2 3 4 5 6 7 8

 s' após aplicação do MoveTSPSwap(2,5) em s

 s': 1 2 6 4 5 3 7 8
 \endportuguese
 */

namespace optframe {

template <class T, class ADS = OPTFRAME_DEFAULT_ADS,
          XBaseSolution<vector<T>, ADS> S = CopySolution<vector<T>, ADS>,
          class MOVE = MoveTSPSwap<T, ADS, S>,
          class P = OPTFRAME_DEFAULT_PROBLEM,
          class NSITERATOR = NSIteratorTSPSwap<T, ADS, S, MOVE, P>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>,
          XSearch<XES> XSH = std::pair<S, XEv>>
class NSSeqTSPSwap : public NSSeq<XES, XSH> {
  typedef vector<T> Route;

 private:
  P* p;  // has to be the last

 public:
  NSSeqTSPSwap(P* _p = nullptr) : p(_p) {}

  virtual ~NSSeqTSPSwap() {}

  // using NSSeq<S, XEv, XSH>::move;
  // using NSSeq<S, XEv, XSH>::getIterator;

  uptr<Move<XES, XSH>> randomMove(const XES& s) override {
    const Route& rep = s.first.getR();
    if (rep.size() < 2) return uptr<Move<XES, XSH>>(new MOVE(-1, -1, p));

    int p1 = rand() % rep.size();

    int p2 = p1;

    while (p2 == p1) p2 = rand() % rep.size();

    return uptr<Move<XES, XSH>>(new MOVE(p1, p2, p));
  }

  uptr<NSIterator<XES>> getIterator(const XES& s) override {
    const Route& r = s.first.getR();
    return uptr<NSIterator<XES>>(new NSITERATOR(r.size(), p));
  }

  static string idComponent() {
    stringstream ss;
    ss << NSSeq<XES, XSH>::idComponent() << ":NSSeqTSPSwap";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (NSSeq<XES, XSH>::compatible(s));
  }

  std::string toString() const override {
    stringstream ss;
    ss << "NSSeqTSPSwap";
    return ss.str();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_NSSEQTSPSWAP_HPP_*/
