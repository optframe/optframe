// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MOILS_FAMILY_H_
#define OPTFRAME_MOILS_FAMILY_H_

#include <string>

namespace optframe {

class FamilyMOILS {
 public:
  static std::string family() { return "MOILS:"; }

  virtual ~FamilyMOILS() {}
};

}  // namespace optframe

#endif /*OPTFRAME_MOILS_FAMILY_H_*/
