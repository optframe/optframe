// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HYPER_HEURISTICFACTORY_HPP_
#define OPTFRAME_HYPER_HEURISTICFACTORY_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C++
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
// thirdparty
#include <OptFrame/modlog/modlog.hpp>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Core/MultiESolution.hpp>
#include <OptFrame/Core/MultiSolution.hpp>
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Heuristics/LocalSearches/EmptyLS.hpp>
#include <OptFrame/Hyper/Action.hpp>
#include <OptFrame/Hyper/ComponentHelper.hpp>
#include <OptFrame/Hyper/OptFrameList.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Search/LocalSearch.hpp>
#include <OptFrame/Search/MultiObjSearch.hpp>
#include <OptFrame/Search/SingleObjSearch.hpp>

// Hyper
#include <OptFrame/Search/LocalSearchBuilder.hpp>
#include <OptFrame/Search/SingleObjSearchBuilder.hpp>

// Heuristics
#include <OptFrame/Heuristics/LocalSearches/BI.hpp>
#include <OptFrame/Heuristics/LocalSearches/CS.hpp>
#include <OptFrame/Heuristics/LocalSearches/EmptyLS.hpp>
#include <OptFrame/Heuristics/LocalSearches/FI.hpp>
#include <OptFrame/Heuristics/LocalSearches/HC.hpp>
#include <OptFrame/Heuristics/LocalSearches/RDM.hpp>
#include <OptFrame/Heuristics/LocalSearches/RVND.hpp>
#include <OptFrame/Heuristics/LocalSearches/VND.hpp>
#include <OptFrame/Heuristics/LocalSearches/VNDUpdateADS.hpp>
//
#include <OptFrame/Heuristics/LocalSearches/BuilderBI.hpp>
#include <OptFrame/Heuristics/LocalSearches/BuilderCS.hpp>
#include <OptFrame/Heuristics/LocalSearches/BuilderEmptyLS.hpp>
#include <OptFrame/Heuristics/LocalSearches/BuilderFI.hpp>
#include <OptFrame/Heuristics/LocalSearches/BuilderHC.hpp>
#include <OptFrame/Heuristics/LocalSearches/BuilderRDM.hpp>
#include <OptFrame/Heuristics/LocalSearches/BuilderRVND.hpp>
#include <OptFrame/Heuristics/LocalSearches/BuilderVND.hpp>
#include <OptFrame/Heuristics/LocalSearches/BuilderVNDUpdateADS.hpp>
// #include "Heuristics/LocalSearches/RVND.hpp"

// Metaheuristics
#include <OptFrame/Heuristics/EmptySingleObjSearch.hpp>
#include <OptFrame/Heuristics/ILS/BasicIteratedLocalSearch.hpp>
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp>
#include <OptFrame/Heuristics/ILS/MetaILS.hpp>
// TODO ERROR on IntensifiedIteratedLocalSearchLevels
// #include "Heuristics/ILS/IntensifiedIteratedLocalSearchLevels.hpp"
#include <OptFrame/Heuristics/GRASP/BasicGRASP.hpp>
#include <OptFrame/Heuristics/ILS/Intensification.hpp>
#include <OptFrame/Heuristics/TS/BasicTabuSearch.hpp>
// TODO ERROR on BasicGeneticAlgorithm
// #include "Heuristics/EvolutionaryAlgorithms/BasicGeneticAlgorithm.hpp"
#include <OptFrame/Heuristics/EmptyMultiObjSearch.hpp>
#include <OptFrame/Heuristics/SA/BasicSA.hpp>
// #include "Heuristics/VNS/MOVNS.hpp"

// #include <OptFrame/Helper/Solutions/CopySolution.hpp>
#include <OptFrame/Component.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/Hyper/ComponentMultiBuilder.hpp>

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

// design pattern: Factory

