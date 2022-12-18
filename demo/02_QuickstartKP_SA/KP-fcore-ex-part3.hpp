
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
