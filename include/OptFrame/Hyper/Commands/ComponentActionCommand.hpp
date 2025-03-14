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

#ifndef OPTFRAME_COMPONENT_ACTION_HPP_
#define OPTFRAME_COMPONENT_ACTION_HPP_

#include "../Command.hpp"
#include "SystemSilentDefineCommand.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class ComponentActionCommand : public Command<R, ADS, DS> {
 public:
  virtual ~ComponentActionCommand() {}

  string id() { return "component.action"; }

  string usage() {
    string u =
        "component.action   Component id   action_name   "
        "specific_action_parameters";
    return u;
  }

  bool run(std::vector<Command<R, ADS, DS>*>& allCommands,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>& factory,
           std::map<std::string, std::string>& dictionary,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string input) {
    // cout << "component.action: " << input << std::endl;

    Scanner scanner(input);

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;

      std::cout << factory.actions.size()
                << " available action handlers:" << std::endl;
      std::cout << "==============================================="
                << std::endl;
      std::cout << "Component index  |  action  | action parameters"
                << std::endl;
      std::cout << "==============================================="
                << std::endl;

      for (unsigned a = 0; a < factory.actions.size(); a++)
        std::cout << factory.actions[a]->usage() << std::endl << std::endl;

      std::cout << "==============================================="
                << std::endl;

      return false;
    }

    string compName;  // component name (or translated shortcut)
    int compNumber;   // component index
    Component* comp = factory.getNextComponent(scanner, &compName, &compNumber);

    if (!comp) {
      std::cout << "module " << id() << " error: nullptr component!"
                << std::endl;
      return false;
    }

    if (!scanner.hasNext()) {
      std::cout << "module " << id() << " error: missing action name!"
                << std::endl;
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    string action = scanner.next();

    // cout << "FOUND COMPONENT '" << compName << "' and action '" << action <<
    // "'" << std::endl; cout << "will look for action: '" << action << "'" <<
    // std::endl;

    for (unsigned a = 0; a < factory.actions.size(); a++)
      if (factory.actions[a]->handleComponent(compName) &&
          factory.actions[a]->handleAction(action)) {
        stringstream ssaction;
        ssaction << compName << " " << compNumber << " " << action
                 << scanner.rest();

        return factory.actions[a]->doAction(ssaction.str(), factory, dictionary,
                                            ldictionary);
      }

    std::cout << "component.action error: component '" << compName
              << "' and action '" << action << "' not found!" << std::endl;
    return false;
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

#endif /* OPTFRAME_COMPONENT_ACTION_HPP_ */
