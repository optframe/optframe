#ifndef OPTFRAME_PYTHON_LIB_H
#define OPTFRAME_PYTHON_LIB_H

// C
#include <stdio.h>
// C++
#include <cstdint>  // int32_t
// OptFrame C only
#include <OptFrameLib/LibCTypes.h>

// ============================ Engine: HeuristicFactory
// ===========================

#ifdef WIN32
#define OPT_MODULE_API extern "C" __declspec(dllexport)
#else
#define OPT_MODULE_API extern "C"
#endif

struct PairMoveDoubleLib {
  FakeMovePtr first;
  double second;
};

OPT_MODULE_API void optframe_api0d_engine_welcome(FakeEnginePtr _engine);

OPT_MODULE_API FakeEnginePtr optframe_api1d_create_engine(int LogLevel);

OPT_MODULE_API int optframe_api1d_engine_rand(FakeEnginePtr _engine);

OPT_MODULE_API int optframe_api1d_engine_rand_n(FakeEnginePtr _engine, int n);

OPT_MODULE_API int optframe_api1d_engine_rand_n_n(FakeEnginePtr _engine, int n1,
                                                  int n2);

OPT_MODULE_API void optframe_api1d_engine_rand_set_seed(FakeEnginePtr _engine,
                                                        uint32_t seed);

OPT_MODULE_API bool optframe_api1d_engine_check(
    FakeEnginePtr _engine, int softTests, int hardTests, bool force_verbose,
    bool (*_fOnFail)(int, FakeEnginePtr));

OPT_MODULE_API LibSearchOutput
optframe_api0d_engine_simulated_annealing(FakeEnginePtr _engine);
// double alpha, int iter, double temp

OPT_MODULE_API LibSearchOutput optframe_api0d_engine_simulated_annealing_params(
    FakeEnginePtr _engine, double timelimit, int id_evaluator,
    int id_constructive, int id_ns, double alpha, int iter, double T);

OPT_MODULE_API int  // ONLY STATUS... CANNOT RETURN PARETO ELEMENTS NOW...
optframe_api0d_engine_classic_nsgaii_params(FakeEnginePtr _engine,
                                            double timelimit, double min_limit,
                                            double max_limit, int id_mevaluator,
                                            int id_popman, int popSize,
                                            int maxIter);

OPT_MODULE_API int optframe_api1d_engine_list_builders(FakeEnginePtr _engine,
                                                       const char* prefix);

OPT_MODULE_API int optframe_api1d_engine_list_components(FakeEnginePtr _engine,
                                                         const char* prefix);

OPT_MODULE_API bool optframe_api0d_engine_test(FakeEnginePtr _engine);

OPT_MODULE_API bool optframe_api1d_destroy_engine(FakeEnginePtr _engine);

OPT_MODULE_API int optframe_api1d_add_evaluator(
    FakeEnginePtr _engine,
    double (*_fevaluate)(FakeProblemPtr, FakeSolutionPtr), bool min_or_max,
    FakeProblemPtr problem_view);

// ============

OPT_MODULE_API int  // index of generalevaluator
optframe_api1d_add_float64_evaluator(FakeEnginePtr _engine,
                                     double (*_fevaluate)(FakePythonObjPtr,
                                                          FakePythonObjPtr),
                                     bool min_or_max,
                                     FakePythonObjPtr problem_view);

OPT_MODULE_API int  // index of constructive
optframe_api1d_add_constructive(
    FakeEnginePtr _engine, FakePythonObjPtr (*_fconstructive)(FakePythonObjPtr),
    FakePythonObjPtr problem_view,
    FakePythonObjPtr (*f_sol_deepcopy)(FakePythonObjPtr),
    size_t (*f_sol_tostring)(FakePythonObjPtr, char*, size_t),
    int (*f_utils_decref)(FakePythonObjPtr));

