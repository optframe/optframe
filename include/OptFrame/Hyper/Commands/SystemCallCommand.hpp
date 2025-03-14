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

#ifndef OPTFRAME_CALLMODULE_HPP_
#define OPTFRAME_CALLMODULE_HPP_

#include "../Command.hpp"

namespace optframe {

//! \english The CallCommand class is a Command that enables the user to do a
//! system call to external software \endenglish \portuguese A classe
//! CallCommand é o Módulo que permite o usuário fazer uma chamada de sistema,
//! executando um programa externo. \endportuguese
/*!
 \sa run()
 */

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class SystemCallCommand : public Command<R, ADS, DS> {
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

  virtual ~SystemCallCommand() {}

  string id() { return "system.call"; }

  //! \english Help for this module \endenglish \portuguese Ajuda para este
  //! módulo \endportuguese
  /*! \english
        \return string with the help for this module
        \endenglish
        \portuguese
        \return string com a ajuda para este módulo
        \endportuguese
        */

  string usage() { return "system.call external_software"; }

  //! \english Execution of the module \endenglish \portuguese Execução do
  //! módulo \endportuguese
  /*! \english
        The CallCommand receives as parameter the desired system call.

        For more information type 'usage call' in OptFrame Command Line
     Interface

        \sa OptFrame::execute()
        \endenglish

        \portuguese
        O CallCommand recebe como parâmetro a chamada de sistema desejada.

        Para mais informações digite 'usage call' na Interface de Linha de
     Comando do OptFrame

        \sa OptFrame::execute()
        \endportuguese
        */

  bool run(std::vector<Command<R, ADS, DS>*>&,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>&, std::map<std::string, std::string>&,
           map<string, vector<string>>&, string command) {
    int c = system(command.c_str());
    return c == 0;  // TODO: get return value from external command!
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

#endif /* OPTFRAME_CALLMODULE_HPP_ */
