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

// C++
#include <vector>
//
#include "ProblemInstance.h"
//

void PCAPSolCheck(PCAPProblemInstance& pPCAP, auto& s) {
  std::vector<int> utilizacao;
  for (unsigned int i = 0; i < s.getR().first.size(); i++)
    utilizacao.push_back(0);

  for (unsigned int i = 0; i < s.getR().second.size(); i++) {
    int med = s.getR().second[i];

    int d = pPCAP.vecCidades[i].demanda;
    utilizacao[med] += d;
    std::cout << "Cidade i= " << i << " Associada Facilidade: " << s.getR().second[i]
         << std::endl;
  }

  for (unsigned int i = 0; i < s.getR().first.size(); i++) {
    std::cout << "Facilidade i: " << i << " eh CIDADE: " << s.getR().first[i]
         << std::endl;
  }

  for (unsigned int i = 0; i < s.getR().first.size(); i++) {
    if (utilizacao[i] > pPCAP.vecCidades[i].capacidade)
      std::cout << "Capacidade Estourada da mediana: " << s.getR().first[i]
           << " Passaram: " << utilizacao[i] - pPCAP.vecCidades[i].capacidade
           << std::endl;

    // std::cout<<"Utilizacao = "<<utilizacao[i]<<" Capacidade=
    // "<<pPCAP.vecCidades[i].capacidade<<endl;
  }
}
