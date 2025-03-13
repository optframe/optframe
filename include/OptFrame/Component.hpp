// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_COMPONENT_HPP_
#define OPTFRAME_COMPONENT_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <cstdlib>
#include <iostream>
#include <memory>  // for shared_ptr
#include <sstream>
#include <string>
#include <vector>  // only for OPTFRAME_AC

//
#include <OptFrame/SemStream.hpp>

#define MOD_EXPORT
#else

// import optframe.component;
import std;

import optframe.semstream;

// maybe do not export here? leave it to .cppm only...
// export module optframe.core.move;

#define MOD_EXPORT export

#endif

namespace optframe {

// SORRY! BUT WE NEED THESE!
using std::cout, std::endl;
using std::fixed;
using std::ostream;
using std::pair, std::make_pair, std::string, std::vector;
using std::stringstream;

/*
// OPTFRAME ALGORITHM COMPREHENSION FLAG
#ifndef OPTFRAME_FAST
#define OPTFRAME_AC
#endif
*/

// |      1      |      2      |      3      |      4      |
// |    error    |   warning   | information | debug/verb. | => this direction =
// more verbose...

// One can use logs like this:
// Suppose my 'loglevel' is Info (default)
// Then, I would also allow everything more restrict than Info, such as Warnings
// and Errors So, if my 'loglevel' (3) is higher than Warning (2), warnings will
// also display, but Debug (4) will not. Example: if (loglevel >=
// LogLevel::Warning) { ... } translates to: if loglevel "more verbose than"
// warning ...
//

enum LogLevel { Silent = 0, Error = 1, Warning = 2, Info = 3, Debug = 4 };

enum StringFormat {
  Undefined = 0,  // undefined... typically 'Human'
  Human = 1,      // human readable
  NoSpaces = 2,   // raw: no spaces (undescore is allowed)
  JSON = 3        // json format
};

MOD_EXPORT class Component;

MOD_EXPORT class ContextAC {
 public:
  std::string id;
  std::string message;
  std::shared_ptr<Component> payload;
};

MOD_EXPORT class Component {
 public:
  // Log* logdata;
  //
  //  'logdata' is for User logs
  std::ostream* logdata{&std::cout};
  // 'mlog' is for machine logs (disabled by default)
  std::ostream* mlog{nullptr};

  static bool safe_delete(Component* c) {
    if (c) {
      delete c;  // NOLINT
      return true;
    } else {
      return false;
    }
  }

  static void safe_print(std::ostream& os, Component* c) {
    // assert(c);
    if (c)
      c->print();
    else
      os << "nullptr Component" << std::endl;
  }

 public:
  static std::ostream& logInfo(LogLevel ll, std::string prefix = "",
                               std::ostream& os = std::cout) {
    if (ll >= LogLevel::Info) {
      if (prefix.length() > 0)
        os << "INFO(" << prefix << "): ";  // add prefix if non-empty
      return os;
    } else {
      return osNull;
    }
  }

  static std::ostream& logWarn(LogLevel ll, std::string prefix = "",
                               std::ostream& os = std::cout) {
    if (ll >= LogLevel::Warning) {
      if (prefix.length() > 0)
        os << "WARN(" << prefix << "): ";  // add prefix if non-empty
      return os;
    } else {
      return osNull;
    }
  }

  // Set user log stream recursive: must be implemented on each component.
  // Returns 'false' is not implemented.
  virtual bool setLogR(std::ostream* _logdata) {
    this->logdata = _logdata;
    return false;
  }

  // Set machine log recursive: must be implemented on each component. Returns
  // 'false' is not implemented.
  virtual bool setMachineLogR(std::ostream* _mlog) {
    this->mlog = _mlog;
    return false;
  }

  // private:
  // KEEP THIS PUBLIC, TO MAKE IT EASY TO USE
  //
  // this is equivalent to LogLevel
  int verboseLevel;

  // auxiliar flags
  bool error;
  bool warning;
  bool information;
  // debug and verbose should be "almost the same"
#ifdef NDEBUG
  constexpr static bool debug{false};
#else
  bool debug;  // perform dead code elimination, having always debug{true} as
               // constexpr, when NDEBUG is ON
#endif
  bool verbose;  // no dead code elimination, even when NDEBUG is ON

  // Mode: SILENT  = 0
  // |      1      |      2      |      3      |      4      |
  // |    error    |   warning   | information | debug/verb. |
  // |      -      |      -      |      -      |      -      |

  // Mode: ERROR = 1
  // |      1      |      2      |      3      |      4      |
  // |    error    |   warning   | information | debug/verb. |
  // |      x      |      -      |      -      |      -      |

  // Mode: WARNING = 2 (DEFAULT)
  // |      1      |      2      |      3      |      4      |
  // |    error    |   warning   | information | debug/verb. |
  // |      x      |      x      |      -      |      -      |

  // Mode: INFORMATION = 3
  // |      1      |      2      |      3      |      4      |
  // |    error    |   warning   | information | debug/verb. |
  // |      x      |      x      |      x      |      -      |

  // Mode: VERBOSE/DEBUG = 4
  // |      1      |      2      |      3      |      4      |
  // |    error    |   warning   | information | debug/verb. |
  // |      x      |      x      |      x      |      x      |

 public:
  Component() {
    // TODO: create 'logless' implementation on OptFrame (is it faster?)
    setMessageLevel(LogLevel::Info);
    // logdata = nullptr;
  }

  virtual ~Component() = default;

#ifdef OPTFRAME_AC
  // clones current Component and it is either: (i) optional (by nullptr); (ii)
  // a shared_ptr instance as output
  virtual std::shared_ptr<Component> sharedClone() const { return nullptr; }

  // list for algorithm comprehension / search comprehension
  std::vector<ContextAC> listAC;

  bool hasInListAC(std::string _id) {
    for (unsigned i = 0; i < listAC.size(); i++)
      if (listAC[i].id == _id) return true;
    return false;
  }

  void printListAC() {
    std::cout << "ContextAC list for '" << id() << "': [";
    for (unsigned i = 0; i < listAC.size(); i++)
      std::cout << listAC[i].id << " {" << listAC[i].message << "} ; ";
    std::cout << "]" << std::endl;
  }

  void printDistinctListAC() {
    std::cout << "ContextAC distinct list for '" << id() << "': [";
    std::string lastId = "";
    int count = 0;
    for (unsigned i = 0; i < listAC.size(); i++) {
      count++;
      if (listAC[i].id != lastId) {
        std::cout << " |" << count << "|; ... => " << listAC[i].id << " {"
                  << listAC[i].message << "} ";
        lastId = listAC[i].id;
        count = 0;
      }
    }
    std::cout << "]" << std::endl;
  }
#endif

  static std::string idComponent() { return "OptFrame"; }

  virtual std::string id() const { return idComponent(); }

  virtual std::string defaultDomain() const { return ""; }

  virtual bool compatible(std::string s) {
    return (s == id()) ||
           (s == idComponent());  // equal to this component or "OptFrame:" base
  }

  // returns 'false' if unsupported
  virtual bool toStream(std::ostream& os) const { return false; }

  // returns "" if unsupported
  virtual std::string toString() const = 0;

  virtual void print() const { (*logdata) << this->toString() << std::endl; }

  // -----------

  void setVerbose() { setMessageLevel(LogLevel::Debug); }

  // set verbose level recursive: returns 'false' if not supported.
  virtual bool setVerboseR() final { return setMessageLevelR(LogLevel::Debug); }

  // -----------

  // set silent level
  virtual void setSilent() { setMessageLevel(LogLevel::Silent); }

  // set silent level recursive: returns 'false' if not supported.
  virtual bool setSilentR() final { return setMessageLevelR(LogLevel::Silent); }

  // ------------------------------

  LogLevel getMessageLevel() const { return (LogLevel)verboseLevel; }

  virtual void setMessageLevel(LogLevel vl) final {
    error = warning = information = verbose = false;
#ifndef NDEBUG
    debug = false;
#endif

    verboseLevel = vl;
    switch (verboseLevel) {
      case LogLevel::Silent:  // SILENT 0
        break;
      case LogLevel::Error:  // ERROR 1
        error = true;
        break;
      case LogLevel::Warning:  // WARNING 2
        warning = true;
        break;
      case LogLevel::Debug:  // VERBOSE 4
#ifdef NDEBUG
        std::cerr << "WARNING: LogLevel::Debug is disabled due to NDEBUG flag! "
                     "Setting LogLevel::Info instead."
                  << std::endl;
        information = true;
#else
        debug = true;
#endif
        break;
      default:  // 3: INFORMATION (default)
        information = true;
    }

    verbose = debug;  // always the same
    error = error || warning || information || debug;
    warning = warning || information || debug;
    information = information || debug;
  }

  // set log level recursive: returns 'false' if not supported.
  virtual bool setMessageLevelR(LogLevel vl) {
    setMessageLevel(vl);
    return false;
  }

  // =============================
  // for old/compatibility reasons

  bool getVerboseLevel() { return verboseLevel >= 4; }
};

}  // namespace optframe

#endif  // OPTFRAME_COMPONENT_HPP_
