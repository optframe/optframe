// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_CALLMODULE_HPP_
#define OPTFRAME_CALLMODULE_HPP_

#include "../OptFrameModule.hpp"

//! \english The CallModule class is a Module that enables the user to do a system call to external software \endenglish \portuguese A classe CallModule é o Módulo que permite o usuário fazer uma chamada de sistema, executando um programa externo. \endportuguese
/*!
  \sa run()
*/

template< class R, class M >
class CallModule :
      public OptFrameModule<R, M>
{
public:

  //! \english Module id \endenglish \portuguese Identificação do módulo \endportuguese
  /*! \english
      \return string with the id of this module
      \endenglish
      \portuguese
      \return string com a identificação deste módulo
      \endportuguese
  */

   string id()
   {
      return "call";
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
      return "call external_software";
   }

   //! \english Execution of the module \endenglish \portuguese Execução do módulo \endportuguese
   /*! \english
       The CallModule receives as parameter the desired system call.

       For more information type 'usage call' in OptFrame Command Line Interface

       \sa OptFrame::execute()
       \endenglish

       \portuguese
       O CallModule recebe como parâmetro a chamada de sistema desejada.

       Para mais informações digite 'usage call' na Interface de Linha de Comando do OptFrame

       \sa OptFrame::execute()
       \endportuguese
   */

   void run(vector<OptFrameModule<R, M>*>& , HeuristicFactory<R, M>* , map<string, string>* , string command)
   {
      int c = system(command.c_str());
   }

};

#endif /* OPTFRAME_CALLMODULE_HPP_ */
