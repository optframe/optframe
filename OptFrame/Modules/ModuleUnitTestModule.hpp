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
		return "module.unit_test target_module_name list_of_testers [validation_key]";
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

		string target = scanner.next();

		if (!moduleExists(target, modules))
		{
			cout << "module.unit_test module: can't test module '" << target << "' because it doesn't exist!" << endl;
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
			cout << "module.unit_test error: ill-formed testers list for check of module '" << target << "'!" << endl;
			return false;
		}

		string skey = "";
		unsigned long key = 0;

		if (scanner.hasNext())
		{
			skey = scanner.next();
			istringstream myStream(skey);
			if (!(myStream >> key))
			{
				cout << "module.unit_test error: failed to convert key '" << skey << "' to unsigned long!" << endl;
				return false;
			}
		}

		// =================
		// get target module
		// =================

		Module<R, ADS, DS>* mtarget = getModule(modules, target, "");

		if (!mtarget)
		{
			cout << "module.unit_test error: target '" << target << "' doesn't exist!" << endl;
			return false;
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
		unsigned long check_key = mtarget->hash();

		for(unsigned i=0; i<mtesters.size(); i++)
			check_key += mtesters[i]->hash();

		// need to check
		if(skey != "")
		{
			if(check_key == key)
				return true;
			else
				cout << "module.unit_test warning: different key for target '" << target << "'. key=" << key << " calculated=" << check_key << endl;
		}

		// ================
		// run test modules
		// ================

		for (unsigned int i = 0; i < mtesters.size(); i++)
		{
			string inp = ""; // no need to preprocess
			if(!mtesters[i]->run(modules,allFunctions, factory, dictionary, ldictionary, inp))
			{
				cout << "module.unit_test error: failed to test '" << target << "' with tester '" << mtesters[i]->id() << "'!" << endl;
				return false;
			}
		}

		cout << "module.unit_test message: performed " << mtesters.size() << " tests on module '" << target << "'! validation key is " << check_key << endl;
		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Module<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}
};

}

#endif /* OPTFRAME_MODULE_UNIT_TEST_HPP_ */
