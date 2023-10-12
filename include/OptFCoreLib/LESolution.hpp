#ifndef OPTFRAME_CORELIB_LESOLUTION_HPP
#define OPTFRAME_CORELIB_LESOLUTION_HPP

// Library ESolution
// Empty Class, for now...

#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Evaluation.hpp>
#include <any>  // c++17

namespace optframe {

//
// OptFrame Library ESolution Objective Type
//
#ifndef LESOLUTION_OBJ_TYPE
#define LESOLUTION_OBJ_TYPE double
#endif

static_assert(XEvaluation<Evaluation<LESOLUTION_OBJ_TYPE>>);

// using LESolution = std::pair<sref<LSolution>, Evaluation<double>>;
//
// using LESolution = std::pair<LSolution, Evaluation<LESOLUTION_OBJ_TYPE>>;
using LESolution = std::pair<std::any, Evaluation<LESOLUTION_OBJ_TYPE>>;

static_assert(XESolution<LESolution>);

}  // namespace optframe

#endif  // OPTFRAME_CORELIB_LESOLUTION_HPP