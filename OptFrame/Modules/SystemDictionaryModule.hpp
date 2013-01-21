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

#ifndef DICTIONARYMODULE_HPP_
#define DICTIONARYMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemDictionaryModule : public OptFrameModule<R, ADS, DS>
{
public:

	virtual ~SystemDictionaryModule()
	{
	}

	string id()
	{
		return "system.dictionary";
	}
	string usage()
	{
		return "system.dictionary";
	}

	bool run(vector<OptFrameModule<R, ADS, DS>*>&, vector<OptFrameFunction*>&, HeuristicFactory<R, ADS, DS>&, map<string,string>& dictionary,  map< string,vector<string> >& ldictionary, string)
	{
		cout << "Text dictionary: " << endl;

		map<string, string>::iterator iter;
		int size1 = 0;
		for(iter = dictionary.begin(); iter != dictionary.end(); iter++)
		{
			string d = iter->first;
			string v = iter->second;
			cout << "'" << d << "' => '" << v << "'" << endl;
			size1++;
		}
		cout << size1 << " entries." << endl;

		// --------------------------------

		cout << "List dictionary: " << endl;

		map<string, vector<string> >::iterator iter2;
		int size2 = 0;
		for(iter2 = ldictionary.begin(); iter2 != ldictionary.end(); iter2++)
		{
			string d = iter2->first;
			vector<string>& v = iter2->second;
			cout << "'" << d << "' => '" << v << "'" << endl;
			size2++;
		}
		cout << size2 << " entries." << endl;

		return true;
	}

};

#endif /* DICTIONARYMODULE_HPP_ */
