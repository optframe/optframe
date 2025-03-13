#define NO_CXX_MODULES 1  // disable CXX_MODULES for optframe

#include <OptFrameLib/OptFrameLib.h>
//

// C
#include <assert.h>
// C++
#include <iomanip>  // std::setprecision
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFCore/EA/RK/FDecoderOpRK.hpp>
#include <OptFCore/FCore.hpp>
// #include <OptFCore/FxCore.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>  // XESolution
#include <OptFrame/Concepts/MyConcepts.hpp>    // sref
#include <OptFrame/Heuristics/EA/RK/BasicDecoderRandomKeys.hpp>
#include <OptFrame/Heuristics/EA/RK/BasicInitialEPopulationRK.hpp>
#include <OptFrame/Heuristics/MultiObjective/ClassicNSGAII.hpp>
#include <OptFrame/Hyper/HeuristicFactory.hpp>
#include <OptFrame/Hyper/Loader.hpp>
#include <OptFrame/Hyper/OptFrameList.hpp>
#include <OptFrame/Timer.hpp>
#include <OptFrame/Util/CheckCommand.hpp>
// C/C++ Library implementation
#include <OptFrameLib/FCoreLibSolution.hpp>

// ============================ Engine: HeuristicFactory
// ===========================

/*
using FCoreApi1Engine = optframe::HeuristicFactory<
  FCoreLibSolution,             //XSolution S,
  optframe::Evaluation<double>, // XEvaluation XEv = Evaluation<>,
  FCoreLibESolution             //  XESolution XES = pair<S, XEv>,
  //X2ESolution<XES> X2ES = MultiESolution<XES>>
  >;
*/

using CB = optframe::ComponentBuilder<FCoreLibESolution>;

using CBSingle = optframe::SingleObjSearchBuilder<FCoreLibESolution>;

// =============================================================================
// adopting GlobalSearch with XSH=XES
// meaning that, Search component is equals to its base type XES
// meaning [2] that, our BestType is XESolution (not Pareto, Population, etc)
// =============================================================================
using CBGlobal = optframe::GlobalSearchBuilder<FCoreLibESolution>;

using CBLocal = optframe::LocalSearchBuilder<FCoreLibESolution>;

using CB = optframe::ComponentBuilder<FCoreLibESolution>;

class FCoreApi1Engine {
 public:
  FCoreApi1Engine() {
    // ======================
    // setup basic parameters
    // ======================
    // parameter: NS_VALID_RANDOM_MOVE_MAX_TRIES
    this->experimentalParams["NS_VALID_RANDOM_MOVE_MAX_TRIES"] = "1";
    // parameter: EVTYPE_NUM_ZERO_PRECISION
    std::stringstream ss_zero;
    ss_zero << std::fixed << std::setprecision(8)
            << optframe::num_zero_precision<double>();
    this->experimentalParams["EVTYPE_NUM_ZERO_PRECISION"] = ss_zero.str();
    // parameter: ENGINE_LOG_LEVEL
    // from Component.hpp
    // |      1      |      2      |      3      |      4      |
    // |    error    |   warning   | information | debug/verb. | => this
    // direction = more verbose... 0 is silent
    this->experimentalParams["ENGINE_LOG_LEVEL"] = "2";
    // parameter: COMPONENT_LOG_LEVEL
    this->experimentalParams["COMPONENT_LOG_LEVEL"] = "3";
    // refresh engine parameters
    updateParameters();
  }

  void updateParameters() {
    int engine_ll = std::stoi(this->experimentalParams["ENGINE_LOG_LEVEL"]);
    int comp_ll = std::stoi(this->experimentalParams["COMPONENT_LOG_LEVEL"]);
    this->engineLogLevel = (optframe::LogLevel)engine_ll;
    this->componentLogLevel = (optframe::LogLevel)comp_ll;
    check.setLogLevel(engineLogLevel);
    loader.factory.setLogLevel(engineLogLevel);
  }

  std::map<std::string, std::string> experimentalParams;

  // =========================================================================
  // LOG SYSTEM: use 'engineLogLevel' for system logs, but 'componentLogLevel'
  // for specific components.
  //
  // engineLogLevel: 0-silent 1-error 2-warning 3-info 4-debug
  optframe::LogLevel engineLogLevel;
  // componentLogLevel: 0-silent 1-error 2-warning 3-info 4-debug
  optframe::LogLevel componentLogLevel;

  optframe::Loader<FCoreLibESolution  //  XESolution XES = pair<S, XEv>,
                                      // X2ESolution<XES> X2ES =
                                      // MultiESolution<XES>>
                   >
      loader;

  optframe::CheckCommand<FCoreLibESolution> check;  // no verbose
};

// IMPORTANT: OptFrame FMove does not require Copy on M (aka,
// FakePythonObjPtr)... I HOPE! Don't remember needing a clone() member on
// OptFrame Moves... but nice to clarify a NoCopy (NoNothing...) concept there.
// using FMoveLib = optframe::FMove<FakePythonObjPtr, FCoreLibESolution>;
// will not use FMove now, because of DecRef on destructor... a necessary
// personalization.

using optframe::Move;  // let's simplify things!!

// class FMoveLib : public optframe::Move<FCoreLibESolution, typename
// FCoreLibESolution::second_type>
template <typename XES = FCoreLibESolution>
class FMoveLib : public optframe::Move<XES> {
  // using XES = FCoreLibESolution;
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective
  using M = FakePythonObjPtr;

 public:
  M m;  // internal structure for move

  typedef std::function<M(const M&, XES&)> FuncTypeMoveApply;
  typedef std::function<bool(const M&, const XES&)> FuncTypeMoveCBA;
  typedef std::function<bool(const M&, const optframe::Move<XES>&)>
      FuncTypeMoveEq;
  typedef std::function<bool(FakePythonObjPtr)> FuncTypeUtilsDecRef;

  // M (*fApply)(const M&, XES&);                    // fApply
  FuncTypeMoveApply fApply;
  // bool (*fCanBeApplied)(const M&, const XES&) ;   // fCanBeApplied
  FuncTypeMoveCBA fCanBeApplied;
  // bool (*fCompareEq)(const M&, const Move<XES>&); // fCompareEq
  FuncTypeMoveEq fCompareEq;
  // utils for decref
  FuncTypeUtilsDecRef f_utils_decref;

  FMoveLib(
      M _m_owned,  // must IncRef before passing here...
                   // M (*_fApply)(const M&, XES&),                   // fApply
      const FuncTypeMoveApply& _fApply,
      // bool (*_fCanBeApplied)(const M&, const XES&) ,  // fCanBeApplied
      const FuncTypeMoveCBA& _fCanBeApplied,
      // bool (*_fCompareEq)(const M&, const Move<XES>&) // fCompareEq
      const FuncTypeMoveEq& _fCompareEq,
      // decref utils
      const FuncTypeUtilsDecRef& _f_utils_decref)
      : m{_m_owned},
        fApply{_fApply},
        fCanBeApplied{_fCanBeApplied},
        fCompareEq{_fCompareEq},
        f_utils_decref{_f_utils_decref} {}

  virtual ~FMoveLib() {
    // std::cout << "~FMoveLib()" << std::endl;
    // int x =
    f_utils_decref(m);
    // std::cout << "~FMoveLib count(m) = " << x << std::endl;
  }

  bool canBeApplied(const XES& se) override { return fCanBeApplied(m, se); }

  optframe::uptr<Move<XES>> apply(XSH& se) override {
    return optframe::uptr<Move<XES>>{new FMoveLib{
        fApply(m, se), fApply, fCanBeApplied, fCompareEq, f_utils_decref}};
  }

  bool operator==(const Move<XES>& move) const override {
    const Move<XES>& move2 = (Move<XES>&)move;
    return fCompareEq(this->m, move2);
  }

  bool operator!=(const Move<XES>& m) const { return !(*this == m); }

  static string idComponent() {
    std::stringstream ss;
    ss << optframe::Component::idComponent() << ":FMoveLib";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  // use 'operator<<' for M
  void print() const override { std::cout << m << std::endl; }
};

// Iterator object for NSSeqIterator
class IMSObjLib {
 public:
  // 'ims_ptr' is internal representation/solution pointer
  FakePythonObjPtr ims_ptr;
  // utils function
  std::function<bool(FakePythonObjPtr)> f_utils_decref;

  IMSObjLib(const IMSObjLib& s) = delete;
  IMSObjLib& operator=(const IMSObjLib& other) = delete;

  IMSObjLib& operator=(IMSObjLib&& other) {
    // is this possible?
    if (this == &other) return *this;
    // check if other exists
    assert(other.ims_ptr);
    // copy functions
    this->f_utils_decref = other.f_utils_decref;
    // steal pointer from corpse
    this->ims_ptr = other.ims_ptr;
    // kill it
    other.ims_ptr = 0;
    return *this;
  }

  IMSObjLib(IMSObjLib&& s) {
    // std::cout << "IMSObjLib(MOVE) BEGIN" << std::endl;
    assert(s.ims_ptr);
    // std::cout << "IMSObjLib(MOVE).other.toString() -> '" << s.toString() <<
    // "'" << std::endl;

    // copy
    this->f_utils_decref = std::move(s.f_utils_decref);

    // std::cout << "\tIMSObjLib(MOVE)-> will steal pointer" << std::endl;
    this->ims_ptr = s.ims_ptr;
    // prepare corpse
    s.ims_ptr = 0;
    //
    // std::cout << "\tIMSObjLib(MOVE finished; ptr=" << ims_ptr << ")" <<
    // std::endl; std::cout << "IMSObjLib(MOVE) ENDS" << std::endl;
  }

  virtual ~IMSObjLib() {
    // std::cout << "~IMSObjLib ptr: " << ims_ptr << std::endl;
    //
    if (this->ims_ptr) {
      // must decref solution_ptr and discard it
      int x = f_utils_decref(this->ims_ptr);
      // std::cout << "~FCoreLibSolution ptr_count = " << x << std::endl;
      if (x > 1) {
        // std::cout << "~IMSObjLib ptr_count = " << x << std::endl;
      }

      ims_ptr = 0;
    }
    // std::cout << "~IMSObjLib finished" << std::endl;
  }

  IMSObjLib(FakePythonObjPtr ims_ptr,
            std::function<int(FakePythonObjPtr)> f_utils_decref)
      : ims_ptr{ims_ptr}, f_utils_decref{f_utils_decref} {
    // printf("IMSObjLib1(%p, func)\n", ims_ptr);
  }

