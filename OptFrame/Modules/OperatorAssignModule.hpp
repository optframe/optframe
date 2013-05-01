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

#ifndef OPTFRAME_OPERATOR_ASSIGN_MODULE_HPP_
#define OPTFRAME_OPERATOR_ASSIGN_MODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class OperatorAssignModule: public OptFrameModule<R, ADS, DS>
{
public:

	virtual ~OperatorAssignModule()
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

		for (unsigned i = 0; i < OptFrameModule<R, ADS, DS>::handles.size(); i++)
			if (OptFrameModule<R, ADS, DS>::handles[i] == module_name)
				return true;

		string tbody = Scanner::trim(command_body);
		if ((tbody.length() > 0) && tbody[0] == '=')
			return true;

		return false;
	}

	virtual bool run(vector<OptFrameModule<R, ADS, DS>*>& allModules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input, string module_name)
	{
		module_name.append(" ");
		module_name.append(input);
		return run(allModules, allFunctions, factory, dictionary, ldictionary, module_name);
	}

	bool run(vector<OptFrameModule<R, ADS, DS>*>&, vector<OptFrameFunction*>&, HeuristicFactory<R, ADS, DS>&, map<string, string>& dictionary, map<string, vector<string> >& ldictionary, string input)
	{
		//cout << "ASSIGN:'" << rest << "'" << endl;

		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "module " << id() << " missing variable name!" << endl;
			return false;
		}

		string var_name = scanner.next();

		if (!scanner.hasNext())
		{
			cout << "module " << id() << " missing assignment '='!" << endl;
			return false;
		}

		string assign = scanner.next();

		if (!scanner.hasNext())
		{
			cout << "module " << id() << " missing value!" << endl;
			return false;
		}

		string rest = scanner.rest();
		Scanner scanner2(rest);

		vector<string>* p_list = OptFrameList::readList(ldictionary, scanner2);
		if (p_list)
		{
			vector<string> list(*p_list);
			delete p_list;

			// confirm that is not defined in another dictionary
			if (dictionary.count(var_name) == 0)
			{
				//cout << "DICTIONARY[" << var_name << "]=" << dictionary.count(var_name) << endl;
				return OptFrameModule<R, ADS, DS>::defineList(var_name, list, ldictionary);
			}
			else
			{
				cout << "module " << id() << " error: variable '" << var_name << "' already defined in text dictionary!" << endl;
				return false;
			}
		}
		else
		{
			string value = Scanner::trim(rest);
			//cout << "VALUE='" << value << "'" << endl;

			// confirm that is not defined in another dictionary
			if (ldictionary.count(var_name) == 0)
			{
				//cout << "LDICTIONARY[" << var_name << "]=" << ldictionary.count(var_name) << endl;
				return OptFrameModule<R, ADS, DS>::defineText(var_name, value, dictionary);
			}
			else
			{
				cout << "module " << id() << " error: variable '" << var_name << "' already defined in list dictionary!" << endl;
				return false;
			}
		}
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

	virtual string* preprocess(vector<OptFrameFunction*>& allFunctions, map<string, string>& dictionary, map<string, vector<string> >& ldictionary, string input)
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
		string* proc = OptFrameModule<R, ADS, DS>::defaultPreprocess(allFunctions, dictionary, ldictionary, addSpacesForDoubleDots(rest));

		if (!proc)
			return NULL;

		new_input.append(*proc);
		delete proc;
		return new string(new_input);
	}

};

#endif /* OPTFRAME_LIST_DEFINE_MODULE_HPP_ */