namespace optframe {

// ===========================================================================
// XES is base-type for the Primary type
// It should be enough for XESS single objective Primary Type
// It should also be enough to form a base-type on X2ESolution Primary-types
// Extra types can be passed here for alternative supported spaces, such as RK
// Keep-it-simple for now, and just assume defaults
//
// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES>  // do not put XESSolution here... not only
                                      // Single!
#else
MOD_EXPORT template <typename XES>
#endif
class HeuristicFactory {
  // TODO: check if this is necessary:  X2ESolution<XES> X2ES / typename X2ES
  //
  // decomposing base-type XES into <S, XEv>
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  // TODO: perhaps add this to template!!!
  using XMEv = MultiEvaluation<typename XEv::objType>;
  using XMES = std::pair<S, XMEv>;
  using X2MES = VEPopulation<XMES>;

 private:
  // for debug purposes (mainly 'memory debug')
  modlog::LogLevel loglevel;
  // default random number generator
  sref<RandGen> rg;

 public:
  // output stream (defaults to std::cout)
  std::ostream* logdata{&std::cout};

  using FuncLogPrefix = typename modlog::LogConfig::FuncLogPrefix;
  FuncLogPrefix fprefixlogs{[](std::ostream& os, modlog::LogLevel l,
                               std::tm local_tm, std::chrono::microseconds us,
                               std::uintptr_t tid, std::string_view short_file,
                               int line, bool debug) -> std::ostream& {
    std::string slevel;
    if (l == modlog::LogLevel::Info)
      slevel = "info";
    else if (l == modlog::LogLevel::Warning)
      slevel = "warn";
    else if (l == modlog::LogLevel::Error)
      slevel = "error";
    else if (l == modlog::LogLevel::Debug)
      debug = true;
    if (debug) slevel = "debug";
    os << "level=" << slevel << " from=HF";
    if (debug) os << " caller=" << short_file << ":" << line;
    os << " msg=";
    return os;
  }};

  std::ostream* logstream{&std::cout};

  modlog::LogConfig log() {
    return {.os = logdata, .minlog = loglevel, .fprefixdata{fprefixlogs}};
  }

 public:
  std::map<std::string, std::vector<sptr<Component>>> components;
  std::vector<ComponentBuilder<XES>*> builders;
  std::vector<ComponentMultiBuilder<S, XMEv, XMES, X2MES>*> xmes_builders;
  std::vector<Action<XES>*> actions;
  std::map<std::string, std::vector<std::vector<sptr<Component>>>>
      componentLists;

  explicit HeuristicFactory(
      modlog::LogLevel _loglevel = modlog::LogLevel::Warning)
      : loglevel{_loglevel}, rg{new RandGen} {}

  explicit HeuristicFactory(
      sref<RandGen> _rg, modlog::LogLevel _loglevel = modlog::LogLevel::Warning)
      : loglevel{_loglevel}, rg{_rg} {}

  virtual ~HeuristicFactory() {
    clear();

    for (unsigned i = 0; i < builders.size(); i++) delete builders.at(i);
    builders.clear();

    for (unsigned i = 0; i < xmes_builders.size(); i++)
      delete xmes_builders.at(i);
    xmes_builders.clear();

    for (unsigned i = 0; i < actions.size(); i++) delete actions.at(i);
    actions.clear();
  }

  const modlog::LogLevel getLogLevel() const { return this->loglevel; }

  void setLogLevel(modlog::LogLevel ll) {
    this->loglevel = ll;
    // must apply loglevel to all builders
    for (unsigned i = 0; i < builders.size(); i++) {
      if (ll == modlog::LogLevel::Debug)
        Log(modlog::LogLevel::Debug)
            << "Setting builder LogLevel to Debug: " << builders[i]->id()
            << std::endl;
      builders[i]->setMessageLevel(ll);
    }
  }

  ComponentBuilder<XES>* getBuilder(std::string id) {
    for (unsigned i = 0; i < builders.size(); i++)
      if (builders[i]->id() == id) return builders[i];
    return nullptr;
  }

  bool inComponents(sptr<Component> c) {
    std::map<std::string, std::vector<sptr<Component>>>::iterator iter;
    for (iter = components.begin(); iter != components.end(); iter++) {
      std::vector<sptr<Component>> v = iter->second;

      for (unsigned int i = 0; i < v.size(); i++)
        if (v[i] == c) return true;
    }

    return false;
  }

