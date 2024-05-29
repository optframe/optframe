// mainKP-fcore-ex.cpp

// C++
#include <iostream>

//
#include "KP-fcore-ex.hpp"  // implementação da mochila

// import everything on main()
using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT
// using namespace KP_fcore;

int main(int argc, char** argv) {std::cout << "======== Carregando Problema ========" << std::endl;
// semente pseudo-aleatória fixa em zero
srand(time(NULL));

std::string sinstance = "knapsack-example.txt";
File f{sinstance};

if (!f.isOpen()) {
  std::cerr << "Problema '" << sinstance << "' não encontrado no diretório!"
            << std::endl;
  return 1;
}

Scanner scanner{std::move(f)};

sref<ProblemContext> pKP{new ProblemContext{}};
pKP->load(scanner);
std::cout << "número de elementos na mochila:" << pKP->n << std::endl;

OptFrameDemoKP demo{pKP};
std::cout << "======== Testa Construtivo Aleatório ========" << std::endl;
// invoca método 'generateSolution' do FCore 'FConstructive' para construtivo
// aleatório
std::vector<bool> sol = *demo.randomConstructive.generateSolution(0.0);
// imprime solução inicial
std::cout << sol << std::endl;
//
std::cout << "======== Testa Avaliador ========" << std::endl;
// avalia solução inicial e cria um par 'ESolution'
ESolutionKP esol(sol, demo.evalKP.evaluate(sol));
// imprime avaliação da solução inicial
esol.second.print();

std::cout << "======== Executa Simulated Annealing ========" << std::endl;
// Especifica um gerador aleatório para o Simulated Annealing
RandGen rg;
//
// Cria objeto da classe 'InitialSearch' (parecido com 'construtivoAleatório')
BasicInitialSearch<ESolutionKP> initRand(nnptr::copy(demo.randomConstructive),
                                         nnptr::copy(demo.evalKP));
// Instancia um Simulated Annealing com alpha=98%, iterações na temp = 100,
// temperatura inicial = 99999
BasicSimulatedAnnealing<ESolutionKP> sa(nnptr::copy(demo.evalKP),
                                        nnptr::copy(initRand),
                                        nnptr::copy(demo.nsFlip), 0.98, 100,
                                        99999, nnptr::copy(rg));
// executa o SA e coleta o 'status' de saída
// passa um 'Criterio de Parada' por tempo (= 10 segundos)
optframe::Timer t;
auto searchOut = sa.search(
    StopCriteria<ESolutionKP::second_type>{10.0});  // 10.0 seconds max
std::cout << "spent time: " << t.now() << "s" << std::endl;
// pega melhor solução do método SA
ESolutionKP melhor = *searchOut.best;  //*sa.getBestSolution();
std::cout << "======== Imprime melhor solução do SA ========" << std::endl;
// imprime representação da melhor solução
cout << melhor.first << endl;
// imprime avaliação da melhor solução
melhor.second.print();


std::cout << "======== Fim da Execução ========" << std::endl;
return 0;
} // main