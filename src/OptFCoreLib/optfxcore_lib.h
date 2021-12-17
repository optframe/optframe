#ifndef OPTFCORE_LIB_H
#define OPTFCORE_LIB_H

#include <cstdint> // int32_t
#include <stdio.h>

//
#include <functional>
//
#include <OptFrame/MyConcepts.hpp> // sref
//

// =======
#define LESOLUTION_OBJ_TYPE int

// definitions for INSSeq, IMove, ...
//#include <OptFrame/CoreLib/CoreLib.hpp>
#include "LESolution.hpp"

// definitions for FCore components
#include <OptFrame/Core.hpp>
#include <OptFrame/SingleObjSearch.hpp>
//
#include <OptFCore/coro/Generator.hpp>
//
//#include <OptFCore/FxCore.hpp>
//#include <OptFCore/FCore.hpp>

// test if python object is received, plus an int
extern "C" int32_t
fcore_test_1(void* vpython, int32_t sz_vr);

// test if func is received
extern "C" int32_t
fcore_test_func(void* vpython, int32_t (*func)(void*), int32_t sz_vr);

// ==============
//
namespace optframe {

//
/*
sref<INSSeq>
generateFNSSeq(std::function<uptr<IMove>(const LESolution& se)> fRand,
               std::function<Generator<IMove*>(const LESolution& se)> fIterator);
*/
sref<NSSeq<LESolution>>
generateFxNSSeq(std::function<uptr<Move<LESolution>>(const LESolution& se)> fRand,
                std::function<Generator<Move<LESolution>*>(const LESolution& se)> fIterator);
//

// register NSSeq into HeuristicFactory
bool
registerNSSeq(sref<NSSeq<LESolution>> nsseq);
//

int
registerComponent(sref<Component> component, std::string sid);

int
registerComponentList(vector<sref<Component>> vcomponent, std::string sid);

//
bool
initializeStandardBuilders();

sptr<SingleObjSearch<LESolution>>
buildSearch(std::string str);

vector<pair<string, vector<pair<string, string>>>>
listBuilders(string pattern);

vector<string>
listComponents(string pattern);

bool
clearFactory();

} // namespace optframe

#endif // OPTFCORE_LIB_H