  // temporary construction (no copy_solution required)
  /*
  FCoreLibSolution(FakePythonObjPtr solution_ptr_view)
    : solution_ptr{ solution_ptr_view }
    , is_view{ true }
  {
     //printf("FCoreLibSolution1(%p) is_view=%d\n", solution_ptr, is_view);
  }
  */

  /*
  FakePythonObjPtr
  releasePtr()
  {
     // pointer must exist
     assert(this->solution_ptr);
     // cannot take this from view
     assert(!this->is_view);
     FakePythonObjPtr sol = this->solution_ptr;
     // "move" from this container
     this->solution_ptr = nullptr;
     this->is_view = true;
     // TODO: do we need to remove the functions too?
     return sol;
  }
  */
};

// ==================

OPT_MODULE_API void optframe_api0d_engine_welcome(FakeEnginePtr _engine) {
  // IGNORING LOG LEVEL FOR WELCOME! ASSUMING AS DEBUG FUNCTION!
  // if(_engine->engineLogLevel >= optframe::LogLevel::Error)
  std::cout << optframe::FCore::welcome() << std::endl;
}

OPT_MODULE_API FakeEnginePtr optframe_api1d_create_engine(int ll) {
  // safety checks on log level
  if (ll <= 0) ll = 0;
  if (ll >= 4) ll = 4;
  auto l = (optframe::LogLevel)ll;
  auto* _eng = new FCoreApi1Engine;
  if (l == optframe::LogLevel::Debug)
    std::cout << "Debug: will set OptFrame Engine loglevel to Debug"
              << std::endl;
  //
  _eng->experimentalParams["ENGINE_LOG_LEVEL"] = std::to_string(ll);
  _eng->updateParameters();
  //
  FakeEnginePtr engine_ptr = _eng;
  return engine_ptr;
}

OPT_MODULE_API bool optframe_api1d_engine_check(FakeEnginePtr _engine, int p1,
                                                int p2, bool verbose,
                                                bool (*_fOnFail)(int)) {
  auto* engine = (FCoreApi1Engine*)_engine;
  if (verbose) engine->check.setParameters(verbose);
  engine->check.onFail = [_fOnFail](int code) -> bool {
    return _fOnFail(code);
  };
  // bool run(int iterMax, int nSolNSSeq)
  auto data = engine->check.run(p1, p2);
  return true;
}

OPT_MODULE_API LibSearchOutput
optframe_api0d_engine_simulated_annealing(FakeEnginePtr _engine) {
  return optframe_api0d_engine_simulated_annealing_params(_engine, 3.0, 0, 0, 0,
                                                          0.99, 100, 9999);
}

OPT_MODULE_API int optframe_api1d_engine_list_builders(FakeEnginePtr _engine,
                                                       char* prefix) {
  std::string sprefix{prefix};
  auto* engine = (FCoreApi1Engine*)_engine;
  std::vector<
      std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
      vlist = engine->loader.factory.listBuilders(sprefix);

  // This will be printed, regardless of message level
  for (auto& p : vlist) {
    std::cout << "builder: " << p.first << " |params|=" << p.second.size()
              << std::endl;
    for (unsigned i = 0; i < p.second.size(); i++)
      std::cout << "\tparam " << i << " => " << p.second[i].first << " : "
                << p.second[i].second << std::endl;
  }

  return vlist.size();
}

OPT_MODULE_API int optframe_api1d_engine_list_components(FakeEnginePtr _engine,
                                                         char* prefix) {
  std::string sprefix{prefix};
  auto* engine = (FCoreApi1Engine*)_engine;
  std::vector<std::string> vlist =
      engine->loader.factory.listComponents(sprefix);

  // This will be printed, regardless of message level
  for (unsigned i = 0; i < vlist.size(); i++)
    std::cout << "component " << i << " => " << vlist[i] << std::endl;

  return vlist.size();
}

OPT_MODULE_API int  // index of ComponentList
optframe_api1d_create_component_list(FakeEnginePtr _engine, char* clist,
                                     char* list_type) {
  auto* engine = (FCoreApi1Engine*)_engine;
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug)
    std::cout << "DEBUG: create_component_list" << std::endl;
  //
  std::string str_list{clist};

  std::string str_type{list_type};
  //
  std::map<std::string, std::vector<std::string>>
      ldictionary;  // TODO(igormcoelho): why??
  //
  std::vector<std::string>* vvlist =
      optframe::OptFrameList::readList(ldictionary, str_list);
  //
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug) {
    std::cout << "create_component_list readList:: size=" << vvlist->size()
              << std::endl;
    std::cout << vvlist->at(0) << std::endl;
  }
  std::vector<sptr<optframe::Component>> vcomp;
  for (unsigned i = 0; i < vvlist->size(); i++) {
    sptr<optframe::Component> comp;
    scannerpp::Scanner scanComp{vvlist->at(i)};
    std::string sid = scanComp.next();
    int cid = *scanComp.nextInt();
    engine->loader.factory.assign(comp, cid, sid);
    vcomp.push_back(comp);
  }
  delete vvlist;
  // std::cout << "DEBUG: Component List size=" << vcomp.size() << std::endl;

  int id = engine->loader.factory.addComponentList(vcomp, str_type);

  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug)
    std::cout << "DEBUG: end create_component_list id=" << id << std::endl;
  return id;
}

OPT_MODULE_API LibSearchOutput optframe_api0d_engine_simulated_annealing_params(
    FakeEnginePtr _engine, double timelimit, int id_evaluator,
    int id_constructive, int id_ns, double alpha, int iter, double T) {
  auto* engine = (FCoreApi1Engine*)_engine;

  //
  using MyEval =
      optframe::Evaluator<FCoreLibSolution, optframe::Evaluation<double>,
                          FCoreLibESolution>;

  // will try to get evaluator to build InitialSolution component...
  std::shared_ptr<MyEval> _ev;
  engine->loader.factory.assign(_ev, id_evaluator,
                                "OptFrame:GeneralEvaluator:Evaluator");
  assert(_ev);
  sref<MyEval> single_ev{_ev};
  //
  //
  using MyConstructive = optframe::Constructive<FCoreLibSolution>;
  //
  std::shared_ptr<MyConstructive> initial;
  engine->loader.factory.assign(initial, id_constructive,
                                "OptFrame:Constructive");
  assert(initial);
  //
  sref<optframe::InitialSearch<FCoreLibESolution>> initSol{
      new optframe::BasicInitialSearch<FCoreLibESolution>(initial, single_ev)};
  //
  using MyNS = optframe::NS<FCoreLibESolution>;
  //
  std::shared_ptr<MyNS> ns;
  engine->loader.factory.assign(ns, id_ns, "OptFrame:NS");
  assert(ns);
  //

  sref<optframe::RandGen> rg = engine->loader.factory.getRandGen();

  // sref<optframe::GeneralEvaluator<FCoreLibESolution,
  // optframe::Evaluation<double>>> evaluator{ gev };
  sref<optframe::InitialSearch<FCoreLibESolution>> constructive{initSol};
  vsref<optframe::NS<FCoreLibESolution>> neighbors;
  neighbors.push_back(ns);

  single_ev->print();
  constructive->print();
  neighbors[0]->print();

  optframe::BasicSimulatedAnnealing<FCoreLibESolution> sa{
      single_ev, constructive, neighbors, alpha, iter, T, rg};
  sa.setVerbose();

  optframe::SearchOutput<FCoreLibESolution> out = sa.search({timelimit});
  // std::cout << "out=" << out.status << std::endl;

  LibSearchOutput lout;
  lout.status = (int)out.status;   // ("status", c_int),
  lout.has_best = (bool)out.best;  // ("has_best", ctypes.c_bool),
  if (out.best) {
    // extract pointer from solution container
    lout.best_s =
        out.best->first.releasePtr();  // ("best_s", ctypes.py_object),
    lout.best_e =
        out.best->second.evaluation();  // ("best_e", ctypes.c_double)]
  }
  return lout;
}

OPT_MODULE_API int  // ONLY STATUS... CANNOT RETURN PARETO ELEMENTS NOW...
optframe_api0d_engine_classic_nsgaii_params(FakeEnginePtr _engine,
                                            double timelimit, double min_limit,
                                            double max_limit, int id_mevaluator,
                                            int id_popman, int popSize,
                                            int maxIter) {
  auto* engine = (FCoreApi1Engine*)_engine;

  //
  using MyMEval =
      optframe::MultiEvaluator<FCoreLibESolution, FCoreLibEMSolution>;

  // will try to get evaluator to build InitialSolution component...
  std::shared_ptr<MyMEval> _mev;
  engine->loader.factory.assign(
      _mev, id_mevaluator, "OptFrame:GeneralEvaluator<XMESf64>:MultiEvaluator");
  assert(_mev);
  sref<MyMEval> multi_ev{_mev};
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Info)
    std::cout << "NDIRECTIONS = " << multi_ev->vDir.size() << std::endl;
  //
  using MyMOPopManager = optframe::MOPopulationManagement<FCoreLibEMSolution>;
  //
  std::shared_ptr<MyMOPopManager> mopop;
  engine->loader.factory.assign(mopop, id_popman,
                                "OptFrame:MOPopulationManagement");
  assert(mopop);
  //

  sref<optframe::RandGen> rg = engine->loader.factory.getRandGen();

  // std::cout << "WARNING: hardcoding limits 0, 100000 for bi-objective
  // nsga-ii" << std::endl; multi_ev->vDir[0]->setLimits(0, 100000);
  // multi_ev->vDir[1]->setLimits(0, 100000);
  for (unsigned i = 0; i < multi_ev->vDir.size(); i++)
    multi_ev->vDir[i]->setLimits(min_limit, max_limit);

  optframe::ClassicNSGAII<FCoreLibEMSolution> classic_nsgaii{
      multi_ev, mopop, (unsigned)popSize, maxIter};

  // sa.setVerbose();

  auto sout = classic_nsgaii.search({timelimit});
  //
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Info) {
    std::cout << "finished classic_nsgaii with: ";
    std::cout << "status=" << sout.status << std::endl;
  }
  optframe::Pareto<FCoreLibEMSolution> best = *sout.best;

  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Info) {
    std::cout << "best pareto: ";
    // best pareto front
    best.print();
  }
  //
  return 0;
}

