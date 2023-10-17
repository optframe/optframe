std::vector<int> frandom(sref<ProblemContext> pTSP) {
  vector<int> v(pTSP->n, -1);  // get information from context
  for (int i = 0; i < (int)v.size(); i++) v[i] = i;
  std::random_shuffle(v.begin(), v.end());
  return v;
}

// Generate random solution
// FConstructive<std::vector<int>, ProblemContext> crand{frandom};
