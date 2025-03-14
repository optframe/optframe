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

#ifndef OPTFRAME_SYSTEM_REQUIRE_MODULE_HPP_
#define OPTFRAME_SYSTEM_REQUIRE_MODULE_HPP_

#include "../Command.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class SystemRequireCommand : public Command<R, ADS, DS> {
 public:
  virtual ~SystemRequireCommand() {}

  string id() { return "system.require"; }

  string usage() { return "system.require module_name"; }

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

    scanner.useSeparators(" \t\r\n;");  // prevent errors with semicolon

    string module_name = scanner.next();

    for (unsigned i = 0; i < allCommands.size(); i++)
      if (allCommands[i]->id() == module_name) return true;

    return false;
  }

  // disable preprocess, only need module name
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

#endif /* OPTFRAME_SYSTEM_REQUIRE_MODULE_HPP_ */