OPT_MODULE_API int  // index of GlobalSearch
optframe_api1d_build_global(FakeEnginePtr _engine, char* builder,
                            char* build_string) {
  auto* engine = (FCoreApi1Engine*)_engine;
  // =============================
  //     build_single (TESTING)
  // =============================
  std::string strBuilder{builder};
  std::string strBuildString{build_string};

  //
  // Example: "OptFrame:ComponentBuilder:GlobalSearch:SA:BasicSA"
  //
  std::string sbuilder = strBuilder;
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug)
    std::cout << "OptFrame Engine GET BUILDER: " << sbuilder << std::endl;
  CB* cb = engine->loader.factory.getBuilder(sbuilder);
  if (!cb) {
    if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Warning)
      std::cout << "WARNING! OptFrame builder for GlobalSearch not found!"
                << std::endl;
    return -1;
  }
  // cb->buildComponent
  CBGlobal* cbglobal = (CBGlobal*)cb;

  //
  // Example: "OptFrame:GeneralEvaluator:Evaluator 0 OptFrame:InitialSearch 0
  // OptFrame:NS[] 0 0.99 100 999";
  //
  std::string scan_params = strBuildString;
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug)
    std::cout << "OptFrame Engine BUILD: " << scan_params << std::endl;
  scannerpp::Scanner scanner{scan_params};
  optframe::GlobalSearch<FCoreLibESolution>* global =
      cbglobal->build(scanner, engine->loader.factory);
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug) {
    std::cout << "global_search =" << global << std::endl;
    global->print();
  }
  //

  optframe::Component* cglobal = (optframe::Component*)global;  // why??
  cglobal->setMessageLevel(engine->componentLogLevel);
  sptr<optframe::Component> sptrGlobal{cglobal};

  int id =
      engine->loader.factory.addComponent(sptrGlobal, "OptFrame:GlobalSearch");
  return id;
}

OPT_MODULE_API int  // index of SingleObjSearch
optframe_api1d_build_single(FakeEnginePtr _engine, char* builder,
                            char* build_string) {
  auto* engine = (FCoreApi1Engine*)_engine;
  // =============================
  //     build_single (TESTING)
  // =============================
  std::string strBuilder{builder};
  std::string strBuildString{build_string};

  //
  // Example: "OptFrame:ComponentBuilder:SingleObjSearch:SA:BasicSA"
  //
  std::string sbuilder = strBuilder;
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug)
    std::cout << "OptFrame Engine GET BUILDER: " << sbuilder << std::endl;
  CB* cb = engine->loader.factory.getBuilder(sbuilder);
  if (!cb) {
    if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Warning)
      std::cout << "WARNING! OptFrame builder for SingleObjSearch not found!"
                << std::endl;
    return -1;
  }
  // cb->buildComponent
  CBSingle* cbsingle = (CBSingle*)cb;

  //
  // Example: "OptFrame:GeneralEvaluator:Evaluator 0 OptFrame:InitialSearch 0
  // OptFrame:NS[] 0 0.99 100 999";
  //
  std::string scan_params = strBuildString;
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug)
    std::cout << "OptFrame Engine BUILD: " << scan_params << std::endl;
  scannerpp::Scanner scanner{scan_params};
  optframe::SingleObjSearch<FCoreLibESolution>* single =
      cbsingle->build(scanner, engine->loader.factory);
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug) {
    std::cout << "single =" << single << std::endl;
    single->print();
  }
  //

  optframe::Component* csingle = (optframe::Component*)single;  // why??
  csingle->setMessageLevel(engine->componentLogLevel);
  sptr<optframe::Component> sptrSingle{csingle};

  int id = engine->loader.factory.addComponent(
      sptrSingle, "OptFrame:GlobalSearch:SingleObjSearch");
  return id;
}

OPT_MODULE_API int  // index of LocalSearch
optframe_api1d_build_local_search(FakeEnginePtr _engine, char* builder,
                                  char* build_string) {
  auto* engine = (FCoreApi1Engine*)_engine;
  // =============================
  //     build_single (TESTING)
  // =============================
  std::string strBuilder{builder};
  std::string strBuildString{build_string};

  //
  // Example: "OptFrame:ComponentBuilder:SingleObjSearch:SA:BasicSA"
  //
  std::string sbuilder = strBuilder;
  CB* cb = engine->loader.factory.getBuilder(sbuilder);
  if (!cb) {
    if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Warning)
      std::cout << "WARNING! OptFrame builder for LocalSearch not found!"
                << std::endl;
    return -1;
  }
  // cb->buildComponent
  CBLocal* cblocal = (CBLocal*)cb;

  //
  // Example: "OptFrame:GeneralEvaluator:Evaluator 0 OptFrame:InitialSearch 0
  // OptFrame:NS[] 0 0.99 100 999";
  //
  std::string scan_params = strBuildString;
  scannerpp::Scanner scanner{scan_params};
  optframe::LocalSearch<FCoreLibESolution>* local =
      cblocal->build(scanner, engine->loader.factory);
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug) {
    std::cout << "local_search =" << local << std::endl;
    local->print();
  }
  //

  optframe::Component* clocal = (optframe::Component*)local;  // why??
  clocal->setMessageLevel(engine->componentLogLevel);
  sptr<optframe::Component> sptrLocal{clocal};

  int id =
      engine->loader.factory.addComponent(sptrLocal, "OptFrame:LocalSearch");
  return id;
}

OPT_MODULE_API int  // index of Component
optframe_api1d_build_component(FakeEnginePtr _engine, char* builder,
                               char* build_string, char* component_type) {
  auto* engine = (FCoreApi1Engine*)_engine;
  // =============================
  //     build_component
  // =============================
  std::string strBuilder{builder};
  std::string strBuildString{build_string};
  std::string strComponentType{component_type};

  //
  // Example: "OptFrame:ComponentBuilder:ILS:LevelPert:LPlus2"
  //
  std::string sbuilder = strBuilder;
  CB* cb = engine->loader.factory.getBuilder(sbuilder);
  if (!cb) {
    if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Warning)
      std::cout << "WARNING! OptFrame builder '" << strBuilder
                << "' for Component not found!" << std::endl;
    return -1;
  }

  //
  // Example: "OptFrame:GeneralEvaluator:Evaluator 0 OptFrame:NS 0";
  //
  std::string scan_params = strBuildString;
  scannerpp::Scanner scanner{scan_params};
  optframe::Component* c = cb->buildComponent(scanner, engine->loader.factory);
  // use componentLogLevel
  c->setMessageLevel(engine->componentLogLevel);
  // use engineLogLevel (this is correct!)
  if (engine->engineLogLevel >= optframe::LogLevel::Debug) {
    std::cout << "component =" << c << std::endl;
    c->print();
  }
  //
  sptr<optframe::Component> sptrComp{c};

  int id = engine->loader.factory.addComponent(sptrComp, strComponentType);
  return id;
}

OPT_MODULE_API LibSearchOutput  // SearchOutput for XSH "best-type"
optframe_api1d_run_global_search(FakeEnginePtr _engine, int g_idx,
                                 double timelimit) {
  auto* engine = (FCoreApi1Engine*)_engine;
  //
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug)
    std::cout << "begin C++ optframe_api1d_run_global_search: g_idx=" << g_idx
              << " timelimit=" << timelimit << std::endl;
  //
  sptr<optframe::GlobalSearch<FCoreLibESolution, FCoreLibESolution>> gs;
  engine->loader.factory.assign(gs, g_idx, "OptFrame:GlobalSearch");
  assert(gs);
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug) {
    gs->print();
  }
  // sos->setVerbose();
  //
  //  ===============================
  //  use same log level from engine
  //  NOPE! now it's fine-grained to each component... respect that!
  //  ===============================
  //
  /*
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug)
     gs->setVerboseR();
  else if (engine->loader.factory.getLogLevel() == optframe::LogLevel::Silent)
     gs->setSilentR();
  else {
     // no recursive here? TODO: fix with setMessageLevelR...
     gs->setMessageLevel(engine->loader.factory.getLogLevel());
  }
  */
  // ===============================
  //
  optframe::SearchOutput<FCoreLibESolution> out = gs->search({timelimit});
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Info)
    std::cout << "run_global_search -> out=" << out.status << std::endl;

  LibSearchOutput lout;
  lout.status = (int)out.status;   // ("status", c_int),
  lout.has_best = (bool)out.best;  // ("has_best", ctypes.c_bool),
  if (out.best) {
    // extract pointer from solution container
    lout.best_s =
        out.best->first.releasePtr();  // ("best_s", ctypes.py_object),
    lout.best_e =
        out.best->second.evaluation();  // ("best_e", ctypes.c_double)]
  }
  return lout;
}

OPT_MODULE_API LibSearchOutput  // SearchOutput for XSH "best-type"
optframe_api1d_run_sos_search(FakeEnginePtr _engine, int sos_idx,
                              double timelimit) {
  auto* engine = (FCoreApi1Engine*)_engine;
  //
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug)
    std::cout << "begin C++ optframe_api1d_run_sos_search: sos_idx=" << sos_idx
              << " timelimit=" << timelimit << std::endl;
  //
  sptr<optframe::SingleObjSearch<FCoreLibESolution>> sos;
  engine->loader.factory.assign(sos, sos_idx,
                                "OptFrame:GlobalSearch:SingleObjSearch");
  assert(sos);
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug)
    sos->print();
  // sos->setVerbose();
  //
  //  ===============================
  //  use same log level from engine
  //  NOPE! now it's fine-grained to each component... respect that!
  //  ===============================
  //
  /*
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug)
     sos->setVerboseR();
  else if (engine->loader.factory.getLogLevel() == optframe::LogLevel::Silent)
     sos->setSilentR();
  else {
     // no recursive here? TODO: fix with setMessageLevelR...
     sos->setMessageLevel(engine->loader.factory.getLogLevel());
  }
  */
  // ===============================

  optframe::SearchOutput<FCoreLibESolution> out = sos->search({timelimit});

  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Info)
    std::cout << "run_sos_search -> out=" << out.status << std::endl;

  LibSearchOutput lout;
  lout.status = (int)out.status;   // ("status", c_int),
  lout.has_best = (bool)out.best;  // ("has_best", ctypes.c_bool),
  if (out.best) {
    // extract pointer from solution container
    lout.best_s =
        out.best->first.releasePtr();  // ("best_s", ctypes.py_object),
    lout.best_e =
        out.best->second.evaluation();  // ("best_e", ctypes.c_double)]
  }
  return lout;
}

