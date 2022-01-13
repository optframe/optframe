// ===================================
// Main.cpp file generated by OptFrame
// Project KnapsackProblem
// ===================================

#include <iostream>
#include <math.h>
#include <stdlib.h>

#include <OptFrame/Evaluation.hpp>
#include <OptFrame/MultiObjValue.hpp> // just for testing
#include <OptFrame/Solution.hpp>
#include <OptFrame/printable/printable.hpp>

#include <OptFrame/Heuristics/EvolutionaryAlgorithms/BRKGA.hpp>
#include <OptFrame/Heuristics/LocalSearches/BestImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/FirstImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/HillClimbing.hpp>
#include <OptFrame/Heuristics/LocalSearches/RandomDescentMethod.hpp>
#include <OptFrame/Heuristics/SA/BasicSimulatedAnnealing.hpp>
#include <OptFrame/Util/CheckCommand.hpp>
#include <OptFrame/Util/RandGenMersenneTwister.hpp>

#include "KP.h"
using namespace std;
using namespace optframe;
using namespace KP;

int
main(int argc, char** argv)
{
   ////////////////////////////////////////////////////////////////////////////
   // Problem Context
   ////////////////////////////////////////////////////////////////////////////

   sref<RandGen> rg = RandGenMersenneTwister(); // declara um bom gerador de números aleatórios

   Scanner scanner(File("instance-4.txt")); // carrega arquivo no disco
   ProblemInstance p(scanner);              // cria um problema-teste usando o arquivo carregado
   MyEvaluator ev(p);                       // cria o avaliador baseado no problema-teste
   StopCriteria<EvaluationKP> sosc;

   ConstructiveRandom c1(p, ev); // construtor de vizinhanças aleatórias
   // dada uma vizinhança aleatória, faz uma busca local do tipo bitflip
   sref<NSSeq<ESolutionKP>> ns1 = new NSSeqBitFlip(p, rg);

   ////////////////////////////////////////////////////////////////////////////
   // Random Solution
   ////////////////////////////////////////////////////////////////////////////
   cout << "\nWill generate random solution" << endl;

   //SolutionKP s = *c1.generateSolution(10); // timelimit (10???)
   op<ESolutionKP> opse = c1.initialSearch(StopCriteria<EvaluationKP>(10)).first; // timelimit (10???)
   ESolutionKP& se = *opse;
   std::cout << "solution: " << opse->first << std::endl;
   ev.reevaluate(se);
   se.second.print();
   cout << "GUD\n\n"; // qq é isso Igor?

   // CheckCommand not working anymore... must CLEAN it!! not accepting R, ADS, ... simplify it, please!

   /*
   CheckCommand<RepKP, MY_ADS, SolutionKP> check; // cria o módulo de testes (opcional)
   check.add(ev1);             // carrega a avaliação para testes
   check.add(c1);             // carrega o construtivo para testes
   check.add(ns1);            // carrega a vizinhança para testes
   check.run(100, 10);        // executa testes com 10 iterações
   */

   //BasicSimulatedAnnealing<ESolutionKP> sa(ev, c1, *nsseq_bit, 0.98, 100, 900.0, rg);
   /*
   std::function<bool(const op<ESolutionKP>&, BasicSimulatedAnnealing<ESolutionKP, EvaluationKP, Component>*)> specificStopBy = 
      [](const op<ESolutionKP>& se, BasicSimulatedAnnealing<ESolutionKP, EvaluationKP, Component>* m) -> bool {
         return ((m->getT() > 0.001) && (m->getTimer().now() < 120)); // 120 seconds and freezing 0.001
      };

   auto soscSA { StopCriteria(specificStopBy) };
*/

   /*
   SpecificMethodStop<ESolutionKP, EvaluationKP, BasicSimulatedAnnealing<ESolutionKP>> spc {
      [&](const ESolutionKP& best, const StopCriteria<EvaluationKP>& sosc, BasicSimulatedAnnealing<ESolutionKP>* me) -> bool {
         return ((me->getT() > 0.001) && (me->getTimer().now() < 120)); // 120 seconds and freezing 0.001
      }
   };
*/

   ////////////////////////////////////////////////////////////////////////////
   // Simulated Annealing
   ////////////////////////////////////////////////////////////////////////////

   sref<GeneralEvaluator<ESolutionKP>> _evaluator = ev;
   sref<InitialSearch<ESolutionKP>> _constructive = c1;
   sref<NS<ESolutionKP>> _ns1 = new NSSeqBitFlip(p, rg);
   sref<NS<ESolutionKP>> _neighbors = _ns1;
   double _alpha = 0.98;
   int _SAmax = 100;
   double _Ti = 900.0;
   BasicSimulatedAnnealing<ESolutionKP> sa(_evaluator, _constructive, _neighbors, _alpha, _SAmax, _Ti, rg);

   /*
   sa.specificStopBy = 
     SpecificMethodStop<ESolutionKP, EvaluationKP, BasicSimulatedAnnealing<ESolutionKP>>(
      [&](const ESolutionKP& best, const StopCriteria<EvaluationKP>& sosc, BasicSimulatedAnnealing<ESolutionKP>* me) -> bool {
         return ((me->getT() > 0.001) && (me->getTimer().now() < 120)); // 120 seconds and freezing 0.001
      }
     );
   */

   sa.onLoopCtx = [](auto& ctx, auto& sosc) {
      return (ctx.T >= 0.001) && !sosc.shouldStop(ctx.best->second);
   };

   SearchOutput<ESolutionKP> sout = sa.search(sosc); // Faz a busca, de fato
   op<ESolutionKP> r = *sout.best;

   //r->first.print();
   std::cout << r->first << std::endl;
   r->second.print();

   ////////////////////////////////////////////////////////////////////////////
   // Hill Climbing
   ////////////////////////////////////////////////////////////////////////////
   std::cout << "\nHill Climbing Search:";

   BestImprovement<ESolutionKP> bi(ev, ns1);
   FirstImprovement<ESolutionKP> fi(ev, ns1);
   HillClimbing<ESolutionKP> sd(ev, bi);
   HillClimbing<ESolutionKP> pm(ev, fi);
   RandomDescentMethod<ESolutionKP> rdm(ev, ns1, 10);
   //
   //pair<SolutionKP, Evaluation<>> se(s, e);
   std::cout << "Best Improvement Search: ";
   sd.lsearch(se, sosc).second.print(); // executa e imprime HC + BI
   std::cout << "First Improvement Search: ";
   pm.lsearch(se, sosc).second.print(); // executa e imprime HC + FI
   std::cout << "Random Descent Search: ";
   rdm.lsearch(se, sosc).second.print(); // executa e imprime RDM com 10 iterações

   // EvaluatorSubsetRandomKeys<EvaluationKP, double, ESolutionKP> eprk(ev1, 0, p.N - 1);
   //EvaluatorSubsetRandomKeys<SolutionKP> eprk(ev1, 0, p.N - 1);
   //Evaluator<std::vector<double>, EvaluationKP, ESolutionKP>& _eprk = eprk;
   //sref<DecoderRandomKeys<SolutionKP, EvaluationKP, double>> _decoder =
   //   *new DecoderRandomKeysEvaluator<SolutionKP, EvaluationKP, double, ESolutionKP>(_eprk);

   ////////////////////////////////////////////////////////////////////////////
   // BRKGA
   ////////////////////////////////////////////////////////////////////////////
   std::cout << "\nBRKGA Search";

   sref<DecoderRandomKeys<ESolutionKP, double>> _decoder =
     new EvaluatorSubsetRandomKeys<EvaluationKP, double, ESolutionKP>(ev, 0, p.N - 1);

   sref<InitialPopulation<std::pair<std::vector<double>, Evaluation<>>>> _genKeys =
     new RandomKeysInitPop<EvaluationKP, double>(p.N, rg);

   BRKGA<ESolutionKP, double> brkga(_decoder, _genKeys, 1000, 30, 0.4, 0.3, 0.6, rg);

   //pair<CopySolution<random_keys>, Evaluation<>>* r2 = brkga.search(sosc);
   SearchOutput<ESolutionKP> sout2 = brkga.search(sosc);

   std::optional<pair<SolutionKP, EvaluationKP>> r2 = sout2.best;
   std::cout << r2->first << std::endl;
   r2->second.print();

   cout << "Program ended successfully" << endl;
   return 0;
}
