#ifndef MaPI_HPP
#define MaPI_HPP

#include "../mri/mri.h"

template<class KeyA, class A, class KeyB, class B, class C> class MaPI_MapReduce;
template<class KeyA, class A, class KeyB, class B, class C> class MaPI_Mapper;
template<class KeyA, class A, class KeyB, class B, class C> class MaPI_Reducer;
template<class KeyA, class A, class KeyB, class B, class C> class MaPI_Serializer;
class MaPI_StrMapReduce;
class MaPI_StrMapper;
class MaPI_StrReducer;

#include <mpi.h>
#include <cstdlib>
#include <sstream>
#include "../util/Scanner++/Scanner.h"
#include "serializer.hpp"
#include "mapreduce.hpp"
#include "mapper.hpp"
#include "reducer.hpp"
#include "fsmapreduce.hpp"

#endif /* MaPI_HPP */
