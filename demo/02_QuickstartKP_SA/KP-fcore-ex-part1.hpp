// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

// OptFrame Demo 0-1 Knapsack Problem + Simulated Annealing
// File: KP-fcore-ex.hpp
#pragma once

#define NO_CXX_MODULES 1

// C++
#include <algorithm>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>
//
#include <OptFrame/printable/printable.hpp>
using namespace optframe;
//
#include <OptFCore/FCore.hpp>
#include <OptFCore/FCoreAll.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT

// namespace para o problema da mochila
// namespace KP_fcore {

// Solução para o problema da mochila, e elemento de avaliação

// ================================
//    Definição do 'ESolution'
// Par: (Representação, Avaliação)
// ================================

using ESolutionKP = std::pair<std::vector<bool>,  // (representation)
                              Evaluation<int>     // (objective value)
                              >;
