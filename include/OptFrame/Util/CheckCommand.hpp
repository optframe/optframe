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

#ifndef OPTFRAME_CHECK_COMMAND_HPP_
#define OPTFRAME_CHECK_COMMAND_HPP_

#include <OptFrame/Constructive.hpp>
#include <OptFrame/Evaluator.hpp>
#include <OptFrame/Hyper/OptFrameList.hpp>
#include <OptFrame/InitialSearch.hpp>
#include <OptFrame/NS.hpp>
#include <OptFrame/NSEnum.hpp>
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/Timer.hpp>

#include "KahanSummation.hpp"
#include "Matrix.hpp"

namespace optframe {

// Time Data - CheckCommand

struct TimeDataCheckCommand {
  TimeDataCheckCommand() {
  }

  void initialize(int ns_size, int ev_size, int c_size) {
    this->timeNSApply = vector<vector<double>>(ns_size);
    this->timeNSCostApply = vector<vector<double>>(ns_size);
    this->timeNSCostApplyDelta = vector<vector<double>>(ns_size);
    this->timeNSCostApplyRealDelta = vector<vector<double>>(ns_size);
    this->timeNSCost = vector<vector<double>>(ns_size);
    this->timeNSEstimatedCost = vector<vector<double>>(ns_size);
    this->errorNSEstimatedCost = vector<vector<double>>(ns_size);
    this->fullTimeEval = vector<vector<double>>(ev_size);
    this->timeReeval = vector<vector<double>>(ev_size);
    this->timeConstructive = vector<vector<double>>(c_size);
    this->overestimate = this->underestimate = false;
  }

  vector<vector<double>> timeNSApply;
  vector<vector<double>> timeNSCostApply;
  vector<vector<double>> timeNSCostApplyDelta;
  vector<vector<double>> timeNSCostApplyRealDelta;
  vector<vector<double>> timeNSCost;
  vector<vector<double>> timeNSEstimatedCost;
  vector<vector<double>> errorNSEstimatedCost;
  vector<double> timeCloneSolution;
  vector<vector<double>> timeInitializeADS;
  vector<vector<double>> fullTimeEval;
  vector<vector<double>> timeReeval;
  vector<vector<double>> timeConstructive;

  bool overestimate, underestimate;

  //
  vector<std::string> logMoves;
};

// Solution Data - CheckCommand (solutions and evaluations)

template <XESolution XES>
struct SolDataCheckCommand {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

  vector<std::shared_ptr<Evaluator<S, XEv, XES>>> evaluators;

  vector<std::shared_ptr<S>> solutions;

  vector<vector<std::shared_ptr<XEv>>> evaluations;

  // ======================================
  // log data for solutions and evaluations
  vector<std::string> logSolutions;
  vector<vector<std::string>> logEvaluations;
};

template <XESolution XES>
struct CountDataCheckCommand {
  vector<vector<int>> vCountMovesSamples;
  vector<vector<int>> vCountValidMovesSamples;
  vector<vector<int>> vCountMovesEnumSamples;
  vector<vector<int>> vCountValidMovesEnumSamples;

  // used on run independent
  vector<vector<int>> vCountIndependentSamples;
  vector<vector<int>> vCountMovePairsSamples;
  // count good and bad examples
  vector<vector<int>> vCountTPIndependentSamples;
  vector<vector<int>> vCountFPIndependentSamples;
  vector<vector<int>> vCountTNIndependentSamples;
  vector<vector<int>> vCountFNIndependentSamples;

  void initialize(int nsseq_size, int nsenum_size) {
    this->vCountMovesSamples = vector<vector<int>>(nsseq_size);  //lNSSeq.size());
    this->vCountValidMovesSamples = vector<vector<int>>(nsseq_size);

    this->vCountMovesEnumSamples = vector<vector<int>>(nsenum_size);       //lNSEnum.size());
    this->vCountValidMovesEnumSamples = vector<vector<int>>(nsenum_size);  //lNSEnum.size());

    this->vCountIndependentSamples = vector<vector<int>>(nsseq_size);
    this->vCountMovePairsSamples = vector<vector<int>>(nsseq_size);

    this->vCountTPIndependentSamples = vector<vector<int>>(nsseq_size);
    this->vCountTNIndependentSamples = vector<vector<int>>(nsseq_size);
    this->vCountFPIndependentSamples = vector<vector<int>>(nsseq_size);
    this->vCountFNIndependentSamples = vector<vector<int>>(nsseq_size);
  }
};

template <XESolution XES>
struct AllDataCheckCommand {
  TimeDataCheckCommand timeData;
  SolDataCheckCommand<XES> solData;
  CountDataCheckCommand<XES> countData;

  bool status;
};

//CheckCommand uses SRand seed TODO
//template<XRepresentation R, class ADS, XSolution S = CopySolution<R, ADS>, XEvaluation XEv = Evaluation<>>
// manually passing 'S' (for safety)

// XSH is currently pair<S,XEv> ... will we test both SingleObj and MultiObj here? If that's the case, we will need XSH1 and XSH2 (two search spaces)... better having two checkmodules than this!
//
//template<XRepresentation R, class ADS, XBaseSolution<R, ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = std::pair<S, XEv>>
//
//template<XESolution XES, XEvaluation XEv = typename XES::second_type, XRepresentation R = typename XES::first_type, class ADS = int, XBaseSolution<R, ADS> S = CopySolution<R, ADS>, X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = std::pair<S, XEv>>
//
//template<XESolution XES, XRepresentation R = typename XES::first_type, class ADS = int, XBaseSolution<R, ADS> S = CopySolution<R, ADS>, X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = std::pair<S, typename XES::second_type>>
//
// NOTE THAT, HERE, XES denotes the pair <S,XEv>, while S can be <REP,ADS>, if desired
template <XESolution XES, XSolution S = typename XES::first_type, XRepresentation REP = S, class ADS = int, X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = std::pair<S, typename XES::second_type>>
class CheckCommand {
  using XEv = typename XES::second_type;

  static_assert(is_same<S, typename XES::first_type>::value);
  static_assert(is_same<XEv, typename XES::second_type>::value);

  static const int CMERR_EV_BETTERTHAN = 2001;
  static const int CMERR_EV_BETTEREQUALS = 2002;
  // Check if operator== is correctly implemented for Move class
  static const int CMERR_MOVE_EQUALS = 3001;

  static const int CMERR_MOVE_HASREVERSE = 3002;
  static const int CMERR_MOVE_REVREV_VALUE = 3004;
  // CMERR_MOVE_REVSIMPLE: check what?
  static const int CMERR_MOVE_REVSIMPLE = 3005;
  // CMERR_MOVE_REVFASTER: check what?
  static const int CMERR_MOVE_REVFASTER = 3006;
  // CMERR_MOVE_REALREVFASTER
  // explanation: disables allowCost from neighborhood, then uses moveCost() function from Evaluator
  static const int CMERR_MOVE_REALREVFASTER = 3008;
  static const int CMERR_MOVE_COST = 3007;

 private:
  // verbose flag
  bool verbose;

  // why this?
  bool paramConvertNS;

 public:
  // independent moves
  bool paramCheckIndependent;

  // store logs in json (string) format
  bool paramJsonLogs{false};

 private:
  //vector<GeneralEvaluator<XES>*> lEvaluator;
  vector<std::shared_ptr<Evaluator<S, XEv, XES>>> lEvaluator;
  //vector<GeneralEvaluator<XES>*> lGenEvaluator;
  //vector<Constructive<S>*> lConstructive;
  vector<std::shared_ptr<InitialSearch<XES>>> lConstructive;
  //
  //vector<std::shared_ptr<CopySolution<R, ADS>>> lSolution;
  vector<std::shared_ptr<S>> lSolution;

  vector<std::shared_ptr<Move<XES, XEv>>> lMove;
  vector<std::shared_ptr<NS<XES, XEv>>> lNS;
  vector<std::shared_ptr<NSSeq<XES, XEv>>> lNSSeq;
  vector<std::shared_ptr<NSEnum<XES, XEv>>> lNSEnum;

#ifdef LEGACY_ADS
  std::shared_ptr<ADSManager<REP, ADS, S>> adsMan;
#endif

 public:
#ifdef LEGACY_ADS
  vector<std::shared_ptr<ADSManager<REP, ADS, S>>> lADSManagerComp;  // optional
#endif

  void setParameters(bool _verbose) {
    this->verbose = _verbose;
  }

  CheckCommand(bool _verbose = false)
      : verbose(_verbose) {
    paramConvertNS = true;

#ifdef LEGACY_ADS
    adsMan = nullptr;
#endif
    paramCheckIndependent = true;
  }

  virtual ~CheckCommand() {
  }

  void add(sref<Constructive<S>> c) {
    //lConstructive.push_back(&c);
    //if (verbose)
    //	cout << "checkcommand: Constructive " << lConstructive.size() << " added!" << endl;
    cout << "checkcommand: Constructive not registered! NOT SUPPORTED! Try InitialSearch instead." << endl;
    assert(false);
  }

  void addConstructive(sref<Constructive<S>> c) {
    add(c);
  }

  void add(sref<InitialSearch<XES>> c) {
    lConstructive.push_back(c.sptr());
    if (verbose)
      cout << "checkcommand: InitialSearch " << lConstructive.size() << " added!" << endl;
  }

  void addInitialSearch(sref<InitialSearch<XES>> c) {
    add(c);
  }

#ifdef LEGACY_ADS
  void add(sref<ADSManager<REP, ADS, S>> adsMan) {
    lADSManagerComp.push_back(adsMan.sptr());
    if (verbose)
      cout << "checkcommand: AdsMan " << lADSManagerComp.size() << " added!" << endl;
  }
#endif

  void add(sref<Evaluator<S, XEv, XES>> c) {
    lEvaluator.push_back(c.sptr());
    if (verbose)
      cout << "checkcommand: Evaluator " << lEvaluator.size() << " added!" << endl;
  }

  // explicit add implementation
  void addEvaluator(sref<Evaluator<S, XEv, XES>> c) {
    add(c);
  }

  /*
   void add(GeneralEvaluator<XES>& c)
	{
		lEvaluator.push_back(&c);
		if (verbose)
			cout << "checkcommand: General Evaluator " << lEvaluator.size() << " added!" << endl;
	}
*/
  void add(CopySolution<REP, ADS>& c) {
    lSolution.push_back(&c);
    if (verbose)
      cout << "checkcommand: Solution " << lSolution.size() << " added!" << endl;
  }

