

pair<Evaluation<int>, vector<int>> fDecodeEval(
    sref<Evaluator<typename ESolutionTSP::first_type,
                   typename ESolutionTSP::second_type, ESolutionTSP>>
        eval,
    const vector<double>& rk) {
  vector<pair<double, int>> v(rk.size());
  //
  for (unsigned i = 0; i < v.size(); i++) v[i] = pair<double, int>(rk[i], i);

  sort(v.begin(), v.end(),
       [](const pair<double, int>& i, const pair<double, int>& j) -> bool {
         return i.first < j.first;
       });

  // R = vector<int>
  vector<int> p(v.size());
  for (unsigned i = 0; i < v.size(); i++) p[i] = v[i].second;

  /*
  // ========== CHECKER ========
  vector<bool> vb(v.size(), false);
  for (unsigned i = 0; i < p.size(); i++)
     vb[p[i]] = true;
  for (unsigned i = 0; i < vb.size(); i++) {
     if (!vb[i]) {
        std::cout << "ERROR rk:" << rk << std::endl;
        std::cout << "ERROR v:" << v << std::endl;
        std::cout << "ERROR p:" << p << std::endl;
        std::cout << "ERROR vb:" << vb << std::endl;
     }
     assert(vb[i]);
  }
  // ===== end CHECKER =====
*/

  Evaluation<int> e = eval->evaluate(p);
  return make_pair(e, p);
}

// evaluator random keys (for TSP)
// FDecoderEvalRK<std::pair<std::vector<int>, Evaluation<int>>, double>
// decoder{fDecode};
