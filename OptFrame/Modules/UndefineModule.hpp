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

#ifndef UNDEFINEMODULE_HPP_
#define UNDEFINEMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class UndefineModule: public OptFrameModule<R, ADS, M>
{
public:
	virtual ~UndefineModule()
	{
	}

	string id()
	{
		return "undefine";
	}

	string usage()
	{
		return "undefine word";
	}

	void run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>*, map<string, string>* dictionary, string rest)
	{
		Scanner scanner(rest);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string word = scanner.next();

		dictionary->erase(word);
	}

	virtual string preprocess(vector<OptFrameFunction*>& allFunctions, map<string, string>*, string input)
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

		return input2;
	}

};

#endif /* UNDEFINEMODULE_HPP_ */
