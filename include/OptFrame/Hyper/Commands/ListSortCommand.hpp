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

#ifndef OPTFRAME_LIST_SORT_MODULE_HPP_
#define OPTFRAME_LIST_SORT_MODULE_HPP_

#include <algorithm>
#include <string>

#include "../Command.hpp"
#include "ListSilentDefineCommand.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class ListSortCommand : public Command<R, ADS, DS> {
 public:
  virtual ~ListSortCommand() {}

  string id() { return "list.sort"; }

  string usage() { return "list.sort list new_name [index]"; }

  string doubleToString(double d) {
    std::stringstream ss;
    ss << d;
    return ss.str();
  }

  static bool compareLess(const pair<double, vector<string>>& a,
                          const pair<double, vector<string>>& b) {
    return a.first < b.first;
  }

  static bool compareGreater(const pair<double, vector<string>>& a,
                             const pair<double, vector<string>>& b) {
    return a.first > b.first;
  }

  bool run(std::vector<Command<R, ADS, DS>*>& all_modules,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>& factory,
           std::map<std::string, std::string>& dictionary,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string input) {
    // cout << "list_sort module input: '" << input << "'" << std::endl;

    Scanner scanner(input);

    if (!scanner.hasNext()) {
      std::cout << "list_sort command: no such list!" << std::endl;
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
      std::cout << "list_sort command: no new list name!" << std::endl;
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    string new_name = scanner.next();

    int index = -1;

    if (scanner.hasNext()) {
      try {
        index = *scanner.nextInt();
      } catch (ConversionError& e) {
        std::cout << "list_sort command: not an index number!" << std::endl;
        return false;
      }
    }

    if ((new_name != "") && (new_name.at(0) == '[')) {
      std::cout << "list_sort command: invalid list new name '" << new_name
                << "'" << std::endl;
      return false;
    }

    if ((index - 1) >= ((int)list.size())) {
      std::cout << "list_sort command: invalid index '" << index << "'!"
                << std::endl;
      return false;
    }

    vector<string> new_list;

    if (index > 0)  // sort list of lists by index
    {
      std::vector<std::pair<double, vector<string>>> v;

      for (unsigned i = 0; i < list.size(); i++) {
        Scanner scan(list[i]);

        vector<string> list1;
        vector<string>* p_list1 = OptFrameList::readList(ldictionary, scan);
        if (p_list1) {
          list1 = vector<string>(*p_list1);
          delete p_list1;
        } else {
          std::cout << "list_sort command: error processing internal list id="
                    << i << std::endl;
          return false;
        }

        vector<string> otherValues;
        double d = 0;

        for (unsigned j = 0; j < list1.size(); j++)
          if (((int)j) != (index - 1))
            otherValues.push_back(list1[j]);
          else {
            try {
              d = Scanner::parseDouble(list1[j]);
            } catch (ConversionError& e) {
              std::cout << "list_sort command: problem in conversion of index "
                           "value in sublist!"
                        << std::endl;
              return false;
            }
          }

        v.push_back(std::make_pair(d, otherValues));
      }

      std::sort(v.begin(), v.end(), compareLess);  // only '<' for now!

      for (unsigned i = 0; i < v.size(); i++) {
        vector<string> sublist(v[i].second.size() + 1);

        double d = v[i].first;
        for (unsigned j = 0; j < sublist.size(); j++)
          if (((int)j) == (index - 1))
            sublist[j] = doubleToString(d);
          else if (((int)j) < (index - 1))
            sublist[j] = v[i].second.at(j);
          else  // j > (index-1)
            sublist[j] = v[i].second.at(j - 1);

        new_list.push_back(OptFrameList::listToString(sublist));
      }
    } else  // sort simple list
    {
      vector<double> v;

      try {
        for (unsigned i = 0; i < list.size(); i++)
          v.push_back(Scanner::parseDouble(list[i]));
      } catch (ConversionError& e) {
        std::cout << "list_sort command: not a list of numbers!" << std::endl;
        return false;
      }

      std::sort(v.begin(), v.end());

      for (unsigned i = 0; i < v.size(); i++)
        new_list.push_back(doubleToString(v[i]));
    }

    std::stringstream ss;
    ss << new_name << " " << OptFrameList::listToString(new_list);

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

#endif /* OPTFRAME_LIST_SORT_MODULE_HPP_ */