  void add(Move<XES, XEv>& c) {
    lMove.push_back(&c);
    if (verbose)
      cout << "checkcommand: Move " << lMove.size() << " added!" << endl;
  }

  void add(sref<NS<XES, XEv>> c) {
    lNS.push_back(c.sptr());
    if (verbose)
      cout << "checkcommand: NS " << lNS.size() << " added!" << endl;
  }

  void addNS(sref<NS<XES, XEv>> c) {
    add(c);
  }

  void add(sref<NSSeq<XES, XEv>> c) {
    lNSSeq.push_back(c.sptr());
    if (verbose)
      cout << "checkcommand: NSSeq " << lNSSeq.size() << " added!" << endl;
    if (paramConvertNS) {
      std::shared_ptr<NS<XES, XEv>> sptr_ns = c.sptr();
      add(sptr_ns);
      //add((std::shared_ptr<NS<XES, XEv>>) c.sptr());
    }
  }

  void addNSSeq(sref<NSSeq<XES, XEv>> c) {
    add(c);
  }

  void add(sref<NSEnum<XES, XEv>> c) {
    lNSEnum.push_back(c.sptr());
    if (verbose)
      cout << "checkcommand: NSEnum " << lNSEnum.size() << " added!" << endl;
    if (paramConvertNS)
      add((sref<NSSeq<XES, XEv>>)c);
  }

  void addNSEnum(sref<NSEnum<XES, XEv>> c) {
    add(c);
  }

  void message(Component* c, int iter, string text) {
    message(c->id(), iter, text);
  }

  void message(std::shared_ptr<Component> c, int iter, string text) {
    message(c->id(), iter, text);
  }

  void message(string component, int iter, string text) {
    if (verbose)
      cout << "checkcommand iter: " << iter << " testing component '" << component << "' => " << text << endl;
  }

  void error(string text) {
    cout << "checkcommand error: " << text << endl;
  }

  void errormsg(string component, int code, string scode, int iter, string text) {
    cout << "checkcommand ERROR " << code << " (" << scode << "): iter=" << iter << " testing component '" << component << "' MSG: " << text << endl;
  }

  bool parseBool(string b) {
    return b == "true";
  }

