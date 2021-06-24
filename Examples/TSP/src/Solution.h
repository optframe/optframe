// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef TSP_SOLUTION_H_
#define TSP_SOLUTION_H_

#include "Evaluation.h"
#include "Representation.h"
#include <OptFrame/BaseConcepts.hpp>
#include <OptFrame/Solution.hpp>
#include <OptFrame/Solutions/CopySolution.hpp>

using namespace optframe;

//typedef Solution<RepTSP> SolutionTSP;
typedef CopySolution<RepTSP> SolutionTSP;

typedef pair<SolutionTSP, EvaluationTSP> ESolutionTSP;

// compilation tests
static_assert(XSolution<SolutionTSP>);              // verify that this is correctly a XSolution
static_assert(XESolution<ESolutionTSP>);            // verify that this is correctly a XESolution
static_assert(XSearch<ESolutionTSP, ESolutionTSP>); // verify that this is correctly a XSearch

#endif /*TSP_SOLUTION_H_*/
