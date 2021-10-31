int
main()
{
   sref<RandGen> rg = new RandGen; // avoids weird windows OS interactions

   // load data into problem context 'pTSP'
   Scanner scanner{ "3\n1 10 10\n2 20 20\n3 30 30\n" };
   pTSP.load(scanner);
   std::cout << pTSP.dist << std::endl;

   // Parameters BRKGA
   // (C1): Evaluator<S, XEv>& _evaluator, int key_size, unsigned numGen, unsigned _popSize, double fracTOP, double fracBOT, double _probElitism) :

   sref<DecoderRandomKeys<ESolutionTSP::first_type, ESolutionTSP::second_type, double>> _decoder = decoder;
   sref<InitialPopulation<std::pair<vector<double>, ESolutionTSP::second_type>>> _initPop = new MyRandomKeysInitPop(pTSP.n); // passing key_size

   //eprk, pTSP.n, 1000, 30, 0.4, 0.3, 0.6
   BRKGA<ESolutionTSP, double> brkga(
     _decoder,
     MyRandomKeysInitPop(pTSP.n, rg), // key_size = pTSP.n
     30,
     1000,
     0.4,
     0.3,
     0.6,
     rg);

   auto searchOut = brkga.search(3.0); // 3.0 seconds max
   ESolutionTSP best = *searchOut.best;
   // best solution value
   best.second.print();

   std::cout << "FINISHED" << std::endl;
   return 0;
}
