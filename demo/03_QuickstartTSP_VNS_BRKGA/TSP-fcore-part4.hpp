std::vector<int>
frandom()
{
   vector<int> v(pTSP.n, -1); // get information from context
   for (unsigned i = 0; i < v.size(); i++)
      v[i] = i;
   std::random_shuffle(v.begin(), v.end());
   return v;
}

// Generate random solution
FConstructive<std::vector<int>> crand{
   frandom
};