OPT_MODULE_API bool optframe_api0d_engine_test(FakeEnginePtr _engine) {
  auto* engine = (FCoreApi1Engine*)_engine;
  //
  using MyEval =
      optframe::Evaluator<FCoreLibSolution, optframe::Evaluation<double>,
                          FCoreLibESolution>;

  // will try to get evaluator to build InitialSolution component...
  std::shared_ptr<MyEval> ev;
  engine->loader.factory.assign(ev, 0, "OptFrame:GeneralEvaluator:Evaluator");
  assert(ev);
  sref<MyEval> ev2{ev};

  using MyIEval = optframe::IEvaluator<FCoreLibESolution>;

  sref<MyIEval> iev2{ev2};

  //
  //
  using MyConstructive = optframe::Constructive<FCoreLibSolution>;
  //
  std::shared_ptr<MyConstructive> initial;
  engine->loader.factory.assign(initial, 0, "OptFrame:Constructive");
  assert(initial);
  //
  sref<optframe::InitialSearch<FCoreLibESolution>> initSol{
      new optframe::BasicInitialSearch<FCoreLibESolution>(initial, iev2)};
  if (engine->loader.factory.getLogLevel() >= optframe::LogLevel::Debug)
    std::cout << "### THIS IS DEBUG LEVEL!" << std::endl;
  //
  std::cout << "### test will generate solution" << std::endl;
  auto ose_status = initSol->initialSearch({10.0});
  std::cout << "### test will copy optional" << std::endl;
  auto ose = ose_status.first;
  std::cout << "### test will get 'se' from optional" << std::endl;
  auto se = *ose;
  std::cout << "### test will copy 'se'" << std::endl;
  auto se2 = se;

  return true;
}

OPT_MODULE_API bool optframe_api1d_destroy_engine(FakeEnginePtr _engine) {
  auto* engine = (FCoreApi1Engine*)_engine;
  delete engine;
  // good
  return true;
}

// ==============================================
//                      ADD
// ==============================================

// min_or_max is needed to correctly cast template on FEvaluator
OPT_MODULE_API int  // index of generalevaluator

optframe_api1d_add_evaluator(FakeEnginePtr _engine,
                             double (*_fevaluate)(FakePythonObjPtr,
                                                  FakePythonObjPtr),
                             bool min_or_max, FakePythonObjPtr problem_view) {
  auto* engine = (FCoreApi1Engine*)_engine;
  // printf("hf=%p\n", (void*)hf);

  auto fevaluate =
      [_fevaluate, problem_view](
          const FCoreLibSolution& s) -> optframe::Evaluation<double> {
    // printf("will invoke _fevaluate(%p) over s.solution_ptr = %p\n",
    // (void*)_fevaluate, s.solution_ptr);
    double r = _fevaluate(problem_view, s.solution_ptr);
    // printf("return r=%f\n", r);
    return r;
  };

  int id = -1;
  if (min_or_max) {
    // Minimization
    auto* ev_ptr =
        new optframe::FEvaluator<FCoreLibESolution,
                                 optframe::MinOrMax::MINIMIZE>{fevaluate};
    sref<optframe::Evaluator<FCoreLibSolution, optframe::Evaluation<double>,
                             FCoreLibESolution>>
        eval2(ev_ptr);
    sref<optframe::Component> eval(eval2);
    // std::cout << "created FEvaluator<MIN> ptr=" << &eval.get() << std::endl;
    // id = engine->loader.factory.addComponent(eval,
    // "OptFrame:GeneralEvaluator");
    //
    //  double add to prevent future down-casts (NOT ANYMORE)
    //
    id = engine->loader.factory.addComponent(
        eval, "OptFrame:GeneralEvaluator:Evaluator");
    int id2 =
        engine->loader.factory.addComponent(eval, "OptFrame:GeneralEvaluator");

    if (!(id == id2)) {
      return -1;
    }
    // also add to check module
    engine->check.addEvaluator(eval2);
  } else {
    // Maximization
    auto* ev_ptr =
        new optframe::FEvaluator<FCoreLibESolution,
                                 optframe::MinOrMax::MAXIMIZE>{fevaluate};
    sref<optframe::Evaluator<FCoreLibSolution, optframe::Evaluation<double>,
                             FCoreLibESolution>>
        eval2(ev_ptr);
    sref<optframe::Component> eval(eval2);

    // id = engine->loader.factory.addComponent(eval,
    // "OptFrame:GeneralEvaluator");
    //  double add to prevent future down-casts (NOT ANYMORE)
    id = engine->loader.factory.addComponent(
        eval, "OptFrame:GeneralEvaluator:Evaluator");
    int id2 =
        engine->loader.factory.addComponent(eval, "OptFrame:GeneralEvaluator");
    if (!(id == id2)) {
      return -1;
    }
    // also add to check module
    engine->check.addEvaluator(eval2);
  }

  return id;
}

// IMPORTANT: method 'add_constructive' receives a 'problem_view', while a
// 'problem_owned' would be desired... However, this would require an extra 'int
// (*f_utils_decref)(FakePythonObjPtr)', but it's doable. Worse, this would also
// require some personalization of std::function destructor over FConstructive,
// or a change in FConstructor to allow personalized destructors...
// Maybe, at this moment, we just require that 'problem_view' must exist at the
// time constructive is invoked. We can somehow ensure this on Python, which
// prevents extra referencing counting on both sides. On Python, storing it on
// the engine (or the opposite) may do the job for us, so, no worry for now.

OPT_MODULE_API int  // index of constructive
optframe_api1d_add_constructive(
    FakeEnginePtr _engine, FakePythonObjPtr (*_fconstructive)(FakePythonObjPtr),
    FakePythonObjPtr problem_view,
    // Support necessary for Solution construction and maintainance
    FakePythonObjPtr (*f_sol_deepcopy)(FakePythonObjPtr),
    size_t (*f_sol_tostring)(FakePythonObjPtr, char*, size_t),
    int (*f_utils_decref)(FakePythonObjPtr)) {
  auto* engine = (FCoreApi1Engine*)_engine;

  // std::cout << "invoking 'optframe_api1d_add_constructive' with "
  //           << "_hf=" << _hf << " _fconstructive and problem_view=" <<
  //           problem_view << std::endl;

  auto fconstructive = [_fconstructive, problem_view, f_sol_deepcopy,
                        f_sol_tostring, f_utils_decref]() -> FCoreLibSolution {
    // IMPORTANT: _fconstructive must IncRef solution on python before
    // returning! I think so...
    FakePythonObjPtr vobj_owned = _fconstructive(problem_view);
    // std::cout << "'optframe_api1d_add_constructive' -> _fconstructive
    // generated pointer: " << vobj_owned << std::endl;
    assert(
        vobj_owned);  // check void* (TODO: for FxConstructive, return nullopt)
    FCoreLibSolution sol(vobj_owned, f_sol_deepcopy, f_sol_tostring,
                         f_utils_decref);
    // std::cout << "'optframe_api1d_add_constructive' -> solution created!" <<
    // std::endl;
    return sol;
  };

  auto* c_ptr = new optframe::FConstructive<FCoreLibSolution>{fconstructive};

  sref<optframe::Constructive<FCoreLibSolution>> fc2(c_ptr);
  sref<optframe::Component> fc(fc2);

  // std::cout << "'optframe_api1d_add_constructive' will add component on hf"
  // << std::endl;

  int id = engine->loader.factory.addComponent(fc, "OptFrame:Constructive");

  // std::cout << "c_id = " << id << std::endl;
  //  ========== add to check module ==========
  using MyEval =
      optframe::Evaluator<FCoreLibSolution, optframe::Evaluation<double>,
                          FCoreLibESolution>;

  // will try to get evaluator to build InitialSolution component...
  std::shared_ptr<MyEval> ev;
  engine->loader.factory.assign(ev, 0, "OptFrame:GeneralEvaluator:Evaluator");
  assert(ev);

  using MyIEval = optframe::IEvaluator<FCoreLibESolution>;

  sref<MyEval> ref_ev{ev};
  sref<MyIEval> iev{ref_ev};
  //
  if (!ev)
    std::cout << "WARNING: No Evaluator! Cannot build InitialSearch for "
                 "Constructive id="
              << id << "!" << std::endl;
  else {
    // auto ev = std::make_shared<optframe::Evaluator<FCoreLibSolution,
    // optframe::Evaluation<double>, FCoreLibESolution>>(
    //   gev);
    // std::shared_ptr<MyEval>
    //   ev(
    //     std::static_pointer_cast<MyEval>(gev));

    sref<optframe::InitialSearch<FCoreLibESolution>> initSol{
        new optframe::BasicInitialSearch<FCoreLibESolution>(fc2, iev)};
    engine->check.add(initSol);
  }
  // fc->print();
  return id;
}

OPT_MODULE_API int  // index of general crossover
optframe_api0d_add_general_crossover(
    FakeEnginePtr _engine,
    FakePythonObjPtr (*_fcross1)(FakePythonObjPtr, FakePythonObjPtr,
                                 FakePythonObjPtr),
    FakePythonObjPtr (*_fcross2)(FakePythonObjPtr, FakePythonObjPtr,
                                 FakePythonObjPtr),
    FakePythonObjPtr problem_view,
    FakePythonObjPtr (*f_sol_deepcopy)(FakePythonObjPtr),
    size_t (*f_sol_tostring)(FakePythonObjPtr, char*, size_t),
    int (*f_utils_decref)(FakePythonObjPtr)) {
  auto* engine = (FCoreApi1Engine*)_engine;

  auto fcross1 = [_fcross1, problem_view, f_sol_deepcopy, f_sol_tostring,
                  f_utils_decref](
                     const FCoreLibSolution& s1,
                     const FCoreLibSolution& s2) -> FCoreLibSolution {
    FakePythonObjPtr vobj_owned =
        _fcross1(problem_view, s1.solution_ptr, s2.solution_ptr);
    assert(vobj_owned);
    FCoreLibSolution sol(vobj_owned, f_sol_deepcopy, f_sol_tostring,
                         f_utils_decref);
    return sol;
  };

  auto fcross2 = [_fcross2, problem_view, f_sol_deepcopy, f_sol_tostring,
                  f_utils_decref](
                     const FCoreLibSolution& s1,
                     const FCoreLibSolution& s2) -> FCoreLibSolution {
    FakePythonObjPtr vobj_owned =
        _fcross2(problem_view, s1.solution_ptr, s2.solution_ptr);
    assert(vobj_owned);
    FCoreLibSolution sol(vobj_owned, f_sol_deepcopy, f_sol_tostring,
                         f_utils_decref);
    return sol;
  };

  // workaround to join both callbacks...
  // for now, complicated to efficiently have multiple returns in
  // python callbacks! It's fixable, but takes time...
  auto fcross = [fcross1, fcross2](const FCoreLibSolution& s1,
                                   const FCoreLibSolution& s2)
      -> std::pair<FCoreLibSolution, FCoreLibSolution> {
    std::pair<FCoreLibSolution, FCoreLibSolution> p = {fcross1(s1, s2),
                                                       fcross2(s1, s2)};
    return p;
  };
  auto* gc_ptr = new optframe::FGeneralCrossover<FCoreLibSolution>{fcross};

  sref<optframe::GeneralCrossover<FCoreLibSolution>> fgc2(gc_ptr);
  sref<optframe::Component> fc(fgc2);

  int id = engine->loader.factory.addComponent(fc, "OptFrame:GeneralCrossover");
  return id;
}

