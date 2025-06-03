// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#include <PN/ProblemInstance.h>

using namespace PN;

ProblemInstance::ProblemInstance(Scanner& scanner) {
  // Le o tamanho da instancia
  size = *scanner.nextInt();

  // Cria o vetor com o tamanho da instancia
  nums = vector<double>(size);

  // Le os numeros e os coloca no vetor
  for (int i = 0; i < size; i++) nums[i] = *scanner.nextDouble();
}

ProblemInstance::~ProblemInstance() {}
