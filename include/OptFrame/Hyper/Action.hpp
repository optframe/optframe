// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_ACTION_HPP_
#define OPTFRAME_ACTION_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <cstdlib>
#include <iostream>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/MultiESolution.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>
#include <OptFrame/Hyper/ComponentHelper.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

// #include "Command.hpp"

// using namespace std;
// using namespace scannerpp;

namespace optframe {

using scannerpp::Scanner;

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES>
class HeuristicFactory;
#else
MOD_EXPORT template <typename XES>
class HeuristicFactory;
#endif

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES>
#else
MOD_EXPORT template <typename XES>
#endif
class Action {
 public:
  virtual ~Action() {}

  virtual std::string usage() = 0;

  virtual bool handleComponent(std::string type) = 0;

  virtual bool handleComponent(Component& component) = 0;

  virtual bool handleAction(std::string action) = 0;

  virtual bool doAction(
      std::string content, HeuristicFactory<XES>& hf,
      std::map<std::string, std::string>& dictionary,
      std::map<std::string, std::vector<std::string>>& ldictionary) = 0;

  virtual bool doCast(std::string component, int id, std::string type,
                      std::string variable, HeuristicFactory<XES>& hf,
                      std::map<std::string, std::string>& d) = 0;

  static bool addAndRegister(Scanner& scanner, Component& comp,
                             HeuristicFactory<XES>& hf,
                             std::map<std::string, std::string>& d) {
    int index = hf.addComponent(comp);

    if (index == -1) return false;

    if (scanner.hasNext()) {
      std::string varName = scanner.next();

      std::stringstream sscomp;
      sscomp << comp.id() << " " << index;

      d[varName] = sscomp.str();  // TODO: fix!!

      return true;
      // return Command<S, XEv>::defineText(varName, sscomp.str(), d);
    }

    return true;
  }

  static bool registerText(Scanner& scanner, std::string value,
                           std::map<std::string, std::string>& d) {
    if (scanner.hasNext()) {
      std::string varName = scanner.next();

      d[varName] = value;  // TODO: fix!!

      return true;
      // return Command<S, XEv>::defineText(varName, sscomp.str(), d);
    } else {
      std::cout << "Action error: no variable to store value '" << value << "'"
                << std::endl;
      return false;
    }
  }

  static std::string formatDouble(double d) {
    std::stringstream ss;
    ss << std::fixed;
    ss << d;
    return ss.str();
  }

  static std::string formatInt(int i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
  }

  static std::string formatBool(bool b) {
    if (b)
      return "true";
    else
      return "false";
  }
};

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
template <XESolution XES>
#else
template <typename XES>
#endif
class ComponentAction : public Action<XES> {
 public:
  virtual ~ComponentAction() {}

  virtual std::string usage() {
    return "OptFrame: idx  log  output_variable\nOptFrame: idx  print";
  }

  virtual bool handleComponent(std::string type) {
    return ComponentHelper::compareBase(Component::idComponent(), type);
  }

  virtual bool handleComponent(Component& component) {
    return component.compatible(Component::idComponent());
  }

  virtual bool handleAction(std::string action) {
    return (action == "log") || (action == "print") ||
           (action == "setVerboseLevel") || (action == "getVerboseLevel");
  }

  virtual bool doCast(std::string component, int id, std::string type,
                      std::string variable, HeuristicFactory<XES>& hf,
                      std::map<std::string, std::string>& d) {
    if (!handleComponent(type)) {
      std::cout
          << "ComponentAction::doCast error: can't handle component type '"
          << type << " " << id << "'" << std::endl;
      return false;
    }

    Component* comp = hf.components[component].at(id);

    if (!comp) {
      std::cout << "ComponentAction::doCast error: nullptr component '"
                << component << " " << id << "'" << std::endl;
      return false;
    }

    // cast object to upper base

    if (!ComponentHelper::compareBase(comp->id(), type)) {
      std::cout << "ComponentAction::doCast error: component '" << comp->id()
                << " is not base of " << type << "'" << std::endl;
      return false;
    }

    // cast object to lower type
    Component* final = (Component*)comp;

    // remove old component from factory
    hf.components[component].at(id) = nullptr;

    // add new component
    Scanner scanner(variable);
    return ComponentAction<XES>::addAndRegister(scanner, *final, hf, d);
  }

  virtual bool doAction(
      std::string content, HeuristicFactory<XES>& hf,
      std::map<std::string, std::string>& dictionary,
      std::map<std::string, std::vector<std::string>>& ldictionary) {
    // std::cout << "Evaluation::doAction '" << content << "'" << std::endl;

    Scanner scanner(content);

    if (!scanner.hasNext()) return false;

    Component* c;
    hf.assign(c, *scanner.nextInt(), scanner.next());

    if (!c) return false;

    if (!scanner.hasNext()) return false;

    std::string action = scanner.next();

    if (!handleAction(action)) return false;

    // TODO: log must be implemented in other manner... maybe store as
    // std::string somewhere else.. maybe, just support this in specific
    // scenarios
    /*
                if (action == "log")
                {
                        if (!scanner.hasNext())
                                return false;

                        std::string var = scanner.next();

                        std::stringstream ss;
                        ss << c->log();

                        dictionary[var] = ss.str();

                        return true;
                }
      */

    if (action == "print") {
      c->print();

      return true;
    }

    if (action == "setVerboseLevel") {
      if (!scanner.hasNext()) return false;

      modlog::LogLevel verboseLevel = (modlog::LogLevel)*scanner.nextInt();

      c->setMessageLevel(verboseLevel);

      return true;
    }

    if (action == "getVerboseLevel") {
      if (!scanner.hasNext()) return false;

      std::string var = scanner.next();

      std::stringstream ss;
      ss << c->getVerboseLevel();

      dictionary[var] = ss.str();

      return true;
    }

    // no action found!
    return false;
  }
};

}  // namespace optframe

#endif /* OPTFRAME_ACTION_HPP_ */
