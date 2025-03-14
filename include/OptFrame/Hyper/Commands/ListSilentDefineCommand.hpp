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

#ifndef OPTFRAME_LIST_DEFINE_MODULE_HPP_
#define OPTFRAME_LIST_DEFINE_MODULE_HPP_

#include "../Command.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class ListSilentDefineCommand : public Command<R, ADS, DS> {
 public:
  virtual ~ListSilentDefineCommand() {}

  string id() { return "list.silent_define"; }
  string usage() { return "list.silent_define name list"; }

  bool run(std::vector<Command<R, ADS, DS>*>&,
           vector<PreprocessFunction<R, ADS, DS>*>&,
           HeuristicFactory<R, ADS, DS>&, std::map<std::string, std::string>&,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string rest) {
    Scanner scanner(rest);

    if (!scanner.hasNext()) return false;

    string list_name = scanner.next();

    if (list_name != "") {
      vector<string> list;
      vector<string>* p_list = OptFrameList::readList(ldictionary, scanner);
      if (p_list) {
        list = vector<string>(*p_list);
        delete p_list;
      } else {
        std::cout << "list.silent_define command: couldn't load list!"
                  << std::endl;
        return false;
      }

      return Command<R, ADS, DS>::defineList(list_name, list, ldictionary);
    } else {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }
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

    scanner = Scanner(input2);

    // Second, use the dictionary, but before...
    // WAIT! Save the definition name!

    if (!scanner.hasNext()) return new string(input2);

    string name = scanner.next();
    string discarded = scanner.getDiscarded();

    // now proceed as usual

    string* input3 = Command<R, ADS, DS>::defaultPreprocess(
        allFunctions, hf, dictionary, ldictionary, scanner.rest());

    if (!input3) return nullptr;

    string input4;
    input4.append(discarded);
    input4.append(name);
    input4.append(" ");
    input4.append(*input3);

    delete input3;

    string input5 = Scanner::trim(input4);

    return new string(input5);
  }
};

}  // namespace optframe

#endif /* OPTFRAME_LIST_DEFINE_MODULE_HPP_ */
