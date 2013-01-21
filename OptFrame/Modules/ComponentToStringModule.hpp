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

#ifndef OPTFRAME_COMPONENT_TOSTRING_MODULE_HPP_
#define OPTFRAME_COMPONENT_TOSTRING_MODULE_HPP_

#include "../OptFrameModule.hpp"

#include "SystemSilentDefineModule.hpp"

#include "../LocalSearch.hpp"
#include "../SingleObjSearch.hpp"
#include "../MultiObjSearch.hpp"

//! \english The BuildModule class is a Module that enables the user to create a heuristic and give a name to it. \endenglish \portuguese A classe BuildModule é o Módulo que permite o usuário criar uma heurística e definir um nome para ela. \endportuguese
/*!
  \sa run()
*/

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentToStringModule: public OptFrameModule<R, ADS, DS>
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

   virtual ~ComponentToStringModule()
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
       The BuildModule receives as parameter a ComponentBuilder and the method parameters, also the desired name is optionally passed.

       For more information type 'usage build' in OptFrame Command Line Interface

       \sa OptFrame::execute()
       \endenglish

       \portuguese
       O BuildModule recebe como parâmetro um ComponentBuilder e os parâmetros do método, bem como um nome desejado para o método, opcionalmente.

       Para mais informações digite 'usage build' na Interface de Linha de Comando do OptFrame

       \sa OptFrame::execute()
       \endportuguese
   */

   bool run(vector<OptFrameModule<R, ADS, DS>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
   {
	   Scanner scanner(input);

	   if (!scanner.hasNext())
	   {
		   cout << "Usage: " << usage() << endl;
		   return false;
	   }

	   OptFrameComponent* comp = factory.getNextComponent(scanner);

	   string text;
	   if(comp)
		   text = comp->toString();
	   else
	   {
		   //text = "NULL Component!";
		   cout << "component.to_string module: NULL Component!" << endl;
		   return false;
	   }

	   if (!scanner.hasNext())
	   {
		   cout << text << endl;
		   return true;
	   }
	   else
	   {
		   string name = scanner.next();

		   stringstream sstr;
		   sstr << name << " " << text;

		   return OptFrameModule<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, sstr.str());
	   }
   }

};

#endif /* OPTFRAME_COMPONENT_TOSTRING_MODULE_HPP_ */