 private:
  //bool testMoveGeneral(int iter, std::shared_ptr<NS<XES, XEv>> ns, int id_ns, CopySolution<R, ADS>& s, int id_s, Move<XES>& move, vector<vector<Evaluation<>*>>& evaluations, TimeCheckWithSamples& timeSamples)
  bool testMoveGeneral(
      int iter,
      std::shared_ptr<NS<XES, XEv>> ns,
      int id_ns,
      S& _s,
      int id_s,
      Move<XES>& move,
      vector<vector<std::shared_ptr<XEv>>>& evaluations,
      TimeDataCheckCommand& timeSamples) {
    for (unsigned ev = 0; ev < lEvaluator.size(); ev++) {
      message(lEvaluator.at(ev), iter, "evaluating random move (apply, revert and moveCost).");

      // ==============
      // creating 'se'
      // ---------------
      XEv _e = lEvaluator[ev]->evaluate(_s);
      XES se = make_pair(_s, _e);
      //

      string moveFrom = "Move ";
      moveFrom.append(move.id());
      // not printing NS for single Move test
      if (ns) {
        moveFrom.append(" from: ");
        moveFrom.append(ns->id());
        moveFrom.append(" toString: ");
        moveFrom.append(ns->toString());
      }

      if (verbose)
        move.print();

      std::stringstream ssMoveLog;
      if (paramJsonLogs) {
        //
        std::string dump = optframe::cjson.dump();
        //std::cout << "DUMP -> " << dump << std::endl;
        assert(dump.length() == 0);
        //
        bool os_ok = move.toStream(optframe::cjson);
        //
        dump = optframe::cjson.dump();
        if (!os_ok)
          dump = "{\"move_type\":\"UnknownMove\"}";
        //std::cout << "DUMP -> " << dump << std::endl;
        //
        ssMoveLog.precision(3);
        ssMoveLog << std::fixed;
        ssMoveLog << "{";
        ssMoveLog << "\"move_log_id\":" << timeSamples.logMoves.size() << ",";
        ssMoveLog << "\"ev_id\":" << ev << ",";
        ssMoveLog << "\"move\":" << dump << ",";
        ssMoveLog << "\"move_sol_id\": " << id_s << ",";
        ssMoveLog << "\"sol_evaluation\":" << se.second.evaluation() << ",";
        ssMoveLog << "\"move_ns_id\": " << id_ns << ",";
        //assert(false);
      }

      message(moveFrom, iter, "testing reverse.");

      Timer t_clone;
      //CopySolution<R, ADS>& sOriginal = s.clone(); // remove if not verbose
      //S sOriginal = s; // copy
      S sOriginal = se.first;  // copy
      timeSamples.timeCloneSolution.push_back(t_clone.inMilliSecs());

      Timer tMovApply;
      uptr<Move<XES, XEv, XES>> rev = move.apply(se);
      //
      if ((!move.hasReverse() && rev) || (move.hasReverse() && !rev)) {
        errormsg(moveFrom, CMERR_MOVE_HASREVERSE, "CMERR_MOVE_HASREVERSE", iter, " conflict between apply result and hasReverse()");
        return false;
      }

      double applyMS = tMovApply.inMilliSecs();
      timeSamples.timeNSApply[id_ns].push_back(applyMS);

      if (paramJsonLogs) {
        ssMoveLog << "\"timeNSApplyMS\":" << applyMS << ",";
      }

      Timer t_clone2;
      //CopySolution<R, ADS>& sNeighbor = s.clone(); // remove if not verbose
      //S sNeighbor = s; // copy
      S sNeighbor = se.first;  // copy
      timeSamples.timeCloneSolution.push_back(t_clone2.inMilliSecs());
      // ===================== tests with ADSManager ======================

      // DEPRECATED ADSManager
#ifdef LEGACY_ADS
      if (adsMan) {
        message(lEvaluator.at(ev), -1, "testing ADS.");

        ADS ads(sNeighbor.getADS());  // copy
        Timer ts_ds;
        adsMan->initializeADS(sNeighbor.getR(), sNeighbor.getADS());
        timeSamples.timeInitializeADS[0].push_back(ts_ds.inMilliSecs());

        if (!adsMan->compareADS(ads, sNeighbor.getADS())) {
          cout << "checkcommand error: ADS not updated correctly! Compared brand new initializeADS with update from move => ";
          move.print();
          cout << "S (sOriginal.getADS()): " << endl;
          adsMan->printADS(sOriginal.getADS());
          cout << "WRONG: (sNeighbor.getADS())" << endl;
          adsMan->printADS(ads);
          cout << "RIGHT (re-initialized): " << endl;
          adsMan->printADS(sNeighbor.getADS());
          return false;
        }
      }
#endif

      //
      Timer te;
      XEv e_rev = lEvaluator.at(ev)->evaluate(se.first);
      timeSamples.fullTimeEval[ev].push_back(te.inMilliSecs());

      //
      Timer tMovRevApply;
      uptr<Move<XES, XEv, XES>> ini = nullptr;
      if (rev)
        ini = rev->apply(se);

      double applyRevMS = tMovRevApply.inMilliSecs();
      timeSamples.timeNSApply[id_ns].push_back(applyRevMS);

      if (paramJsonLogs) {
        ssMoveLog << "\"timeNSApplyRevMS\":" << applyRevMS << ",";
        ssMoveLog << "\"sol_rev_evaluation\":" << e_rev.evaluation() << ",";
      }

      //
      // ===================== tests with ADSManager ======================

#ifdef LEGACY_ADS
      if (adsMan) {
        message(lEvaluator.at(ev), -1, "testing ADS.");

        ADS ads(se.first.getADS());  // copy
        Timer ts_ds2;
        adsMan->initializeADS(se.first.getR(), se.first.getADS());
        timeSamples.timeInitializeADS[0].push_back(ts_ds2.inMilliSecs());

        if (!adsMan->compareADS(ads, se.first.getADS())) {
          cout << "checkcommand error: ADS not updated correctly! Compared brand new initializeADS with update from reverse move => ";
          Component::safe_print(rev.get());
          cout << "S (sOriginal.getADS()): " << endl;
          adsMan->printADS(sOriginal.getADS());
          cout << "WRONG (s.getADS()): " << endl;
          adsMan->printADS(ads);
          cout << "RIGHT (re-initialized): " << endl;
          adsMan->printADS(se.first.getADS());
          return false;
        }
      }
#endif

      // go back by copy (if necessary!)
      if (!rev) {
        //s = sOriginal;
        se.first = sOriginal;
      }

      //delete &sOriginal;

      Timer te2;
      XEv e_ini = lEvaluator.at(ev)->evaluate(se.first);
      timeSamples.fullTimeEval[ev].push_back(te2.inMilliSecs());

      if (ini && (*ini != move)) {
        errormsg(moveFrom, CMERR_MOVE_EQUALS, "CMERR_MOVE_EQUALS", iter, "reverse of reverse is not the original move!");
        move.print();
        cout << "move: ";
        move.print();
        cout << "rev: ";
        Component::safe_print(rev.get());
        cout << "ini (reverse of rev): ";
        Component::safe_print(ini.get());

        return false;
      }

      message(lEvaluator.at(ev), iter, "testing reverse value.");

      //if (EVALUATION_ABS(e_ini.evaluation() - e.evaluation()) >= optframe::get_numeric_zero<evtype>())
      if (!optframe::numeric_is_zero(e_ini.evaluation() - _e.evaluation())) {
        errormsg(moveFrom, CMERR_MOVE_REVREV_VALUE, "CMERR_MOVE_REVREV_VALUE", iter, "reverse of reverse has a different evaluation value!");
        move.print();
        cout << "move: ";
        move.print();
        cout << "original: ";
        _e.print();
        cout << "reverse of reverse:";
        e_ini.print();

        return false;
      }

      // =============================================================

      message(lEvaluator.at(ev), iter, "testing move cost.");

      // ==============
      // revCost
      // ---------------
      //

      evtype revCost = e_rev.evaluation() - _e.evaluation();
      message(lEvaluator.at(ev), iter, "revCost calculated!");

      Timer tMoveCostApply;
      ///MoveCost<>* mcSimpleCost = lEvaluator[ev]->moveCostComplete(move, s);
      op<XEv> mcSimpleCost = lEvaluator[ev]->moveCostComplete(move, se);
      //evtype simpleCost = mcSimpleCost->cost();
      evtype simpleCost = mcSimpleCost->evaluation();
      //
      double revCostMS = tMoveCostApply.inMilliSecs();
      timeSamples.timeNSCostApply[id_ns].push_back(revCostMS);

      if (paramJsonLogs) {
        ssMoveLog << "\"timeNSCostApplyMS\":" << revCostMS << ",";
        ssMoveLog << "\"simpleCost\":" << simpleCost << ",";
        //timeSamples.logMoves.push_back(ssMoveLog.str());
      }

      //delete mcSimpleCost;
      message(lEvaluator.at(ev), iter, "simpleCost calculated!");

      if (!optframe::numeric_is_zero(se.second.evaluation() - _e.evaluation())) {
        std::cout << "ERROR: calculation of revCost is wrong! " << se.second.evaluation() << " != " << _e.evaluation() << std::endl;
        assert(false);
      }

      // TODO: fix ABS with is_zero pattern
      //if (EVALUATION_ABS(revCost - simpleCost) >= optframe::get_numeric_zero<evtype>())
      if (!optframe::numeric_is_zero(revCost - simpleCost)) {
        errormsg(moveFrom, CMERR_MOVE_REVSIMPLE, "CMERR_MOVE_REVSIMPLE", iter, "difference between revCost and simpleCost");
        cout << "move: ";
        move.print();
        cout << "revCost = " << revCost << "= ..." << endl;
        cout << "   ... = (e_rev after move) " << e_rev.evaluation() << " -  (_e before move) " << _e.evaluation() << ")" << std::endl;
        cout << "simpleCost = " << simpleCost << " (from 'ev->moveCostComplete(move, se)' method)" << endl;
        cout << "Current Evaluation: se.second.evaluation() = " << se.second.evaluation() << std::endl;
        return false;
      }

      //pair<S, XEv> se(s, e); // TODO: check if copy is really good here.

      // ==============
      // fasterCost
      // --------------
      //
      // update from original version
      se.first = _s;
      se.second = _e;
      //
      Timer tMoveCostApplyDelta;
      //
      //XEv evBeginFasterCost(_e);
      XEv evBeginFasterCost(se.second);
      //
      uptr<Move<XES, XEv, XES>> rev1 = lEvaluator[ev]->applyMoveReevaluate(move, se);
      //evtype e_end1 = _e.evaluation();
      evtype e_end1 = se.second.evaluation();
      if (se.second.isOutdated()) {
        std::cout << "WARNING: evaluation is OUTDATED! WHAT TO DO HERE?" << std::endl;
      }
      //
      //
      // TODO: check outdated status
      // TODO: if(e.outdated), it means that user did not implement Move::applyMoveReevaluate(e,R,ADS)!
      //			Move<XES, XEv, XES>& ini1 = *lEvaluator[ev]->applyMoveReevaluate(e, rev1, s);
      //
      uptr<Move<XES, XEv, XES>> ini1 = rev1->applyUpdate(se);
      if (se.second.isOutdated()) {
        //std::cout << "WARNING: evaluation is OUTDATED after applyUpdate! manual evaluate" << std::endl;
        se.second = lEvaluator[ev]->evaluate(se.first);
      }
      //_e = std::move(evBeginFasterCost);
      //evtype e_ini1 = _e.evaluation();
      evtype e_ini1 = evBeginFasterCost.evaluation();

      double fasterCostMS = tMoveCostApplyDelta.inMilliSecs();
      timeSamples.timeNSCostApplyDelta[id_ns].push_back(fasterCostMS);

      if (paramJsonLogs) {
        ssMoveLog << "\"timeNSCostApplyDeltaMS\":" << fasterCostMS << ",";
        //timeSamples.logMoves.push_back(ssMoveLog.str());
      }

      //delete &rev1;
      //delete &ini1;

      evtype fasterCost = e_end1 - e_ini1;
      message(lEvaluator.at(ev), iter, "fasterCost calculated!");

      if (!optframe::numeric_is_zero(se.second.evaluation() - _e.evaluation())) {
        std::cout << "ERROR: calculation of faster is wrong! " << se.second.evaluation() << " != " << _e.evaluation() << std::endl;
        assert(false);
      }

      //if (EVALUATION_ABS(revCost - fasterCost) >= optframe::get_numeric_zero<evtype>())
      if (!optframe::numeric_is_zero(revCost - fasterCost)) {
        errormsg(moveFrom, CMERR_MOVE_REVFASTER, "CMERR_MOVE_REVFASTER", iter, "difference between revCost and fasterCost!");
        cout << "move: ";
        move.print();
        cout << "revCost = " << revCost << endl;
        cout << "fasterCost = " << fasterCost << endl;
        cout << "_e = " << _e.evaluation() << endl;
        cout << "e_rev = " << e_rev.evaluation() << endl;
        return false;
      }
      // ==============

      // ==============
      // real fasterCost
      // ---------------
      //
      // update 'se' from original version
      se.first = _s;
      se.second = _e;

      Timer tMoveCostApplyRealDelta;
      // NOTE THAT GeneralEvaluator does not have a Allow Costs strategy...
      bool oldAllowCostsStatus = lEvaluator[ev]->getAllowCosts();
      lEvaluator[ev]->setAllowCosts(false);

      //MoveCost<>* mcRealFasterCost = lEvaluator[ev]->moveCost(move, se);
      XEv mcRealFasterCost = lEvaluator[ev]->moveCost(move, se);

      lEvaluator[ev]->setAllowCosts(oldAllowCostsStatus);
      //evtype realFasterCost = mcRealFasterCost->cost();
      evtype realFasterCost = mcRealFasterCost.evaluation();

      double realFasterCostMS = tMoveCostApplyRealDelta.inMilliSecs();
      timeSamples.timeNSCostApplyRealDelta[id_ns].push_back(realFasterCostMS);

      if (paramJsonLogs) {
        ssMoveLog << "\"timeNSCostApplyRealDelta\":" << realFasterCostMS << ",";
        //timeSamples.logMoves.push_back(ssMoveLog.str());
      }

      //delete mcRealFasterCost;
      message(lEvaluator.at(ev), iter, "realFasterCost calculated!");

      if (!optframe::numeric_is_zero(se.second.evaluation() - _e.evaluation())) {
        std::cout << "ERROR: calculation of realFasterCost is wrong! " << se.second.evaluation() << " != " << _e.evaluation() << std::endl;
        assert(false);
      }

      //if (EVALUATION_ABS(revCost - realFasterCost) >= optframe::get_numeric_zero<evtype>())
      if (!optframe::numeric_is_zero(revCost - realFasterCost)) {
        errormsg(moveFrom, CMERR_MOVE_REALREVFASTER, "CMERR_MOVE_REALREVFASTER", iter, "difference between revCost and realfasterCost!");
        cout << "move: ";
        move.print();
        cout << "revCost = " << revCost << endl;
        cout << "simpleCost = " << realFasterCost << endl;
        return false;
      }
      // ==============

      Timer tMoveCost;
      ///MoveCost<>* cost = nullptr;
      op<XEv> cost = nullopt;

      if (lEvaluator[ev]->getAllowCosts())
        cost = move.cost(se, false);

      if (cost && !cost->isEstimated()) {
        double costMS = tMoveCost.inMilliSecs();
        timeSamples.timeNSCost[id_ns].push_back(costMS);

        if (paramJsonLogs) {
          ssMoveLog << "\"timeNSCost\":" << costMS;  // THIS IS THE LAST, NO COMMA ','
        }
      }

      if (cost && cost->isEstimated()) {
        double estimatedCostMS = tMoveCost.inMilliSecs();
        timeSamples.timeNSEstimatedCost[id_ns].push_back(estimatedCostMS);

        if (paramJsonLogs) {
          ssMoveLog << "\"timeNSEstimatedCost\":" << estimatedCostMS << ",";
          //timeSamples.logMoves.push_back(ssMoveLog.str());
        }

        //if (cost->cost() > revCost)
        if (cost->evaluation() > revCost)
          timeSamples.overestimate = true;
        //if (cost->cost() < revCost)
        if (cost->evaluation() < revCost)
          timeSamples.underestimate = true;
      }

      message(lEvaluator.at(ev), iter, "cost() calculated!");

      if (cost && !cost->isEstimated()) {
        //double cValue = cost->cost();
        double cValue = cost->evaluation();
        //if (EVALUATION_ABS(revCost - cValue) >= optframe::get_numeric_zero<evtype>())
        if (!optframe::numeric_is_zero(revCost - cValue)) {
          errormsg(moveFrom, CMERR_MOVE_COST, "CMERR_MOVE_COST", iter, "difference between expected cost and cost()");
          cout << "move: ";
          move.print();
          cout << "expected cost: (e' - e) =\t" << revCost << endl;
          cout << "cost() =\t" << cValue << endl;
          cout << "_e: \t obj:" << _e.getObjFunction() << "\t inf: " << _e.getInfMeasure() << " \t total:" << _e.evaluation() << endl;
          cout << "e':\t obj:" << e_rev.getObjFunction() << "\t inf:" << e_rev.getInfMeasure() << " \t total:" << e_rev.evaluation() << endl;
          //cout << "e+cost():\t obj:" << e.getObjFunction() + cost->getObjFunctionCost() << "\t inf:" << e.getInfMeasure() + cost->getInfMeasureCost() << "\t total:" << e.evaluation() + cost->cost() << endl;
          cout << "_e+cost():\t obj:" << _e.getObjFunction() + cost->getObjFunction() << "\t inf:" << _e.getInfMeasure() + cost->getInfMeasure() << "\t total:" << _e.evaluation() + cost->evaluation() << endl;
          cout << "_s: ";
          //s.print();
          cout << _s;
          //
          cout << "s': ";
          //sNeighbor.print();
          cout << sNeighbor;
          //
          cout << "move: ";
          move.print();
          cout << "==============" << endl;
          cout << "  GOOD LUCK!  " << endl;
          cout << "==============" << endl;
          return false;
        }

        // testing double move costs! (for MoveCost betterThan)
        if (ns)  // not testing for single Move
        {
          // TODO: consider that randomMove can be null
          uptr<Move<XES, XEv, XES>> move2 = ns->randomMove(se);
          if (verbose) {
            cout << "testing double move!" << endl;
            move2->print();
          }

          if (!move2->canBeApplied(se)) {
            if (verbose) {
              cout << "double move cannot be applied: ";
              move2->print();
            }
          } else {
            //MoveCost<>* cost2 = nullptr;
            op<XEv> cost2 = nullopt;
            if (lEvaluator[ev]->getAllowCosts()) {
              cost2 = move2->cost(se, false);
              if (cost2) {
                lEvaluator[ev]->betterThan(*cost, *cost2);
                //delete cost2;
              }
            }
          }
        }
        // finish double cost test

        //delete cost;
      }

      message(lEvaluator.at(ev), iter, "all move costs okay!");

      //if (rev)
      //	delete rev;
      //
      //delete &sNeighbor;
      //
      //if (ini)
      //	delete ini;

      if (paramJsonLogs) {
        ssMoveLog << "}";
        timeSamples.logMoves.push_back(ssMoveLog.str());
      }
    }

    //delete &move;  // ONLY IF NECESSARY! DO IT OUTSIDE...

    return true;
  }

