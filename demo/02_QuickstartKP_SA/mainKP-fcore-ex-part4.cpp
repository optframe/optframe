
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
