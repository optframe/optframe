
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
