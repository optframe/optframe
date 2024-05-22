// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MOILS_FAMILY_H_
#define OPTFRAME_MOILS_FAMILY_H_

#include <string.h>

using namespace std;

namespace optframe {

class MOILS {
 public:
  static string family() { return "MOILS:"; }

  virtual ~MOILS() {}
};

}  // namespace optframe

#endif /*OPTFRAME_MOILS_FAMILY_H_*/
