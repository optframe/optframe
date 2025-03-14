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