 public:
  AllDataCheckCommand<XES> run(int iterMax, int nSolNSSeq) {
    // ======================================
    //           BEGIN TESTS
    // ======================================

    cout << "---------------------------------------" << endl;
    cout << "tests/(nSolutions*|constructives|)=" << iterMax << " tests(NSSeq)=" << nSolNSSeq << endl;
    cout << "---------------------------------------" << endl;
    cout << "evaluators=" << lEvaluator.size() << endl;
    cout << "constructives=" << lConstructive.size() << endl;
    cout << "moves=" << lMove.size() << endl;
    cout << "ns=" << lNS.size() << endl;
    cout << "nsseq=" << lNSSeq.size() << endl;
    cout << "nsenum=" << lNSEnum.size() << endl;

#ifdef LEGACY_ADS
    // ADSManager is deprecated!
    cout << "DEPRECATED:adsmanager=" << lADSManagerComp.size() << endl;
#endif
    cout << "---------------------------------------" << endl
         << endl;

    return doRun(iterMax, nSolNSSeq);
  }

 private:
  bool doRunConstructive(int iterMax, TimeDataCheckCommand& timeSamples, SolDataCheckCommand<XES>& solData) {
    // ----------------------------------------------------------------------------------------
    // generate 'iterMax' OptFrame:Solution for each OptFrame:Constructive and store evaluation
    // ----------------------------------------------------------------------------------------

    using SolOrRepType = typename XES::first_type;
    //using EvalType = typename XES::second_type;
    //vector<CopySolution<R, ADS>*> solutions;
    vector<std::shared_ptr<SolOrRepType>> solutions;

    vector<std::shared_ptr<Evaluator<SolOrRepType, XEv, XES>>>& evaluators = solData.evaluators;

    vector<vector<std::shared_ptr<XEv>>> evaluations(evaluators.size());

    if (paramJsonLogs) {
      solData.logEvaluations = vector<vector<std::string>>(evaluators.size());
    }

    if (lConstructive.size() > 0)
      cout << "checkcommand  will test " << lConstructive.size() << " constructive components (iterMax=" << iterMax << ")" << endl;
    for (unsigned c = 0; c < lConstructive.size(); c++) {
      //Constructive<S>* constructive = lConstructive.at(c);
      std::shared_ptr<InitialSearch<XES>> constructive = lConstructive.at(c);

      cout << "checkcommand: testing Constructive " << c << " => " << constructive->toString();
      cout << endl;

      for (int iter = 1; iter <= iterMax; iter++) {
        message(lConstructive.at(c), iter, "generating solution.");

        Timer ts;
        //CopySolution<R,ADS> s = *constructive->generateSolution(10000000);
        op<XES> ps = constructive->initialSearch({0.0}).first;
        //CopySolution<R,ADS> s = *constructive->initialSolution(10000000);
        //
        //CopySolution<R, ADS> s = ps->first;
        auto s = ps->first;

        timeSamples.timeConstructive[c].push_back(ts.inMilliSecs());

        // only enable adsMan in LEGACY code
        using T = decltype(s);

#ifdef LEGACY_ADS
        if constexpr (is_same<SolOrRepType, S>::value) {
          if (adsMan) {
            Timer ts2;
            adsMan->initializeADS(s.getR(), s.getADS());
            timeSamples.timeInitializeADS[0].push_back(ts2.inMilliSecs());
          }
        }
#endif
        //solutions.push_back(new CopySolution<R, ADS>(s));
        std::shared_ptr<SolOrRepType> sptr_sol(new T(s));
        //solutions.push_back(new T(s));
        solutions.push_back(sptr_sol);
        //
        if (paramJsonLogs) {
          //solData.logSolutions.push_back(sptr_sol->toStringFormat(StringFormat::JSON));
          std::string dump = optframe::cjson.dump();
          //std::cout << "DUMP -> " << dump << std::endl;
          assert(dump.length() == 0);
          //bool ret = sptr_sol->toStream(optframe::cjson);
          //assert(ret);
          optframe::cjson << *sptr_sol;
          dump = optframe::cjson.dump();
          //
          std::stringstream ss;
          ss << "{\"sol_log_id\":" << solData.logSolutions.size() << ",";
          ss << "\"solution\":" << dump << "}";
          //std::cout << "DUMP -> " << dump << std::endl;
          solData.logSolutions.push_back(ss.str());  //(dump);
                                                     //assert(false);
        }

        for (unsigned ev = 0; ev < evaluators.size(); ev++) {
          message(lEvaluator.at(ev), iter, "evaluating solution.");
          Timer te;
          XEv e = evaluators.at(ev)->evaluate(s);
          timeSamples.fullTimeEval[ev].push_back(te.inMilliSecs());

          std::shared_ptr<XEv> sptr_ev(new Evaluation(e));
          //evaluations.at(ev).push_back(new Evaluation(e));
          evaluations.at(ev).push_back(sptr_ev);
          //
          if (paramJsonLogs) {
            std::string dump = optframe::cjson.dump();
            //std::cout << "DUMP -> '" << dump << "'" << std::endl;
            assert(dump.length() == 0);
            //bool ret = sptr_sol->toStream(optframe::cjson);
            //assert(ret);
            optframe::cjson << *sptr_ev;
            dump = optframe::cjson.dump();
            //
            std::stringstream ss;
            ss << "{\"evaluation_log_id\":" << solData.logEvaluations.at(ev).size() << ",";
            ss << "\"ev_id\": " << ev << ",";
            ss << "\"evaluation\":" << dump << "}";
            //solData.logEvaluations.at(ev).push_back(sptr_ev->toStringFormat(StringFormat::JSON));
            solData.logEvaluations.at(ev).push_back(ss.str());  //(dump);
                                                                //std::cout << "DUMP -> " << dump << std::endl;
                                                                //assert(false);
          }

          if (lEvaluator.at(ev)->direction->betterThan(e, e)) {
            errormsg(lEvaluator.at(ev)->toString(), CMERR_EV_BETTERTHAN, "CMERR_EV_BETTERTHAN", iter, "error in betterThan(X,X)=true");
            return false;
          }

          if (!lEvaluator.at(ev)->direction->betterOrEquals(e, e)) {
            errormsg(lEvaluator.at(ev)->toString(), CMERR_EV_BETTEREQUALS, "CMERR_EV_BETTEREQUALS", iter, "error in betterOrEquals(X,X)=false");
            return false;
          }
        }
      }

      cout << "checkcommand: Constructive " << c << " => " << lConstructive.at(c)->id() << " finished." << endl;
      if (verbose)
        cout << endl
             << endl;
    }

    if (verbose)
      cout << endl
           << endl;

    solData.solutions = solutions;
    solData.evaluations = evaluations;
    return true;
  }

  bool doRunMove(int iterMax, TimeDataCheckCommand& timeSamples, SolDataCheckCommand<XES>& solData) {
    using SolOrRepType = typename XES::first_type;
    vector<std::shared_ptr<SolOrRepType>>& solutions = solData.solutions;
    vector<vector<std::shared_ptr<XEv>>>& evaluations = solData.evaluations;

    vector<std::shared_ptr<Evaluator<SolOrRepType, XEv, XES>>>& evaluators = solData.evaluators;

    // ====================================================================
    // testing Move
    // ====================================================================

    if (lMove.size() > 0)
      cout << "checkcommand  will test given Move components (|Move|=" << lMove.size() << "; numSolutions=" << solutions.size() << ")" << endl;

    for (unsigned p = 0; p < lSolution.size(); p++) {
      //solutions.push_back(&lSolution[p]->clone());
      //
      std::shared_ptr<S> sptr_sol(new S(*lSolution[p]));
      //
      //solutions.push_back(new S(*lSolution[p]));
      solutions.push_back(sptr_sol);

      for (unsigned ev = 0; ev < evaluators.size(); ev++) {
        message(lEvaluator.at(ev), p, "evaluating input solution.");
        Timer te;
        XEv e = evaluators.at(ev)->evaluate(*lSolution[p]);
        timeSamples.fullTimeEval[ev].push_back(te.inMilliSecs());

        std::shared_ptr<XEv> sptr_ev(new Evaluation(e));

        //evaluations.at(ev).push_back(new Evaluation(e));
        evaluations.at(ev).push_back(sptr_ev);
      }
    }

    for (unsigned id_move = 0; id_move < lMove.size(); id_move++) {
      shared_ptr<Move<XES, XEv, XES>> pmove = lMove.at(id_move);

      cout << "checkcommand: testing Move " << id_move << " => " << pmove->toString() << endl;

      for (unsigned id_s = 0; id_s < solutions.size(); id_s++) {
        message(lMove.at(id_move).get(), -1, "working on move.");

        //CopySolution<R, ADS>& s = *solutions.at(id_s);
        S& s = *solutions.at(id_s);

        Move<XES, XEv, XES>& move = *pmove;

        XES se = make_pair(s, XEv());

        if (!move.canBeApplied(se)) {
          if (verbose) {
            cout << "move cannot be applied: ";
            move.print();
          }
          continue;
        }

        // 	bool testMoveGeneral(int iter, NS<R,ADS>* ns, int id_ns, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, TimeCheckSol& timeSol, TimeNS& timeNS)

        if (!testMoveGeneral(-1, nullptr, -1, s, id_s, move, evaluations, timeSamples))
          return false;

        /////delete& move; // TODO NEVER DESTROY THIS OptFrame:Move!
      }
    }

    return true;
  }

