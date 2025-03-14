// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef CHECKMODULE_HPP_
#define CHECKMODULE_HPP_

#include "../Command.hpp"
#include "../Timer.hpp"

#include "SystemRequireCommand.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentCheckCommand : public Command<R, ADS, DS>
{
private:
   bool verbose;

public:
   virtual ~ComponentCheckCommand()
   {
   }

   string id()
   {
      return "component.check";
   }

   string usage()
   {
      return "component.check iterMax nSolNSSeq verbose=false [ OptFrame:Constructive[] OptFrame:Evaluator[] OptFrame:Move[] OptFrame:NS[] OptFrame:NS:NSSeq[] OptFrame:NS:NSSeq:NSEnum[] OptFrame:ADSManager[] ]";
   }

   void message(string component, int iter, string text)
   {
      if (verbose)
         std::cout << "module " << id() << " iter: " << iter << " testing component '" << component << "' => " << text << std::endl;
   }

   void error(string text)
   {
      std::cout << "module " << id() << " error: " << text << std::endl;
   }

   bool parseBool(string b)
   {
      return b == "true";
   }

   bool run(std::vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<std::string, std::string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      std::cout << "check: " << input << std::endl;
      Scanner scanner(input);

      // -------------------
      //     iterMax
      // -------------------

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      int iterMax;
      string sIterMax = scanner.next();
      try {
         iterMax = Scanner::parseInt(sIterMax);
      } catch (ConversionError& e) {
         std::cout << "component.check module error: no such integer 'iterMax' (number of solutions to be build and moves to be tested per solution)" << std::endl;
         return false;
      }

      // -----------------------------------------
      //  number of solutions to apply NSSeq tests
      // -----------------------------------------

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }
      int nSolNSSeq;
      string sNSolNSSeq = scanner.next();
      try {
         nSolNSSeq = Scanner::parseInt(sNSolNSSeq);
      } catch (ConversionError& e) {
         std::cout << "component.check module error: no such integer 'number of solutions to test NSSeq exploration'" << std::endl;
         return false;
      }

      // -------------------
      //     verbose
      // -------------------

      if (!scanner.hasNext())
         verbose = false;
      else {
         string sverbose = scanner.next();
         verbose = parseBool(sverbose);
      }

      // optional parameters taken automatically from the system
      if (!scanner.hasNext()) {
         // check dependency on 'command.rename' module
         if (!Command<R, ADS, DS>::run_module("system.require", allCommands, allFunctions, factory, dictionary, ldictionary, "component.list")) {
            std::cout << "error: system.use module depends on 'component.list' module, which is not loaded!" << std::endl;
            return false;
         }

         Command<R, ADS, DS>::undefine("_aux_check_lconstructive", dictionary, ldictionary);
         if (!Command<R, ADS, DS>::run_module("component.list", allCommands, allFunctions, factory, dictionary, ldictionary, "OptFrame:Constructive _aux_check_lconstructive")) {
            std::cout << "error: reading list of OptFrame:Constructive!" << std::endl;
            return false;
         }

         Command<R, ADS, DS>::undefine("_aux_check_levaluator", dictionary, ldictionary);
         if (!Command<R, ADS, DS>::run_module("component.list", allCommands, allFunctions, factory, dictionary, ldictionary, "OptFrame:Evaluator _aux_check_levaluator")) {
            std::cout << "error: reading list of OptFrame:Evaluator!" << std::endl;
            return false;
         }

         Command<R, ADS, DS>::undefine("_aux_check_lmove", dictionary, ldictionary);
         if (!Command<R, ADS, DS>::run_module("component.list", allCommands, allFunctions, factory, dictionary, ldictionary, "OptFrame:Move _aux_check_lmove")) {
            std::cout << "error: reading list of OptFrame:Move!" << std::endl;
            return false;
         }

         Command<R, ADS, DS>::undefine("_aux_check_lns", dictionary, ldictionary);
         if (!Command<R, ADS, DS>::run_module("component.list", allCommands, allFunctions, factory, dictionary, ldictionary, "OptFrame:NS _aux_check_lns")) {
            std::cout << "error: reading list of OptFrame:NS!" << std::endl;
            return false;
         }

         Command<R, ADS, DS>::undefine("_aux_check_lnsseq", dictionary, ldictionary);
         if (!Command<R, ADS, DS>::run_module("component.list", allCommands, allFunctions, factory, dictionary, ldictionary, "OptFrame:NS:NSSeq _aux_check_lnsseq")) {
            std::cout << "error: reading list of OptFrame:NS:NSSeq!" << std::endl;
            return false;
         }

         Command<R, ADS, DS>::undefine("_aux_check_lnsenum", dictionary, ldictionary);
         if (!Command<R, ADS, DS>::run_module("component.list", allCommands, allFunctions, factory, dictionary, ldictionary, "OptFrame:NS:NSSeq:NSEnum _aux_check_lnsenum")) {
            std::cout << "error: reading list of OptFrame:NS:NSSeq:NSEnum!" << std::endl;
            return false;
         }

         Command<R, ADS, DS>::undefine("_aux_check_ladsmanager", dictionary, ldictionary);
         if (!Command<R, ADS, DS>::run_module("component.list", allCommands, allFunctions, factory, dictionary, ldictionary, "OptFrame:ADSManager _aux_check_ladsmanager")) {
            std::cout << "error: reading list of OptFrame:ADSManager!" << std::endl;
            return false;
         }

         scanner = Scanner("_aux_check_lconstructive  _aux_check_levaluator  _aux_check_lmove  _aux_check_lns  _aux_check_lnsseq  _aux_check_lnsenum   _aux_check_ladsmanager");
      }

      //string rest = scanner.rest();
      //cout << "REST: '" << rest << "'" << std::endl;
      //scanner = Scanner(rest);

      // -------------------
      //    Constructive
      // -------------------

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }
      vector<string> lConstructive;
      vector<string>* p_lConstructive = OptFrameList::readList(ldictionary, scanner);
      if (p_lConstructive) {
         lConstructive = vector<string>(*p_lConstructive);
         delete p_lConstructive;
      } else {
         std::cout << "module " << id() << " error: couldn't read list of OptFrame:Constructive!" << std::endl;
         return false;
      }

      Constructive<S>* exConstructive = nullptr;
      vector<Constructive<S>*> lConstructiveComp = assignVector(lConstructive, exConstructive, factory);

      // -------------------
      //     Evaluator
      // -------------------

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }
      vector<string> lEvaluator;
      vector<string>* p_lEvaluator = OptFrameList::readList(ldictionary, scanner);
      if (p_lEvaluator) {
         lEvaluator = vector<string>(*p_lEvaluator);
         delete p_lEvaluator;
      } else {
         std::cout << "module " << id() << " error: couldn't read list of OptFrame:Evaluator!" << std::endl;
         return false;
      }

      Evaluator<R, ADS, DS>* exEvaluator = nullptr;
      vector<Evaluator<R, ADS, DS>*> lEvaluatorComp = assignVector(lEvaluator, exEvaluator, factory);

      // -------------------
      //        Move
      // -------------------

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }
      vector<string> lMove;
      vector<string>* p_lMove = OptFrameList::readList(ldictionary, scanner);
      if (p_lMove) {
         lMove = vector<string>(*p_lMove);
         delete p_lMove;
      } else {
         std::cout << "module " << id() << " error: couldn't read list of OptFrame:Move!" << std::endl;
         return false;
      }

      Move<R, ADS, DS>* exMove = nullptr;
      vector<Move<R, ADS, DS>*> lMoveComp = assignVector(lMove, exMove, factory);

      // -------------------
      //        NS
      // -------------------

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }
      vector<string> lNS;
      vector<string>* p_lNS = OptFrameList::readList(ldictionary, scanner);
      if (p_lNS) {
         lNS = vector<string>(*p_lNS);
         delete p_lNS;
      } else {
         std::cout << "module " << id() << " error: couldn't read list of OptFrame:NS!" << std::endl;
         return false;
      }

      NS<R, ADS, DS>* exNS = nullptr;
      vector<NS<R, ADS, DS>*> lNSComp = assignVector(lNS, exNS, factory);

      // -------------------
      //     NSSeq
      // -------------------

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }
      vector<string> lNSSeq;
      vector<string>* p_lNSSeq = OptFrameList::readList(ldictionary, scanner);
      if (p_lNSSeq) {
         lNSSeq = vector<string>(*p_lNSSeq);
         delete p_lNSSeq;
      } else {
         std::cout << "module " << id() << " error: couldn't read list of OptFrame:NS:NSSeq!" << std::endl;
         return false;
      }

      NSSeq<R, ADS, DS>* exNSSeq = nullptr;
      vector<NSSeq<R, ADS, DS>*> lNSSeqComp = assignVector(lNSSeq, exNSSeq, factory);

      // -------------------
      //     NSEnum
      // -------------------

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }
      vector<string> lNSEnum;
      vector<string>* p_lNSEnum = OptFrameList::readList(ldictionary, scanner);
      if (p_lNSEnum) {
         lNSEnum = vector<string>(*p_lNSEnum);
         delete p_lNSEnum;
      } else {
         std::cout << "module " << id() << " error: couldn't read list of OptFrame:NS:NSSeq:NSEnum!" << std::endl;
         return false;
      }

      NSEnum<R, ADS, DS>* exNSEnum = nullptr;
      vector<NSEnum<R, ADS, DS>*> lNSEnumComp = assignVector(lNSEnum, exNSEnum, factory);

      // -------------------
      //     ADSManager
      // -------------------

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }
      vector<string> lADSManager;
      vector<string>* p_lADSManager = OptFrameList::readList(ldictionary, scanner);

      if (p_lADSManager) {
         lADSManager = vector<string>(*p_lADSManager);
         delete p_lADSManager;
      } else {
         std::cout << "module " << id() << " error: couldn't read list of OptFrame:ADSManager!" << std::endl;
         return false;
      }

      ADSManager<R, ADS, S>* exADSManager = nullptr;
      vector<ADSManager<R, ADS, S>*> lADSManagerComp = assignVector(lADSManager, exADSManager, factory);

      // cleanup auxiliar list definitions
      Command<R, ADS, DS>::undefine("_aux_check_lconstructive", dictionary, ldictionary);
      Command<R, ADS, DS>::undefine("_aux_check_levaluator", dictionary, ldictionary);
      Command<R, ADS, DS>::undefine("_aux_check_lmove", dictionary, ldictionary);
      Command<R, ADS, DS>::undefine("_aux_check_lns", dictionary, ldictionary);
      Command<R, ADS, DS>::undefine("_aux_check_lnsseq", dictionary, ldictionary);
      Command<R, ADS, DS>::undefine("_aux_check_lnsenum", dictionary, ldictionary);
      Command<R, ADS, DS>::undefine("_aux_check_ladsmanager", dictionary, ldictionary);

      // ======================================
      //           BEGIN TESTS
      // ======================================

      // time to clone a solution
      pair<int, double> timeCloneSolution(0, 0.0);

      ADSManager<R, ADS, S>* adsMan = nullptr;
      if (lADSManager.size() > 0) {
         Scanner scan(lADSManager.at(0));
         factory.assign(adsMan, scan.nextInt(), scan.next()); // reversed!

         if (lADSManager.size() > 1)
            std::cout << id() << " module warning: more than 1 ADSManager (" << lADSManager.size() << ")" << std::endl;
      }

      std::vector<std::pair<int, double>> timeInitializeADS(1, make_pair(0, 0.0));

      // ----------------
      // read evaluators
      // ----------------

      vector<Evaluator<R, ADS, DS>*> evaluators;
      for (unsigned ev = 0; ev < lEvaluator.size(); ev++) {
         Scanner scan(lEvaluator.at(ev));
         Evaluator<R, ADS, DS>* evaluator;
         factory.assign(evaluator, scan.nextInt(), scan.next()); // reversed!

         if (!evaluator) {
            std::cout << "module " << id() << " error: nullptr evaluator!" << std::endl;
            return false;
         }

         evaluators.push_back(evaluator);
      }

      std::vector<std::pair<int, double>> fullTimeEval(evaluators.size(), make_pair(0, 0.0));
      std::vector<std::pair<int, double>> timeReeval(evaluators.size(), make_pair(0, 0.0));

      // ----------------------------------------------------------------------------------------
      // generate 'iterMax' OptFrame:Solution for each OptFrame:Constructive and store evaluation
      // ----------------------------------------------------------------------------------------

      vector<Solution<R, ADS>*> solutions;
      vector<vector<Evaluation<DS>*>> evaluations(evaluators.size());

      std::vector<std::pair<int, double>> timeConstructive(lConstructive.size(), make_pair(0, 0.0));

      std::cout << "module " << id() << " will test constructive components (iterMax=" << iterMax << ")" << std::endl;
      for (unsigned c = 0; c < lConstructive.size(); c++) {
         Scanner scan(lConstructive.at(c));
         Constructive<S>* constructive;
         factory.assign(constructive, scan.nextInt(), scan.next()); // reversed!

         if (!constructive) {
            std::cout << "module " << id() << " error: nullptr constructive!" << std::endl;
            return false;
         }

         std::cout << "component.check command: testing " << constructive->toString();
         std::cout << std::endl;

         for (int iter = 1; iter <= iterMax; iter++) {
            message(lConstructive.at(c), iter, "generating solution.");

            Timer ts;
            Solution<R, ADS>& s = constructive->generateSolution();
            timeConstructive[c].second += ts.inMilliSecs();
            timeConstructive[c].first++;

            if (adsMan) {
               Timer ts2;
               adsMan->initializeADS(s.getR(), s.getADS());
               timeInitializeADS[0].second += ts2.inMilliSecs();
               timeInitializeADS[0].first++;
            }

            solutions.push_back(&s);

            for (unsigned ev = 0; ev < evaluators.size(); ev++) {
               message(lEvaluator.at(ev), iter, "evaluating solution.");
               Timer te;
               Evaluation<DS>& e = evaluators.at(ev)->evaluate(s);
               fullTimeEval[ev].second += te.inMilliSecs();
               fullTimeEval[ev].first++;

               evaluations.at(ev).push_back(&e);
            }
         }
         if(logLevel >= LogLevel::Info)
            std::cout << "component.check: " << lConstructive.at(c) << " finished." << std::endl;
         if (verbose)
            std::cout << std::endl << std::endl;
      }

      if (verbose)
         std::cout << std::endl
              << std::endl;

      // ====================================================================
      // testing Move
      // ====================================================================

      std::cout << "module " << id() << " will test given Move components (|Move|=" << lMove.size() << "; numSolutions=" << solutions.size() << ")";
      std::cout << std::endl;

      for (unsigned id_move = 0; id_move < lMove.size(); id_move++) {
         Scanner scan(lMove.at(id_move));
         Move<R, ADS, DS>* pmove;
         factory.assign(pmove, scan.nextInt(), scan.next()); // reversed!

         if (!pmove) {
            std::cout << "module " << id() << " error: nullptr OptFrame:Move!" << std::endl;
            return false;
         }

         std::cout << "component.check command: testing " << pmove->toString();
         std::cout << std::endl;

         for (unsigned id_s = 0; id_s < solutions.size(); id_s++) {
            if (verbose)
               std::cout << std::endl;
            message(lMove.at(id_move), -1, "working on move.");

            Solution<R, ADS>& s = *solutions.at(id_s);

            Move<R, ADS, DS>& move = *pmove;

            if (!move.canBeApplied(s)) {
               if (verbose) {
                  std::cout << "move cannot be applied: ";
                  move.print();
               }
               continue;
            }

            for (unsigned ev = 0; ev < evaluators.size(); ev++) {
               message(lEvaluator.at(ev), -1, "evaluating move (apply, revert and moveCost).");

               string moveFrom = "Move ";
               moveFrom.append(move.id());

               if (verbose)
                  move.print();

               message(moveFrom, -1, "testing reverse.");

               Move<R, ADS, DS>& rev = move.apply(s);

               Timer t_clone;
               Solution<R, ADS>& sNeighbor = s.clone(); // remove if not verbose
               timeCloneSolution.second += t_clone.inMilliSecs();
               timeCloneSolution.first++;

               // ===================== tests with ADSManager ======================

               if (adsMan) {
                  message(lEvaluator.at(ev), -1, "testing ADS.");

                  ADS ads(sNeighbor.getADS()); // copy
                  Timer ts_ds;
                  adsMan->initializeADS(sNeighbor.getR(), sNeighbor.getADS());
                  timeInitializeADS[0].second += ts_ds.inMilliSecs();
                  timeInitializeADS[0].first++;

                  if (!adsMan->compareADS(ads, sNeighbor.getADS())) {
                     std::cout << id() << " module error: ADS not updated correctly! Compared brand new initializeADS with update from move => ";
                     move.print();
                     return false;
                  }
               }

               Evaluation<DS>& e_rev = evaluators.at(ev)->evaluate(s);

               Move<R, ADS, DS>& ini = rev.apply(s);

               // ===================== tests with ADSManager ======================

               if (adsMan) {
                  message(lEvaluator.at(ev), -1, "testing ADS.");

                  ADS ads(s.getADS()); // copy
                  Timer ts_ds2;
                  adsMan->initializeADS(s.getR(), s.getADS());
                  timeInitializeADS[0].second += ts_ds2.inMilliSecs();
                  timeInitializeADS[0].first++;

                  if (!adsMan->compareADS(ads, s.getADS())) {
                     std::cout << id() << " module error: ADS not updated correctly! Compared brand new initializeADS with update from reverse move => ";
                     rev.print();
                     return false;
                  }
               }

               Evaluation<DS>& e_ini = evaluators.at(ev)->evaluate(s);

               if (ini != move) {
                  error("reverse of reverse is not the original move!");
                  move.print();
                  std::cout << "move: ";
                  move.print();
                  std::cout << "rev: ";
                  rev.print();
                  std::cout << "ini (reverse of rev): ";
                  ini.print();

                  return false;
               }

               message(lEvaluator.at(ev), -1, "testing reverse value.");
               Evaluation<DS>& e = *evaluations.at(ev).at(id_s);

               if (abs(e_ini.evaluation() - e.evaluation()) > 0.0001) {
                  error("reverse of reverse has a different evaluation value!");
                  move.print();
                  std::cout << "move: ";
                  move.print();
                  std::cout << "original: ";
                  e.print();
                  std::cout << "reverse of reverse:";
                  e_ini.print();

                  return false;
               }

               // =============================================================

               message(lEvaluator.at(ev), -1, "testing move cost.");

               double revCost = e_rev.evaluation() - e.evaluation();

               MoveCost<>& mcSimpleCost = evaluators[ev]->moveCost(move, s);
               double simpleCost = mcSimpleCost.cost();
               delete &mcSimpleCost;

               if (abs(revCost - simpleCost) > 0.0001) {
                  error("difference between revCost and simpleCost");
                  move.print();
                  printf("revCost = %.4f\n", revCost);
                  printf("simpleCost = %.4f\n", simpleCost);
                  return false;
               }

               // fasterCost
               Move<R, ADS, DS>& rev1 = evaluators[ev]->applyMove(e, move, s);
               double e_end1 = e.evaluation();
               Move<R, ADS, DS>& ini1 = evaluators[ev]->applyMove(e, rev1, s);
               double e_ini1 = e.evaluation();

               delete &rev1;
               delete &ini1;

               double fasterCost = e_end1 - e_ini1;

               if (abs(revCost - fasterCost) > 0.0001) {
                  error("difference between revCost and fasterCost");
                  move.print();
                  printf("revCost = %.4f\n", revCost);
                  printf("fasterCost = %.4f\n", fasterCost);
                  printf("e = %.4f\n", e.evaluation());
                  printf("e_rev = %.4f\n", e_rev.evaluation());
                  return false;
               }

               MoveCost<>* cost = nullptr;

               if (evaluators[ev]->getAllowCosts())
                  cost = move.cost(e, s.getR(), s.getADS());

               if (cost) {
                  double cValue = cost->getObjFunctionCost() + cost->getInfMeasureCost();
                  if (abs(revCost - cValue) > 0.0001) {
                     error("difference between expected cost and cost()");
                     move.print();
                     printf("expected =\t %.4f\n", revCost);
                     printf("cost() =\t %.4f\n", cValue);
                     printf("==============\n");
                     printf("CORRECT VALUES \n");
                     printf("==============\n");
                     printf("e: \t obj:%.4f \t inf:%.4f \t total:%.4f\n", e.getObjFunction(), e.getInfMeasure(), e.evaluation());
                     printf("e':\t obj:%.4f \t inf:%.4f \t total:%.4f\n", e_rev.getObjFunction(), e_rev.getInfMeasure(), e_rev.evaluation());
                     std::cout << "s: ";
                     s.print();
                     std::cout << "s': ";
                     sNeighbor.print();
                     std::cout << "move: ";
                     move.print();
                     printf("==============\n");
                     printf("  GOOD LUCK!  \n");
                     printf("==============\n");
                     return false;
                  }

                  delete cost;
               }

               message(lEvaluator.at(ev), -1, "all move costs okay!");

               delete &rev;
               delete &sNeighbor;
               delete &e_rev;
               delete &ini;
               delete &e_ini;
            }

            /////delete& move; // NEVER DESTROY THIS OptFrame:Move!
         }
      }

      if (verbose)
         std::cout << std::endl
              << std::endl;

      // ====================================================================
      // testing NS
      // ====================================================================

      std::cout << "module " << id() << " will test NS components (iterMax=" << iterMax << "; numSolutions=" << solutions.size() << ")";
      std::cout << std::endl;

      std::vector<std::pair<int, double>> timeNSApply(lNS.size(), make_pair(0, 0.0));
      std::vector<std::pair<int, double>> timeNSCostApply(lNS.size(), make_pair(0, 0.0));
      std::vector<std::pair<int, double>> timeNSCostApplyDelta(lNS.size(), make_pair(0, 0.0));
      std::vector<std::pair<int, double>> timeNSCost(lNS.size(), make_pair(0, 0.0));
      std::vector<std::pair<int, double>> timeNSEstimatedCost(lNS.size(), make_pair(0, 0.0));
      std::vector<std::pair<int, double>> errorNSEstimatedCost(lNS.size(), make_pair(0, 0.0));

      for (unsigned id_ns = 0; id_ns < lNS.size(); id_ns++) {
         Scanner scan(lNS.at(id_ns));
         NS<R, ADS, DS>* ns;
         factory.assign(ns, scan.nextInt(), scan.next()); // reversed!

         if (!ns) {
            std::cout << "module " << id() << " error: nullptr OptFrame:NS!" << std::endl;
            return false;
         }

         std::cout << "component.check command: testing " << ns->toString();
         std::cout << std::endl;

         for (int iter = 1; iter <= iterMax; iter++) {
            message(lNS.at(id_ns), iter, "starting tests!");

            for (unsigned id_s = 0; id_s < solutions.size(); id_s++) {
               if (verbose)
                  std::cout << std::endl;

               stringstream ss_msg1;
               ss_msg1 << "generating random move for solution id=" << id_s;
               message(lNS.at(id_ns), iter, ss_msg1.str());

               Solution<R, ADS>& s = *solutions.at(id_s);

               Move<R, ADS, DS>& move = ns->move(s);
               if (verbose)
                  move.print();

               if (!move.canBeApplied(s)) {
                  if (verbose) {
                     std::cout << "move cannot be applied: ";
                     move.print();
                  }

                  delete &move;
                  continue;
               }

               for (unsigned ev = 0; ev < evaluators.size(); ev++) {
                  message(lEvaluator.at(ev), iter, "evaluating random move (apply, revert and moveCost).");

                  string moveFrom = "Move ";
                  moveFrom.append(move.id());
                  moveFrom.append(" from: ");
                  moveFrom.append(ns->id());
                  moveFrom.append(" toString: ");
                  moveFrom.append(ns->toString());

                  if (verbose)
                     move.print();

                  message(moveFrom, iter, "testing reverse.");

                  Timer t_clone;
                  Solution<R, ADS>& sOriginal = s.clone(); // remove if not verbose
                  timeCloneSolution.second += t_clone.inMilliSecs();
                  timeCloneSolution.first++;

                  Timer tMovApply;
                  Move<R, ADS, DS>& rev = move.apply(s);
                  timeNSApply[id_ns].second += tMovApply.inMilliSecs();
                  timeNSApply[id_ns].first++;

                  Timer t_clone2;
                  Solution<R, ADS>& sNeighbor = s.clone(); // remove if not verbose
                  timeCloneSolution.second += t_clone2.inMilliSecs();
                  timeCloneSolution.first++;

                  // ===================== tests with ADSManager ======================

                  if (adsMan) {
                     message(lEvaluator.at(ev), -1, "testing ADS.");

                     ADS ads(sNeighbor.getADS()); // copy
                     Timer ts_ds;
                     adsMan->initializeADS(sNeighbor.getR(), sNeighbor.getADS());
                     timeInitializeADS[0].second += ts_ds.inMilliSecs();
                     timeInitializeADS[0].first++;

                     if (!adsMan->compareADS(ads, sNeighbor.getADS())) {
                        std::cout << id() << " module error: ADS not updated correctly! Compared brand new initializeADS with update from move => ";
                        move.print();
                        std::cout << "S (sOriginal.getADS()): " << std::endl;
                        adsMan->printADS(sOriginal.getADS());
                        std::cout << "WRONG: (sNeighbor.getADS())" << std::endl;
                        adsMan->printADS(ads);
                        std::cout << "RIGHT (re-initialized): " << std::endl;
                        adsMan->printADS(sNeighbor.getADS());
                        return false;
                     }
                  }

                  Timer te;
                  Evaluation<DS>& e_rev = evaluators.at(ev)->evaluate(s);
                  fullTimeEval[ev].second += te.inMilliSecs();
                  fullTimeEval[ev].first++;

                  Timer tMovRevApply;
                  Move<R, ADS, DS>& ini = rev.apply(s);
                  timeNSApply[id_ns].second += tMovRevApply.inMilliSecs();
                  timeNSApply[id_ns].first++;

                  // ===================== tests with ADSManager ======================

                  if (adsMan) {
                     message(lEvaluator.at(ev), -1, "testing ADS.");

                     ADS ads(s.getADS()); // copy
                     Timer ts_ds2;
                     adsMan->initializeADS(s.getR(), s.getADS());
                     timeInitializeADS[0].second += ts_ds2.inMilliSecs();
                     timeInitializeADS[0].first++;

                     if (!adsMan->compareADS(ads, s.getADS())) {
                        std::cout << id() << " module error: ADS not updated correctly! Compared brand new initializeADS with update from reverse move => ";
                        rev.print();
                        std::cout << "S (sOriginal.getADS()): " << std::endl;
                        adsMan->printADS(sOriginal.getADS());
                        std::cout << "WRONG (s.getADS()): " << std::endl;
                        adsMan->printADS(ads);
                        std::cout << "RIGHT (re-initialized): " << std::endl;
                        adsMan->printADS(s.getADS());
                        return false;
                     }
                  }

                  delete &sOriginal;

                  Timer te2;
                  Evaluation<DS>& e_ini = evaluators.at(ev)->evaluate(s);
                  fullTimeEval[ev].second += te2.inMilliSecs();
                  fullTimeEval[ev].first++;

                  if (ini != move) {
                     error("reverse of reverse is not the original move!");
                     move.print();
                     std::cout << "move: ";
                     move.print();
                     std::cout << "rev: ";
                     rev.print();
                     std::cout << "ini (reverse of rev): ";
                     ini.print();

                     return false;
                  }

                  message(lEvaluator.at(ev), iter, "testing reverse value.");
                  Evaluation<DS>& e = *evaluations.at(ev).at(id_s);

                  if (abs(e_ini.evaluation() - e.evaluation()) > 0.0001) {
                     error("reverse of reverse has a different evaluation value!");
                     move.print();
                     std::cout << "move: ";
                     move.print();
                     std::cout << "original: ";
                     e.print();
                     std::cout << "reverse of reverse:";
                     e_ini.print();

                     return false;
                  }

                  // =============================================================

                  message(lEvaluator.at(ev), iter, "testing move cost.");

                  double revCost = e_rev.evaluation() - e.evaluation();
                  message(lEvaluator.at(ev), iter, "revCost calculated!");

                  Timer tMoveCostApply;
                  MoveCost<>& mcSimpleCost = evaluators[ev]->moveCost(move, s);
                  double simpleCost = mcSimpleCost.cost();
                  delete &mcSimpleCost;
                  message(lEvaluator.at(ev), iter, "simpleCost calculated!");
                  timeNSCostApply[id_ns].second += tMoveCostApply.inMilliSecs();
                  timeNSCostApply[id_ns].first++;

                  if (abs(revCost - simpleCost) > 0.0001) {
                     error("difference between revCost and simpleCost");
                     move.print();
                     printf("revCost = %.4f\n", revCost);
                     printf("simpleCost = %.4f\n", simpleCost);
                     return false;
                  }

                  // fasterCost
                  Timer tMoveCostApplyDelta;
                  Move<R, ADS, DS>& rev1 = evaluators[ev]->applyMove(e, move, s);
                  double e_end1 = e.evaluation();
                  Move<R, ADS, DS>& ini1 = evaluators[ev]->applyMove(e, rev1, s);
                  double e_ini1 = e.evaluation();
                  timeNSCostApplyDelta[id_ns].second += tMoveCostApplyDelta.inMilliSecs();
                  timeNSCostApplyDelta[id_ns].first++;

                  delete &rev1;
                  delete &ini1;

                  double fasterCost = e_end1 - e_ini1;
                  message(lEvaluator.at(ev), iter, "fasterCost calculated!");

                  if (abs(revCost - fasterCost) > 0.0001) {
                     error("difference between revCost and fasterCost");
                     move.print();
                     printf("revCost = %.4f\n", revCost);
                     printf("fasterCost = %.4f\n", fasterCost);
                     printf("e = %.4f\n", e.evaluation());
                     printf("e_rev = %.4f\n", e_rev.evaluation());
                     return false;
                  }

                  Timer tMoveCost;
                  MoveCost<>* cost = nullptr;

                  if (evaluators[ev]->getAllowCosts())
                     cost = move.cost(e, s.getR(), s.getADS());

                  message(lEvaluator.at(ev), iter, "cost() calculated!");

                  if (cost && !cost->isEstimated()) {
                     timeNSCost[id_ns].second += tMoveCost.inMilliSecs();
                     timeNSCost[id_ns].first++;
                  }

                  if (cost && cost->isEstimated()) {
                     timeNSEstimatedCost[id_ns].second += tMoveCost.inMilliSecs();
                     timeNSEstimatedCost[id_ns].first++;
                  }

                  if (cost && !cost->isEstimated()) {
                     double cValue = cost->cost();
                     if (abs(revCost - cValue) > 0.0001) {
                        error("difference between expected cost and cost()");
                        move.print();
                        printf("expected =\t %.4f\n", revCost);
                        printf("cost() =\t %.4f\n", cValue);
                        printf("==============\n");
                        printf("CORRECT VALUES \n");
                        printf("==============\n");
                        printf("e: \t obj:%.4f \t inf:%.4f \t total:%.4f\n", e.getObjFunction(), e.getInfMeasure(), e.evaluation());
                        printf("e':\t obj:%.4f \t inf:%.4f \t total:%.4f\n", e_rev.getObjFunction(), e_rev.getInfMeasure(), e_rev.evaluation());
                        std::cout << "s: ";
                        s.print();
                        std::cout << "s': ";
                        sNeighbor.print();
                        std::cout << "move: ";
                        move.print();
                        printf("==============\n");
                        printf("  GOOD LUCK!  \n");
                        printf("==============\n");
                        return false;
                     }

                     // testing double move costs! (for MoveCost betterThan)

                     Move<R, ADS, DS>& move2 = ns->move(s);
                     if (verbose) {
                        std::cout << "testing double move!" << std::endl;
                        move2.print();
                     }

                     if (!move2.canBeApplied(s)) {
                        if (verbose) {
                           std::cout << "double move cannot be applied: ";
                           move2.print();
                        }
                     } else {
                        MoveCost<>* cost2 = nullptr;
                        if (evaluators[ev]->getAllowCosts()) {
                           cost2 = move2.cost(e, s.getR(), s.getADS());
                           if (cost2) {
                              evaluators[ev]->betterThan(*cost, *cost2);
                              delete cost2;
                           }
                        }
                     }

                     delete &move2;

                     // finish double cost test

                     delete cost;
                  }

                  message(lEvaluator.at(ev), iter, "all move costs okay!");

                  delete &rev;
                  delete &sNeighbor;
                  delete &e_rev;
                  delete &ini;
                  delete &e_ini;
               }

               delete &move;
            }
         }
         if(logLevel >= LogLevel::Info)
            std::cout << "component.check: " << lNS.at(id_ns) << " finished." << std::endl;
         if (verbose)
            std::cout << std::endl << std::endl;
      }

      // ====================================================================
      // testing NSSeq
      // ====================================================================

      std::cout << "module " << id() << " will test NSSeq components (nSolNSSeq=" << nSolNSSeq << " of numSolutions=" << solutions.size() << ")";
      std::cout << std::endl;

      vector<int> vCountMoves(lNSSeq.size());
      vector<int> vCountValidMoves(lNSSeq.size());

      for (unsigned id_nsseq = 0; id_nsseq < lNSSeq.size(); id_nsseq++) {
         Scanner scan(lNSSeq.at(id_nsseq));
         NSSeq<R, ADS, DS>* nsseq;
         factory.assign(nsseq, scan.nextInt(), scan.next()); // reversed!

         if (!nsseq) {
            std::cout << "module " << id() << " error: nullptr OptFrame:NS:NSSeq!" << std::endl;
            return false;
         }

         std::cout << "component.check command: testing " << nsseq->toString();
         std::cout << std::endl;

         int countMoves = 0;
         int countValidMoves = 0;

         for (int nqs = 1; nqs <= nSolNSSeq; nqs++) {
            message(lNSSeq.at(id_nsseq), nqs, "starting tests!");

            int randomIndex = rand() % solutions.size();
            Solution<R, ADS>& s = *solutions.at(randomIndex);

            // ===================
            // apply a single move
            // ===================
            /*

				bool moveApplied = false;
				int shaking = rand() % 5 + 1; //Max moves applied in each nSolNSSeq obtained from solutions vector

				for (int i = 0; i < shaking; i++)
				{
					Move<R, ADS, DS>* moveValid = nsseq->validMove(s);

					if (moveValid != nullptr)
					{
						delete &moveValid->apply(s);
						delete moveValid;
						moveApplied = true;
						break;
					}
					else
						delete moveValid;
				}

				if (!moveApplied)
					message(lNSSeq.at(id_nsseq), nqs, "Warning. Couldn't apply a move before iterator tests (NSSeq tests).");
				*/

            NSIterator<R, ADS, DS>& it = nsseq->getIterator(s);

            for (it.first(); !it.isDone(); it.next()) {
               if (verbose)
                  std::cout << std::endl;
               message(lNSSeq.at(id_nsseq), nqs, "getting current move (NSSeq tests).");

               Move<R, ADS, DS>& move = it.current();
               countMoves++;

               if (!move.canBeApplied(s)) {
                  if (verbose) {
                     std::cout << "move cannot be applied (NSSeq tests): ";
                     move.print();
                  }
                  continue;
               }

               countValidMoves++;

               for (unsigned ev = 0; ev < evaluators.size(); ev++) {
                  message(lEvaluator.at(ev), nqs, "evaluating random move (apply, revert and moveCost) in NSSeq tests.");

                  Evaluation<DS>& e = evaluators[ev]->evaluate(s);

                  string moveFrom = "Move ";
                  moveFrom.append(move.id());
                  moveFrom.append(" from: ");
                  moveFrom.append(nsseq->id());
                  moveFrom.append(" toString: ");
                  moveFrom.append(nsseq->toString());

                  if (verbose)
                     move.print();

                  message(moveFrom, nqs, "testing reverse.");

                  Timer tMovApply;
                  Move<R, ADS, DS>& rev = move.apply(s);
                  timeNSApply[id_nsseq].second += tMovApply.inMilliSecs();
                  timeNSApply[id_nsseq].first++;

                  Timer t_clone;
                  Solution<R, ADS>& sNeighbor = s.clone(); // remove if not verbose
                  timeCloneSolution.second += t_clone.inMilliSecs();
                  timeCloneSolution.first++;

                  Timer te;
                  Evaluation<DS>& e_rev = evaluators.at(ev)->evaluate(s);
                  fullTimeEval[ev].second += te.inMilliSecs();
                  fullTimeEval[ev].first++;

                  Timer tMovRevApply;
                  Move<R, ADS, DS>& ini = rev.apply(s);
                  timeNSApply[id_nsseq].second += tMovRevApply.inMilliSecs();
                  timeNSApply[id_nsseq].first++;

                  Timer te2;
                  Evaluation<DS>& e_ini = evaluators.at(ev)->evaluate(s);
                  fullTimeEval[ev].second += te2.inMilliSecs();
                  fullTimeEval[ev].first++;

                  if (ini != move) {
                     error("reverse of reverse is not the original move!");
                     move.print();
                     std::cout << "move: ";
                     move.print();
                     std::cout << "rev: ";
                     rev.print();
                     std::cout << "ini (reverse of rev): ";
                     ini.print();

                     return false;
                  }

                  message(lEvaluator.at(ev), nqs, "testing reverse value (NSSeq tests).");

                  if (abs(e_ini.evaluation() - e.evaluation()) > 0.0001) {
                     error("reverse of reverse has a different evaluation value!");
                     move.print();
                     std::cout << "move: ";
                     move.print();
                     std::cout << "original: ";
                     e.print();
                     std::cout << "reverse of reverse:";
                     e_ini.print();

                     return false;
                  }

                  // ===================== tests with ADSManager ======================

                  if (adsMan) {
                     message(lEvaluator.at(ev), -1, "testing ADS (NSSeq tests).");

                     ADS ads(sNeighbor.getADS()); // copy
                     Timer ts_ds;
                     adsMan->initializeADS(sNeighbor.getR(), sNeighbor.getADS());
                     timeInitializeADS[0].second += ts_ds.inMilliSecs();
                     timeInitializeADS[0].first++;

                     if (!adsMan->compareADS(ads, sNeighbor.getADS())) {
                        std::cout << id() << " module error: ADS not updated correctly! Compared brand new initializeADS with update from move => ";
                        move.print();
                        return false;
                     }

                     ads = ADS(s.getADS()); // copy
                     Timer ts_ds2;
                     adsMan->initializeADS(s.getR(), s.getADS());
                     timeInitializeADS[0].second += ts_ds2.inMilliSecs();
                     timeInitializeADS[0].first++;

                     if (!adsMan->compareADS(ads, s.getADS())) {
                        std::cout << id() << " module error: ADS not updated correctly! Compared brand new initializeADS with update from reverse move => ";
                        rev.print();
                        return false;
                     }
                  }

                  // =============================================================

                  message(lEvaluator.at(ev), nqs, "testing move cost (NSSeq tests).");

                  double revCost = e_rev.evaluation() - e.evaluation();

                  Timer tMoveCostApply;
                  MoveCost<>& mcSimpleCost = evaluators[ev]->moveCost(move, s);
                  double simpleCost = mcSimpleCost.cost();
                  delete &mcSimpleCost;
                  timeNSCostApply[id_nsseq].second += tMoveCostApply.inMilliSecs();
                  timeNSCostApply[id_nsseq].first++;

                  if (abs(revCost - simpleCost) > 0.0001) {
                     error("difference between revCost and simpleCost");
                     move.print();
                     printf("revCost = %.4f\n", revCost);
                     printf("simpleCost = %.4f\n", simpleCost);
                     return false;
                  }

                  // fasterCost
                  Timer tMoveCostApplyDelta;
                  Move<R, ADS, DS>& rev1 = evaluators[ev]->applyMove(e, move, s);
                  double e_end1 = e.evaluation();
                  Move<R, ADS, DS>& ini1 = evaluators[ev]->applyMove(e, rev1, s);
                  double e_ini1 = e.evaluation();
                  timeNSCostApplyDelta[id_nsseq].second += tMoveCostApplyDelta.inMilliSecs();
                  timeNSCostApplyDelta[id_nsseq].first++;

                  delete &rev1;
                  delete &ini1;

                  double fasterCost = e_end1 - e_ini1;

                  if (abs(revCost - fasterCost) > 0.0001) {
                     error("difference between revCost and fasterCost");
                     move.print();
                     printf("revCost = %.4f\n", revCost);
                     printf("fasterCost = %.4f\n", fasterCost);
                     printf("e = %.4f\n", e.evaluation());
                     printf("e_rev = %.4f\n", e_rev.evaluation());
                     return false;
                  }

                  Timer tMoveCost;
                  MoveCost<>* cost = nullptr;

                  if (evaluators[ev]->getAllowCosts())
                     cost = move.cost(e, s.getR(), s.getADS());

                  if (cost && !cost->isEstimated()) {
                     timeNSCost[id_nsseq].second += tMoveCost.inMilliSecs();
                     timeNSCost[id_nsseq].first++;
                  }

                  if (cost && cost->isEstimated()) {
                     timeNSEstimatedCost[id_nsseq].second += tMoveCost.inMilliSecs();
                     timeNSEstimatedCost[id_nsseq].first++;
                  }

                  if (cost && !cost->isEstimated()) {
                     double cValue = cost->cost();
                     if (abs(revCost - cValue) > 0.0001) {
                        error("difference between expected cost and cost()");
                        move.print();
                        printf("expected =\t %.4f\n", revCost);
                        printf("cost() =\t %.4f\n", cValue);
                        printf("==============\n");
                        printf("CORRECT VALUES \n");
                        printf("==============\n");
                        printf("e: \t obj:%.4f \t inf:%.4f \t total:%.4f\n", e.getObjFunction(), e.getInfMeasure(), e.evaluation());
                        printf("e':\t obj:%.4f \t inf:%.4f \t total:%.4f\n", e_rev.getObjFunction(), e_rev.getInfMeasure(), e_rev.evaluation());
                        std::cout << "s: ";
                        s.print();
                        std::cout << "s': ";
                        sNeighbor.print();
                        std::cout << "move: ";
                        move.print();
                        printf("==============\n");
                        printf("  GOOD LUCK!  \n");
                        printf("==============\n");
                        return false;
                     }

                     delete cost;
                  }

                  message(lEvaluator.at(ev), nqs, "all move costs okay (NSSeq tests)!");

                  delete &e;

                  delete &rev;
                  delete &sNeighbor;
                  delete &e_rev;
                  delete &ini;
                  delete &e_ini;
               }

               delete &move;
            }

            delete &it;
         }

         vCountMoves[id_nsseq] += countMoves;
         vCountValidMoves[id_nsseq] += countValidMoves;

         if(logLevel >= LogLevel::Info)
            std::cout << "component.check: " << lNSSeq.at(id_nsseq) << " finished." << std::endl;
         if (verbose)
            std::cout << std::endl << std::endl;
      }

      for (unsigned i = 0; i < solutions.size(); i++)
         delete solutions[i];

      for (unsigned i = 0; i < evaluations.size(); i++)
         for (unsigned j = 0; j < evaluations[i].size(); j++)
            delete evaluations[i][j];

      std::cout << "===============================" << std::endl;
      std::cout << "           SUMMARY             " << std::endl;
      std::cout << "===============================" << std::endl
           << std::endl;

      printSingleSummary("Solution", timeCloneSolution, "Time to clone a solution");

      printSummary(convertVector(lConstructiveComp), timeConstructive, "Constructive", "testing construction of initial solution");

      if (adsMan)
         printSummary(convertVector(lADSManagerComp), timeInitializeADS, "ADSManager::initializeADS()", "testing lazy initializeADS in solutions");
      else
         std::cout << std::endl
              << "No ADSManager was tested." << std::endl
              << std::endl;

      printSummary(convertVector(lEvaluatorComp), fullTimeEval, "Evaluators", "testing full evaluate(s) of a solution");

      printSummary(convertVector(lNSComp), timeNSApply, "NS", "testing time of move apply(s)");

      printSummary(convertVector(lNSComp), timeNSCostApply, "NS", "testing time of cost based on move apply(s)");

      printSummary(convertVector(lNSComp), timeNSCostApplyDelta, "NS", "testing time of cost based on move apply(e, s)");

      printSummary(convertVector(lNSComp), timeNSCost, "NS", "testing time of move cost()");

      printSummary(convertVector(lNSComp), timeNSEstimatedCost, "NS", "testing time of move estimatedCost()");

      printSummary(convertVector(lNSComp), errorNSEstimatedCost, "NS", "testing error (%) of move estimatedCost()");

      printSummarySimple(convertVector(lNSSeqComp), vCountMoves, nSolNSSeq, "NSSeq", "counting moves of NSSeq iterator");

      printSummarySimple(convertVector(lNSSeqComp), vCountValidMoves, nSolNSSeq, "NSSeq", "counting valid moves of NSSeq iterator");

      std::cout << "component.check command: tests finished successfully!" << std::endl;
      return true;
   }

   template<class T>
   vector<T*> assignVector(const vector<string> lComponents, T* type, HeuristicFactory<R, ADS, DS>& factory)
   {
      vector<T*> vComp;
      for (unsigned i = 0; i < lComponents.size(); i++) {
         Scanner scan(lComponents.at(i));
         type = nullptr;
         factory.assign(type, scan.nextInt(), scan.next()); // reversed!

         if (!type) {
            std::cout << "module " << id() << " warning: nullptr component " << lComponents[i] << "!" << std::endl;
         } else
            vComp.push_back(type);
      }

      return vComp;
   }

   template<class T>
   vector<Component*> convertVector(const vector<T*>& v)
   {
      vector<Component*> vcomp;
      for (unsigned c = 0; c < v.size(); c++)
         vcomp.push_back((Component*)(v[c]));

      return vcomp;
   }

   void printSingleSummary(string component, const pair<int, double>& value, string title)
   {
      printf("---------------------------------\n");
      std::cout << component << "\t" << title << std::endl;
      printf("---------------------------------\n");
      printf("title\t#tests\tavg(ms)\tsum(ms)\n");
      double avg = 0;
      int validValues = 0;
      if (value.first > 0) {
         printf("%s\t%d\t%.4f\t%.4f\n", component.c_str(), value.first, (value.second / value.first), value.second);
         avg += (value.second / value.first);
         validValues++;
      } else
         printf("%s\t%d\tUNTESTED OR UNIMPLEMENTED\n", component.c_str(), 0);
      printf("---------------------------------\n");
      printf("all\t-\t%.4f\t-\n", (avg / validValues));
      std::cout << std::endl;
   }

   void printSummary(const vector<Component*>& vcomp, const std::vector<std::pair<int, double>>& values, string type, string title)
   {
      printf("---------------------------------\n");
      std::cout << "|" << type << "|=" << values.size() << "\t" << title << std::endl;
      printf("---------------------------------\n");
      printf("#id\ttitle\t#tests\tavg(ms)\tsum(ms)\n");
      double avg = 0;
      int validValues = 0;
      for (unsigned id = 0; id < values.size(); id++) {
         if (values[id].first > 0) {
            printf("#%d\t%s\t%d\t%.4f\t%.4f\n", ((int)id), vcomp[id]->toString().c_str(), values[id].first, (values[id].second / values[id].first), values[id].second);
            avg += (values[id].second / values[id].first);
            validValues++;
         } else
            printf("#%d\t%s\t%d\tUNTESTED OR UNIMPLEMENTED\n", ((int)id), vcomp[id]->toString().c_str(), 0);
      }
      printf("---------------------------------\n");
      printf("all\t*\t-\t%.4f\t-\n", (avg / validValues));
      std::cout << std::endl;
   }

   void printSummarySimple(const vector<Component*>& vcomp, const vector<int>& values, int numTests, string type, string title)
   {
      printf("---------------------------------\n");
      std::cout << "|" << type << "|=" << values.size() << "\t" << title << std::endl;
      printf("---------------------------------\n");
      printf("#id\ttitle\t#tests\tavg of %d tests\n", numTests);
      double avg = 0;
      int validValues = 0;
      for (unsigned id = 0; id < values.size(); id++) {
         if (values[id] > 0) {
            printf("#%d\t%s\t%d\t%d\n", ((int)id), vcomp[id]->toString().c_str(), values[id], (values[id] / numTests));
            avg += (values[id] / numTests);
            validValues++;
         } else
            printf("#%d\t%s\t%d\tUNTESTED OR UNIMPLEMENTED\n", ((int)id), vcomp[id]->toString().c_str(), 0);
      }
      printf("---------------------------------\n");
      printf("all\t*\t-\t%.4f\t-\n", (avg / validValues));
      std::cout << std::endl;
   }

   virtual string* preprocess(std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<std::string, std::string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* CHECKMODULE_HPP_ */
