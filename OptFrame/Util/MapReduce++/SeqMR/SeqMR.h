#ifndef SeqMR_HPP
#define SeqMR_HPP

#include "../mri/mri.h"

template<class KeyA, class A, class KeyB, class B, class C> class SeqMR_MapReduce;
template<class KeyA, class A, class KeyB, class B, class C> class SeqMR_Mapper;
template<class KeyA, class A, class KeyB, class B, class C> class SeqMR_Reducer;
class SeqMR_StrMapReduce;
class SeqMR_StrMapper;
class SeqMR_StrReducer;

#include "mapreduce.hpp"
#include "mapper.hpp"
#include "reducer.hpp"

#endif /* SeqMR_HPP */
