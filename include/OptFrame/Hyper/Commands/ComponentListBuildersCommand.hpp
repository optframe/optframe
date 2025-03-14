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

#ifndef LIST_BUILDERS_MODULE_HPP_
#define LIST_BUILDERS_MODULE_HPP_

#include "../Command.hpp"
#include "SystemSilentDefineCommand.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class ComponentListBuildersCommand : public Command<R, ADS, DS> {
 public:
  virtual ~ComponentListBuildersCommand() {}

  string id() { return "component.list_builders"; }

  string usage() {
    return "component.list_builders pattern [store_list]\nWhere: store_list is "
           "an optional variable to store the builders.";
  }

  bool run(std::vector<Command<R, ADS, DS>*>& all_modules,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>& factory,
           std::map<std::string, std::string>& dictionary,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string input) {
    Scanner scanner(input);

    string pattern;

    if (scanner.hasNext())
      pattern = scanner.next();
    else
      pattern = "OptFrame:";

    std::vector<
        std::pair<string, std::vector<std::pair<std::string, std::string>>>>
        builders = factory.listBuilders(pattern);

    if (!scanner.hasNext()) {
      for (int i = 0; i < (int)builders.size(); i++) {
        std::cout << builders[i].first << "\t";
        for (unsigned j = 0; j < builders[i].second.size(); j++)
          std::cout << builders[i].second[j].first << "=>'"
                    << builders[i].second[j].second << "' ";
        std::cout << std::endl;
      }

      return true;
    } else {
      string new_name = scanner.next();
      std::stringstream ss;

      ss << new_name << " [";
      for (unsigned i = 0; i < builders.size(); i++) {
        ss << " [ " << builders[i].first << " , [ ";
        for (unsigned j = 0; j < builders[i].second.size(); j++) {
          ss << "[ " << builders[i].second[j].first << " , "
             << builders[i].second[j].second << "] ";
          if (j != builders[i].second.size() - 1) ss << ",";
        }
        ss << " ] ] ";

        if (i != builders.size() - 1) ss << ",";
      }
      ss << " ]";

      return Command<R, ADS, DS>::run_module("system.silent_define",
                                             all_modules, allFunctions, factory,
                                             dictionary, ldictionary, ss.str());
    }
  }

  // disable preprocess to don't destroy type!
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

#endif /* LIST_BUILDERS_MODULE_HPP_ */
