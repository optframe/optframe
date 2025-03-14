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

#ifndef LISTFROMFILEMODULE_HPP_
#define LISTFROMFILEMODULE_HPP_

#include "../Command.hpp"
#include "ListSilentDefineCommand.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class FileToListCommand : public Command<R, ADS, DS> {
 public:
  virtual ~FileToListCommand() {}

  string id() { return "file.to_list"; }
  string usage() { return "file.to_list new_list_name filename"; }

  bool run(std::vector<Command<R, ADS, DS>*>& all_modules,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>& factory,
           std::map<std::string, std::string>& dictionary,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string input) {
    Scanner scan(input);
    if (!scan.hasNext())  // no file
    {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    string listName = scan.next();

    if (!scan.hasNext())  // no file
    {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    // Open file
    Scanner* scanner;

    try {
      scanner = new Scanner(new File(scan.trim(scan.rest())));
    } catch (FileNotFound& e) {
      std::cout << "File '" << e.getFile() << "' not found!" << std::endl;
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    vector<string> elements;

    while (scanner->hasNextLine()) {
      string line = scanner->nextLine();

      // WHY?
      /*
                     for (unsigned int c = 0; c < line.size(); c++)
                             if ((line.at(c) == ',') || (line.at(c) == '[') ||
         (line.at(c) == ']')) line[c] = '?';
                     */

      elements.push_back(line);
    }

    delete scanner;

    stringstream listContent;
    listContent << listName << " " << OptFrameList::listToString(elements);

    // TODO: should register directly (for performance)!

    return Command<R, ADS, DS>::run_module("list.silent_define", all_modules,
                                           allFunctions, factory, dictionary,
                                           ldictionary, listContent.str());
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

#endif /* LISTFROMFILEMODULE_HPP_ */