  bool doRunNS(int iterMax, TimeDataCheckCommand& timeSamples, SolDataCheckCommand<XES>& solData) {
    using SolOrRepType = typename XES::first_type;
    vector<std::shared_ptr<SolOrRepType>>& solutions = solData.solutions;
    vector<vector<std::shared_ptr<XEv>>>& evaluations = solData.evaluations;

    // ====================================================================
    // testing NS
    // ====================================================================

    if (lNS.size() > 0)
      cout << "checkcommand  will test " << lNS.size() << " NS components (iterMax=" << iterMax << " Solutions=" << solutions.size() << ")" << endl;

    for (unsigned id_ns = 0; id_ns < lNS.size(); id_ns++) {
      std::shared_ptr<NS<XES, XEv>> ns = lNS.at(id_ns);

      cout << "checkcommand: testing NS " << id_ns << " => " << ns->toString();
      cout << endl;

      for (int iter = 1; iter <= iterMax; iter++) {
        message(lNS.at(id_ns), iter, "starting tests!");

        for (unsigned id_s = 0; id_s < solutions.size(); id_s++) {
          stringstream ss_msg1;
          ss_msg1 << "generating random move for solution id=" << id_s;
          message(lNS.at(id_ns), iter, ss_msg1.str());

          //CopySolution<R, ADS>& s = *solutions.at(id_s);
          S& s = *solutions.at(id_s);

          XES se = make_pair(s, XEv());

          // TODO: consider that randomMove can be null
          uptr<Move<XES, XEv, XES>> move = ns->randomMove(se);
          if (verbose)
            move->print();

          if (!move->canBeApplied(se)) {
            if (verbose) {
              cout << "move cannot be applied: ";
              move->print();
            }

            continue;
          }

          // EXEC TESTS HERE

          // bool testMoveNS(int iter, NS<R,ADS>* ns, int id_ns, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, pair<int, double>& timeCloneSolution, vector<pair<int, double> >& timeInitializeADS, vector<pair<int, double> >& fullTimeEval, vector<pair<int, double> >& timeReeval, TimeNS& timeNS)
          // bool testMoveGeneral(int iter, NS<R,ADS>* ns, int id_ns, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, TimeCheckSol& timeSol, TimeNS& timeNS)

          if (!testMoveGeneral(iter, ns, id_ns, s, id_s, *move, evaluations, timeSamples)) {
            return false;
          }
        }
      }

      cout << "checkcommand: " << lNS.at(id_ns)->id() << " finished." << endl;
    }
    return true;
  }

  bool doRunNSSeq(int nSolNSSeq, TimeDataCheckCommand& timeSamples, SolDataCheckCommand<XES>& solData, CountDataCheckCommand<XES>& countData) {
    using SolOrRepType = typename XES::first_type;
    vector<std::shared_ptr<SolOrRepType>>& solutions = solData.solutions;
    vector<vector<std::shared_ptr<XEv>>>& evaluations = solData.evaluations;

    // ====================================================================
    // testing NSSeq
    // ====================================================================

    if (lNSSeq.size() > 0)
      cout << "checkcommand  will test " << lNSSeq.size() << " NSSeq components (nSolNSSeq=" << nSolNSSeq << " of numSolutions=" << solutions.size() << ")" << endl;

    vector<vector<int>>& vCountMovesSamples = countData.vCountMovesSamples;            ///(lNSSeq.size());
    vector<vector<int>>& vCountValidMovesSamples = countData.vCountValidMovesSamples;  //(lNSSeq.size());

    for (unsigned id_nsseq = 0; id_nsseq < lNSSeq.size(); id_nsseq++) {
      std::shared_ptr<NSSeq<XES, XEv, XES>> nsseq = lNSSeq.at(id_nsseq);

      cout << "checkcommand: testing NSSeq " << id_nsseq << " => " << nsseq->toString();
      cout << endl;

      for (int nqs = 1; nqs <= nSolNSSeq; nqs++) {
        message(lNSSeq.at(id_nsseq), nqs, "starting tests!");

        int randomIndex = rand() % solutions.size();
        //CopySolution<R, ADS>& s = *solutions.at(randomIndex);
        S& s = *solutions.at(randomIndex);
        int id_s = randomIndex;

        XES se = make_pair(s, XEv());

        // ===================
        // apply a single move
        // ===================

        // TODO: consider that iterator can be null!
        //NSIterator<XES, XEv>& it = *nsseq->getIterator(se);
        auto it = nsseq->getIterator(se);

        int countMovesNSSeq = 0;
        int countValidMovesNSSeq = 0;

        for (it->first(); !it->isDone(); it->next()) {
          if (verbose)
            // cout << endl;
            message(lNSSeq.at(id_nsseq), nqs, "getting current move (NSSeq tests).");

          // TODO: verify if it's not null!
          uptr<Move<XES, XEv, XES>> pmove = it->current();
          Move<XES, XEv, XES>& move = *pmove;
          countMovesNSSeq++;

          if (!move.canBeApplied(se)) {
            if (verbose) {
              cout << "move cannot be applied (NSSeq tests): ";
              move.print();
            }
            continue;
          }

          countValidMovesNSSeq++;

          // EXEC MOVES HERE

          //	bool testMoveNSSeq(int iter, NSSeq<R,ADS>* nsseq, int nqs, int id_nsseq, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, pair<int, double>& timeCloneSolution, vector<pair<int, double> >& timeInitializeADS, vector<pair<int, double> >& fullTimeEval, vector<pair<int, double> >& timeReeval, TimeNS& timeNS)
          // 	bool testMoveGeneral(int iter, NS<R,ADS>* ns, int id_ns, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, TimeCheckSol& timeSol, TimeNS& timeNS)

          if (!testMoveGeneral(nqs, nsseq, id_nsseq, s, id_s, move, evaluations, timeSamples)) {
            return false;
          }
        }

        vCountMovesSamples[id_nsseq].push_back(countMovesNSSeq);
        vCountValidMovesSamples[id_nsseq].push_back(countValidMovesNSSeq);
      }

      cout << "checkcommand: " << lNSSeq.at(id_nsseq)->id() << " finished." << endl;
    }
    return true;
  }

  bool doRunNSEnum(int nSolNSSeq, TimeDataCheckCommand& timeSamples, SolDataCheckCommand<XES>& solData, CountDataCheckCommand<XES>& countData) {
    using SolOrRepType = typename XES::first_type;
    vector<std::shared_ptr<SolOrRepType>>& solutions = solData.solutions;
    vector<vector<std::shared_ptr<XEv>>>& evaluations = solData.evaluations;
    //vector<std::shared_ptr<Evaluator<SolOrRepType, XEv, XES>>>& evaluators = solData.evaluators;

    // ====================================================================
    // testing NSEnum
    // ====================================================================

    if (lNSEnum.size() > 0)
      cout << "checkcommand  will test " << lNSEnum.size() << " NSEnum components (nSolNSSeq=" << nSolNSSeq << " of numSolutions=" << solutions.size() << ")" << endl;

    vector<vector<int>>& vCountMovesEnumSamples = countData.vCountMovesEnumSamples;
    vector<vector<int>>& vCountValidMovesEnumSamples = countData.vCountValidMovesEnumSamples;

    //vector<vector<int>>& vCountIndependentEnumSamples = countData.vCountIndependentEnumSamples;
    //vector<vector<int>>& vCountMovePairsEnumSamples = countData.vCountMovePairsEnumSamples;

    for (unsigned id_nsenum = 0; id_nsenum < lNSEnum.size(); id_nsenum++) {
      std::shared_ptr<NSEnum<XES, XEv>> nsenum = lNSEnum.at(id_nsenum);

      cout << "checkcommand: testing NSEnum " << id_nsenum << " => " << nsenum->toString();
      cout << endl;

      for (int nqs = 1; nqs <= nSolNSSeq; nqs++) {
        message(lNSEnum.at(id_nsenum), nqs, "starting tests!");

        int randomIndex = rand() % solutions.size();
        //CopySolution<R, ADS>& s = *solutions.at(randomIndex);
        S& s = *solutions.at(randomIndex);
        int id_s = randomIndex;

        XES se = make_pair(s, XEv());

        // ===================
        // apply a single move
        // ===================

        // TODO: consider that iterator can be null!
        uptr<NSIterator<XES, XEv>> it = nsenum->getIterator(se);
        int countMovesNSEnum = 0;
        int countValidMovesNSEnum = 0;

        for (it->first(); !it->isDone(); it->next()) {
          if (verbose)
            cout << endl;
          message(lNSEnum.at(id_nsenum), nqs, "getting current move (NSEnum tests).");

          // TODO: verify if it's not null!
          uptr<Move<XES, XEv, XES>> pmove = it->current();
          Move<XES, XEv, XES>& move = *pmove;
          countMovesNSEnum++;

          if (!move.canBeApplied(se)) {
            if (verbose) {
              cout << "move cannot be applied (NSEnum tests): ";
              move.print();
            }
            //delete &move;
            continue;
          }

          countValidMovesNSEnum++;
          // EXEC MOVES HERE

          //	bool testMoveNSSeq(int iter, NSSeq<R,ADS>* nsseq, int nqs, int id_nsseq, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, pair<int, double>& timeCloneSolution, vector<pair<int, double> >& timeInitializeADS, vector<pair<int, double> >& fullTimeEval, vector<pair<int, double> >& timeReeval, TimeNS& timeNS)
          // 	bool testMoveGeneral(int iter, NS<R,ADS>* ns, int id_ns, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, TimeCheckSol& timeSol, TimeNS& timeNS)

          if (!testMoveGeneral(nqs, nsenum, id_nsenum, s, id_s, move, evaluations, timeSamples)) {
            //delete &move;
            return false;
          }

          //delete &move;
        }

        vCountMovesEnumSamples[id_nsenum].push_back(countMovesNSEnum);
        vCountValidMovesEnumSamples[id_nsenum].push_back(countValidMovesNSEnum);

        //delete &it;
      }

      /*
         if (paramCheckIndependent) {
            cout << "checkcommand: will try to identify independent moves from '" << nsenum->id() << "' (can take some time... deactivate with 'paramCheckIndependent=false')" << endl;
            // indicate possible independent moves

            // adopting Evaluator 0...
            //Evaluator<S>& ev = *evaluators.at(0);
            Evaluator<S, XEv, XES>& ev = *evaluators.at(0);

            int countMovePairsEnum = 0;
            int countMoveIndependentEnum = 0;
            // check for independence between moves m1 and m2
            for (int m1 = 0; m1 < (int)nsenum->size(); m1++) {
               // slow...
               cout << "checkcommand: independence test for move #" << m1 << " / " << nsenum->size() << endl;
               int count_ind_m1 = 0;
               for (int m2 = m1 + 1; m2 < int(nsenum->size()); m2++) {
                  bool conflict = false;
                  // compute another move pair
                  countMovePairsEnum++;

                  if (!conflict) {
                     // TODO: increase performance of this method
                     for (unsigned sol = 0; sol < solutions.size(); sol++) {
                        //CopySolution<R, ADS>& s = *solutions.at(sol);
                        S& s = *solutions.at(sol);
                        XES se = make_pair(s, XEv());

                        // TODO: verify if return is return is not null!
                        uptr<Move<XES, XEv, XES>> move1 = nsenum->indexMove(m1);
                        uptr<Move<XES, XEv, XES>> move2 = nsenum->indexMove(m2);
                        // moves must be valid
                        if (!(move1->canBeApplied(se) && move2->canBeApplied(se)))
                           break;
                        // move 1 must have reverse
                        if (!move1->hasReverse()) {
                           cout << "checkcommand: NSEnum independent check expected reverse move... (deactivate with 'checkIndependent=false')" << endl;
                           break;
                        }

                        // calculate cost for move 2
                        ///MoveCost<>* cost_m2 = ev.moveCostComplete(move2, s);
                        op<XEv> cost_m2 = ev.moveCostComplete(*move2, se);

                        // apply move 1 (consider reverse is not nullptr)
                        uptr<Move<XES, XEv, XES>> rev_m1 = move1->apply(se);

                        // calculate new cost for move 2
                        ///MoveCost<>* cost2_m2 = ev.moveCostComplete(move2, s);
                        op<XEv> cost2_m2 = ev.moveCostComplete(*move2, se);

                        // return solution to original value and free
                        rev_m1->apply(se);
                        //delete &rev_m1;
                        //delete &move1;
                        //delete &move2;

                        // if costs differ, moves are not independent
                        if (!ev.equals(*cost2_m2, *cost_m2)) {
                           // mark conflict between m1 and m2
                           conflict = true;
                           //delete cost2_m2;
                           //delete cost_m2;
                           break;
                        }
                        //delete cost2_m2;
                        //delete cost_m2;
                     }

                     if (!conflict) {
                        // if here, m1 'could' be independent from m2
                        countMoveIndependentEnum++;
                        count_ind_m1++;
                        if (verbose) {
                           cout << "independent(m1=" << m1 << ";m2=" << m2 << ")" << endl;
                           nsenum->indexMove(m1)->print(); // TODO: fix leak
                           nsenum->indexMove(m2)->print(); // TODO: fix leak
                           cout << endl;
                        }
                     }
                  }
               }
               cout << "checkcommand: found " << count_ind_m1 << " independent move pairs." << endl;
            }

            //Aigor - Check if this counter is right - Any example was tested here
            vCountMovePairsEnumSamples[id_nsenum].push_back(countMovePairsEnum);
            vCountIndependentEnumSamples[id_nsenum].push_back(countMoveIndependentEnum);
         }
         */

      cout << "checkcommand: " << lNSEnum.at(id_nsenum)->id() << " finished." << endl;
    }
    return true;
  }

