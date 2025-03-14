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

#ifndef OPTFRAME_MODULE_EXISTS_MODULE_HPP_
#define OPTFRAME_MODULE_EXISTS_MODULE_HPP_

#include "../Command.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class CommandExistsCommand : public Command<R, ADS, DS> {
 public:
  virtual ~CommandExistsCommand() {}

  string id() { return "command.exists"; }

  string usage() {
    string u = id();
    u += " module_name result_variable\n";
    return u;
  }

  string formatBool(bool b) {
    if (b)
      return "true";
    else
      return "false";
  }

  bool parseBool(string b) { return b == "true"; }

  Command<R, ADS, DS>* getCommand(std::vector<Command<R, ADS, DS>*>& modules,
                                  string module, string rest) {
    for (unsigned int i = 0; i < modules.size(); i++) {
      // cout << "run: testing module '" << modules[i]->id() << "'" <<
      // std::endl;
      if (modules[i]->canHandle(module, rest)) return modules[i];
    }
    // cout << "run: nullptr MODULE! module='" << module << "' rest='" << rest
    // << "'" << std::endl;
    return nullptr;
  }

  bool run(std::vector<Command<R, ADS, DS>*>& allCommands,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>& factory,
           std::map<std::string, std::string>& dictionary,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string input) {
    // cout << "command.exists command: " << input << std::endl;

    Scanner scanner(input);

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    string module_name = scanner.next();

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    string variable = scanner.next();

    Command<R, ADS, DS>* m = getCommand(allCommands, module_name, "");

    // if(m)
    //    std::cout << "command.exists real id is: " << m->id() << std::endl;

    string result = formatBool(m != nullptr);

    // cout << "command.exists result is: " << result << std::endl;

    return Command<R, ADS, DS>::defineText(variable, result, dictionary);
  }

  virtual string* preprocess(
      std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
      HeuristicFactory<R, ADS, DS>& hf,
      const std::map<std::string, std::string>& dictionary,
      const std::map<std::string, std::vector<std::string>>& ldictionary,
      string input) {
    return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary,
                                                  ldictionary, input);
  }
};

}  // namespace optframe

#endif /* OPTFRAME_MODULE_EXISTS_MODULE_HPP_ */
