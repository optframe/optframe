// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

#include "../Command.hpp"

#include "SystemSilentDefineCommand.hpp"

#include "../LocalSearch.hpp"
#include "../SingleObjSearch.hpp"
#include "../MultiObjSearch.hpp"

namespace optframe
{

//! \english The BuildCommand class is a Command that enables the user to create a heuristic and give a name to it. \endenglish \portuguese A classe BuildCommand é o Módulo que permite o usuário criar uma heurística e definir um nome para ela. \endportuguese
/*!
  \sa run()
*/

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentToStringCommand: public Command<R, ADS, DS>
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

   bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
   {
	   Scanner scanner(input);

	   if (!scanner.hasNext())
	   {
		   cout << "Usage: " << usage() << endl;
		   return false;
	   }

	   Component* comp = factory.getNextComponent(scanner);

	   string text;
	   if(comp)
		   text = comp->toString();
	   else
	   {
		   //text = "nullptr Component!";
		   cout << "component.to_string command: nullptr Component!" << endl;
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

		   return Command<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, sstr.str());
	   }
   }

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}

};

}

#endif /* OPTFRAME_COMPONENT_TOSTRING_MODULE_HPP_ */