  bool doRunIndependent(int nSolNSSeq, TimeDataCheckCommand& timeSamples, SolDataCheckCommand<XES>& solData, CountDataCheckCommand<XES>& countData) {
    using SolOrRepType = typename XES::first_type;
    vector<std::shared_ptr<SolOrRepType>>& solutions = solData.solutions;
    //vector<vector<std::shared_ptr<XEv>>>& evaluations = solData.evaluations;
    vector<std::shared_ptr<Evaluator<SolOrRepType, XEv, XES>>>& evaluators = solData.evaluators;

    if (lNS.size() > 0)
      cout << "checkcommand  will test " << lNS.size() << " NS random components (nSolNSSeq=" << nSolNSSeq << " of numSolutions=" << solutions.size() << ")" << endl;

    //
    // NS
    //

    for (unsigned id_ns = 0; id_ns < lNS.size(); id_ns++) {
      std::shared_ptr<NS<XES, XEv>> ns = lNS.at(id_ns);

      cout << "checkcommand: testing NS " << id_ns << " => " << ns->toString();
      cout << endl;
      if (!ns->isSolutionIndependent()) {
        cout << "checkcommand: WARNING ignoring NS " << id_ns << " isSolutionIndependent() returns FALSE -> " << ns->toString();
        cout << endl;
        continue;
      }

      if (paramCheckIndependent) {
        cout << "checkcommand: will try to identify RANDOM independent moves from '" << ns->id() << "' (can take some time... deactivate with 'paramCheckIndependent=false')" << endl;
        // indicate possible independent moves

        // adopting Evaluator 0...
        //Evaluator<S>& ev = *evaluators.at(0);
        Evaluator<S, XEv, XES>& ev = *evaluators.at(0);

        int countMovePairs = 0;
        int countMoveIndependent = 0;
        //
        // count true positive
        int countTPMoveIndependent = 0;
        // count false positive
        int countFPMoveIndependent = 0;
        // count true negative
        int countTNMoveIndependent = 0;
        // count false negative
        int countFNMoveIndependent = 0;
        //
        // SOLUTION ZERO WILL BE REFERENCE (SHOULD NOT MATTER!!! BECAUSE OF isSolutionIndependent())
        //
        S& s = *solutions.at(0);
        XES se = make_pair(s, XEv());
        //
        std::cout << "checkcommand: randomizing " << nSolNSSeq * nSolNSSeq << " random moves" << std::endl;
        //

        //uptr<NSIterator<XES, XEv>> it = nsseq->getIterator(se);
        //int countMoves = 0;
        //int countValidMoves = 0;

        std::vector<Move<XES>*> allMoves;
        for (unsigned i = 0; i < (unsigned)nSolNSSeq * (unsigned)nSolNSSeq; i++)
          allMoves.push_back(ns->randomMove(se).release());

        //
        // check for independence between moves m1 and m2
        for (int m1 = 0; m1 < (int)allMoves.size(); m1++) {
          // slow...
          cout << "checkcommand: independence test for RANDOM move #" << m1 << " / " << allMoves.size() << endl;
          int count_ind_m1 = 0;
          for (int m2 = m1 + 1; m2 < int(allMoves.size()); m2++) {
            bool conflict = false;
            // compute another move pair
            countMovePairs++;

            if (!conflict) {
              // TODO: increase performance of this method
              for (unsigned sol = 0; sol < solutions.size(); sol++) {
                //CopySolution<R, ADS>& s = *solutions.at(sol);
                S& s = *solutions.at(sol);
                XES se = make_pair(s, XEv());

                // TODO: verify if return is return is not null!
                Move<XES, XEv, XES>* move1{allMoves.at(m1)};
                Move<XES, XEv, XES>* move2{allMoves.at(m2)};
                // moves must be valid
                if (!(move1->canBeApplied(se) && move2->canBeApplied(se)))
                  break;
                // move 1 must have reverse
                if (!move1->hasReverse()) {
                  cout << "checkcommand: NS independent RANDOM check expected reverse move... (deactivate with 'checkIndependent=false')" << endl;
                  break;
                }

                // calculate cost for move 2
                ///MoveCost<>* cost_m2 = ev.moveCostComplete(move2, s);
                op<XEv> cost_m2 = ev.moveCostComplete(*move2, se);

                // apply move 1 (consider reverse is not nullptr)
                uptr<Move<XES, XEv, XES>> rev_m1 = move1->apply(se);

                // calculate new cost for move 2
                ///MoveCost<>* cost2_m2 = ev.moveCostComplete(move2, s);
                op<XEv> cost2_m2 = ev.moveCostComplete(*move2, se);

                // return solution to original value and free
                rev_m1->apply(se);
                //delete &rev_m1;
                //delete &move1;
                //delete &move2;

                // leave pointers
                //move1.release();
                //move2.release();

                // if costs differ, moves are not independent
                if (!ev.equals(*cost2_m2, *cost_m2)) {
                  // mark conflict between m1 and m2
                  conflict = true;
                  //delete cost2_m2;
                  //delete cost_m2;
                  break;
                }
                //delete cost2_m2;
                //delete cost_m2;
                //
              }  // for (solutions)

              // ==================================================
              // check for "apparent" support of independence
              //
              if (!conflict) {
                // if here, m1 'could' be independent from m2
                countMoveIndependent++;
                count_ind_m1++;
                if (verbose) {
                  cout << "RANDOM independent(m1=" << m1 << ";m2=" << m2 << ")" << endl;
                  allMoves.at(m1)->print();  // TODO: fix leak
                  allMoves.at(m2)->print();  // TODO: fix leak
                  cout << endl;
                }
              }  // if (!conflict)

              // ==================================================
              // check if move independence is officially supported
              //
              if (ns->supportsMoveIndependence()) {
                bool realIndep = allMoves.at(m1)->independentOf(*allMoves.at(m2));
                bool indep = !conflict;
                // true positive
                countTPMoveIndependent += indep && realIndep;
                // false positive
                countFPMoveIndependent += indep && !realIndep;
                // true negative
                countTNMoveIndependent += !indep && !realIndep;
                // false negative
                countFNMoveIndependent += !indep && realIndep;
              }  // if officialy supported move independence
            }    // first if !conflict
          }      // for every move
          cout << "checkcommand: found " << count_ind_m1 << " independent move pairs." << endl;
        }

        std::cout << "checkcommand: RANDOM independent short summary {";
        std::cout << "countMovePairs=" << countMovePairs << " countMoveIndependent=" << countMoveIndependent << "}" << std::endl;
        std::cout << "checkcommand: RANDOM TP=" << countTPMoveIndependent << " TN=" << countTNMoveIndependent;
        std::cout << " FP=" << countFPMoveIndependent << " FN=" << countFNMoveIndependent << std::endl;

        double accuracy = ((double)countTPMoveIndependent + countTNMoveIndependent) / countMovePairs;
        double precision = ((double)countTPMoveIndependent) / (countTPMoveIndependent + countFPMoveIndependent);
        double recall = ((double)countTPMoveIndependent) / (countTPMoveIndependent + countFNMoveIndependent);

        if (!ns->supportsMoveIndependence())
          std::cout << "checkcommand: WARNING supportsMoveIndependence() is false, so Accuracy may be zero (no real independence was tested!)" << std::endl;
        std::cout << "checkcommand: RANDOM indep: accuracy=" << accuracy << " precision=" << precision << " recall=" << recall << std::endl;

        /*
            //Aigor - Check if this counter is right - Any example was tested here
            vCountMovePairsSamples[id_nsseq].push_back(countMovePairs);
            vCountIndependentSamples[id_nsseq].push_back(countMoveIndependent);
            //
            // add good and bad (if officially supported)
            vCountTPIndependentSamples[id_nsseq].push_back(countTPMoveIndependent);
            vCountTNIndependentSamples[id_nsseq].push_back(countTNMoveIndependent);
            vCountFPIndependentSamples[id_nsseq].push_back(countFPMoveIndependent);
            vCountFNIndependentSamples[id_nsseq].push_back(countFNMoveIndependent);
            //
            */

        // clears all used moves
        for (unsigned k = 0; k < allMoves.size(); k++)
          delete allMoves.at(k);
        allMoves.clear();
      }  // if param independent
    }    // for NS

    // ====================================================================
    // testing every NSSeq that isSolutionIndependent()
    // ====================================================================

    if (lNSSeq.size() > 0)
      cout << "checkcommand  will test " << lNSSeq.size() << " NSSeq (and NSEnum) iterator-based components (nSolNSSeq=" << nSolNSSeq << " of numSolutions=" << solutions.size() << ")" << endl;

    vector<vector<int>>& vCountIndependentSamples = countData.vCountIndependentSamples;
    vector<vector<int>>& vCountMovePairsSamples = countData.vCountMovePairsSamples;

    vector<vector<int>>& vCountTPIndependentSamples = countData.vCountTPIndependentSamples;
    vector<vector<int>>& vCountTNIndependentSamples = countData.vCountTNIndependentSamples;
    vector<vector<int>>& vCountFPIndependentSamples = countData.vCountFPIndependentSamples;
    vector<vector<int>>& vCountFNIndependentSamples = countData.vCountFNIndependentSamples;

    for (unsigned id_nsseq = 0; id_nsseq < lNSSeq.size(); id_nsseq++) {
      std::shared_ptr<NSSeq<XES, XEv>> nsseq = lNSSeq.at(id_nsseq);

      cout << "checkcommand: testing NSSeq " << id_nsseq << " => " << nsseq->toString();
      cout << endl;
      if (!nsseq->isSolutionIndependent()) {
        cout << "checkcommand: WARNING ignoring NSSeq " << id_nsseq << " isSolutionIndependent() returns FALSE -> " << nsseq->toString();
        cout << endl;
        continue;
      }

      if (paramCheckIndependent) {
        cout << "checkcommand: will try to identify independent moves from '" << nsseq->id() << "' (can take some time... deactivate with 'paramCheckIndependent=false')" << endl;
        // indicate possible independent moves

        // adopting Evaluator 0...
        //Evaluator<S>& ev = *evaluators.at(0);
        Evaluator<S, XEv, XES>& ev = *evaluators.at(0);

        int countMovePairs = 0;
        int countMoveIndependent = 0;
        //
        // count true positive
        int countTPMoveIndependent = 0;
        // count false positive
        int countFPMoveIndependent = 0;
        // count true negative
        int countTNMoveIndependent = 0;
        // count false negative
        int countFNMoveIndependent = 0;
        //
        // SOLUTION ZERO WILL BE REFERENCE (SHOULD NOT MATTER!!! BECAUSE OF isSolutionIndependent())
        //
        S& s = *solutions.at(0);
        XES se = make_pair(s, XEv());
        //
        uptr<NSIterator<XES, XEv>> it = nsseq->getIterator(se);
        //int countMoves = 0;
        //int countValidMoves = 0;

        std::vector<Move<XES>*> allMoves;

        for (it->first(); !it->isDone(); it->next()) {
          allMoves.push_back(it->current().release());
        }

        //
        // check for independence between moves m1 and m2
        for (int m1 = 0; m1 < (int)allMoves.size(); m1++) {
          // slow...
          cout << "checkcommand: independence test for move #" << m1 << " / " << allMoves.size() << endl;
          int count_ind_m1 = 0;
          for (int m2 = m1 + 1; m2 < int(allMoves.size()); m2++) {
            bool conflict = false;
            // compute another move pair
            countMovePairs++;

            if (!conflict) {
              // TODO: increase performance of this method
              for (unsigned sol = 0; sol < solutions.size(); sol++) {
                //CopySolution<R, ADS>& s = *solutions.at(sol);
                S& s = *solutions.at(sol);
                XES se = make_pair(s, XEv());

                // TODO: verify if return is return is not null!
                Move<XES, XEv, XES>* move1{allMoves.at(m1)};
                Move<XES, XEv, XES>* move2{allMoves.at(m2)};
                // moves must be valid
                if (!(move1->canBeApplied(se) && move2->canBeApplied(se)))
                  break;
                // move 1 must have reverse
                if (!move1->hasReverse()) {
                  cout << "checkcommand: NSSeq independent check expected reverse move... (deactivate with 'checkIndependent=false')" << endl;
                  break;
                }

                // calculate cost for move 2
                ///MoveCost<>* cost_m2 = ev.moveCostComplete(move2, s);
                op<XEv> cost_m2 = ev.moveCostComplete(*move2, se);

                // apply move 1 (consider reverse is not nullptr)
                uptr<Move<XES, XEv, XES>> rev_m1 = move1->apply(se);

                // calculate new cost for move 2
                ///MoveCost<>* cost2_m2 = ev.moveCostComplete(move2, s);
                op<XEv> cost2_m2 = ev.moveCostComplete(*move2, se);

                // return solution to original value and free
                rev_m1->apply(se);
                //delete &rev_m1;
                //delete &move1;
                //delete &move2;

                // leave pointers
                //move1.release();
                //move2.release();

                // if costs differ, moves are not independent
                if (!ev.equals(*cost2_m2, *cost_m2)) {
                  // mark conflict between m1 and m2
                  conflict = true;
                  //delete cost2_m2;
                  //delete cost_m2;
                  break;
                }
                //delete cost2_m2;
                //delete cost_m2;
                //
              }  // for (solutions)

              // ==================================================
              // check for "apparent" support of independence
              //
              if (!conflict) {
                // if here, m1 'could' be independent from m2
                countMoveIndependent++;
                count_ind_m1++;
                if (verbose) {
                  cout << "independent(m1=" << m1 << ";m2=" << m2 << ")" << endl;
                  allMoves.at(m1)->print();  // TODO: fix leak
                  allMoves.at(m2)->print();  // TODO: fix leak
                  cout << endl;
                }
              }  // if (!conflict)

              // ==================================================
              // check if move independence is officially supported
              //
              if (nsseq->supportsMoveIndependence()) {
                bool realIndep = allMoves.at(m1)->independentOf(*allMoves.at(m2));
                bool indep = !conflict;
                // true positive
                countTPMoveIndependent += indep && realIndep;
                // false positive
                countFPMoveIndependent += indep && !realIndep;
                // true negative
                countTNMoveIndependent += !indep && !realIndep;
                // false negative
                countFNMoveIndependent += !indep && realIndep;
              }  // if officialy supported move independence
            }    // first if !conflict
          }      // for every move
          cout << "checkcommand: found " << count_ind_m1 << " independent move pairs." << endl;
        }

        std::cout << "checkcommand: independent short summary {";
        std::cout << "countMovePairs=" << countMovePairs << " countMoveIndependent=" << countMoveIndependent << "}" << std::endl;

        double accuracy = ((double)countTPMoveIndependent + countTNMoveIndependent) / countMovePairs;
        double precision = ((double)countTPMoveIndependent) / (countTPMoveIndependent + countFPMoveIndependent);
        double recall = ((double)countTPMoveIndependent) / (countTPMoveIndependent + countFNMoveIndependent);

        std::cout << "checkcommand: indep: accuracy=" << accuracy << " precision=" << precision << " recall=" << recall << std::endl;

        //Aigor - Check if this counter is right - Any example was tested here
        vCountMovePairsSamples[id_nsseq].push_back(countMovePairs);
        vCountIndependentSamples[id_nsseq].push_back(countMoveIndependent);
        //
        // add good and bad (if officially supported)
        vCountTPIndependentSamples[id_nsseq].push_back(countTPMoveIndependent);
        vCountTNIndependentSamples[id_nsseq].push_back(countTNMoveIndependent);
        vCountFPIndependentSamples[id_nsseq].push_back(countFPMoveIndependent);
        vCountFNIndependentSamples[id_nsseq].push_back(countFNMoveIndependent);
        //

        // clears all used moves
        for (unsigned k = 0; k < allMoves.size(); k++)
          delete allMoves.at(k);
        allMoves.clear();
      }

      cout << "checkcommand: " << lNSSeq.at(id_nsseq)->id() << " finished." << endl;
    }
    return true;
  }

