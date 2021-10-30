// OptFrame Demo 0-1 Knapsack Problem + Simulated Annealing
// File: KP-fcore-ex.hpp
#pragma once

#include <OptFCore/FCore.hpp>
#include <iostream>
#include <vector>

#include <algorithm>
#include <functional>

#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp> // many metaheuristics here...
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

using namespace std;
using namespace optframe;
using namespace scannerpp;

// namespace para o problema da mochila
// namespace KP_fcore {

// Solução para o problema da mochila, e elemento de avaliação

// ================================
//    Definição do 'ESolution'
// Par: (Representação, Avaliação)
// ================================

using ESolutionKP = std::pair<
  std::vector<bool>, // (representation)
  Evaluation<int>    // (objective value)
  >;
