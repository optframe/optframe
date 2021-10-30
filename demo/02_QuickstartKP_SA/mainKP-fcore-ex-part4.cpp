
std::cout << "======== Executa Simulated Annealing ========" << std::endl;
// Especifica um gerador aleatório para o Simulated Annealing
RandGen rg;
//
// Cria objeto da classe 'InitialSearch' (parecido com 'construtivoAleatório')
BasicInitialSearch<ESolutionKP> initRand(randomConstructive, evalKP);
// Instancia um Simulated Annealing com alpha=98%, iterações na temp = 100, temperatura inicial = 99999
BasicSimulatedAnnealing<ESolutionKP> sa{
   evalKP,
   initRand,
   nsFlip,
   0.98,
   100,
   99999,
   rg
};
// executa o SA e coleta o 'status' de saída
// passa um 'Criterio de Parada' por tempo (= 10 segundos)
auto searchOut = sa.search(StopCriteria<ESolutionKP::second_type>{ 10.0 }); // 10.0 seconds max
// pega melhor solução do método SA
ESolutionKP melhor = *searchOut.best; //*sa.getBestSolution();
std::cout << "======== Imprime melhor solução do SA ========" << std::endl;
// imprime representação da melhor solução
cout << melhor.first << endl;
// imprime avaliação da melhor solução
melhor.second.print();
