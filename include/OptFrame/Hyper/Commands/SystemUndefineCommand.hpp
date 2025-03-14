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

#ifndef UNDEFINEMODULE_HPP_
#define UNDEFINEMODULE_HPP_

#include "../Command.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class SystemUndefineCommand : public Command<R, ADS, DS> {
 public:
  virtual ~SystemUndefineCommand() {}

  string id() { return "system.undefine"; }

  string usage() { return "system.undefine word or list"; }

  bool run(std::vector<Command<R, ADS, DS>*>& all_modules,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>&,
           std::map<std::string, std::string>& dictionary,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string rest) {
    // cout << "undefine command: '" << rest << "'" << std::endl;

    Scanner scanner(rest);

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    while (scanner.hasNext()) {
      string word = scanner.next();
      Command<R, ADS, DS>::undefine(word, dictionary, ldictionary);
    }

    return true;
  }

  virtual string* preprocess(
      std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
      HeuristicFactory<R, ADS, DS>& hf,
      const std::map<std::string, std::string>& dictionary,
      const std::map<std::string, std::vector<std::string>>& ldictionary,
      string input) {
    Scanner scanner(input);

    // First, remove the comments '%'

    string input2 = "";

    while (scanner.hasNextChar()) {
      char c = *scanner.nextChar();
      if (c != '%')
        input2 += c;
      else
        break;
    }

    return new string(input2);
  }
};

}  // namespace optframe

#endif /* UNDEFINEMODULE_HPP_ */