OPT_MODULE_API int  // index of Constructive (for RK)
optframe_api1d_add_rk_constructive(FakeEnginePtr _engine,
                                   int (*_fconstructive)(FakePythonObjPtr,
                                                         LibArrayDouble*),
                                   FakePythonObjPtr problem_view) {
  auto* engine = (FCoreApi1Engine*)_engine;
  //
  auto fconstructive_vec = [_fconstructive,
                            problem_view]() -> std::vector<double> {
    //
    LibArrayDouble vobj_local;
    vobj_local.size = -1;
    vobj_local.v = 0;
    //
    int ret_sz = _fconstructive(problem_view, &vobj_local);
    (void)ret_sz;  // prevent unused
    assert(ret_sz >= 0);
    std::vector<double> vec(vobj_local.v, vobj_local.v + vobj_local.size);
    //
    // destroy array .v
    //  => created by c++ call to 'optframe_api0_set_array_double' inside of
    //  _fconstructive
    delete[] vobj_local.v;
    //
    vobj_local.size = 0;
    vobj_local.v = 0;  // nullptr
    //
    return vec;
  };

  auto* c_ptr = new optframe::FConstructiveRK<double>{fconstructive_vec};

  sref<optframe::Constructive<std::vector<double>>> fc2(c_ptr);
  sref<optframe::Component> fc(fc2);

  // std::cout << "   ==== optframe_api1d_add_rk_constructive: will try c_ptr="
  // << c_ptr << " ->generateSolution()" << std::endl; auto opv =
  // c_ptr->generateSolution({ 0.0 }); std::vector<double> vd = *opv; std::cout
  // << "   ==== optframe_api1d_add_rk_constructive: vd.size()=" << vd.size() <<
  // std::endl;

  int id = engine->loader.factory.addComponent(
      fc, "OptFrame:Constructive<XRKf64>:EA:RK:ConstructiveRK");
  //
  return id;
}

OPT_MODULE_API int  // error or size
optframe_api0_set_array_double(int sz, double* vec, LibArrayDouble* lad_ptr) {
  assert(sz >= 0);
  // arbitrary limit, just for safety...
  constexpr int LIMIT_ARRAY_DOUBLE = 10'000;
  (void)LIMIT_ARRAY_DOUBLE;  // prevent unused
  assert(sz <= LIMIT_ARRAY_DOUBLE);
  (*lad_ptr).size = sz;
  (*lad_ptr).v = new double[sz];
  // copy by loop (TODO: improve!)
  for (int i = 0; i < sz; i++) (*lad_ptr).v[i] = vec[i];
  return sz;
}

OPT_MODULE_API int  // index of Decoder (for RK)
optframe_api1d_add_rk_decoder(
    FakeEnginePtr _engine,
    FakePythonObjPtr (*_fdecoder)(FakePythonObjPtr, LibArrayDouble),
    FakePythonObjPtr problem_view,
    // Support necessary for Solution construction and maintainance
    FakePythonObjPtr (*f_sol_deepcopy)(FakePythonObjPtr),
    size_t (*f_sol_tostring)(FakePythonObjPtr, char*, size_t),
    int (*f_utils_decref)(FakePythonObjPtr)) {
  auto* engine = (FCoreApi1Engine*)_engine;
  //
  auto funcdecoder =
      [_fdecoder, problem_view, f_sol_deepcopy, f_sol_tostring,
       f_utils_decref](const std::vector<double>& vec) -> FCoreLibSolution {
    LibArrayDouble lad;
    lad.size = vec.size();
    // removes constness (afterall, LAD is supposed to be a view!)
    lad.v = const_cast<double*>(vec.data());
    //
    FakePythonObjPtr vobj_owned = _fdecoder(problem_view, lad);
    // std::cout << "'optframe_api1d_add_constructive' -> _fconstructive
    // generated pointer: " << vobj_owned << std::endl;
    assert(
        vobj_owned);  // check void* (TODO: for FxConstructive, return nullopt)
    FCoreLibSolution sol(vobj_owned, f_sol_deepcopy, f_sol_tostring,
                         f_utils_decref);
    // std::cout << "'optframe_api1d_add_constructive' -> solution created!" <<
    // std::endl;

    return sol;
  };

  optframe::DecoderRandomKeysNoEvaluation<FCoreLibSolution, double>* dec_ptr;
  dec_ptr = new optframe::FDecoderNoEvaluationRK<FCoreLibSolution, double>{
      funcdecoder};

  sref<optframe::DecoderRandomKeysNoEvaluation<FCoreLibSolution, double>> fc2(
      dec_ptr);
  sref<optframe::Component> fc(fc2);

  int id = engine->loader.factory.addComponent(
      fc, "OptFrame:EA:RK:DecoderRandomKeysNoEvaluation");
  //
  return id;
}

OPT_MODULE_API int  // index of Decoder (for RK)
optframe_api1d_add_rk_edecoder_op(
    FakeEnginePtr _engine,
    FakePythonObjPtr (*_fdecoder)(FakePythonObjPtr, LibArrayDouble, double*,
                                  bool),  // evaluation (passed by reference)
    FakePythonObjPtr problem_view,
    //
    bool is_minimization,
    // Support necessary for Solution construction and maintainance
    FakePythonObjPtr (*f_sol_deepcopy)(FakePythonObjPtr),
    size_t (*f_sol_tostring)(FakePythonObjPtr, char*, size_t),
    int (*f_utils_decref)(FakePythonObjPtr)) {
  auto* engine = (FCoreApi1Engine*)_engine;
  //
  using DecoderPair =
      std::pair<optframe::Evaluation<double>, optframe::op<FCoreLibSolution>>;
  auto funcdecoder = [_fdecoder, problem_view, f_sol_deepcopy, f_sol_tostring,
                      f_utils_decref](const std::vector<double>& vec,
                                      bool needsSolution) -> DecoderPair {
    LibArrayDouble lad;
    lad.size = vec.size();
    // removes constness (afterall, LAD is supposed to be a view!)
    lad.v = const_cast<double*>(vec.data());
    //
    double eval = 0;
    FakePythonObjPtr vobj_owned =
        _fdecoder(problem_view, lad, &eval, needsSolution);
    if (!vobj_owned) {
      // nullopt solution
      return DecoderPair{optframe::Evaluation<double>{eval}, std::nullopt};
    } else {
      // std::cout << "'optframe_api1d_add_constructive' -> _fconstructive
      // generated pointer: " << vobj_owned << std::endl;
      assert(vobj_owned);  // check void* (TODO: for FxConstructive, return
                           // nullopt)
      FCoreLibSolution sol(vobj_owned, f_sol_deepcopy, f_sol_tostring,
                           f_utils_decref);
      // std::cout << "'optframe_api1d_add_constructive' -> solution created!"
      // << std::endl;
      return DecoderPair{optframe::Evaluation<double>{eval}, sol};
    }
  };
  //
  optframe::DecoderRandomKeys<FCoreLibESolution, double>* dec_ptr;
  if (is_minimization)
    dec_ptr =
        new optframe::FDecoderOpRK<FCoreLibESolution, double,
                                   optframe::MinOrMax::MINIMIZE>{funcdecoder};
  else
    dec_ptr =
        new optframe::FDecoderOpRK<FCoreLibESolution, double,
                                   optframe::MinOrMax::MAXIMIZE>{funcdecoder};

  sref<optframe::DecoderRandomKeys<FCoreLibESolution, double>> fc2(dec_ptr);
  sref<optframe::Component> fc(fc2);

  int id = engine->loader.factory.addComponent(
      fc, "OptFrame:EA:RK:DecoderRandomKeys");
  //
  return id;
}

