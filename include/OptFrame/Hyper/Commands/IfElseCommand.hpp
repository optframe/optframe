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

#ifndef IFELSE_MODULE_HPP_
#define IFELSE_MODULE_HPP_

#include <string>

#include "../Command.hpp"
#include "SystemRunCommand.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class IfElseCommand : public Command<R, ADS, DS> {
 public:
  virtual ~IfElseCommand() {}

  string id() { return "if"; }

  string usage() {
    return "if boolean block_of_if_commands [else block_of_else_commands]";
  }

  bool run(std::vector<Command<R, ADS, DS>*>& all_modules,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>& factory,
           std::map<std::string, std::string>& dictionary,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string input) {
    Scanner scanner(input);

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    string sbool = scanner.next();

    bool condition;
    if (sbool == "true")
      condition = true;
    else if (sbool == "false")
      condition = false;
    else {
      std::cout << "if command: no such boolean '" << sbool << "'" << std::endl;
      return false;
    }

    vector<string> lif;
    vector<string>* p_lif = OptFrameList::readBlock(scanner);
    if (p_lif) {
      lif = vector<string>(*p_lif);
      delete p_lif;
    } else
      return false;

    vector<string> lelse;
    if (scanner.hasNext()) {
      string text_else = scanner.next();  // drop 'else'

      if (text_else != "else") {
        std::cout << "if command: expected else and found '" << text_else << "'"
                  << std::endl;
        return false;
      }

      vector<string>* p_lelse = OptFrameList::readBlock(scanner);
      if (p_lelse) {
        lelse = vector<string>(*p_lelse);
        delete p_lelse;
      } else
        return false;
    }

    // check if all the text was used!
    if (!Command<R, ADS, DS>::testUnused(id(), scanner)) return false;

    if (condition) {
      if (!Command<R, ADS, DS>::run_module(
              "system.run", all_modules, allFunctions, factory, dictionary,
              ldictionary, OptFrameList::blockToString(lif))) {
        std::cout << "if command: error in IF command!" << std::endl;
        return false;
      } else
        return true;
    } else {
      if (!Command<R, ADS, DS>::run_module(
              "system.run", all_modules, allFunctions, factory, dictionary,
              ldictionary, OptFrameList::blockToString(lelse))) {
        std::cout << "if command: error in ELSE command!" << std::endl;
        return false;
      } else
        return true;
    }
  }

  // should preprocess only until list of commands
  virtual string* preprocess(
      std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
      HeuristicFactory<R, ADS, DS>& hf,
      const std::map<std::string, std::string>& dictionary,
      const std::map<std::string, std::vector<std::string>>& ldictionary,
      string input) {
    string ibegin = "";
    string iend = "";
    unsigned j = 0;
    for (unsigned i = 0; i < input.length(); i++) {
      if (input.at(i) == '{')
        break;
      else
        ibegin += input.at(i);
      j++;
    }

    for (unsigned k = j; k < input.length(); k++) iend += input.at(k);

    string* ninput = Command<R, ADS, DS>::defaultPreprocess(
        allFunctions, hf, dictionary, ldictionary, ibegin);

    if (!ninput) return nullptr;

    ninput->append(" ");  // after boolean value
    ninput->append(iend);

    return ninput;
  }
};

}  // namespace optframe

#endif /* IFELSE_MODULE_HPP_ */
