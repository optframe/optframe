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

#ifndef OPTFRAME_FILE_PRINTLN_MODULE_HPP_
#define OPTFRAME_FILE_PRINTLN_MODULE_HPP_

#include "../Command.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class FilePrintlnCommand : public Command<R, ADS, DS> {
 public:
  virtual ~FilePrintlnCommand() {}

  string id() { return "file.println"; }

  string usage() { return "file.println filename text"; }

  bool run(std::vector<Command<R, ADS, DS>*>&,
           vector<PreprocessFunction<R, ADS, DS>*>&,
           HeuristicFactory<R, ADS, DS>&, std::map<std::string, std::string>&,
           map<string, vector<string>>&, string input) {
    Scanner scanner(input);
    if (!scanner.hasNext())  // no file
    {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    string filename = scanner.next();

    FILE* file = fopen(filename.c_str(), "a");

    if (!file) {
      std::cout << "file.println command: couldn't open file: '" << filename
                << "'" << std::endl;
      return false;
    }

    string text = Scanner::trim(scanner.nextLine());

    fprintf(file, "%s\r\n", text.c_str());

    fclose(file);

    return true;
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

#endif /* OPTFRAME_FILE_PRINTLN_MODULE_HPP_ */