OPT_MODULE_API int  // index of ns
optframe_api1d_add_ns(
    FakeEnginePtr _engine,
    FakePythonObjPtr (*_fns_rand)(FakePythonObjPtr, FakePythonObjPtr),
    FakePythonObjPtr (*_fmove_apply)(FakePythonObjPtr, FakePythonObjPtr,
                                     FakePythonObjPtr),
    bool (*_fmove_eq)(FakePythonObjPtr, FakePythonObjPtr, FakePythonObjPtr),
    bool (*_fmove_cba)(FakePythonObjPtr, FakePythonObjPtr, FakePythonObjPtr),
    FakePythonObjPtr problem_view, int (*_f_utils_decref)(FakePythonObjPtr)) {
  auto* engine = (FCoreApi1Engine*)_engine;

  // std::cout << "invoking 'optframe_api1d_add_constructive' with "
  //           << "_hf=" << _hf << " _fconstructive and problem_view=" <<
  //           problem_view << std::endl;

  // ======== preparing move functions ========
  typedef std::function<FakePythonObjPtr(const FakePythonObjPtr&,
                                         FCoreLibESolution&)>
      FuncTypeMoveApply;
  FuncTypeMoveApply func_fmove_apply =
      [_fmove_apply, problem_view](const FakePythonObjPtr& m_view,
                                   FCoreLibESolution& se) -> FakePythonObjPtr {
    // IMPORTANT: _fmove_apply must IncRef Move on python before returning! I
    // think so... m_view seems to come from Python, to be used on Python...
    // don't know if we need to IncRef or DecRef that...
    // TODO: will pass ESolution as a Solution in API1... ignoring
    // Evaluation/Re-evaluation!
    //
    // vobj_owned should come IncRef'ed before! I guess...
    //
    FakePythonObjPtr vobj_owned =
        _fmove_apply(problem_view, m_view, se.first.solution_ptr);
    // TODO: don't know if IncRef or not solution_ptr... I THINK it's a "view"
    // for python, so no IncRef here.
    return vobj_owned;
  };

  typedef std::function<bool(const FakePythonObjPtr&, const FCoreLibESolution&)>
      FuncTypeMoveCBA;
  FuncTypeMoveCBA func_fmove_cba = [_fmove_cba, problem_view](
                                       const FakePythonObjPtr& m_view,
                                       const FCoreLibESolution& se) -> bool {
    // IMPORTANT: python will receive all as views..
    // TODO: will pass ESolution as a Solution in API1... ignoring
    // Evaluation/Re-evaluation!
    bool r = _fmove_cba(problem_view, m_view, se.first.solution_ptr);
    return r;
  };

  typedef std::function<bool(const FakePythonObjPtr&,
                             const optframe::Move<FCoreLibESolution>&)>
      FuncTypeMoveEq;
  FuncTypeMoveEq func_fmove_eq =
      [_fmove_eq, problem_view](
          const FakePythonObjPtr& my_m_view,
          const optframe::Move<FCoreLibESolution>& _mOther) -> bool {
    // cast to to lib move type. (TODO: check type... how? use OptFrame
    // Component id()? or must improve OptFrame in this regard... not worry
    // now!)
    auto& mOther = (FMoveLib<>&)_mOther;
    //
    FakePythonObjPtr mStructOtherView = mOther.m;
    // IMPORTANT: python will receive all as views..
    bool r = _fmove_eq(problem_view, my_m_view, mStructOtherView);
    return r;
  };

  typedef std::function<bool(FakePythonObjPtr)> FuncTypeUtilsDecRef;
  FuncTypeUtilsDecRef func_utils_decref = _f_utils_decref;

  // std::function<uptr<Move<XES>>(const XES&)>
  auto func_fns = [_fns_rand, problem_view, func_fmove_apply, func_fmove_cba,
                   func_fmove_eq,
                   func_utils_decref](const FCoreLibESolution& se)
      -> optframe::uptr<optframe::Move<FCoreLibESolution>> {
    // IMPORTANT: _fns_rand must IncRef Move on python before returning! I think
    // so...
    // TODO: will pass ESolution as a Solution in API1... ignoring
    // Evaluation/Re-evaluation!
    // TODO: don't know if IncRef or not solution_ptr... I THINK it's a "view"
    // for python, so no IncRef here.
    //
    // vobj_owned should come IncRef'ed before! I guess...
    FakePythonObjPtr vobj_owned =
        _fns_rand(problem_view, se.first.solution_ptr);
    // std::cout << "'optframe_api1d_add_ns' -> _fns_rand generated pointer: "
    // << vobj_owned << std::endl;
    assert(vobj_owned);  // check void* (TODO: allow non-existing move, return
                         // nullptr)

    //
    auto* m_ptr = new FMoveLib(vobj_owned, func_fmove_apply, func_fmove_cba,
                               func_fmove_eq, func_utils_decref);

    // std::cout << "'optframe_api1d_add_ns' -> move created!" << std::endl;
    return optframe::uptr<optframe::Move<FCoreLibESolution>>(m_ptr);
  };

  auto* p_fns = new optframe::FNS<FCoreLibESolution>{func_fns};
  // adjust NS_VALID_RANDOM_MOVE_MAX_TRIES for NS
  int max_tries =
      std::stoi(engine->experimentalParams["NS_VALID_RANDOM_MOVE_MAX_TRIES"]);
  if (max_tries > 1) {
    if (engine->engineLogLevel >= optframe::LogLevel::Warning)
      std::cout << "WARNING: using NS_VALID_RANDOM_MOVE_MAX_TRIES=" << max_tries
                << std::endl;
    p_fns->fValidRandom = [p_fns, max_tries](const FCoreLibESolution& se)
        -> optframe::uptr<Move<FCoreLibESolution>> {
      int t = 0;
      while (t < max_tries) {
        optframe::uptr<Move<FCoreLibESolution>> moveValid = p_fns->fRandom(se);
        if (moveValid && moveValid->canBeApplied(se))
          return moveValid;
        else
          t++;
      }
      // no way!
      return nullptr;
    };
  }
  sref<optframe::NS<FCoreLibESolution>> fns{p_fns};

  sref<optframe::Component> fns_comp(fns);
  // new optframe::FNS<FCoreLibESolution>{ func_fns });

  // std::cout << "'optframe_api1d_add_ns' will add component on hf" <<
  // std::endl;

  int id = engine->loader.factory.addComponent(fns_comp, "OptFrame:NS");
  //
  engine->check.add(fns);
  // fns->print();
  return id;
}

OPT_MODULE_API int  // index of ns<XMES>
optframe_api3d_add_ns_xmes(
    FakeEnginePtr _engine,
    FakePythonObjPtr (*_fns_rand)(FakePythonObjPtr, FakePythonObjPtr),
    FakePythonObjPtr (*_fmove_apply)(FakePythonObjPtr, FakePythonObjPtr,
                                     FakePythonObjPtr),
    bool (*_fmove_eq)(FakePythonObjPtr, FakePythonObjPtr, FakePythonObjPtr),
    bool (*_fmove_cba)(FakePythonObjPtr, FakePythonObjPtr, FakePythonObjPtr),
    FakePythonObjPtr problem_view, int (*_f_utils_decref)(FakePythonObjPtr)) {
  auto* engine = (FCoreApi1Engine*)_engine;

  // std::cout << "invoking 'optframe_api1d_add_constructive' with "
  //           << "_hf=" << _hf << " _fconstructive and problem_view=" <<
  //           problem_view << std::endl;

  // ======== preparing move functions ========
  typedef std::function<FakePythonObjPtr(const FakePythonObjPtr&,
                                         FCoreLibEMSolution&)>
      FuncTypeMoveApply;
  FuncTypeMoveApply func_fmove_apply =
      [_fmove_apply, problem_view](const FakePythonObjPtr& m_view,
                                   FCoreLibEMSolution& se) -> FakePythonObjPtr {
    // IMPORTANT: _fmove_apply must IncRef Move on python before returning! I
    // think so... m_view seems to come from Python, to be used on Python...
    // don't know if we need to IncRef or DecRef that...
    // TODO: will pass ESolution as a Solution in API1... ignoring
    // Evaluation/Re-evaluation!
    //
    // vobj_owned should come IncRef'ed before! I guess...
    //
    FakePythonObjPtr vobj_owned =
        _fmove_apply(problem_view, m_view, se.first.solution_ptr);
    // TODO: don't know if IncRef or not solution_ptr... I THINK it's a "view"
    // for python, so no IncRef here.
    return vobj_owned;
  };

  typedef std::function<bool(const FakePythonObjPtr&,
                             const FCoreLibEMSolution&)>
      FuncTypeMoveCBA;
  FuncTypeMoveCBA func_fmove_cba = [_fmove_cba, problem_view](
                                       const FakePythonObjPtr& m_view,
                                       const FCoreLibEMSolution& se) -> bool {
    // IMPORTANT: python will receive all as views..
    // TODO: will pass ESolution as a Solution in API1... ignoring
    // Evaluation/Re-evaluation!
    bool r = _fmove_cba(problem_view, m_view, se.first.solution_ptr);
    return r;
  };

  typedef std::function<bool(const FakePythonObjPtr&,
                             const optframe::Move<FCoreLibEMSolution>&)>
      FuncTypeMoveEq;
  FuncTypeMoveEq func_fmove_eq =
      [_fmove_eq, problem_view](
          const FakePythonObjPtr& my_m_view,
          const optframe::Move<FCoreLibEMSolution>& _mOther) -> bool {
    // cast to to lib move type. (TODO: check type... how? use OptFrame
    // Component id()? or must improve OptFrame in this regard... not worry
    // now!)
    auto& mOther = (FMoveLib<FCoreLibEMSolution>&)_mOther;
    //
    FakePythonObjPtr mStructOtherView = mOther.m;
    // IMPORTANT: python will receive all as views..
    bool r = _fmove_eq(problem_view, my_m_view, mStructOtherView);
    return r;
  };

  typedef std::function<bool(FakePythonObjPtr)> FuncTypeUtilsDecRef;
  FuncTypeUtilsDecRef func_utils_decref = _f_utils_decref;

  // std::function<uptr<Move<XES>>(const XES&)>
  auto func_fns = [_fns_rand, problem_view, func_fmove_apply, func_fmove_cba,
                   func_fmove_eq,
                   func_utils_decref](const FCoreLibEMSolution& se)
      -> optframe::uptr<optframe::Move<FCoreLibEMSolution>> {
    // IMPORTANT: _fns_rand must IncRef Move on python before returning! I think
    // so...
    // TODO: will pass ESolution as a Solution in API1... ignoring
    // Evaluation/Re-evaluation!
    // TODO: don't know if IncRef or not solution_ptr... I THINK it's a "view"
    // for python, so no IncRef here.
    //
    // vobj_owned should come IncRef'ed before! I guess...
    FakePythonObjPtr vobj_owned =
        _fns_rand(problem_view, se.first.solution_ptr);
    // std::cout << "'optframe_api1d_add_ns' -> _fns_rand generated pointer: "
    // << vobj_owned << std::endl;
    assert(vobj_owned);  // check void* (TODO: allow non-existing move, return
                         // nullptr)

    //
    auto* m_ptr = new FMoveLib(vobj_owned, func_fmove_apply, func_fmove_cba,
                               func_fmove_eq, func_utils_decref);

    // std::cout << "'optframe_api1d_add_ns' -> move created!" << std::endl;
    return optframe::uptr<optframe::Move<FCoreLibEMSolution>>(m_ptr);
  };

  sref<optframe::NS<FCoreLibEMSolution>> fns(
      new optframe::FNS<FCoreLibEMSolution>{func_fns});

  sref<optframe::Component> fns_comp(fns);
  // new optframe::FNS<FCoreLibESolution>{ func_fns });

  // std::cout << "'optframe_api1d_add_ns' will add component on hf" <<
  // std::endl;

  int id =
      engine->loader.factory.addComponent(fns_comp, "OptFrame:NS<XMESf64>");
  //
  // TODO: cannot add to check module here!
  // engine->check.add(fns);
  // fns->print();
  return id;
}

// FOR NOW, WE IGNORE 'const XES' AND JUST USE 'const S'.... LET'S SEE!

