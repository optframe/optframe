// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_ACTION_HPP_
#define OPTFRAME_ACTION_HPP_

#include <cstdlib>
#include <iostream>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Helper/MultiESolution.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>
#include <OptFrame/Hyper/ComponentHelper.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>

//#include "Command.hpp"

// using namespace std;
// using namespace scannerpp;

namespace optframe {

using scannerpp::Scanner;

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
class HeuristicFactory;
#else
template <typename XES>
class HeuristicFactory;
#endif

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class Action {
 public:
  virtual ~Action() {}

  virtual string usage() = 0;

  virtual bool handleComponent(string type) = 0;

  virtual bool handleComponent(Component& component) = 0;

  virtual bool handleAction(string action) = 0;

  virtual bool doAction(string content, HeuristicFactory<XES>& hf,
                        map<string, string>& dictionary,
                        map<string, vector<string>>& ldictionary) = 0;

  virtual bool doCast(string component, int id, string type, string variable,
                      HeuristicFactory<XES>& hf, map<string, string>& d) = 0;

  static bool addAndRegister(Scanner& scanner, Component& comp,
                             HeuristicFactory<XES>& hf,
                             map<string, string>& d) {
    int index = hf.addComponent(comp);

    if (index == -1) return false;

    if (scanner.hasNext()) {
      string varName = scanner.next();

      stringstream sscomp;
      sscomp << comp.id() << " " << index;

      d[varName] = sscomp.str();  // TODO: fix!!

      return true;
      // return Command<S, XEv>::defineText(varName, sscomp.str(), d);
    }

    return true;
  }

  static bool registerText(Scanner& scanner, string value,
                           map<string, string>& d) {
    if (scanner.hasNext()) {
      string varName = scanner.next();

      d[varName] = value;  // TODO: fix!!

      return true;
      // return Command<S, XEv>::defineText(varName, sscomp.str(), d);
    } else {
      cout << "Action error: no variable to store value '" << value << "'"
           << endl;
      return false;
    }
  }

  static string formatDouble(double d) {
    stringstream ss;
    ss << fixed;
    ss << d;
    return ss.str();
  }

  static string formatInt(int i) {
    stringstream ss;
    ss << i;
    return ss.str();
  }

  static string formatBool(bool b) {
    if (b)
      return "true";
    else
      return "false";
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class ComponentAction : public Action<XES> {
 public:
  virtual ~ComponentAction() {}

  virtual string usage() {
    return "OptFrame: idx  log  output_variable\nOptFrame: idx  print";
  }

  virtual bool handleComponent(string type) {
    return ComponentHelper::compareBase(Component::idComponent(), type);
  }

  virtual bool handleComponent(Component& component) {
    return component.compatible(Component::idComponent());
  }

  virtual bool handleAction(string action) {
    return (action == "log") || (action == "print") ||
           (action == "setVerboseLevel") || (action == "getVerboseLevel");
  }

  virtual bool doCast(string component, int id, string type, string variable,
                      HeuristicFactory<XES>& hf, map<string, string>& d) {
    if (!handleComponent(type)) {
      cout << "ComponentAction::doCast error: can't handle component type '"
           << type << " " << id << "'" << endl;
      return false;
    }

    Component* comp = hf.components[component].at(id);

    if (!comp) {
      cout << "ComponentAction::doCast error: nullptr component '" << component
           << " " << id << "'" << endl;
      return false;
    }

    // cast object to upper base

    if (!ComponentHelper::compareBase(comp->id(), type)) {
      cout << "ComponentAction::doCast error: component '" << comp->id()
           << " is not base of " << type << "'" << endl;
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

  virtual bool doAction(string content, HeuristicFactory<XES>& hf,
                        map<string, string>& dictionary,
                        map<string, vector<string>>& ldictionary) {
    // cout << "Evaluation::doAction '" << content << "'" << endl;

    Scanner scanner(content);

    if (!scanner.hasNext()) return false;

    Component* c;
    hf.assign(c, *scanner.nextInt(), scanner.next());

    if (!c) return false;

    if (!scanner.hasNext()) return false;

    string action = scanner.next();

    if (!handleAction(action)) return false;

    // TODO: log must be implemented in other manner... maybe store as string
    // somewhere else.. maybe, just support this in specific scenarios
    /*
                if (action == "log")
                {
                        if (!scanner.hasNext())
                                return false;

                        string var = scanner.next();

                        stringstream ss;
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

      LogLevel verboseLevel = (LogLevel)*scanner.nextInt();

      c->setMessageLevel(verboseLevel);

      return true;
    }

    if (action == "getVerboseLevel") {
      if (!scanner.hasNext()) return false;

      string var = scanner.next();

      stringstream ss;
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
