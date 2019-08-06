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

#ifndef OPTFRAME_CALLMODULE_HPP_
#define OPTFRAME_CALLMODULE_HPP_

#include "../Command.hpp"

namespace optframe
{

//! \english The CallCommand class is a Command that enables the user to do a system call to external software \endenglish \portuguese A classe CallCommand é o Módulo que permite o usuário fazer uma chamada de sistema, executando um programa externo. \endportuguese
/*!
 \sa run()
 */

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemCallCommand: public Command<R, ADS, DS>
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

	virtual ~SystemCallCommand()
	{
	}

	string id()
	{
		return "system.call";
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
		return "system.call external_software";
	}

	//! \english Execution of the module \endenglish \portuguese Execução do módulo \endportuguese
	/*! \english
	 The CallCommand receives as parameter the desired system call.

	 For more information type 'usage call' in OptFrame Command Line Interface

	 \sa OptFrame::execute()
	 \endenglish

	 \portuguese
	 O CallCommand recebe como parâmetro a chamada de sistema desejada.

	 Para mais informações digite 'usage call' na Interface de Linha de Comando do OptFrame

	 \sa OptFrame::execute()
	 \endportuguese
	 */

	bool run(vector<Command<R, ADS, DS>*>&, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>&, map<string, string>&, map< string,vector<string> >&, string command)
	{
		int c = system(command.c_str());
		return c == 0; // TODO: get return value from external command!
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* OPTFRAME_CALLMODULE_HPP_ */
