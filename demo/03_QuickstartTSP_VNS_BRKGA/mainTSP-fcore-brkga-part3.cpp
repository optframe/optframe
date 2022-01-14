
pair<Evaluation<int>, vector<int>>
fDecode(const vector<double>& rk)
{
   vector<pair<double, int>> v(rk.size());
   int k = 0;
   for (unsigned i = 0; i < v.size(); i++)
      v[k] = pair<double, int>(rk[i], i);

   sort(v.begin(), v.end(), [](const pair<double, int>& i, const pair<double, int>& j) -> bool {
      return i.first < j.first;
   });

   // R = vector<int>
   vector<int> p(v.size());
   for (unsigned i = 0; i < v.size(); i++)
      p[i] = v[i].second;

   Evaluation<int> e = eval.evaluate(p);
   return make_pair(e, p);
}

// evaluator random keys (for TSP)
FDecoderRK<std::pair<std::vector<int>, Evaluation<int>>, double, MinOrMax::MINIMIZE> decoder{
   fDecode
};
