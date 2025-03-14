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

#ifndef BUILDMODULE_HPP_
#define BUILDMODULE_HPP_

#include <OptFrame/LocalSearch.hpp>

#include "../Command.hpp"
#include "../MultiObjSearch.hpp"
#include "../Search/SingleObjSearch.hpp"
#include "SystemSilentDefineCommand.hpp"

namespace optframe {

//! \english The BuildCommand class is a Command that enables the user to create
//! a heuristic and give a name to it. \endenglish \portuguese A classe
//! BuildCommand é o Módulo que permite o usuário criar uma heurística e definir
//! um nome para ela. \endportuguese
/*!
  \sa run()
*/

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class ComponentBuildCommand : public Command<R, ADS, DS> {
 public:
  //! \english Command id \endenglish \portuguese Identificação do módulo
  //! \endportuguese
  /*! \english
      \return string with the id of this module
      \endenglish
      \portuguese
      \return string com a identificação deste módulo
      \endportuguese
  */

  virtual ~ComponentBuildCommand() {}

  string id() { return "component.build"; }

  //! \english Help for this module \endenglish \portuguese Ajuda para este
  //! módulo \endportuguese
  /*! \english
      \return string with the help for this module
      \endenglish
      \portuguese
      \return string com a ajuda para este módulo
      \endportuguese
  */

  string usage() {
    string u = "component.build builder_and_parameters [my_method_name]\n";
    u += "to see the full list of builders type: 'list_builders OptFrame:'\n";

    return u;
  }

  //! \english Execution of the module \endenglish \portuguese Execução do
  //! módulo \endportuguese
  /*! \english
      The BuildCommand receives as parameter a ComponentBuilder and the method
     parameters, also the desired name is optionally passed.

      For more information type 'usage build' in OptFrame Command Line Interface

      \sa OptFrame::execute()
      \endenglish

      \portuguese
      O BuildCommand recebe como parâmetro um ComponentBuilder e os parâmetros
     do método, bem como um nome desejado para o método, opcionalmente.

      Para mais informações digite 'usage build' na Interface de Linha de
     Comando do OptFrame

      \sa OptFrame::execute()
      \endportuguese
  */

  bool run(vector<Command<R, ADS, DS>*>& all_modules,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>& factory,
           map<string, string>& dictionary,
           map<string, vector<string>>& ldictionary, string input) {
    // cout << "build command: " << input << endl;
    Scanner scanner1(input);

    if (!scanner1.hasNext()) {
      cout << "Usage: " << usage() << endl;

      listBuilders(all_modules, allFunctions, factory, dictionary, ldictionary,
                   "");
      return false;
    }

    string type = scanner1.next();
    stringstream ss;
    ss << type << " " << scanner1.rest();
    // cout << "rest: " << ss.str() << endl;

    Scanner scanner(ss.str());

    int new_id = -1;
    string base = "";

    if (ComponentHelper::compareBase(
            LocalSearchBuilder<R, ADS, DS>::idComponent(), type)) {
      pair<LocalSearch<R, ADS, DS>*, string> method;
      method = factory.createLocalSearch(scanner.rest());
      scanner = Scanner(method.second);

      if (method.first != nullptr) {
        base = LocalSearch<R, ADS, DS>::idComponent();
        new_id = factory.addComponent(
            *method.first, base);  // Adicionando como 'base', poderia adicionar
                                   // como o proprio... o que eh melhor?
      }
    } else if (ComponentHelper::compareBase(
                   SingleObjSearchBuilder<R, ADS, DS>::idComponent(), type)) {
      pair<SingleObjSearch<R, ADS, DS>*, string> method;
      method = factory.createSingleObjSearch(scanner.rest());
      scanner = Scanner(method.second);

      if (method.first != nullptr) {
        base = SingleObjSearch<R, ADS, DS>::idComponent();
        new_id = factory.addComponent(
            *method.first, base);  // Adicionando como 'base', poderia adicionar
                                   // como o proprio... o que eh melhor?
      }
    } else if (ComponentHelper::compareBase(
                   MultiObjSearchBuilder<R, ADS, DS>::idComponent(), type)) {
      pair<MultiObjSearch<R, ADS, DS>*, string> method;
      method = factory.createMultiObjSearch(scanner.rest());
      scanner = Scanner(method.second);

      if (method.first != nullptr) {
        base = MultiObjSearch<R, ADS, DS>::idComponent();
        new_id = factory.addComponent(
            *method.first, base);  // Adicionando como 'base', poderia adicionar
                                   // como o proprio... o que eh melhor?
      }
    } else if (factory.getBuilder(type)) {
      scanner.next();  // drop builder name!

      ComponentBuilder<R, ADS, DS>* builder = factory.getBuilder(type);
      Component* comp = builder->buildComponent(scanner, factory);

      if (comp) {
        base = comp->id();
        new_id = factory.addComponent(*comp, base);
      }
    } else {
      cout << "build_module: error '" << type << "' is not a known builder!"
           << endl;
      return false;
    }

    if (new_id < 0) {
      cout << "build command: couldn't build component!" << endl;
      return false;
    }

    stringstream str;
    str << base << " " << new_id;
    string s_new_id = str.str();

    // cout << "'" << s_new_id << "' added." << endl;

    if (scanner.hasNext()) {
      string new_name = scanner.next();
      return Command<R, ADS, DS>::run_module(
          "system.silent_define", all_modules, allFunctions, factory,
          dictionary, ldictionary, new_name + " " + s_new_id);
    }

    return true;
  }

  bool listBuilders(vector<Command<R, ADS, DS>*>& all_modules,
                    vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
                    HeuristicFactory<R, ADS, DS>& factory,
                    map<string, string>& dictionary,
                    map<string, vector<string>>& ldictionary, string input) {
    Scanner scanner(input);

    string pattern;

    if (scanner.hasNext())
      pattern = scanner.next();
    else
      pattern = "OptFrame:";

    vector<pair<string, vector<pair<string, string>>>> builders =
        factory.listBuilders(pattern);

    if (!scanner.hasNext()) {
      for (int i = 0; i < (int)builders.size(); i++) {
        cout << builders[i].first << "\t";
        for (unsigned j = 0; j < builders[i].second.size(); j++)
          cout << builders[i].second[j].first << "=>'"
               << builders[i].second[j].second << "' ";
        cout << endl;
      }

      return true;
    } else {
      string new_name = scanner.next();
      stringstream ss;

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

  virtual string* preprocess(
      vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
      HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary,
      const map<string, vector<string>>& ldictionary, string input) {
    return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary,
                                                  ldictionary, input);
  }
};

}  // namespace optframe

#endif /* BUILDMODULE_HPP_ */
