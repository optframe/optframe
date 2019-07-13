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

#ifndef UNDEFINEMODULE_HPP_
#define UNDEFINEMODULE_HPP_

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemUndefineCommand: public Command<R, ADS, DS>
{
public:
	virtual ~SystemUndefineCommand()
	{
	}

	string id()
	{
		return "system.undefine";
	}

	string usage()
	{
		return "system.undefine word or list";
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>&, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string rest)
	{
		//cout << "undefine command: '" << rest << "'" << endl;

		Scanner scanner(rest);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		while (scanner.hasNext())
		{
			string word = scanner.next();
			Command<R, ADS, DS>::undefine(word, dictionary, ldictionary);
		}

		return true;
	}


	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		// First, remove the comments '%'

		string input2 = "";

		while (scanner.hasNextChar())
		{
			char c = scanner.nextChar();
			if (c != '%')
				input2 += c;
			else
				break;
		}

		return new string(input2);
	}

};

}

#endif /* UNDEFINEMODULE_HPP_ */
