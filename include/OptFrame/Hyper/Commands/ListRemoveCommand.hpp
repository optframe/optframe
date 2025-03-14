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

#ifndef OPTFRAME_LIST_REMOVE_MODULE_HPP_
#define OPTFRAME_LIST_REMOVE_MODULE_HPP_

#include <string>

#include "../Command.hpp"
#include "ListSilentDefineCommand.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class ListRemoveCommand : public Command<R, ADS, DS> {
 public:
  virtual ~ListRemoveCommand() {}

  string id() { return "list.remove"; }

  string usage() { return "list.remove list index new_list_name"; }

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

    vector<string> list;
    vector<string>* p_list = OptFrameList::readList(ldictionary, scanner);
    if (p_list) {
      list = vector<string>(*p_list);
      delete p_list;
    } else
      return false;

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    int index = *scanner.nextInt();
    index--;  // index is [1..size()], but in c++ is [0..size()-1]

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    string new_name = Scanner::trim(scanner.next());

    if ((new_name != "") && (new_name.at(0) == '[')) {
      std::cout << "list_remove command: invalid list new name '" << new_name
                << "'" << std::endl;
      return false;
    }

    if ((index < 0) || (index >= ((int)list.size()))) {
      std::cout << "list_remove command: invalid index '" << index << "'!"
                << std::endl;
      return false;
    }

    list.erase(list.begin() + index);

    std::stringstream ss;
    ss << new_name << " " << OptFrameList::listToString(list);

    // TODO: acessar dicionario de listas diretamente!! Bem mais eficiente!

    return Command<R, ADS, DS>::run_module("list.silent_define", all_modules,
                                           allFunctions, factory, dictionary,
                                           ldictionary, ss.str());
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

#endif /* OPTFRAME_LIST_REMOVE_MODULE_HPP_ */