  /// compName is an optional reference return value
  /// compNumber is an optional reference return value
  sptr<Component> getNextComponent(Scanner& scanner,
                                   std::string* compName = nullptr,
                                   int* compNumber = nullptr) {
    if (!scanner.hasNext()) return nullptr;

    std::string id = scanner.next();

    if (!scanner.hasNextInt()) return nullptr;

    int inumber = *scanner.nextInt();

    if (inumber < 0) return nullptr;

    unsigned number = inumber;

    sptr<Component> component = nullptr;

    if (id[0] == ':') {
      // COMPONENT SHORTCUT!
      // look for pattern
      std::map<std::string, std::vector<std::shared_ptr<Component>>>::iterator
          iter;
      for (iter = components.begin(); iter != components.end(); iter++) {
        std::string name = iter->first;

        int p = name.find(id, 0);
        if ((p > 0) && (p + id.length() == name.length())) {
          // exact match after position 'p'
          component = iter->second.at(number);
          id = name;
          break;
        }
      }
      if (!component) {
        if (getLogLevel() <= modlog::LogLevel::Warning)
          std::cout << "HeuristicFactory warning: pattern of component '" << id
                    << " " << number << "' not found" << std::endl;
      }
    } else {
      // look for exact
      if (components.count(id) > 0) {
        std::vector<sptr<Component>> v = components[id];

        if (number < v.size()) component = v[number];
      } else {
        if (getLogLevel() <= modlog::LogLevel::Warning)
          std::cout << "HeuristicFactory warning: component '" << id << " "
                    << number << "' not found!" << std::endl;
      }
    }

    if (compName && compNumber) {
      (*compName) = id;
      (*compNumber) = number;
    }

    return component;
  }

  template <class T>
  sptr<T> tryAssign(Scanner& scanner) {
    sptr<T> myComponent;
    if (scanner.hasNext()) {
      std::string sid_0 = scanner.next();
      if (scanner.hasNextInt()) {
        int id_0 = *scanner.nextInt();
        this->assign(myComponent, id_0, sid_0);
      }
    }
    if (!myComponent) {
      using modlog::LogLevel::Warning;
      Log(Warning, this) << "tryAssign failed to load component" << std::endl;
    }
    return myComponent;
  }

  template <class T>
  sptr<T> tryAssignIf(bool condition, Scanner& scanner, int& counter) {
    counter++;
    if (!condition) {
      using modlog::LogLevel::Warning;
      Log(Warning, this) << "tryAssignIf skipping parameter #" << counter
                         << std::endl;
      return nullptr;
    }
    sptr<T> myComponent;
    if (scanner.hasNext()) {
      std::string sid_0 = scanner.next();
      if (scanner.hasNextInt()) {
        int id_0 = *scanner.nextInt();
        this->assign(myComponent, id_0, sid_0);
      }
    }
    if (!myComponent) {
      using modlog::LogLevel::Warning;
      Log(Warning, this) << "tryAssignIf failed to load component #" << counter
                         << std::endl;
    }
    return myComponent;
  }

  template <class T>
  op<vsptr<T>> tryAssignListIf(bool condition, Scanner& scanner, int& counter) {
    counter++;
    if (!condition) {
      using modlog::LogLevel::Warning;
      Log(Warning, this) << "tryAssignListIf skipping parameter #" << counter
                         << std::endl;
      return std::nullopt;
    }
    vsptr<T> myComponentList;
    if (scanner.hasNext()) {
      std::string sid_0 = scanner.next();
      if (scanner.hasNextInt()) {
        int id_0 = *scanner.nextInt();
        bool b = this->assignList(myComponentList, id_0, sid_0);
        if (!b) return std::nullopt;
      }
    }

    for (sptr<T> x : myComponentList) {
      if (!x) {
        using modlog::LogLevel::Warning;
        Log(Warning, this) << "tryAssignListIf failed to load internal list "
                              "component of parameter #"
                           << counter << std::endl;
        return std::nullopt;
      }
    }

    return myComponentList;
  }

