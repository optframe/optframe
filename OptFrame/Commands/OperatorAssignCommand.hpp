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

#ifndef OPTFRAME_OPERATOR_ASSIGN_MODULE_HPP_
#define OPTFRAME_OPERATOR_ASSIGN_MODULE_HPP_

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class OperatorAssignCommand: public Command<R, ADS, DS>
{
public:

	virtual ~OperatorAssignCommand()
	{
	}

	string id()
	{
		return "operator.assign";
	}

	string usage()
	{
		return "operator.assign variable = value \nvariable=value (module called implicitly)";
	}

	virtual bool canHandle(string module_name, string command_body)
	{
		if (id() == module_name)
			return true;

		for (unsigned i = 0; i < Command<R, ADS, DS>::handles.size(); i++)
			if (Command<R, ADS, DS>::handles[i] == module_name)
				return true;

		string tbody = Scanner::trim(command_body);
		if ((tbody.length() > 0) && tbody[0] == '=')
			return true;

		return false;
	}

	virtual bool run(vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary, string input, string module_name)
	{
		///cout << "ASSIGN:'" << input << "' and module_name='" << module_name << "'" << endl;
		module_name.append(" ");
		module_name.append(input);
		return run(allCommands, allFunctions, factory, dictionary, ldictionary, module_name);
	}

	bool run(vector<Command<R, ADS, DS>*>&, vector<PreprocessFunction<R, ADS, DS>*>&, HeuristicFactory<R, ADS, DS>&, map<string, string>& dictionary, map<string, vector<string> >& ldictionary, string input)
	{
		///cout << "ASSIGN:'" << input << "'" << endl;

		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "module " << id() << " missing variable name!" << endl;
			return false;
		}

		string first = scanner.next();
		string var_name = first;
		if ((first == "operator.assign") || (first == "assign"))
		{
			// read again the variable
			if (!scanner.hasNext())
			{
				cout << "module " << id() << " missing variable name!" << endl;
				return false;
			}

			var_name = scanner.next();
		}

		if (!scanner.hasNext())
		{
			cout << "module " << id() << " missing assignment '='!" << endl;
			return false;
		}

		string assign = scanner.next();

		if (assign != "=")
		{
			cout << "module " << id() << " error: expected '='!" << endl;
			return false;
		}

		if (!scanner.hasNext())
		{
			cout << "module " << id() << " missing value!" << endl;
			return false;
		}

		string value = scanner.rest();

		if (!Command<R, ADS, DS>::define(var_name, value, dictionary, ldictionary))
		{
			cout << "module " << id() << " error: failed to define '" << var_name << "' to '" << value << "'" << endl;
			return false;
		}

		return true;
	}

	string addSpacesForDoubleDots(string input)
	{
		string output = "";
		unsigned i = 0;
		while (i < input.length())
		{
			if ((input[i] == '.') && ((i + 1) < input.length()) && (input[i + 1] == '.'))
			{
				output.append(" .. ");
				i += 2;
			}
			else
			{
				output += input[i];
				i++;
			}
		}

		return output;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		// ============================================
		// add spaces before and after FIRST assignment
		// ============================================

		string new_input = "";

		unsigned irest = 0;
		for (unsigned i = 0; i < input.length(); i++)
			if (input[i] == '=')
			{
				new_input.append(" = ");
				irest = i + 1;
				break;
			}
			else
				new_input += input[i];

		string rest = "";

		for (unsigned i = irest; i < input.length(); i++)
			rest += input[i];

		// ==========================
		// call default preprocessing
		// ==========================

		// add spaces before and after double dots '..' and do normal preprocessing
		string* proc = Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, addSpacesForDoubleDots(rest));

		if (!proc)
			return nullptr;

		new_input.append(*proc);
		delete proc;
		return new string(new_input);
	}

};

}

#endif /* OPTFRAME_LIST_DEFINE_MODULE_HPP_ */
