#define NO_CXX_MODULES 1  // disable CXX_MODULES for optframe

// load first to guarantee all tests work fine!
#include <OptFrame/printable/printable.hpp>
//
// TODO: clean some of these stuff and move to test-specific settings...
#include <OptFCore/EA/RK/FDecoderOpRK.hpp>
#include <OptFCore/FCore.hpp>
// #include <OptFCore/FxCore.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>  // XESolution
#include <OptFrame/Concepts/MyConcepts.hpp>    // sref
#include <OptFrame/Heuristics/EA/RK/BasicDecoderRandomKeys.hpp>
#include <OptFrame/Heuristics/EA/RK/BasicInitialEPopulationRK.hpp>
#include <OptFrame/Heuristics/MultiObjective/ClassicNSGAII.hpp>
#include <OptFrame/Hyper/CheckCommand.hpp>
#include <OptFrame/Hyper/HeuristicFactory.hpp>
#include <OptFrame/Hyper/Loader.hpp>
#include <OptFrame/Hyper/OptFrameList.hpp>
// =============================================

// collection of 'OptFrame' tests

#include "OptFrame.Test.cpp"

// good