  op<double> tryAssignDoubleIf(bool condition, Scanner& scanner, int& counter) {
    counter++;
    if (!condition) {
      using modlog::LogLevel::Warning;
      Log(Warning, this) << "tryAssignDoubleIf skipping parameter #" << counter
                         << std::endl;
      return std::nullopt;
    }
    op<double> myComponent;
    if (scanner.hasNextDouble()) myComponent = scanner.nextDouble();

    if (!myComponent) {
      using modlog::LogLevel::Warning;
      Log(Warning, this) << "tryAssignDoubleIf failed to load component #"
                         << counter << std::endl;
    }
    return myComponent;
  }

  op<int> tryAssignIntIf(bool condition, Scanner& scanner, int& counter) {
    counter++;
    if (!condition) {
      using modlog::LogLevel::Warning;
      Log(Warning, this) << "tryAssignIntIf skipping parameter #" << counter
                         << std::endl;
      return std::nullopt;
    }
    op<int> myComponent;
    if (scanner.hasNextDouble()) myComponent = scanner.nextInt();

    if (!myComponent) {
      using modlog::LogLevel::Warning;
      Log(Warning, this) << "tryAssignIntIf failed to load component #"
                         << counter << std::endl;
    }
    return myComponent;
  }

  template <class T>
  void assign(std::shared_ptr<T>& component, unsigned number, std::string id) {
    // NOTE THAT component is likely to be NULL!!
    if (getLogLevel() <= modlog::LogLevel::Debug) {
      std::cout << "Debug: hf will try to assign component '"
                << (component ? component->id() : "nullptr");
      std::cout << "' with id = '" << id << " #" << number << std::endl;
    }
    // check prefix "OptFrame:"
    if (id[0] != 'O') {
      std::string id2 = id;
      id = "OptFrame:";
      id.append(id2);
    }

    if (!ComponentHelper::compareBase(T::idComponent(), id)) {
      if (getLogLevel() <= modlog::LogLevel::Warning)
        std::cout << "HeuristicFactory: incompatible assign '"
                  << T::idComponent() << "' <- '" << id << "'" << std::endl;

      component = nullptr;
      return;
    }

    if (components.count(id) > 0) {
      std::vector<sptr<Component>>& v = components[id];
      if (number < v.size()) {
        // component = std::shared_ptr<T>((T*)v[number].get()); // need to cast
        // to type T...
        component = std::shared_ptr<T>(
            (std::shared_ptr<T>&)v[number]);  // need to cast to type T...
        return;
      }
    } else {
      if (getLogLevel() <= modlog::LogLevel::Warning)
        std::cout << "'" << id << "' not found!" << std::endl;
    }

    // not found!
    component = nullptr;
  }

  template <class T>
  bool assignList(std::vector<std::shared_ptr<T>>& cList, unsigned number,
                  std::string _listId) {
    // type checking for safety!
    std::string noList = ComponentHelper::typeOfList(_listId);
    std::string listId = noList;
    listId += "[]";

    if (!ComponentHelper::compareBase(T::idComponent(), noList)) {
      if (getLogLevel() <= modlog::LogLevel::Warning)
        std::cout << "HeuristicFactory: incompatible list assign '["
                  << T::idComponent() << "]' <- '[" << noList << "]'"
                  << std::endl;

      return false;
    }

    if (componentLists.count(listId) > 0) {
      std::vector<std::vector<sptr<Component>>>& vv = componentLists[listId];
      if (number < vv.size()) {
        for (unsigned i = 0; i < vv[number].size(); i++)
          cList.push_back(sptr<T>((std::shared_ptr<T>&)vv[number][i]));
      }
    } else {
      if (getLogLevel() <= modlog::LogLevel::Warning)
        std::cout << "'" << listId << " " << number << "' not found!"
                  << std::endl;
      return false;
    }
    return true;
  }