OPT_MODULE_API int  // index of constructive
optframe_api1d_add_constructive_dangling(
    FakeEnginePtr _engine, FakeSolutionPtr (*_fconstructive)(FakeProblemPtr),
    FakeProblemPtr problem_view,
    FakePythonObjPtr (*f_sol_deepcopy)(FakePythonObjPtr),
    size_t (*f_sol_tostring)(FakeSolutionPtr, char*, size_t),
    int (*f_utils_decref)(FakePythonObjPtr));

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
    int (*f_utils_decref)(FakePythonObjPtr));

OPT_MODULE_API int  // index of ns
optframe_api1d_add_ns(
    FakeEnginePtr _engine,
    FakePythonObjPtr (*_fns_rand)(FakePythonObjPtr, FakePythonObjPtr),
    FakePythonObjPtr (*_fmove_apply)(FakePythonObjPtr, FakePythonObjPtr,
                                     FakePythonObjPtr),
    bool (*_fmove_eq)(FakePythonObjPtr, FakePythonObjPtr, FakePythonObjPtr),
    bool (*_fmove_cba)(FakePythonObjPtr, FakePythonObjPtr, FakePythonObjPtr),
    FakePythonObjPtr problem_view, int (*f_utils_decref)(FakePythonObjPtr));

OPT_MODULE_API int  // index of ns
optframe_api2d_add_ns(
    FakeEnginePtr _engine,
    FakeMovePtr (*_fns_rand)(FakeProblemPtr, FakeSolutionPtr),
    FakeMovePtr (*_fmove_apply)(FakeProblemPtr, FakeMovePtr, FakeSolutionPtr),
    bool (*_fmove_eq)(FakeProblemPtr, FakeMovePtr, FakeMovePtr),
    bool (*_fmove_cba)(FakeProblemPtr, FakeMovePtr, FakeSolutionPtr),
    PairMoveDoubleLib (*_fmove_apply_update)(FakeProblemPtr, FakeMovePtr,
                                             FakeSolutionPtr, double),
    FakeProblemPtr problem_view, int (*_f_utils_decref)(FakePythonObjPtr));

OPT_MODULE_API int  // index of nsseq
optframe_api1d_add_nsseq(
    FakeEnginePtr _engine,
    FakeMovePtr (*_fns_rand)(FakeProblemPtr, FakeSolutionPtr),
    FakePythonObjPtr (*_fIterator)(FakeProblemPtr, FakeSolutionPtr),
    // problem*, ims* (maybe move*?)
    void (*_fFirst)(FakeProblemPtr, FakePythonObjPtr),
    void (*_fNext)(FakeProblemPtr, FakePythonObjPtr),
    bool (*_fIsDone)(FakeProblemPtr, FakePythonObjPtr),
    FakeMovePtr (*_fCurrent)(FakeProblemPtr, FakePythonObjPtr),
    FakeMovePtr (*_fmove_apply)(FakeProblemPtr, FakeMovePtr, FakeSolutionPtr),
    bool (*_fmove_eq)(FakeProblemPtr, FakeMovePtr, FakeMovePtr),
    bool (*_fmove_cba)(FakeProblemPtr, FakeMovePtr, FakeSolutionPtr),
    FakeProblemPtr problem_view, int (*_f_utils_decref)(FakePythonObjPtr));

OPT_MODULE_API int  // index of ns<XMES>
optframe_api3d_add_ns_xmes(
    FakeEnginePtr _engine,
    FakePythonObjPtr (*_fns_rand)(FakePythonObjPtr, FakePythonObjPtr),
    FakePythonObjPtr (*_fmove_apply)(FakePythonObjPtr, FakePythonObjPtr,
                                     FakePythonObjPtr),
    bool (*_fmove_eq)(FakePythonObjPtr, FakePythonObjPtr, FakePythonObjPtr),
    bool (*_fmove_cba)(FakePythonObjPtr, FakePythonObjPtr, FakePythonObjPtr),
    FakePythonObjPtr problem_view, int (*f_utils_decref)(FakePythonObjPtr));

// ADD (specific)

OPT_MODULE_API int  // index of Constructive (for RK)
optframe_api1d_add_rk_constructive(FakeEnginePtr _engine,
                                   int (*_fconstructive)(FakePythonObjPtr,
                                                         LibArrayDouble*),
                                   FakePythonObjPtr problem_view);

