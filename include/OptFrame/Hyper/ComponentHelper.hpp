// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_COMPONENT_HELPER_HPP_
#define OPTFRAME_COMPONENT_HELPER_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <cstdlib>
#include <iostream>
#include <vector>
//
#include <OptFrame/Scanner++/Scanner.hpp>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

using scannerpp::Scanner;

MOD_EXPORT class ComponentHelper {
 public:
  // taken from HeuristicFactory

  //! \english compareBase is an auxiliar method to compare a pattern to a
  //! component id. Check if 'component' inherits from 'base'. \endenglish
  //! \portuguese compareBase eh um metodo auxiliar para comparar um padrao a um
  //! id de componente. Testa se 'component' herda de 'base'. \endportuguese
  /*!
         \sa compareBase(std::string, std::string)
         */

  // Check if 'base' is inherited by 'component'
  // EXAMPLE: compareBase("OptFrame:", "OptFrame:Evaluator") returns TRUE!
  static bool compareBase(std::string _base, std::string _component) {
    if ((_base.length() < 3) || (_component.length() < 3)) {
      std::cout << "ComponentHelper::compareBase warning: comparing less than "
                   "3 characters! with base='"
                << _base << "' component='" << _component << "'" << std::endl;
      return false;
    }

    bool baseIsList = (_base.at(_base.length() - 2) == '[') &&
                      (_base.at(_base.length() - 1) == ']');
    bool componentIsList = (_component.at(_component.length() - 2) == '[') &&
                           (_component.at(_component.length() - 1) == ']');

    if (baseIsList != componentIsList) return false;

    // remove list (if exists)
    std::string base = typeOfList(_base);
    std::string component = typeOfList(_component);

    bool sameBase = true;

    if (base.length() <= component.length()) {
      for (unsigned i = 0; i < base.length(); i++)
        if (base.at(i) != component.at(i)) sameBase = false;
    } else
      sameBase = false;

    if (sameBase) return true;

    // ------------------
    // check last family
    // ------------------

    Scanner scanner(base);
    scanner.useSeparators(":");

    std::string family = scanner.next();
    while (scanner.hasNext()) family = scanner.next();

    Scanner scanComponent(component);
    scanComponent.useSeparators(":");
    std::string part;
    while (scanComponent.hasNext()) {
      part = scanComponent.next();
      if (part == family) sameBase = true;
    }

    return sameBase;
  }

  static std::string typeOfList(std::string listId) {
    Scanner scanner(listId);
    scanner.useSeparators(" \t\n[]");

    return scanner.next();
  }
};

}  // namespace optframe

#endif /* OPTFRAME_COMPONENT_HPP_ */