  // TODO: experimental
  bool checkCompatible(std::string scomponent, std::string starget) {
    // This is equivalent to: component->compatible(starget)
    // Meaning that, either scomponent == starget, or starget is subpart of
    // scomponent To make this beautiful, let's split in parts, separated by ':'
    Scanner scan1(scomponent);
    scan1.useSeparators(":");
    std::vector<std::string> vparts1;
    while (scan1.hasNext()) vparts1.push_back(scan1.next());
    //
    Scanner scan2(starget);
    scan2.useSeparators(":");
    std::vector<std::string> vparts2;
    while (scan2.hasNext()) vparts2.push_back(scan2.next());
    //
    if (getLogLevel() <= modlog::LogLevel::Debug) {
      std::cout << "DEBUG: HF checkCompatible(" << scomponent << ";" << starget
                << ")" << std::endl;
      for (unsigned i = 0; i < vparts1.size(); i++)
        std::cout << vparts1[i] << " | ";
      std::cout << std::endl;
      for (unsigned i = 0; i < vparts2.size(); i++)
        std::cout << vparts2[i] << " | ";
      std::cout << std::endl;
    }

    if ((vparts1[0] != std::string{"OptFrame"}) ||
        (vparts2[0] != std::string{"OptFrame"})) {
      std::cout << "HF: WARNING: checkCompatible has STRANGE NON-OptFrame "
                   "components '"
                << scomponent << "' AND '" << starget << "'" << std::endl;
      return false;
    }
    // check if equals
    if (scomponent == starget) return true;
    // cannot upcast
    if (vparts2.size() > vparts1.size()) {
      std::cout << "HF: WARNING: cannot upcast components '" << scomponent
                << "' AND '" << starget << "'" << std::endl;
      return false;
    }
    // check if starget is subpart, a.k.a., "subclass"
    for (unsigned i = 0; i < vparts2.size(); i++)
      if (vparts1[i] != vparts2[i]) return false;
    return true;
  }

  int addComponent(sref<Component> component, std::string id) {
    // NO NEED FOR DOUBLE POINTER WARNING ANYMORE... LONG LIVE SHARED_PTR!!!
    /*
      if (inComponents(component.sptr())) {
         if (getLogLevel() <= modlog::LogLevel::Warning)
            std::cout << "WARNING: HeuristicFactory addComponent: component '"
      << component->id() << "' already registered!" << std::endl;
         //return -1;
      }
      */

    bool b = checkCompatible(component->id(), id);
    if (getLogLevel() <= modlog::LogLevel::Debug)
      std::cout << "DEBUG: HF checkCompatible returns b=" << b << std::endl;

    bool b2 = component->compatible(id);

    // experimental: disagreement is not ok
    // xor
    if (b ^ b2) {
      if (getLogLevel() <= modlog::LogLevel::Warning) {
        std::cout << "HF WARNING: disagreement between b and b2! Must Fix this!"
                  << std::endl;
        // assert(false);
        std::cout << "Will not do anything if any is true... b=" << b
                  << " b2=" << b2 << std::endl;
      }
    }

    // experimental: but agreement is ok
    if ((!b) && (!b2)) {
      if (getLogLevel() <= modlog::LogLevel::Warning) {
        std::cout << "HeuristicFactory addComponent: incompatible components. "
                     "NOTE: component->id():'";
        std::cout << component->id() << "' ->compatible('" << id
                  << "') returned FALSE! " << std::endl;
        std::cout << "Rejecting component '" << component->toString() << "'."
                  << std::endl;
      }

      return -1;
    }

    // agreement: ok
    // assert(b && b2);

    std::vector<std::shared_ptr<Component>>& v = components[id];
    std::shared_ptr<Component> scomp = component.sptr();
    v.push_back(scomp);

    int idx = components[id].size() - 1;

    // std::cout << "HeuristicFactory: added component '" << id << " " << idx <<
    // "'"
    // << std::endl;

    return idx;
  }

  int addComponentRef(Component& component) {
    return addComponent(component, component.id());
  }

  template <class T>
  void readComponent(T*& component, Scanner& scanner) {
    std::string tmp = scanner.next();

    if (tmp != T::idComponent()) {
      if (getLogLevel() <= modlog::LogLevel::Error)
        std::cout << "Error: expected '" << T::idComponent() << "' and found '"
                  << tmp << "'." << std::endl;

      component = nullptr;

      return;
    }

    unsigned int number = *scanner.nextInt();

    component = nullptr;

    assign(component, number, tmp);
  }