OPT_MODULE_API int  // index of Decoder (for RK)
optframe_api1d_add_rk_decoder(
    FakeEnginePtr _engine,
    FakePythonObjPtr (*_fdecoder)(FakePythonObjPtr, LibArrayDouble),
    FakePythonObjPtr problem_view,
    FakePythonObjPtr (*f_sol_deepcopy)(FakePythonObjPtr),
    size_t (*f_sol_tostring)(FakePythonObjPtr, char*, size_t),
    int (*f_utils_decref)(FakePythonObjPtr));

OPT_MODULE_API int  // error or not
optframe_api0_set_array_double(int, double[], LibArrayDouble*);

OPT_MODULE_API int  // error?
optframe_api1d_run_experiments(FakeEnginePtr _engine, int numRuns,
                               const char* buildersLines, int firstSeed,
                               const char* outfile, double timelimit,
                               int componentLogLevel);

// CREATE

OPT_MODULE_API int  // index of ComponentList
optframe_api1d_create_component_list(FakeEnginePtr _engine, const char* clist,
                                     const char* list_type);

OPT_MODULE_API int  // index of InitialSearch
optframe_api1d_create_initial_search(FakeEnginePtr _engine, int ev_idx,
                                     int c_idx);

// BUILD

OPT_MODULE_API int  // index of GlobalSearch
optframe_api1d_build_global(FakeEnginePtr _engine, char* builder,
                            char* build_string);

OPT_MODULE_API int  // index of SingleObjSearch
optframe_api1d_build_single(FakeEnginePtr _engine, const char* builder,
                            const char* build_string);

OPT_MODULE_API int  // index of LocalSearch
optframe_api1d_build_local_search(FakeEnginePtr _engine, const char* builder,
                                  const char* build_string);

OPT_MODULE_API int  // index of Component
optframe_api1d_build_component(FakeEnginePtr _engine, const char* builder,
                               const char* build_string,
                               const char* component_type);

// ================

OPT_MODULE_API void*  // raw (non-owned) pointer to GeneralEvaluator
optframe_api0d_get_evaluator(FakeEnginePtr _engine, int idx_ev);

OPT_MODULE_API void*  // raw (non-owned) pointer to Constructive
optframe_api0d_get_constructive(FakeEnginePtr _hf, int idx_c);

// ===============

// SPECIFIC / INVOKE

OPT_MODULE_API double optframe_api0d_fevaluator_evaluate(
    FakeFEvaluatorPtr fevaluator, bool min_or_max,
    FakePythonObjPtr solution_ptr);

OPT_MODULE_API FakePythonObjPtr  // Python solution object (owned??? by who??
                                 // maybe non-owned, but alive...)
optframe_api0_fconstructive_gensolution(FakeFConstructivePtr _fconstructive);

// RUN

OPT_MODULE_API LibSearchOutput  // SearchOutput for XSH "best-type"
optframe_api1d_run_global_search(FakeEnginePtr _engine, int sos_idx,
                                 double timelimit);

OPT_MODULE_API LibSearchOutput  // SearchOutput for XSH "best-type"
optframe_api1d_run_sos_search(FakeEnginePtr _engine, int sos_idx,
                              double timelimit);

// ============================ COMPONENT ===========================
//
// RAW METHOD: SHOULD WE KEEP IT?
OPT_MODULE_API void optframe_api0_component_print(void* component);

// STANDARD METHOD FOR API1
OPT_MODULE_API bool optframe_api1d_engine_component_set_loglevel(
    FakeEnginePtr _engine, char* _scomponent, int loglevel, bool recursive);

// ============================
// EXPERIMENTAL

OPT_MODULE_API bool optframe_api1d_engine_experimental_set_parameter(
    FakeEnginePtr _engine, const char* parameter, const char* svalue);

OPT_MODULE_API char* optframe_api1d_engine_experimental_get_parameter(
    FakeEnginePtr _engine, const char* parameter);

OPT_MODULE_API void optframe_api1d_free_string(const char* p);

// ============================

#endif  // OPTFRAME_PYTHON_LIB_H
