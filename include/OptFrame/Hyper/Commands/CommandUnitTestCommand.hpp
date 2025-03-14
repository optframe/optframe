// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_MODULE_UNIT_TEST_HPP_
#define OPTFRAME_MODULE_UNIT_TEST_HPP_

#include <string>

#include "../Command.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class CommandUnitTestCommand : public Command<R, ADS, DS>
{
private:
   bool moduleExists(string moduleName, vector<Command<R, ADS, DS>*>& allCommands)
   {
      for (unsigned i = 0; i < allCommands.size(); i++)
         if (allCommands[i]->id() == moduleName)
            return true;
      return false;
   }

   Command<R, ADS, DS>* getCommand(std::vector<Command<R, ADS, DS>*>& modules, string module, string rest)
   {
      for (unsigned int i = 0; i < modules.size(); i++) {
         //cout << "run: testing module '" << modules[i]->id() << "'" << std::endl;
         if (modules[i]->canHandle(module, rest))
            return modules[i];
      }
      //cout << "run: nullptr MODULE! module='" << module << "' rest='" << rest << "'" << std::endl;
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

   bool run(std::vector<Command<R, ADS, DS>*>& modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<std::string, std::string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);
      //cout << "command.unit_test run: '" << input << "'" << std::endl;

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      vector<string>* plist2 = OptFrameList::readList(ldictionary, scanner);
      vector<string> targets;
      if (plist2) {
         targets = vector<string>(*plist2);
         delete plist2;
      } else {
         std::cout << "command.unit_test error: ill-formed targets list!" << std::endl;
         return false;
      }

      for (unsigned t = 0; t < targets.size(); t++)
         if (!moduleExists(targets[t], modules)) {
            std::cout << "command.unit_test command: can't test module '" << targets[t] << "' because it doesn't exist!" << std::endl;
            return false;
         }

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      vector<string>* plist1 = OptFrameList::readList(ldictionary, scanner);
      vector<string> testers;
      if (plist1) {
         testers = vector<string>(*plist1);
         delete plist1;
      } else {
         std::cout << "command.unit_test error: ill-formed testers list!" << std::endl;
         return false;
      }

      string skey = "";
      unsigned long key = 0;

      if (scanner.hasNext()) {
         skey = scanner.next();
         std::istringstream iss(skey);
         if (!(iss >> std::hex >> key)) {
            std::cout << "command.unit_test error: failed to convert key '" << skey << "' to unsigned long!" << std::endl;
            return false;
         }
      }

      // =================
      // get target module
      // =================

      vector<Command<R, ADS, DS>*> mtargets;
      for (unsigned i = 0; i < targets.size(); i++) {
         Command<R, ADS, DS>* m = getCommand(modules, targets[i], "");
         if (!m) {
            std::cout << "command.unit_test error: target '" << targets[i] << "' doesn't exist!" << std::endl;
            return false;
         }

         mtargets.push_back(m);
      }

      // ================
      // get test modules
      // ================

      vector<Command<R, ADS, DS>*> mtesters;
      for (unsigned i = 0; i < testers.size(); i++) {
         Command<R, ADS, DS>* m = getCommand(modules, testers[i], "");
         if (!m) {
            std::cout << "command.unit_test error: tester '" << testers[i] << "' doesn't exist!" << std::endl;
            return false;
         }

         mtesters.push_back(m);
      }

      // ============
      // validate key
      // ============
      unsigned long check_key = 0;

      for (unsigned i = 0; i < mtargets.size(); i++)
         check_key += mtargets[i]->hash();

      for (unsigned i = 0; i < mtesters.size(); i++)
         check_key += mtesters[i]->hash();

      // need to check
      if (skey != "") {
         if (check_key == key)
            return true;
         else {
            std::cout << "command.unit_test WARNING: different key=" << skey << " calculated=";
            printf("%#lx\n", check_key);
            std::cout << "Will perform tests! Press any key to continue..." << std::endl;
            getchar();
         }
      }

      // ================
      // run test modules
      // ================

      for (unsigned int i = 0; i < mtesters.size(); i++) {
         string inp = ""; // no need to preprocess
         if (!mtesters[i]->run(modules, allFunctions, factory, dictionary, ldictionary, inp)) {
            std::cout << "command.unit_test error: failed to test with tester '" << mtesters[i]->id() << "'!" << std::endl;
            return false;
         }
      }

      std::cout << "command.unit_test message: performed " << mtesters.size() << " tests! validation key is: ";
      printf("%#lx\n", check_key);
      return true;
   }

   virtual string* preprocess(std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<std::string, std::string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* OPTFRAME_MODULE_UNIT_TEST_HPP_ */