  int addComponentListRef(vsref<Component>& cList, std::string _listId) {
    std::vector<sptr<Component>> sptrList;
    for (unsigned i = 0; i < cList.size(); i++)
      sptrList.push_back(cList[i].sptr());
    return addComponentList(sptrList, _listId);
  }

  int addComponentList(std::vector<sptr<Component>>& cList,
                       std::string _listId) {
    // type checking for safety!
    std::string noList = ComponentHelper::typeOfList(_listId);
    std::string listId = noList;
    listId += "[]";

    for (unsigned i = 0; i < cList.size(); i++)
      if ((cList[i] == nullptr) || (!cList[i]->compatible(noList))) {
        Component::logWarn(getLogLevel(), "HeuristicFactory", *logdata)
            << "incompatible components '" << cList[i]->id() << "' and '"
            << ComponentHelper::typeOfList(listId) << "'!" << std::endl;

        return -1;
      }

    std::vector<std::vector<sptr<Component>>>& v = componentLists[listId];
    v.push_back(cList);

    int idx = componentLists[listId].size() - 1;

    Component::logInfo(getLogLevel(), "HeuristicFactory", *logdata)
        << "adding to list type '" << listId << "' id " << idx << " size "
        << cList.size() << std::endl;

    return idx;
  }

  int addComponentList(std::vector<Component*>& cList) {
    if ((cList.size() > 0) && (cList[0] != nullptr)) {
      std::string listId = cList[0]->id();
      listId += "[]";

      return addComponentList(cList, listId);
    } else {
      return -1;
    }
  }

  //! \english listComponents lists all available components that match a given
  //! pattern. \endenglish \portuguese listComponents lista todos componentes
  //! disponiveis que coincidem com um padrao dado. \endportuguese
  /*!
                 \sa listComponents(std::string)
         */
  std::vector<std::string> listComponents(std::string_view basePattern) {
    std::vector<std::string> list;

    std::map<std::string, std::vector<std::shared_ptr<Component>>>::iterator
        iter;

    for (iter = components.begin(); iter != components.end(); iter++) {
      //
      std::vector<std::shared_ptr<Component>> v = iter->second;
      // std::vector<Component*> v = iter->second;

      for (unsigned int i = 0; i < v.size(); i++)
        if (ComponentHelper::compareBase(basePattern, v[i]->id())) {
          std::stringstream ss;
          ss << iter->first << " " << i;
          list.push_back(ss.str());
        }
    }

    return list;
  }

  //! \english listAllComponents lists all available OptFrame components.
  //! \endenglish \portuguese listAllComponents lista todos os componentes do
  //! OptFrame disponiveis. \endportuguese
  /*!
                 \sa listAllComponents()
         */

  std::vector<std::string> listAllComponents() {
    return listComponents("OptFrame:");
  }

  //! \english listComponents lists all available components that match a given
  //! pattern. \endenglish \portuguese listComponents lista todos componentes
  //! disponiveis que coincidem com um padrao dado. \endportuguese
  /*!
                 \sa listComponents(std::string)
         */
  std::vector<std::string> listComponentLists(std::string pattern) {
    if (getLogLevel() <= modlog::LogLevel::Debug)
      std::cout << "listing component lists for pattern = '" << pattern << "'"
                << std::endl;
    std::vector<std::string> list;

    std::map<std::string, std::vector<std::vector<sptr<Component>>>>::iterator
        iter;

    for (iter = componentLists.begin(); iter != componentLists.end(); iter++) {
      std::vector<std::vector<std::shared_ptr<Component>>>& vl = iter->second;

      for (unsigned int i = 0; i < vl.size(); i++)
        if (ComponentHelper::compareBase(pattern, iter->first)) {
          std::stringstream ss;
          ss << iter->first << " " << i;
          list.push_back(ss.str());
        }
    }

    return list;
  }

