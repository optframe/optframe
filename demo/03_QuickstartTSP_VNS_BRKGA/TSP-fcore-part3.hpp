
Evaluation<int> fevaluate(sref<ProblemContext> pTSP,
                          const std::vector<int>& s) {
  int f = 0;
  for (int i = 0; i < int(pTSP->n) - 1; i++) f += pTSP->dist(s[i], s[i + 1]);
  f += pTSP->dist(s[int(pTSP->n) - 1], s[0]);
  return Evaluation<int>{f};
}

// Evaluate
// FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE> eval{fevaluate};