  void doPrintSummary(TimeDataCheckCommand& timeSamples, CountDataCheckCommand<XES>& countData) {
    cout << "===============================" << endl;
    cout << "           SUMMARY             " << endl;
    cout << "===============================" << endl
         << endl;

    printSingleSummarySamples("Solution", timeSamples.timeCloneSolution, "Time to clone a solution");

    printSummarySamples(convertVector(lConstructive), timeSamples.timeConstructive, "Constructive", "testing construction of initial solution");

#ifdef LEGACY_ADS
    if (adsMan)
      printSummarySamples(convertVector(lADSManagerComp), timeSamples.timeInitializeADS, "ADSManager::initializeADS()", "testing lazy initializeADS in solutions");
    else
      cout << endl
           << "No ADSManager was tested." << endl
           << endl;
#endif

    printSummarySamples(convertVector(lEvaluator), timeSamples.fullTimeEval, "Evaluators", "testing full evaluate(s) of a solution");

    printSummarySamples(convertVector(lNS), timeSamples.timeNSApply, "NS", "testing time of move apply(s) [apply_no_evaluation]");

    printSummarySamples(convertVector(lNS), timeSamples.timeNSCostApply, "NS", "testing time of cost based on move apply(s) [revCost]");

    printSummarySamples(convertVector(lNS), timeSamples.timeNSCostApplyDelta, "NS", "testing time of cost based on move apply(e, s) [fasterCost]");

    printSummarySamples(convertVector(lNS), timeSamples.timeNSCostApplyRealDelta, "NS", "testing time of real cost based on move apply(e, s) - forcing allowCosts to False [realFasterCost]");

    printSummarySamples(convertVector(lNS), timeSamples.timeNSCost, "NS", "testing time of move cost()");

    printSummarySamples(convertVector(lNS), timeSamples.timeNSEstimatedCost, "NS", "testing time of move estimatedCost()");

    printSummarySamples(convertVector(lNS), timeSamples.errorNSEstimatedCost, "NS", "testing error (%) of move estimatedCost()");

    printSummarySimpleSamples(convertVector(lNSSeq), countData.vCountMovesSamples, "NSSeq", "counting moves of NSSeq iterator");

    printSummarySimpleSamples(convertVector(lNSSeq), countData.vCountValidMovesSamples, "NSSeq", "counting valid moves of NSSeq iterator");

    printSummarySimpleSamples(convertVector(lNSEnum), countData.vCountMovesEnumSamples, "NSEnum", "counting moves of NSEnum iterator");

    printSummarySimpleSamples(convertVector(lNSEnum), countData.vCountValidMovesEnumSamples, "NSEnum", "counting valid moves of NSEnum iterator");

    printSummarySimpleSamples(convertVector(lNSSeq), countData.vCountMovePairsSamples, "NSSeq", "counting general move pairs for NSSeq with isSolutionIndependent()");

    printSummarySimpleSamples(convertVector(lNSSeq), countData.vCountIndependentSamples, "NSSeq", "counting independent move pairs for NSSeq with isSolutionIndependent()");

    // confusion matrix for multi improvement
    printSummarySimpleSamples(convertVector(lNSSeq), countData.vCountTPIndependentSamples, "NSSeq", "counting True Positive independence (estimated independent and it is officially independent) move pairs for NSSeq (officially supported) with isSolutionIndependent()");
    printSummarySimpleSamples(convertVector(lNSSeq), countData.vCountTNIndependentSamples, "NSSeq", "counting True Negative independent (estimated conflict and it is officially conflict) move pairs for NSSeq (officially supported) with isSolutionIndependent()");
    printSummarySimpleSamples(convertVector(lNSSeq), countData.vCountFPIndependentSamples, "NSSeq", "counting False Positive independent (estimated independent and it is officially conflict) move pairs for NSSeq (officially supported) with isSolutionIndependent()");
    printSummarySimpleSamples(convertVector(lNSSeq), countData.vCountFNIndependentSamples, "NSSeq", "counting False Negative independent (estimated conflict and it is officially independent) move pairs for NSSeq (officially supported) with isSolutionIndependent()");
  }

