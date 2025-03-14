std::vector<int> frandom(sref<ProblemContext> pTSP) {
  vector<int> v(pTSP->n, -1);  // get information from context
  for (int i = 0; i < (int)v.size(); i++) v[i] = i;

  std::random_device rd;
  std::mt19937 g{rd()};

  std::shuffle(v.begin(), v.end(), g);
  return v;
}

// Generate random solution
// FConstructive<std::vector<int>, ProblemContext> crand{frandom};
