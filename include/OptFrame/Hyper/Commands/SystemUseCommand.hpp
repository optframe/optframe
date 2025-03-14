// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_SYSTEM_USE_MODULE_HPP_
#define OPTFRAME_SYSTEM_USE_MODULE_HPP_

#include "../Command.hpp"
#include "SystemRequireCommand.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class SystemUseCommand : public Command<R, ADS, DS> {
 public:
  virtual ~SystemUseCommand() {}

  string id() { return "system.use"; }

  string usage() { return "system.use prefix -module_name ..."; }

  bool moduleExists(string moduleName,
                    vector<Command<R, ADS, DS>*>& allCommands) {
    for (unsigned i = 0; i < allCommands.size(); i++)
      if (allCommands[i]->id() == moduleName) return true;
    return false;
  }

  bool functionExists(string functionName,
                      vector<PreprocessFunction<R, ADS, DS>*>& allFunctions) {
    for (unsigned i = 0; i < allFunctions.size(); i++)
      if (allFunctions[i]->id() == functionName) return true;
    return false;
  }

  bool in(string s, vector<string>& vs) {
    for (unsigned i = 0; i < vs.size(); i++)
      if (vs[i] == s) return true;
    return false;
  }

  bool run(std::vector<Command<R, ADS, DS>*>& allCommands,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>& factory,
           std::map<std::string, std::string>& dictionary,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string input) {
    /*
              // check dependency on 'command.rename' module
              if(!Command<R, ADS, DS>::run_module("system.require", allCommands,
       allFunctions, factory, dictionary, ldictionary, "command.rename"))
              {
                      cout << "error: system.use module depends on
       'command.rename' module, which is not loaded!" << std::endl; return
       false;
              }
              */

    /*
              // check dependency on 'function.rename' module
              if(!Command<R, ADS, DS>::run_module("system.require", allCommands,
       allFunctions, factory, dictionary, ldictionary, "function.rename"))
              {
                      cout << "error: system.use module depends on
       'function.rename' module, which is not loaded!" << std::endl; return
       false;
              }
              */

    Scanner scanner(input);

    if (!scanner.hasNext())  // no module
    {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    string prefix = scanner.next();

    vector<string> negative_exceptions;

    while (scanner.hasNext()) {
      string exception = scanner.next();

      Scanner scanReal(exception);
      scanReal.useSeparators("-");
      string real = scanReal.next();

      negative_exceptions.push_back(real);
    }

    for (unsigned i = 0; i < allCommands.size(); i++) {
      Scanner scanPrefix(allCommands[i]->id());
      scanPrefix.useSeparators(".");

      if (scanPrefix.next() == prefix) {
        string smallName = scanPrefix.next();

        if (in(smallName,
               negative_exceptions))  // jump negative exceptions in modules
          continue;

        if (moduleExists(smallName, allCommands)) {
          // WARNING!
          std::cout << "system.use module warning: couldn't rename '"
                    << allCommands[i]->id() << "' because module '" << smallName
                    << "' is already registered!" << std::endl;
          continue;
        }

        /*
                            stringstream ss;
                            ss << allCommands[i]->id() << " " << smallName;


                            if(!Command<R, ADS,
           DS>::run_module("command.rename", allCommands, allFunctions, factory,
           dictionary, ldictionary, ss.str()))
                            {
                                    cout << "system.use module error: failed to
           do a command.rename with parameters '" << ss.str() << "'" <<
           std::endl; return false;
                            }
                            */

        allCommands[i]->handles.push_back(smallName);
      }
    }

    for (unsigned i = 0; i < allFunctions.size(); i++) {
      Scanner scanPrefix(allFunctions[i]->id());
      scanPrefix.useSeparators(".");

      if (scanPrefix.next() == prefix) {
        string smallName = scanPrefix.next();

        if (in(smallName,
               negative_exceptions))  // jump negative exceptions in functions
          continue;

        if (functionExists(smallName, allFunctions)) {
          // WARNING!
          std::cout << "system.use module warning: couldn't rename '"
                    << allFunctions[i]->id() << "' because function '"
                    << smallName << "' is already registered!" << std::endl;
          continue;
        }

        /*
                            stringstream ss;
                            ss << allFunctions[i]->id() << " " << smallName;

                            if(!Command<R, ADS,
           DS>::run_module("function.rename", allCommands, allFunctions,
           factory, dictionary, ldictionary, ss.str()))
                            {
                                    cout << "system.use module error: failed to
           do a function.rename with parameters '" << ss.str() << "'" <<
           std::endl; return false;
                            }
                            */

        allFunctions[i]->handles.push_back(smallName);
      }
    }

    return true;
  }

  // disable preprocess, only need module prefix
  virtual string* preprocess(
      std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
      HeuristicFactory<R, ADS, DS>& hf,
      const std::map<std::string, std::string>& dictionary,
      const std::map<std::string, std::vector<std::string>>& ldictionary,
      string input) {
    // disable preprocess!!
    return new string(input);
  }
};

}  // namespace optframe

#endif /* OPTFRAME_SYSTEM_USE_MODULE_HPP_ */
