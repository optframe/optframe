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

#ifndef InitServersMODULE_HPP_
#define InitServersMODULE_HPP_

#include "../Command.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class InitServersCommand : public Command<R, ADS, DS> {
 public:
  virtual ~InitServersCommand() {}

  string id() { return "initservers"; }
  string usage() { return "initservers"; }

  void run(std::vector<Command<R, ADS, DS>*>&,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>& factory,
           std::map<std::string, std::string>& dictionary,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string rest) {
    factory->mapReduce->initServers(factory->argc, factory->argv);
  }

  virtual string preprocess(
      std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
      std::map<std::string, std::string>&, map<string, vector<string>>&,
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

    return input2;
  }
};

}  // namespace optframe

#endif /* InitServersMODULE_HPP_ */
