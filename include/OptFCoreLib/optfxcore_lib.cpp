#include "optfxcore_lib.h"
//
#include <OptFCore/FxCore.hpp>
//
#include <OptFCore/FCore.hpp>
//
#include <OptFrame/Hyper/HeuristicFactory.hpp>
#include <iostream>

// ============================================================================

// test if python object is received, plus an int
extern "C" int32_t
fcore_test_1(void* vpython, int32_t sz_vr) {
  printf("%p | %d\n", vpython, sz_vr);
  return 1;
}

// test if func is received
extern "C" int32_t
fcore_test_func(void* vpython, int32_t (*func)(void*), int32_t sz_vr) {
  printf("%p | %d\n", vpython, sz_vr);
  printf("%p | func_out= %d | %d\n", vpython, func(vpython), sz_vr);
  return 1;
}

// ============================================================================

namespace optframe {

inline HeuristicFactory<typename LESolution::first_type, typename LESolution::second_type, LESolution> hfactory;

bool initializeStandardBuilders() {
  hfactory.builders.push_back(new BasicSimulatedAnnealingBuilder<typename LESolution::first_type, typename LESolution::second_type, LESolution>);

  return true;
}

bool clearFactory() {
  hfactory.clear();
  return true;
}

sptr<SingleObjSearch<LESolution>>
buildSearch(std::string str) {
  pair<sptr<SingleObjSearch<LESolution>>, std::string> psearch = hfactory.createSingleObjSearch(str);
  if (!psearch.first) {
    std::cout << "Cannot build '" << str << "' rest='" << psearch.second << "'" << std::endl;
    return nullptr;
  }
  //hfactory.clear();
  return psearch.first;
  //return nullptr;
}

vector<pair<string, vector<pair<string, string>>>>
listBuilders(string pattern) {
  return hfactory.listBuilders(pattern);
}

vector<string>
listComponents(string pattern) {
  return hfactory.listComponents(pattern);
}

vector<string>
listComponentLists(string pattern) {
  return hfactory.listComponentLists(pattern);
}

int registerComponent(sref<Component> component, std::string sid) {
  std::cout << "registerComponent -> " << component->id() << std::endl;
  return hfactory.addComponent(component, sid);
}

int registerComponentList(vsref<Component> vcomponent, std::string sid) {
  std::cout << "registerComponentList -> " << sid << std::endl;
  return hfactory.addComponentListRef(vcomponent, sid);
}

int registerComponentListPtr(std::vector<sptr<Component>> vcomponent, std::string sid) {
  std::cout << "registerComponentListPtr -> " << sid << std::endl;
  return hfactory.addComponentList(vcomponent, sid);
}

bool registerNSSeq(sref<NSSeq<LESolution>> nsseq);

/*
sref<INSSeq>
generateFNSSeq(std::function<uptr<IMove>(const LESolution& se)> fRand,
               std::function<Generator<IMove*>(const LESolution& se)> fIterator)
{

   std::function<uptr<Move<LESolution>>(const LESolution&)> fRand2 = [&fRand](const LESolution& se) -> uptr<Move<LESolution>> {
      uptr<IMove> mv = fRand(se);
      return uptr<Move<LESolution>>{ (Move<LESolution>*)mv.release() };
   };

   std::function<Generator<Move<LESolution>*>(const LESolution& se)> fIterator2 = [&fIterator](const LESolution& se) -> Generator<Move<LESolution>*> {
      Generator<IMove*> gen = fIterator(se);
      return (Generator<Move<LESolution>*> &&) std::move(gen);
   };

   INSSeq* insseq = new FxNSSeq<LESolution>(fRand2, fIterator2);
   sref<INSSeq> nsseq{ insseq };
   return nsseq;
}
*/
sref<NSSeq<LESolution>>
generateFxNSSeq(std::function<uptr<Move<LESolution>>(const LESolution& se)> fRand,
                std::function<Generator<Move<LESolution>*>(const LESolution& se)> fIterator) {
  /*
   std::function<uptr<Move<LESolution>>(const LESolution&)> fRand2 = [&fRand](const LESolution& se) -> uptr<Move<LESolution>> {
      uptr<IMove> mv = fRand(se);
      return uptr<Move<LESolution>>{ (Move<LESolution>*)mv.release() };
   };

   std::function<Generator<Move<LESolution>*>(const LESolution& se)> fIterator2 = [&fIterator](const LESolution& se) -> Generator<Move<LESolution>*> {
      Generator<IMove*> gen = fIterator(se);
      return (Generator<Move<LESolution>*> &&) std::move(gen);
   };
   */

  //INSSeq* insseq = new FxNSSeq<LESolution>(fRand2, fIterator2);
  NSSeq<LESolution>* insseq = new FxNSSeq<LESolution>(fRand, fIterator);
  sref<NSSeq<LESolution>> nsseq{insseq};
  return nsseq;
}

sref<NSSeq<LESolution>>
generateFConstructive(std::function<uptr<Move<LESolution>>(const LESolution& se)> fRand,
                      std::function<Generator<Move<LESolution>*>(const LESolution& se)> fIterator) {
  /*
   std::function<uptr<Move<LESolution>>(const LESolution&)> fRand2 = [&fRand](const LESolution& se) -> uptr<Move<LESolution>> {
      uptr<IMove> mv = fRand(se);
      return uptr<Move<LESolution>>{ (Move<LESolution>*)mv.release() };
   };

   std::function<Generator<Move<LESolution>*>(const LESolution& se)> fIterator2 = [&fIterator](const LESolution& se) -> Generator<Move<LESolution>*> {
      Generator<IMove*> gen = fIterator(se);
      return (Generator<Move<LESolution>*> &&) std::move(gen);
   };
   */

  //INSSeq* insseq = new FxNSSeq<LESolution>(fRand2, fIterator2);
  NSSeq<LESolution>* insseq = new FxNSSeq<LESolution>(fRand, fIterator);
  sref<NSSeq<LESolution>> nsseq{insseq};
  return nsseq;
}

}  // namespace optframe