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

#ifndef OPTFRAME_COMPONENT_TOSTRING_MODULE_HPP_
#define OPTFRAME_COMPONENT_TOSTRING_MODULE_HPP_

#include "../Command.hpp"

#include "SystemSilentDefineCommand.hpp"

#include "../LocalSearch.hpp"
#include "../MultiObjSearch.hpp"
#include "../SingleObjSearch.hpp"

namespace optframe {

//! \english The BuildCommand class is a Command that enables the user to create a heuristic and give a name to it. \endenglish \portuguese A classe BuildCommand é o Módulo que permite o usuário criar uma heurística e definir um nome para ela. \endportuguese
/*!
  \sa run()
*/

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentToStringCommand : public Command<R, ADS, DS>
{
public:
   //! \english Command id \endenglish \portuguese Identificação do módulo \endportuguese
   /*! \english
       \return string with the id of this module
       \endenglish
       \portuguese
       \return string com a identificação deste módulo
       \endportuguese
   */

   virtual ~ComponentToStringCommand()
   {}

   string id()
   {
      return "component.to_string";
   }

   //! \english Help for this module \endenglish \portuguese Ajuda para este módulo \endportuguese
   /*! \english
       \return string with the help for this module
       \endenglish
       \portuguese
       \return string com a ajuda para este módulo
       \endportuguese
   */

   string usage()
   {
      return "component.to_string component [store_str]";
   }

   //! \english Execution of the module \endenglish \portuguese Execução do módulo \endportuguese
   /*! \english
       The BuildCommand receives as parameter a ComponentBuilder and the method parameters, also the desired name is optionally passed.

       For more information type 'usage build' in OptFrame Command Line Interface

       \sa OptFrame::execute()
       \endenglish

       \portuguese
       O BuildCommand recebe como parâmetro um ComponentBuilder e os parâmetros do método, bem como um nome desejado para o método, opcionalmente.

       Para mais informações digite 'usage build' na Interface de Linha de Comando do OptFrame

       \sa OptFrame::execute()
       \endportuguese
   */

   bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);

      if (!scanner.hasNext()) {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      Component* comp = factory.getNextComponent(scanner);

      string text;
      if (comp)
         text = comp->toString();
      else {
         //text = "nullptr Component!";
         cout << "component.to_string command: nullptr Component!" << endl;
         return false;
      }

      if (!scanner.hasNext()) {
         cout << text << endl;
         return true;
      } else {
         string name = scanner.next();

         stringstream sstr;
         sstr << name << " " << text;

         return Command<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, sstr.str());
      }
   }

   virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* OPTFRAME_COMPONENT_TOSTRING_MODULE_HPP_ */
