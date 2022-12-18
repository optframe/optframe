std::cout << "======== Carregando Problema ========" << std::endl;
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