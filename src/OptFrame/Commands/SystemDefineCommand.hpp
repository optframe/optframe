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

#ifndef OPTFRAME_SYSTEM_DEFINE_MODULE_HPP_
#define OPTFRAME_SYSTEM_DEFINE_MODULE_HPP_

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemDefineCommand : public Command<R, ADS, DS>
{
public:

	virtual ~SystemDefineCommand()
	{
	}

	string id()
	{
		return "system.define";
	}
	string usage()
	{
		return "system.define new_name text_to_be_substituted_from_the_new_name";
	}

	bool run(vector<Command<R, ADS, DS>*>&, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>&, map<string,string>& dictionary,  map< string,vector<string> >&, string rest)
	{
		Scanner scanner(rest);

		if(!scanner.hasNext())
			return false;

		string new_name = scanner.next();

		if(new_name != "")
		{
			string second_word = scanner.rest();
			if(Command<R, ADS, DS>::defineText(new_name, second_word, dictionary))
			{
				cout << "Word '" << new_name << "' now means: '" << dictionary[new_name] << "'" << endl;
				return true;
			}
			else
				return false;
		}
		else
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}
	}


	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		// First, remove the comments '%'

		string input2 = "";

		while(scanner.hasNextChar())
		{
			char c = scanner.nextChar();
			if(c != '%')
				input2 += c;
			else
				break;
		}


		scanner = Scanner(input2);

		// Second, use the dictionary, but before...
		// WAIT! Save the definition name!

		if(!scanner.hasNext())
			return new string(input2);

		string name      = scanner.next();
		string discarded = scanner.getDiscarded();

		// now proceed as usual

		string* input3 = Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, scanner.rest());

		if(!input3)
			return nullptr;

		string input4;
		input4.append(discarded);
		input4.append(name);
		input4.append(" ");
		input4.append(*input3);

		delete input3;

		string input5 = Scanner::trim(input4);

		return new string(input5);
	}

};

}

#endif /* OPTFRAME_SYSTEM_DEFINE_MODULE_HPP_ */
