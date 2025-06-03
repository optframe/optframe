// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef PN_PROBLEMINSTANCE_H_
#define PN_PROBLEMINSTANCE_H_

// C++
#include <iostream>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>

using namespace std;
using namespace scannerpp;
using namespace optframe;

namespace PN {

class ProblemInstance : public Component {
 private:
  // Your private vars

 public:
  int size;
  vector<double> nums;
  ProblemInstance(Scanner& scanner);

  virtual ~ProblemInstance();

  virtual std::string toString() const { return "PROBLEM_PN"; }
};

}  // namespace PN

#endif /*PN_PROBLEMINSTANCE_H_*/
