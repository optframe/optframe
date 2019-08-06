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

#ifndef IFELSE_MODULE_HPP_
#define IFELSE_MODULE_HPP_

#include<string>

#include "../Command.hpp"

#include "SystemRunCommand.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class IfElseCommand: public Command<R, ADS, DS>
{
public:

	virtual ~IfElseCommand()
	{
	}

	string id()
	{
		return "if";
	}

	string usage()
	{
		return "if boolean block_of_if_commands [else block_of_else_commands]";
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string sbool = scanner.next();

		bool condition;
		if(sbool=="true")
			condition = true;
		else if(sbool=="false")
			condition = false;
		else
		{
			cout << "if command: no such boolean '" << sbool << "'" << endl;
			return false;
		}


		vector<string>  lif;
		vector<string>* p_lif = OptFrameList::readBlock(scanner);
		if(p_lif)
		{
			lif = vector<string>(*p_lif);
			delete p_lif;
		}
		else
			return false;

		vector<string>  lelse;
		if(scanner.hasNext())
		{
			string text_else = scanner.next(); // drop 'else'

			if(text_else != "else")
			{
				cout << "if command: expected else and found '" << text_else << "'" << endl;
				return false;
			}

			vector<string>* p_lelse = OptFrameList::readBlock(scanner);
			if(p_lelse)
			{
				lelse = vector<string>(*p_lelse);
				delete p_lelse;
			}
			else
				return false;
		}

		// check if all the text was used!
		if(!Command<R, ADS, DS>::testUnused(id(), scanner))
			return false;

		if (condition)
		{
			if(!Command<R, ADS, DS>::run_module("system.run", all_modules, allFunctions, factory, dictionary, ldictionary, OptFrameList::blockToString(lif)))
			{
				cout << "if command: error in IF command!" << endl;
				return false;
			}
			else
				return true;
		}
		else
		{
			if(!Command<R, ADS, DS>::run_module("system.run", all_modules, allFunctions, factory, dictionary, ldictionary, OptFrameList::blockToString(lelse)))
			{
				cout << "if command: error in ELSE command!" << endl;
				return false;
			}
			else
				return true;
		}
	}

	// should preprocess only until list of commands
	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		string ibegin = "";
		string iend   = "";
		unsigned j = 0;
		for(unsigned i=0; i<input.length(); i++)
		{
			if(input.at(i)=='{')
				break;
			else
				ibegin += input.at(i);
			j++;
		}

		for(unsigned k=j; k<input.length(); k++)
			iend += input.at(k);

		string* ninput = Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, ibegin);

		if(!ninput)
			return nullptr;

		ninput->append(" "); // after boolean value
		ninput->append(iend);

		return ninput;
	}
};

}

#endif /* IFELSE_MODULE_HPP_ */