OPT_MODULE_API int  // index of ns
optframe_api1d_add_nsseq(
    FakeEnginePtr _engine,
    FakePythonObjPtr (*_fns_rand)(FakePythonObjPtr, FakePythonObjPtr),
    FakePythonObjPtr (*_fIterator)(
        FakePythonObjPtr,
        FakePythonObjPtr),  // fIterator (just initializes IMS)
    // problem*, ims*
    void (*_fFirst)(FakePythonObjPtr, FakePythonObjPtr),   // iterator.first()
    void (*_fNext)(FakePythonObjPtr, FakePythonObjPtr),    // iterator.next()
    bool (*_fIsDone)(FakePythonObjPtr, FakePythonObjPtr),  // iterator.isDone()
    FakePythonObjPtr (*_fCurrent)(FakePythonObjPtr,
                                  FakePythonObjPtr),  // iterator.current()
    FakePythonObjPtr (*_fmove_apply)(FakePythonObjPtr, FakePythonObjPtr,
                                     FakePythonObjPtr),
    bool (*_fmove_eq)(FakePythonObjPtr, FakePythonObjPtr, FakePythonObjPtr),
    bool (*_fmove_cba)(FakePythonObjPtr, FakePythonObjPtr, FakePythonObjPtr),
    FakePythonObjPtr problem_view, int (*_f_utils_decref)(FakePythonObjPtr)) {
  auto* engine = (FCoreApi1Engine*)_engine;

  // std::cout << "invoking 'optframe_api1d_add_constructive' with "
  //           << "_hf=" << _hf << " _fconstructive and problem_view=" <<
  //           problem_view << std::endl;

  // ======== preparing move functions ========
  typedef std::function<FakePythonObjPtr(const FakePythonObjPtr&,
                                         FCoreLibESolution&)>
      FuncTypeMoveApply;
  FuncTypeMoveApply func_fmove_apply =
      [_fmove_apply, problem_view](const FakePythonObjPtr& m_view,
                                   FCoreLibESolution& se) -> FakePythonObjPtr {
    // TODO: will pass ESolution as a Solution in API1... ignoring
    // Evaluation/Re-evaluation! IMPORTANT: python will receive all as views..
    FakePythonObjPtr vobj_owned =
        _fmove_apply(problem_view, m_view, se.first.solution_ptr);
    return vobj_owned;
  };

  typedef std::function<bool(const FakePythonObjPtr&, const FCoreLibESolution&)>
      FuncTypeMoveCBA;
  FuncTypeMoveCBA func_fmove_cba = [_fmove_cba, problem_view](
                                       const FakePythonObjPtr& m_view,
                                       const FCoreLibESolution& se) -> bool {
    // TODO: will pass ESolution as a Solution in API1... ignoring
    // Evaluation/Re-evaluation! IMPORTANT: python will receive all as views..
    bool r = _fmove_cba(problem_view, m_view, se.first.solution_ptr);
    return r;
  };

  typedef std::function<bool(const FakePythonObjPtr&,
                             const optframe::Move<FCoreLibESolution>&)>
      FuncTypeMoveEq;
  FuncTypeMoveEq func_fmove_eq =
      [_fmove_eq, problem_view](
          const FakePythonObjPtr& my_m_view,
          const optframe::Move<FCoreLibESolution>& _mOther) -> bool {
    // cast to to lib move type. (TODO: check type... how? use OptFrame
    // Component id()? or must improve OptFrame in this regard... not worry
    // now!)
    auto& mOther = (FMoveLib<>&)_mOther;
    //
    FakePythonObjPtr mStructOtherView = mOther.m;
    // IMPORTANT: python will receive all as views..
    bool r = _fmove_eq(problem_view, my_m_view, mStructOtherView);
    return r;
  };

  typedef std::function<bool(FakePythonObjPtr)> FuncTypeUtilsDecRef;
  FuncTypeUtilsDecRef func_utils_decref = _f_utils_decref;

  // std::function<uptr<Move<XES>>(const XES&)>
  auto func_fnsrand = [_fns_rand, problem_view, func_fmove_apply,
                       func_fmove_cba, func_fmove_eq,
                       func_utils_decref](const FCoreLibESolution& se)
      -> optframe::uptr<optframe::Move<FCoreLibESolution>> {
    // IMPORTANT: _fns_rand must IncRef Move on python before returning! I think
    // so...
    // TODO: will pass ESolution as a Solution in API1... ignoring
    // Evaluation/Re-evaluation!
    // TODO: don't know if IncRef or not solution_ptr... I THINK it's a "view"
    // for python, so no IncRef here.
    //
    // vobj_owned should come IncRef'ed before! I guess...
    FakePythonObjPtr vobj_owned =
        _fns_rand(problem_view, se.first.solution_ptr);
    // std::cout << "'optframe_api1d_add_ns' -> _fns_rand generated pointer: "
    // << vobj_owned << std::endl;
    assert(vobj_owned);  // check void* (TODO: allow non-existing move, return
                         // nullptr)

    //
    auto* m_ptr = new FMoveLib(vobj_owned, func_fmove_apply, func_fmove_cba,
                               func_fmove_eq, func_utils_decref);

    // std::cout << "'optframe_api1d_add_ns' -> move created!" << std::endl;
    return optframe::uptr<optframe::Move<FCoreLibESolution>>(m_ptr);
  };

  // =============================
  //      Iterator Functions
  // =============================

  typedef std::function<IMSObjLib(const FCoreLibESolution&)>
      FuncTypeNSSeqItInit;
  // FakePythonObjPtr (*_fIterator)(FakePythonObjPtr, FakePythonObjPtr),
  FuncTypeNSSeqItInit func_fnsseq_it_init =
      [_fIterator, problem_view,
       func_utils_decref](const FCoreLibESolution& se) -> IMSObjLib {
    // TODO: will pass ESolution as a Solution in API1... ignoring
    // Evaluation/Re-evaluation! IMPORTANT: python will receive all as views..
    FakePythonObjPtr ims_obj_owned =
        _fIterator(problem_view, se.first.solution_ptr);
    assert(ims_obj_owned);

    IMSObjLib ims{ims_obj_owned, func_utils_decref};
    return ims;
  };

  typedef std::function<void(IMSObjLib&)> FuncTypeNSSeqItFirst;
  // void (*_fFirst)(FakePythonObjPtr, FakePythonObjPtr),
  FuncTypeNSSeqItFirst func_fnsseq_it_first =
      [_fFirst, problem_view](IMSObjLib& it) -> void {
    // IMPORTANT: python will receive all as views..
    _fFirst(problem_view, it.ims_ptr);
  };

  typedef std::function<void(IMSObjLib&)> FuncTypeNSSeqItNext;
  // void (*_fFirst)(FakePythonObjPtr, FakePythonObjPtr),
  FuncTypeNSSeqItNext func_fnsseq_it_next =
      [_fNext, problem_view](IMSObjLib& it) -> void {
    // IMPORTANT: python will receive all as views..
    _fNext(problem_view, it.ims_ptr);
  };

  typedef std::function<bool(IMSObjLib&)> FuncTypeNSSeqItIsDone;

  FuncTypeNSSeqItIsDone func_fnsseq_it_isdone =
      [_fIsDone, problem_view](IMSObjLib& it) -> bool {
    // IMPORTANT: python will receive all as views..
    bool b = _fIsDone(problem_view, it.ims_ptr);
    return b;
  };

  typedef std::function<optframe::uptr<optframe::Move<FCoreLibESolution>>(
      IMSObjLib & it)>
      FuncTypeNSSeqItCurrent;

  FuncTypeNSSeqItCurrent func_fnsseq_it_current =
      [_fCurrent, problem_view, func_fmove_apply, func_fmove_cba, func_fmove_eq,
       func_utils_decref](
          IMSObjLib& it) -> optframe::uptr<optframe::Move<FCoreLibESolution>> {
    // TODO: will pass ESolution as a Solution in API1... ignoring
    // Evaluation/Re-evaluation! IMPORTANT: python will receive all as views..
    FakePythonObjPtr vobj_owned = _fCurrent(problem_view, it.ims_ptr);
    assert(vobj_owned);  // check void* (TODO: allow non-existing move, return
                         // nullptr)

    //
    auto* m_ptr = new FMoveLib(vobj_owned, func_fmove_apply, func_fmove_cba,
                               func_fmove_eq, func_utils_decref);

    // std::cout << "'optframe_api1d_add_ns' -> move created!" << std::endl;
    return optframe::uptr<optframe::Move<FCoreLibESolution>>(m_ptr);
  };
  /*
     FNSSeq(
    uptr<Move<XES>> (*_fRandom)(const XES&), // fRandom
    IMS (*_fIterator)(const XES&),           // fIterator (just initializes IMS)
    void (*_fFirst)(IMS&),                   // iterator.first()
    void (*_fNext)(IMS&),                    // iterator.next()
    bool (*_fIsDone)(IMS&),                  // iterator.isDone()
    uptr<Move<XES>> (*_fCurrent)(IMS&)       // iterator.current()
    )
  */
  auto* p_fnsseq = new optframe::FNSSeq<IMSObjLib, FCoreLibESolution>{
      func_fnsrand,        func_fnsseq_it_init,   func_fnsseq_it_first,
      func_fnsseq_it_next, func_fnsseq_it_isdone, func_fnsseq_it_current};

  // adjust NS_VALID_RANDOM_MOVE_MAX_TRIES for NSSeq
  int max_tries =
      std::stoi(engine->experimentalParams["NS_VALID_RANDOM_MOVE_MAX_TRIES"]);
  if (max_tries > 1) {
    if (engine->engineLogLevel >= optframe::LogLevel::Warning)
      std::cout << "WARNING: using NS_VALID_RANDOM_MOVE_MAX_TRIES=" << max_tries
                << std::endl;
    p_fnsseq->fValidRandom = [p_fnsseq, max_tries](const FCoreLibESolution& se)
        -> optframe::uptr<Move<FCoreLibESolution>> {
      int t = 0;
      while (t < max_tries) {
        optframe::uptr<Move<FCoreLibESolution>> moveValid =
            p_fnsseq->fRandom(se);
        if (moveValid && moveValid->canBeApplied(se))
          return moveValid;
        else
          t++;
      }
      // no way!
      return nullptr;
    };
  }

  sref<optframe::NSSeq<FCoreLibESolution>> fnsseq{p_fnsseq};

  sref<optframe::Component> fnsseq_comp(fnsseq);

  // std::cout << "'optframe_api1d_add_nsseq' will add component on hf" <<
  // std::endl;

  int id = engine->loader.factory.addComponent(fnsseq_comp,
                                               "OptFrame:NS:NSFind:NSSeq");
  //
  engine->check.add(fnsseq);
  // fns->print();
  return id;
}

