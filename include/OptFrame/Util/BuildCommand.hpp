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

#include <OptFrame/Search/LocalSearch.hpp>

#include "../Search/MultiObjSearch.hpp"
#include "../Search/SingleObjSearch.hpp"

namespace optframe {

//! \english The BuildCommand class is a Command that enables the user to create
//! a heuristic and give a name to it. \endenglish \portuguese A classe
//! BuildCommand é o Módulo que permite o usuário criar uma heurística e definir
//! um nome para ela. \endportuguese
/*!
  \sa run()
*/

template <class R, class ADS, XSolution S>
class BuildCommand {
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

  virtual ~BuildCommand() {}

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

  string run(HeuristicFactory<XES>& factory,
             map<std::string, std::string>& dictionary,
             map<string, vector<string>>& ldictionary, string input) {
    // std::cout << "build command: " << input << std::endl;
    Scanner scanner1(input);

    if (!scanner1.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;

      listBuilders(factory, dictionary, ldictionary, "");
      return "";
    }

    string type = scanner1.next();
    std::stringstream ss;
    ss << type << " " << scanner1.rest();
    // std::cout << "rest: " << ss.str() << std::endl;

    Scanner scanner(ss.str());

    int new_id = -1;
    string base = "";

    if (ComponentHelper::compareBase(LocalSearchBuilder<R, ADS>::idComponent(),
                                     type)) {
      pair<LocalSearch<XES>*, string> method;
      method = factory.createLocalSearch(scanner.rest());
      scanner = Scanner(method.second);

      if (method.first != nullptr) {
        base = LocalSearch<XES>::idComponent();
        new_id = factory.addComponent(
            *method.first, base);  // Adicionando como 'base', poderia adicionar
                                   // como o proprio... o que eh melhor?
      }
    } else if (ComponentHelper::compareBase(
                   SingleObjSearchBuilder<XES>::idComponent(), type)) {
      pair<SingleObjSearch<XES>*, string> method;
      method = factory.createSingleObjSearch(scanner.rest());
      scanner = Scanner(method.second);

      if (method.first != nullptr) {
        base = SingleObjSearch<XES>::idComponent();
        new_id = factory.addComponent(
            *method.first, base);  // Adicionando como 'base', poderia adicionar
                                   // como o proprio... o que eh melhor?
      }
    } else if (ComponentHelper::compareBase(
                   MultiObjSearchBuilder<R, ADS>::idComponent(), type)) {
      pair<MultiObjSearch<R, ADS>*, string> method;
      method = factory.createMultiObjSearch(scanner.rest());
      scanner = Scanner(method.second);

      if (method.first != nullptr) {
        base = MultiObjSearch<R, ADS>::idComponent();
        new_id = factory.addComponent(
            *method.first, base);  // Adicionando como 'base', poderia adicionar
                                   // como o proprio... o que eh melhor?
      }
    } else if (factory.getBuilder(type)) {
      scanner.next();  // drop builder name!

      ComponentBuilder<R, ADS>* builder = factory.getBuilder(type);
      Component* comp = builder->buildComponent(scanner, factory);

      if (comp) {
        base = comp->id();
        new_id = factory.addComponent(*comp, base);
      }
    } else {
      std::cout << "build_module: error '" << type
                << "' is not a known builder!" << std::endl;
      return "";
    }

    if (new_id < 0) {
      std::cout << "build command: couldn't build component!" << std::endl;
      return "";
    }

    stringstream str;
    str << base << " " << new_id;
    string s_new_id = str.str();

    // std::cout << "'" << s_new_id << "' added." << std::endl;

    string new_name = scanner.next();

    return new_name + " " + s_new_id;
  }

  string listBuilders(HeuristicFactory<XES>& factory,
                      map<std::string, std::string>& dictionary,
                      map<string, vector<string>>& ldictionary, string input) {
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

      return "";  // TODO: CHECK, IT WAS TRUE BEFORE, SO RETURN SHOULDN'T BE
                  // EMPTY: ""
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

      return ss.str();
    }
  }
};

}  // namespace optframe

#endif /* BUILDMODULE_HPP_ */
