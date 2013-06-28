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

#ifndef OPTFRAME_MODULE_UNIT_TEST_HPP_
#define OPTFRAME_MODULE_UNIT_TEST_HPP_

#include<string>

#include "../Module.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ModuleUnitTestModule: public Module<R, ADS, DS>
{
private:

	bool moduleExists(string moduleName, vector<Module<R, ADS, DS>*>& allModules)
	{
		for (unsigned i = 0; i < allModules.size(); i++)
			if (allModules[i]->id() == moduleName)
				return true;
		return false;
	}

	Module<R, ADS, DS>* getModule(vector<Module<R, ADS, DS>*>& modules, string module, string rest)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
		{
			//cout << "run: testing module '" << modules[i]->id() << "'" << endl;
			if (modules[i]->canHandle(module, rest))
				return modules[i];
		}
		//cout << "run: NULL MODULE! module='" << module << "' rest='" << rest << "'" << endl;
		return NULL;
	}

public:

	virtual ~ModuleUnitTestModule()
	{
	}

	string id()
	{
		return "module.unit_test";
	}

	string usage()
	{
		return "module.unit_test list_target_modules_names list_of_testers [validation_key]";
	}

	bool run(vector<Module<R, ADS, DS>*>& modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);
		//cout << "module.unit_test run: '" << input << "'" << endl;

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
			cout << "module.unit_test error: ill-formed targets list!" << endl;
			return false;
		}

		for (unsigned t = 0; t < targets.size(); t++)
			if (!moduleExists(targets[t], modules))
			{
				cout << "module.unit_test module: can't test module '" << targets[t] << "' because it doesn't exist!" << endl;
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
			cout << "module.unit_test error: ill-formed testers list!" << endl;
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
				cout << "module.unit_test error: failed to convert key '" << skey << "' to unsigned long!" << endl;
				return false;
			}
		}

		// =================
		// get target module
		// =================

		vector<Module<R, ADS, DS>*> mtargets;
		for (unsigned i = 0; i < targets.size(); i++)
		{
			Module<R, ADS, DS>* m = getModule(modules, targets[i], "");
			if (!m)
			{
				cout << "module.unit_test error: target '" << targets[i] << "' doesn't exist!" << endl;
				return false;
			}

			mtargets.push_back(m);
		}

		// ================
		// get test modules
		// ================

		vector<Module<R, ADS, DS>*> mtesters;
		for (unsigned i = 0; i < testers.size(); i++)
		{
			Module<R, ADS, DS>* m = getModule(modules, testers[i], "");
			if (!m)
			{
				cout << "module.unit_test error: tester '" << testers[i] << "' doesn't exist!" << endl;
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
				cout << "module.unit_test WARNING: different key=" << skey << " calculated=";
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
				cout << "module.unit_test error: failed to test with tester '" << mtesters[i]->id() << "'!" << endl;
				return false;
			}
		}

		cout << "module.unit_test message: performed " << mtesters.size() << " tests! validation key is: ";
		printf("%#lx\n", check_key);
		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Module<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}
};

}

#endif /* OPTFRAME_MODULE_UNIT_TEST_HPP_ */
