/*
 * OptFrameModule.hpp
 *
 * LGPLv3
 *
 */

#ifndef OPTFRAMEMODULE_HPP_
#define OPTFRAMEMODULE_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "Scanner++/Scanner.h"

#include "HeuristicFactory.hpp"

#include <algorithm>

template<class R, class M>
class OptFrameModule
{
protected:
	void run_module(string mod, vector<OptFrameModule<R,M>*> v, HeuristicFactory<R,M>* f, map<string,string>* dictionary, string input)
	{
		for(int i=0;i<v.size();i++)
			if(mod==v[i]->id())
			{
				v[i]->run(v,f,dictionary,input);
				return;
			}
		cout << "Module '"<<mod<<"' not found." << endl;
	}

public:
	virtual string id() = 0;
	virtual string usage() = 0;

	virtual void run(vector<OptFrameModule<R,M>*>, HeuristicFactory<R,M>*, map<string,string>* dictionary, string) = 0;

	virtual string preprocess(map<string,string>* dictionary, string input)
	{
		return defaultPreprocess(dictionary,input);
	}

	static string defaultPreprocess(map<string,string>* dictionary, string input)
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

		// Second, use the dictionary

		string input3 = "";

		while(scanner.hasNext())
		{
			string new_word = scanner.next();

			if(dictionary->count(new_word) == 0) // Not found in dictionary!
			{
				if(input3=="")
					input3+=new_word;
				else
					input3 = input3+" "+new_word;
			}
			else
			{
				string found = dictionary->find(new_word)->second;
				scanner = Scanner(input3+" "+found+" "+scanner.rest());
				input3 = "";
			}
		}

		return input3;
	}
};

#endif /* OPTFRAMEMODULE_HPP_ */
