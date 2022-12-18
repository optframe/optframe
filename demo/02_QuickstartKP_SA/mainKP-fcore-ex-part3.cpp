
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
