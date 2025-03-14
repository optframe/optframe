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

#ifndef OPTFRAME_COMMAND_RENAME_MODULE_HPP_
#define OPTFRAME_COMMAND_RENAME_MODULE_HPP_

#include "../Command.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class CommandRenameCommand : public Command<R, ADS, DS> {
 public:
  virtual ~CommandRenameCommand() {}

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

  string id() { return "command.rename"; }

  string usage() { return "command.rename command_name new_name"; }

  bool run(std::vector<Command<R, ADS, DS>*>& allCommands,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>& factory,
           std::map<std::string, std::string>& dictionary,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string input) {
    Scanner scanner(input);

    if (!scanner.hasNext())  // no module
    {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    string command_name = scanner.next();

    if (!scanner.hasNext())  // no module
    {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    string new_name = scanner.next();

    Command<R, ADS, DS>* m = getCommand(allCommands, command_name, "");

    if (!m) {
      std::cout << "command " << id() << " error: command '" << command_name
                << "' not found!" << std::endl;
      return false;
    }

    m->handles.push_back(new_name);

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

#endif /* OPTFRAME_COMMAND_RENAME_MODULE_HPP_ */
