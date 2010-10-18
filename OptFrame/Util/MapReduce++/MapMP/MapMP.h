#ifndef MapMP_HPP
#define MapMP_HPP

#include "../mri/mri.h"

template<class KeyA, class A, class KeyB, class B, class C> class MapMP_MapReduce;
template<class KeyA, class A, class KeyB, class B, class C> class MapMP_Mapper;
template<class KeyA, class A, class KeyB, class B, class C> class MapMP_Reducer;
class MapMP_StrMapReduce;
class MapMP_StrMapper;
class MapMP_StrReducer;

#include "mapreduce.hpp"
#include "mapper.hpp"
#include "reducer.hpp"

#endif /* MapMP_HPP */
