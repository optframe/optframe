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

#ifndef PROBLEMMODULE_HPP_
#define PROBLEMMODULE_HPP_

#include "../Command.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class ProblemCommand : public Command<R, ADS, DS> {
 public:
  virtual ~ProblemCommand() {}

  virtual string id() {
    return "problem.";  // leave dot to implement next module in heritage
  }

  virtual string usage() {
    string usage = id();
    usage.append(" [load instance_path | unload]");
    return usage;
  }

  bool run(std::vector<Command<R, ADS, DS>*>& allCommands,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>& factory,
           std::map<std::string, std::string>& dictionary,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string input) {
    Scanner scanner(input);
    if (!scanner.hasNext()) {
      std::cout << "USAGE: " << usage() << std::endl;
      return false;
    }

    string mode = scanner.next();
    if (mode == "load")
      return load(Scanner::trim(scanner.rest()), factory, dictionary,
                  ldictionary);

    if (mode == "unload") return unload(factory, dictionary, ldictionary);

    std::cout << "problem command: unknown mode '" << mode
              << "'! (should be 'load instance_path' or 'unload')";

    return false;
  }

  bool registerComponent(Component& component, string name, string type,
                         HeuristicFactory<R, ADS, DS>& hf,
                         std::map<std::string, std::string>& dictionary) {
    int idx = hf.addComponent(component, type);
    if (idx < 0) return false;
    std::stringstream ss;
    ss << type << " " << idx;
    return Command<R, ADS, DS>::defineText(name, ss.str(), dictionary);
  }

  bool registerComponent(Component& component, string name,
                         HeuristicFactory<R, ADS, DS>& hf,
                         std::map<std::string, std::string>& dictionary) {
    string type = component.id();
    return registerComponent(component, name, type, hf, dictionary);
  }

  virtual bool load(
      string filename, HeuristicFactory<R, ADS, DS>& factory,
      std::map<std::string, std::string>& dictionary,
      std::map<std::string, std::vector<std::string>>& ldictionary) = 0;

  virtual bool unload(
      HeuristicFactory<R, ADS, DS>& factory,
      std::map<std::string, std::string>& dictionary,
      std::map<std::string, std::vector<std::string>>& ldictionary) = 0;

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

#endif /* PROBLEMMODULE_HPP_ */