  //! \english listBuilders lists all component builders that match a given
  //! pattern, with their respective parameters. \endenglish \portuguese
  //! listBuilders lista todos component builders, com seus respectivos
  //! parametros, que coincidem com um padrao dado. \endportuguese
  /*!
                 \sa listComponents(std::string)
         */

  using VParameters = std::vector<std::pair<std::string, std::string>>;

  std::vector<std::pair<std::string, VParameters>> listBuilders(
      std::string_view pattern) {
    std::vector<std::pair<std::string, VParameters>> list;

    for (unsigned i = 0; i < builders.size(); i++)
      if (ComponentHelper::compareBase(pattern, builders[i]->id()))
        list.push_back(
            std::make_pair(builders[i]->id(), builders[i]->parameters()));

    return list;
  }

  // ================================================================
  // ================================================================

  bool drop(std::string type, int id) {
    if (components.count(type) > 0) {
      std::vector<sptr<Component>> v = components[type];

      if (id < ((int)v.size())) {
        if (v[id] != nullptr) {
          // delete v[id];
          v[id] = nullptr;
          components[type] = v;

          return true;
        }  // else return false?
      }  // else return false?
    }

    return false;
  }

  // EMPTY ALL LISTS!
  // Components, Builders and Actions
  void clear() {
    std::map<std::string, std::vector<std::shared_ptr<Component>>>::iterator
        iter;

    for (iter = components.begin(); iter != components.end(); iter++) {
      std::vector<std::shared_ptr<Component>>& v = iter->second;

      for (unsigned int i = 0; i < v.size(); i++) {
        if (getLogLevel() <= modlog::LogLevel::Debug)
          std::cout << "HF: will clear v.toString()=" << v[i]->toString()
                    << std::endl;
        // delete v[i];
        v[i] = nullptr;
      }

      if (getLogLevel() <= modlog::LogLevel::Debug)
        std::cout << "    => HF: CLEARING COMPONENT: '" << iter->first << "'"
                  << std::endl;
      //
      // TODO: MUST KEEP LINE BELOW!
      // iter->second.clear();
      v.clear();
    }

    std::map<std::string, std::vector<std::vector<sptr<Component>>>>::iterator
        iter2;

    for (iter2 = componentLists.begin(); iter2 != componentLists.end();
         iter2++) {
      std::vector<std::vector<std::shared_ptr<Component>>>& v = iter2->second;

      for (unsigned int i = 0; i < v.size(); i++)
        // delete v[i];
        v[i].clear();

      // Should not delete the components inside lists. They're already deleted!
      iter2->second.clear();
    }
  }

  sref<RandGen> getRandGen() { return rg; }

  std::pair<sptr<LocalSearch<XES>>, std::string> createLocalSearch(
      std::string str) {
    Scanner scanner(str);

    // No heuristic!
    if (!scanner.hasNext())
      return std::pair<sptr<LocalSearch<XES>>, std::string>(nullptr, "");

    std::string h = scanner.next();

    if ((h == LocalSearch<XES>::idComponent()) || (h == "LocalSearch")) {
      unsigned int id = *scanner.nextInt();

      sptr<LocalSearch<XES>> mtd = nullptr;

      assign(mtd, id, LocalSearch<XES>::idComponent());

      if (!mtd)
        return std::pair<sptr<LocalSearch<XES>>, std::string>(new EmptyLS<XES>,
                                                              scanner.rest());

      return make_pair(mtd, scanner.rest());
    }

    if (h == EmptyLS<XES>::idComponent())
      return std::pair<sptr<LocalSearch<XES>>, std::string>(new EmptyLS<XES>,
                                                            scanner.rest());

    for (unsigned i = 0; i < builders.size(); i++) {
      // build local search directly by builder name
      if (builders[i]->id() == h) {
        LocalSearch<XES>* ls =
            ((LocalSearchBuilder<XES>*)(builders[i]))->build(scanner, *this);
        return std::pair<sptr<LocalSearch<XES>>, std::string>(ls,
                                                              scanner.rest());
      }

      // locate builder by local search name
      if (builders[i]->canBuild(h)) {
        LocalSearch<XES>* ls =
            ((LocalSearchBuilder<XES>*)(builders[i]))->build(scanner, *this);
        return std::pair<sptr<LocalSearch<XES>>, std::string>(ls,
                                                              scanner.rest());
      }
    }

    if (getLogLevel() <= modlog::LogLevel::Warning)
      std::cout
          << "HeuristicFactory::createLocalSearch warning: no LocalSearch '"
          << h << "' found! ignoring..." << std::endl;

    return std::pair<sptr<LocalSearch<XES>>, std::string>(nullptr,
                                                          scanner.rest());
  }

