#ifndef PARALLEL_H_
#define PARALLEL_H_

#include "OffsetBestImprovement.hpp"

#include "BI_SeqMR.hpp"

#ifdef MapMP
#include "BI_MapMP.hpp"
#endif

#ifdef MaPI
#include "BI_MaPI.hpp"
#endif

#endif /*PARALLEL_H_*/
