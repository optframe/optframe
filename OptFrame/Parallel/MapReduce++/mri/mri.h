#ifndef MRI_H
#define MRI_H

#include <map>
#include <vector>
#include <string>

using namespace std;

template<class KeyA, class A, class KeyB, class B, class C> class MapReduce;
template<class KeyA, class A, class KeyB, class B, class C> class Mapper;
template<class KeyA, class A, class KeyB, class B, class C> class Reducer;
class StrMapReduce;
class StrMapper;
class StrReducer;

#include "mapreduce.h"
#include "mapper.h"
#include "reducer.h"
#include "printable.h"

#endif /* MRI_H */