 private:
  AllDataCheckCommand<XES> doRun(int iterMax, int nSolNSSeq) {
    // ----------------
    // read evaluators
    // ----------------

    vector<std::shared_ptr<Evaluator<S, XEv, XES>>> evaluators;
    for (unsigned ev = 0; ev < lEvaluator.size(); ev++)
      evaluators.push_back(lEvaluator[ev]);

#ifdef LEGACY_ADS
    adsMan = nullptr;
    if (lADSManagerComp.size() > 0) {
      adsMan = lADSManagerComp[0];

      if (lADSManagerComp.size() > 1)
        cout << " checkcommand warning: more than 1 ADSManager (" << lADSManagerComp.size() << ")" << endl;
    }
#endif

    TimeDataCheckCommand timeSamples;
    timeSamples.initialize(lNS.size(), evaluators.size(), lConstructive.size());

    CountDataCheckCommand<XES> countData;
    countData.initialize(this->lNSSeq.size(), this->lNSEnum.size());

    // testing Constructive
    SolDataCheckCommand<XES> solData;
    solData.evaluators = evaluators;
    doRunConstructive(iterMax, timeSamples, solData);

    // testing Move
    doRunMove(iterMax, timeSamples, solData);

    // testing NS
    doRunNS(iterMax, timeSamples, solData);

    // testing NSSeq
    doRunNSSeq(nSolNSSeq, timeSamples, solData, countData);

    // testing NSEnum
    doRunNSEnum(nSolNSSeq, timeSamples, solData, countData);

    // testing independent
    doRunIndependent(nSolNSSeq, timeSamples, solData, countData);

    /*
      // SHARED POINTERS! DO NOT DELETE!
      for (unsigned i = 0; i < solData.solutions.size(); i++)
         delete solData.solutions[i];

      for (unsigned i = 0; i < solData.evaluations.size(); i++)
         for (unsigned j = 0; j < solData.evaluations[i].size(); j++)
            delete solData.evaluations[i][j];
*/
    // print summary
    doPrintSummary(timeSamples, countData);

    AllDataCheckCommand<XES> allData;
    //allData.timeData = timeSamples; // TODO: restore here!!!
    //allData.countData = countData;
    //allData.solData = solData;

    std::cout << "checkcommand: tests finished successfully!" << std::endl;
    return allData;
  }

  template <class T>
  vector<T*>
  assignVector(const vector<string> lComponents, T* type, HeuristicFactory<S, XEv, XES, X2ES>& factory) {
    vector<T*> vComp;
    for (unsigned i = 0; i < lComponents.size(); i++) {
      Scanner scan(lComponents.at(i));
      type = nullptr;
      factory.assign(type, scan.nextInt(), scan.next());  // reversed!

      if (!type) {
        cout << "checkcommand  warning: nullptr component " << lComponents[i] << "!" << endl;
      } else
        vComp.push_back(type);
    }

    return vComp;
  }

  template <class T>
  vector<std::shared_ptr<Component>>
  convertVector(const vector<sptr<T>>& v) {
    vector<sptr<Component>> vcomp;
    for (unsigned c = 0; c < v.size(); c++)
      vcomp.push_back((sptr<Component>)(v[c]));

    return vcomp;
  }

  void
  printSummarySimpleSamples(const vector<std::shared_ptr<Component>>& vcomp, const vector<vector<int>>& vMoveSamples, string type, string title) {
    unsigned nComponents = vMoveSamples.size();
    printf("---------------------------------\n");
    cout << "|" << type << "|=" << nComponents << "\t" << title << endl;
    printf("---------------------------------\n");
    printf("#id\ttitle\t#tests\ttotal\tavg\tstd \n");
    double avg = 0;
    int validValues = 0;
    for (unsigned id = 0; id < nComponents; id++) {
      int nSamples = vMoveSamples[id].size();
      if (nSamples > 0) {
        vector<double> vMoveSamplesIDDouble(vMoveSamples[id].begin(), vMoveSamples[id].end());
        KahanAccumulation init = {0};
        KahanAccumulation kahanSum = accumulate(vMoveSamplesIDDouble.begin(), vMoveSamplesIDDouble.end(), init, KahanSum);
        pair<double, double> avgStd = calculateAvgStd(vMoveSamplesIDDouble);
        printf("#%d\t%s\t%d\t%.4f\t%.4f\t%.4f\n", ((int)id), vcomp[id]->toString().c_str(), nSamples, kahanSum.sum, avgStd.first, avgStd.second);
        avg += avgStd.first;
        validValues++;
      } else
        printf("#%d\t%s\t%d\tUNTESTED OR UNIMPLEMENTED\n", ((int)id), vcomp[id]->toString().c_str(), 0);
    }
    printf("---------------------------------\n");
    printf("all\t*\t-\t%.4f\t-\n", (avg / validValues));
    cout << endl;
  }

  void
  printSingleSummarySamples(string component, const vector<double> vTimeSamples, string title) {
    printf("---------------------------------\n");
    cout << component << "\t" << title << endl;
    printf("---------------------------------\n");
    printf("title\t#tests\tavg(ms)\tstd(ms)\n");
    double avg = 0;
    int validValues = 0;
    int nSamples = vTimeSamples.size();
    if (nSamples > 0) {
      pair<double, double> avgStd = calculateAvgStd(vTimeSamples);
      printf("%s\t%d\t%.4f\t%.4f\n", component.c_str(), nSamples, avgStd.first, avgStd.second);
      avg += avgStd.first;
      validValues++;
    } else
      printf("%s\t%d\tUNTESTED OR UNIMPLEMENTED\n", component.c_str(), 0);
    printf("---------------------------------\n");
    printf("all\t-\t%.4f\t-\n", (avg / validValues));
    cout << endl;
  }

  void
  printSummarySamples(const vector<std::shared_ptr<Component>>& vcomp, const vector<vector<double>>& vTimeSamples, string type, string title) {
    unsigned nTests = vTimeSamples.size();
    printf("---------------------------------\n");
    cout << "|" << type << "|=" << nTests << "\t" << title << endl;
    printf("---------------------------------\n");
    printf("#id\ttitle\t#tests\tavg(ms)\tstd(ms)\n");
    double avg = 0;
    int validValues = 0;
    for (unsigned id = 0; id < nTests; id++) {
      int nSamples = vTimeSamples[id].size();
      if (nSamples > 0) {
        pair<double, double> avgStd = calculateAvgStd(vTimeSamples[id]);
        printf("#%d\t%s\t%d\t%.4f\t%.4f\n", ((int)id), vcomp[id]->toString().c_str(), nSamples, avgStd.first, avgStd.second);
        avg += avgStd.first;
        validValues++;
      } else
        printf("#%d\t%s\t%d\tUNTESTED OR UNIMPLEMENTED\n", ((int)id), vcomp[id]->toString().c_str(), 0);
    }
    printf("---------------------------------\n");
    printf("all\t*\t-\t%.4f\t-\n", (avg / validValues));
    cout << endl;
  }
};
}  // namespace optframe

#endif /* OPTFRAME_CHECK_COMMAND_HPP_ */