  std::pair<sptr<SingleObjSearch<XES>>, std::string> createSingleObjSearch(
      std::string str) {
    Scanner scanner(str);

    // No heuristic!
    if (!scanner.hasNext())
      return std::pair<sptr<SingleObjSearch<XES>>, std::string>(nullptr, "");

    std::string h = scanner.next();

    if (h == SingleObjSearch<XES>::idComponent()) {
      unsigned int id = *scanner.nextInt();

      sptr<SingleObjSearch<XES>> mtd = nullptr;

      assign(mtd, id, SingleObjSearch<XES>::idComponent());

      if (!mtd)
        return std::pair<sptr<SingleObjSearch<XES>>, std::string>(
            new EmptySingleObjSearch<XES, XEv>, scanner.rest());

      return std::pair<sptr<SingleObjSearch<XES>>, std::string>(mtd,
                                                                scanner.rest());
    }

    if (h == EmptySingleObjSearch<XES, XEv>::idComponent())
      return std::pair<sptr<SingleObjSearch<XES>>, std::string>(
          new EmptySingleObjSearch<XES, XEv>, scanner.rest());

    for (unsigned i = 0; i < builders.size(); i++) {
      // build local search directly by builder name
      if (builders[i]->id() == h) {
        SingleObjSearch<XES>* sios =
            ((SingleObjSearchBuilder<XES>*)(builders[i]))
                ->build(scanner, *this);
        return std::pair<sptr<SingleObjSearch<XES>>, std::string>(
            sios, scanner.rest());
      }

      // locate builder by local search name
      if (builders[i]->canBuild(h)) {
        SingleObjSearch<XES>* sios =
            ((SingleObjSearchBuilder<XES>*)(builders[i]))
                ->build(scanner, *this);
        return std::pair<sptr<SingleObjSearch<XES>>, std::string>(
            sios, scanner.rest());
      }
    }

    if (getLogLevel() <= modlog::LogLevel::Warning)
      std::cout << "HeuristicFactory::createSingleObjSearch warning: no "
                   "SingleObjSearch '"
                << h << "' found! ignoring..." << std::endl;

    return std::pair<sptr<SingleObjSearch<XES>>, std::string>(nullptr,
                                                              scanner.rest());
  }

  std::pair<MultiObjSearch<XMES>*, std::string> createMultiObjSearch(
      std::string str) {
    Scanner scanner(str);

    // No heuristic!
    if (!scanner.hasNext())
      return std::pair<MultiObjSearch<XMES>*, std::string>(nullptr, "");

    std::string h = scanner.next();

    if (h == MultiObjSearch<XMES>::idComponent()) {
      unsigned int id = *scanner.nextInt();

      MultiObjSearch<XES>* mtd = nullptr;

      assign(mtd, id, MultiObjSearch<XMES>::idComponent());

      if (!mtd) return make_pair(new EmptyMultiObjSearch<XES>, scanner.rest());

      return make_pair(mtd, scanner.rest());
    }

    if (h == EmptyMultiObjSearch<XES>::idComponent())
      return make_pair(new EmptyMultiObjSearch<XES>, scanner.rest());

    if (getLogLevel() <= modlog::LogLevel::Warning)
      std::cout << "HeuristicFactory::createMultiObjSearch warning: no "
                   "MultiObjSearch '"
                << h << "' found! ignoring..." << std::endl;

    return std::pair<MultiObjSearch<XMES>*, std::string>(nullptr,
                                                         scanner.rest());
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HYPER_HEURISTICFACTORY_HPP_
