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

#ifndef DEFINEMODULE_HPP_
#define DEFINEMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class DefineModule : public OptFrameModule<R, ADS, M>
{
public:

	virtual ~DefineModule()
	{
	}

	string id()
	{
		return "define";
	}
	string usage()
	{
		return "define new_name string_to_be_substituted_from_the_new_name";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>&, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>&, map<string,string>& dictionary,  map< string,vector<string> >&, string rest)
	{
		Scanner scanner(rest);

		string new_name = scanner.next();

		if(new_name != "")
		{
			string second_word = scanner.rest();

			Scanner s2(second_word);

			while(s2.hasNext())
			{
				string next_word = s2.next();
				if(new_name==next_word)
				{
					cout << "define module: recursive definitions are not allowed! (define: '" << new_name << "' as '" << next_word << "')" << endl;
					return false;
				}
			}

			dictionary[new_name] = scanner.trim(second_word);
			cout << "Word '" << new_name << "' now means: '" << dictionary[new_name] << "'" << endl;
			return true;
		}
		else
		{
			cout << "Usage: "<<usage()<<endl;
			return false;
		}
	}

	virtual string preprocess(vector<OptFrameFunction*>& allFunctions, map<string,string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
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
			return input2;

		string name      = scanner.next();
		string discarded = scanner.getDiscarded();

		// now proceed as usual

		string input3 = OptFrameModule<R, ADS, M>::defaultPreprocess(allFunctions, dictionary, ldictionary, scanner.rest());

		string input4;
		input4.append(discarded);
		input4.append(name);
		input4.append(" ");
		input4.append(input3);

		string input5 = Scanner::trim(input4);

		return input5;
	}

};

#endif /* DEFINEMODULE_HPP_ */
