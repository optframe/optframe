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

#ifndef OPTFRAME_MODULE_UNIT_TEST_HPP_
#define OPTFRAME_MODULE_UNIT_TEST_HPP_

#include<string>

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class CommandUnitTestCommand: public Command<R, ADS, DS>
{
private:

	bool moduleExists(string moduleName, vector<Command<R, ADS, DS>*>& allCommands)
	{
		for (unsigned i = 0; i < allCommands.size(); i++)
			if (allCommands[i]->id() == moduleName)
				return true;
		return false;
	}

	Command<R, ADS, DS>* getCommand(vector<Command<R, ADS, DS>*>& modules, string module, string rest)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
		{
			//cout << "run: testing module '" << modules[i]->id() << "'" << endl;
			if (modules[i]->canHandle(module, rest))
				return modules[i];
		}
		//cout << "run: nullptr MODULE! module='" << module << "' rest='" << rest << "'" << endl;
		return nullptr;
	}

public:

	virtual ~CommandUnitTestCommand()
	{
	}

	string id()
	{
		return "command.unit_test";
	}

	string usage()
	{
		return "command.unit_test list_target_modules_names list_of_testers [validation_key]";
	}

	bool run(vector<Command<R, ADS, DS>*>& modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);
		//cout << "command.unit_test run: '" << input << "'" << endl;

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector<string>* plist2 = OptFrameList::readList(ldictionary, scanner);
		vector<string> targets;
		if (plist2)
		{
			targets = vector<string>(*plist2);
			delete plist2;
		}
		else
		{
			cout << "command.unit_test error: ill-formed targets list!" << endl;
			return false;
		}

		for (unsigned t = 0; t < targets.size(); t++)
			if (!moduleExists(targets[t], modules))
			{
				cout << "command.unit_test command: can't test module '" << targets[t] << "' because it doesn't exist!" << endl;
				return false;
			}

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector<string>* plist1 = OptFrameList::readList(ldictionary, scanner);
		vector<string> testers;
		if (plist1)
		{
			testers = vector<string>(*plist1);
			delete plist1;
		}
		else
		{
			cout << "command.unit_test error: ill-formed testers list!" << endl;
			return false;
		}

		string skey = "";
		unsigned long key = 0;

		if (scanner.hasNext())
		{
			skey = scanner.next();
			std::istringstream iss(skey);
			if (!(iss >> std::hex >> key))
			{
				cout << "command.unit_test error: failed to convert key '" << skey << "' to unsigned long!" << endl;
				return false;
			}
		}

		// =================
		// get target module
		// =================

		vector<Command<R, ADS, DS>*> mtargets;
		for (unsigned i = 0; i < targets.size(); i++)
		{
			Command<R, ADS, DS>* m = getCommand(modules, targets[i], "");
			if (!m)
			{
				cout << "command.unit_test error: target '" << targets[i] << "' doesn't exist!" << endl;
				return false;
			}

			mtargets.push_back(m);
		}

		// ================
		// get test modules
		// ================

		vector<Command<R, ADS, DS>*> mtesters;
		for (unsigned i = 0; i < testers.size(); i++)
		{
			Command<R, ADS, DS>* m = getCommand(modules, testers[i], "");
			if (!m)
			{
				cout << "command.unit_test error: tester '" << testers[i] << "' doesn't exist!" << endl;
				return false;
			}

			mtesters.push_back(m);
		}

		// ============
		// validate key
		// ============
		unsigned long check_key = 0;

		for(unsigned i=0; i<mtargets.size(); i++)
			check_key += mtargets[i]->hash();

		for(unsigned i=0; i<mtesters.size(); i++)
			check_key += mtesters[i]->hash();

		// need to check
		if(skey != "")
		{
			if(check_key == key)
				return true;
			else
			{
				cout << "command.unit_test WARNING: different key=" << skey << " calculated=";
				printf("%#lx\n", check_key);
				cout << "Will perform tests! Press any key to continue..." << endl;
				getchar();
			}
		}

		// ================
		// run test modules
		// ================

		for (unsigned int i = 0; i < mtesters.size(); i++)
		{
			string inp = ""; // no need to preprocess
			if(!mtesters[i]->run(modules,allFunctions, factory, dictionary, ldictionary, inp))
			{
				cout << "command.unit_test error: failed to test with tester '" << mtesters[i]->id() << "'!" << endl;
				return false;
			}
		}

		cout << "command.unit_test message: performed " << mtesters.size() << " tests! validation key is: ";
		printf("%#lx\n", check_key);
		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}
};

}

#endif /* OPTFRAME_MODULE_UNIT_TEST_HPP_ */