OPT_MODULE_API int  // index of InitialSearch
optframe_api1d_create_initial_search(FakeEnginePtr _engine, int ev_idx,
                                     int c_idx) {
  auto* engine = (FCoreApi1Engine*)_engine;

  using MyEval =
      optframe::Evaluator<FCoreLibSolution, optframe::Evaluation<double>,
                          FCoreLibESolution>;

  // will try to get evaluator to build InitialSolution component...
  std::shared_ptr<MyEval> _ev;
  engine->loader.factory.assign(_ev, ev_idx,
                                "OptFrame:GeneralEvaluator:Evaluator");
  assert(_ev);
  sref<MyEval> single_ev{_ev};
  //
  //
  using MyConstructive = optframe::Constructive<FCoreLibSolution>;
  //
  std::shared_ptr<MyConstructive> initial;
  engine->loader.factory.assign(initial, c_idx, "OptFrame:Constructive");
  if (!initial) {
    std::cout << "ERROR: failed to assign component '"
              << MyConstructive::idComponent() << "' as 'OptFrame:Constructive'"
              << std::endl;
    assert(initial);
  }
  //
  sref<optframe::InitialSearch<FCoreLibESolution>> initSol{
      new optframe::BasicInitialSearch<FCoreLibESolution>(initial, single_ev)};

  int id =
      engine->loader.factory.addComponent(initSol, "OptFrame:InitialSearch");
  //
  // engine->check.add(fns);

  return id;
}

// ==============================================
//                      GET
// ==============================================

OPT_MODULE_API void*  // raw (non-owned) pointer to GeneralEvaluator
optframe_api0d_get_evaluator(FakeEnginePtr _engine, int idx_ev) {
  auto* engine = (FCoreApi1Engine*)_engine;

  std::shared_ptr<optframe::GeneralEvaluator<FCoreLibESolution>> component;

  engine->loader.factory.assign(component, idx_ev, "OptFrame:GeneralEvaluator");
  if (!component) {
    std::cout << "ERROR: failed to assign component '"
              << optframe::GeneralEvaluator<FCoreLibESolution>::idComponent()
              << "' as 'OptFrame:GeneralEvaluator'" << std::endl;
    assert(false);
  }
  void* ptr = component.get();
  return ptr;
}

OPT_MODULE_API void*  // raw (non-owned) pointer to FConstructive
optframe_api0d_get_constructive(FakeEnginePtr _engine, int idx_c) {
  auto* engine = (FCoreApi1Engine*)_engine;

  std::shared_ptr<optframe::Constructive<FCoreLibSolution>> component;

  engine->loader.factory.assign(component, idx_c, "OptFrame:Constructive");
  if (!component) assert(false);
  void* ptr = component.get();
  return ptr;
}

// ==============================================
//            SPECIFIC INVOCATIONS
// ==============================================

// min_or_max is needed to correctly cast template on FEvaluator
OPT_MODULE_API double  // FEvaluator object
optframe_api0d_fevaluator_evaluate(FakeFEvaluatorPtr _fevaluator,
                                   bool min_or_max,
                                   FakePythonObjPtr solution_ptr_view) {
  FCoreLibSolution sol(solution_ptr_view);
  optframe::Evaluation<double> ev(0);
  if (min_or_max) {
    // MINIMIZE
    auto* fevaluator =
        (optframe::FEvaluator<FCoreLibESolution, optframe::MinOrMax::MINIMIZE>*)
            _fevaluator;
    ev = fevaluator->evaluate(sol);
  } else {
    // MAXIMIZE
    auto* fevaluator =
        (optframe::FEvaluator<FCoreLibESolution, optframe::MinOrMax::MAXIMIZE>*)
            _fevaluator;
    ev = fevaluator->evaluate(sol);
  }
  return ev.evaluation();
}

OPT_MODULE_API FakePythonObjPtr  // Python solution object
optframe_api0_fconstructive_gensolution(FakeFConstructivePtr _fconstructive) {
  // std::cout << "begin 'optframe_api1d_fconstructive_gensolution'" <<
  // std::endl;
  auto* fconstructive =
      (optframe::FConstructive<FCoreLibSolution>*)_fconstructive;
  std::optional<FCoreLibSolution> sol = fconstructive->generateSolution(0.0);
  // std::cout << "will check if optional solution exists: " << !!sol <<
  // std::endl;
  assert(sol);
  // will return solution to Python... must make sure it will live!
  // should IncRef it here?? Perhaps...
  // will move it out from boxed Sol object, and make it a fake is_view=1 object
  // here.
  FakePythonObjPtr ptr = sol->solution_ptr;
  // std::cout << "finished 'optframe_api1d_fconstructive_gensolution'...
  // returning ptr=" << ptr << std::endl;
  //  ======= "kill" sol container =======
  sol->solution_ptr = 0;
  sol->is_view = 1;
  return ptr;
}

// ==============

// RAW METHOD (SHOULD WE KEEP IT?)
OPT_MODULE_API void optframe_api0_component_print(void* component) {
  auto* c = (optframe::Component*)component;
  // std::cout << "optframe_component_print ptr=" << c << " => ";
  c->print();
}

OPT_MODULE_API bool optframe_api1d_engine_component_set_loglevel(
    FakeEnginePtr _engine, char* _scomponent, int loglevel, bool recursive) {
  auto* engine = (FCoreApi1Engine*)_engine;

  std::string scomponent{_scomponent};
  scannerpp::Scanner scanner{scomponent};

  sptr<optframe::Component> c =
      engine->loader.factory.getNextComponent(scanner);

  if (!c) return false;

  assert(loglevel >= 0);
  assert(loglevel <= 5);

  /*
  enum LogLevel
  {
     Silent = 0,
     Error = 1,
     Warning = 2,
     Info = 3, (DEFAULT)
     Debug = 4
  };
  */

  auto ll = (optframe::LogLevel)loglevel;

  assert(!recursive);  // TODO: Must create 'setMessageLevelR'

  c->setMessageLevel(ll);

  return true;
}

OPT_MODULE_API bool optframe_api1d_engine_experimental_set_parameter(
    FakeEnginePtr _engine, char* _parameter, char* _svalue) {
  auto* engine = (FCoreApi1Engine*)_engine;

  std::string parameter{_parameter};
  std::string svalue{_svalue};

  auto cleanParam = scannerpp::Scanner::trim(parameter);
  auto cleanValue = scannerpp::Scanner::trim(svalue);

  if ((cleanParam.length() < 3) || (cleanParam.length() > 100) ||
      (cleanValue.length() < 0) || (cleanValue.length() > 100)) {
    std::cout << "WARNING: invalid call "
                 "optframe_api1d_engine_experimental_set_parameter(...):"
              << std::endl;
    std::cout << "parameter: '" << parameter << "'" << std::endl;
    std::cout << "value: '" << svalue << "'" << std::endl;
    return false;
  }

  if ((cleanParam != "NS_VALID_RANDOM_MOVE_MAX_TRIES") &&
      (cleanParam != "ENGINE_LOG_LEVEL") &&
      (cleanParam != "COMPONENT_LOG_LEVEL")) {
    std::cout << "WARNING: non-existing parameter "
                 "optframe_api1d_engine_experimental_set_parameter(...):"
              << std::endl;
    std::cout << "parameter: '" << parameter << "'" << std::endl;
    return false;
  }

  if (cleanParam == "NS_VALID_RANDOM_MOVE_MAX_TRIES") {
    scannerpp::Scanner scan{cleanValue};
    // just to be extra safe...
    if (!scan.hasNextInt()) return false;
    auto op_x = scan.nextInt();
    if (!op_x || *op_x <= 0 || *op_x >= 2'000'000) {
      std::cout << "WARNING: optframe_api1d_engine_experimental_set_parameter:";
      std::cout << "bad value '" << cleanValue << "' for param '" << cleanParam
                << "'" << std::endl;
      return false;
    }
  }

  if (cleanParam == "ENGINE_LOG_LEVEL") {
    scannerpp::Scanner scan{cleanValue};
    // just to be extra safe...
    if (!scan.hasNextInt()) return false;
    auto op_x = scan.nextInt();
    if (!op_x || *op_x < 0 || *op_x > 4) {
      std::cout << "WARNING: optframe_api1d_engine_experimental_set_parameter:";
      std::cout << "bad value '" << cleanValue << "' for param '" << cleanParam
                << "'" << std::endl;
      return false;
    }
  }

  if (cleanParam == "COMPONENT_LOG_LEVEL") {
    scannerpp::Scanner scan{cleanValue};
    // just to be extra safe...
    if (!scan.hasNextInt()) return false;
    auto op_x = scan.nextInt();
    if (!op_x || *op_x < 0 || *op_x > 4) {
      std::cout << "WARNING: optframe_api1d_engine_experimental_set_parameter:";
      std::cout << "bad value '" << cleanValue << "' for param '" << cleanParam
                << "'" << std::endl;
      return false;
    }
  }

  engine->experimentalParams[cleanParam] = cleanValue;
  engine->updateParameters();

  return true;
}

OPT_MODULE_API char* optframe_api1d_engine_experimental_get_parameter(
    FakeEnginePtr _engine, char* _parameter) {
  auto* engine = (FCoreApi1Engine*)_engine;

  std::string parameter{_parameter};
  auto cleanParam = scannerpp::Scanner::trim(parameter);

  auto& m = engine->experimentalParams;

  // empty param means HELP
  if (cleanParam.length() == 0) {
    std::stringstream ss;
    ss << "[";
    bool first = true;
    for (auto it = m.begin(); it != m.end(); it++) {
      std::string sparam = it->first;
      std::string svalue = it->second;
      if (first)
        first = false;
      else
        ss << ",";
      ss << "{\"parameter\": \"" << sparam << "\",";
      ss << "\"value\": \"" << svalue << "\"}";
    }
    ss << "]";
    std::string sout = ss.str();
    char* c_sout = new char[sout.length()];
    ::strcpy(c_sout, sout.c_str());
    return c_sout;
  }

  if ((cleanParam.length() < 3) || (cleanParam.length() > 100)) {
    std::cout << "WARNING: invalid call "
                 "optframe_api1d_engine_experimental_get_parameter(...):"
              << std::endl;
    std::cout << "parameter: '" << parameter << "'" << std::endl;
    char* c_sout = new char[1];
    c_sout[0] = '\0';
    return c_sout;
  }

  if (m.find(cleanParam) == m.end()) {
    std::cout << "WARNING: non-existing parameter "
                 "optframe_api1d_engine_experimental_get_parameter(...):"
              << std::endl;
    std::cout << "parameter: '" << parameter << "'" << std::endl;
    char* c_sout = new char[1];
    c_sout[0] = '\0';
    return c_sout;
  } else {
    std::string svalue = m.at(cleanParam);
    std::stringstream ss;
    ss << "\"" << svalue << "\"";
    std::string sout = ss.str();
    char* c_sout = new char[sout.length()];
    ::strcpy(c_sout, sout.c_str());
    return c_sout;
  }
}

// ==============================================
