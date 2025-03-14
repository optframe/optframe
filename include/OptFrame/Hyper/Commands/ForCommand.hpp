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

#ifndef OPTFRAME_FOR_MODULE_HPP_
#define OPTFRAME_FOR_MODULE_HPP_

#include <string>

#include "../Command.hpp"

#include "SystemRunCommand.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ForCommand : public Command<R, ADS, DS>
{
private:
   Command<R, ADS, DS>* getCommand(std::vector<Command<R, ADS, DS>*>& modules, string module)
   {
      for (unsigned int i = 0; i < modules.size(); i++)
         if (module == modules[i]->id())
            return modules[i];
      return nullptr;
   }

   bool exec_command(std::vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<std::string, std::string>& dictionary, map<string, vector<string>>& ldictionary, string command)
   {
      Scanner scanner(command);
      string module = scanner.next();
      Command<R, ADS, DS>* m = getCommand(all_modules, module);

      if (m == nullptr)
         return false;

      string* rest = m->preprocess(allFunctions, dictionary, ldictionary, scanner.rest());

      if (!rest)
         return nullptr;

      //cout << "FOR COMMAND: '" << module << "' input: '" << *rest << "'"<< std::endl;
      bool b = m->run(all_modules, allFunctions, factory, dictionary, ldictionary, *rest);

      delete rest;

      return b;
   }

public:
   virtual ~ForCommand()
   {
   }

   string id()
   {
      return "for";
   }

   string usage()
   {
      return "for var [ = value1 to value2 | = value2 downto value1 ] block_of_commands";
   }

   bool run(std::vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<std::string, std::string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);
      //cout << "for run: '" << input << "'" << std::endl;

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      string var = scanner.next();

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      scanner.next(); // '='

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      int value1 = *scanner.nextInt();

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      string option = scanner.next(); // 'to' or 'downto'

      if ((option != "to") && (option != "downto")) {
         std::cout << "for command: unknown option '" << option << "'! Should be 'to' or 'downto'." << std::endl;
         return false;
      }

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      int value2 = *scanner.nextInt();

      vector<string>* pcommands = OptFrameList::readBlock(scanner);
      vector<string> commands;
      if (pcommands) {
         commands = vector<string>(*pcommands);
         delete pcommands;
      } else
         return false;

      // check if all the text was used!
      if (!Command<R, ADS, DS>::testUnused(id(), scanner))
         return false;

      if (option == "to")
         for (int v = value1; v <= value2; v++) {
            std::stringstream ss;
            ss << var << " " << v;
            if (!Command<R, ADS, DS>::run_module("silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str()))
               return false;

            if (!Command<R, ADS, DS>::run_module("system.run", all_modules, allFunctions, factory, dictionary, ldictionary, OptFrameList::blockToString(commands))) {
               std::cout << "for command: (TO) error in command with value='" << v << "'!" << std::endl;
               return false;
            } else
               return true;
         }
      else // downto
         for (int v = value1; v >= value2; v--) {
            std::stringstream ss;
            ss << var << " " << v;
            if (!Command<R, ADS, DS>::run_module("silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str()))
               return false;

            if (!Command<R, ADS, DS>::run_module("system.run", all_modules, allFunctions, factory, dictionary, ldictionary, OptFrameList::blockToString(commands))) {
               std::cout << "for command: (DOWNTO) error in command with value='" << v << "'!" << std::endl;
               return false;
            } else
               return true;
         }

      return true;
   }

   // should preprocess only until list of commands
   virtual string* preprocess(std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<std::string, std::string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);

      // should skip the removal of comments '%' and variable definitions, otherwise it destroys the commands!

      string input2 = "";

      if (!scanner.hasNext())
         return new string(input2);

      string variable = scanner.next();
      string discarded1 = scanner.getDiscarded();
      input2.append(discarded1);
      input2.append(variable);

      if (!scanner.hasNext())
         return new string(input2);

      string equals = scanner.next();
      string discarded_eq = scanner.getDiscarded();
      input2.append(discarded_eq);
      input2.append(equals);

      if (!scanner.hasNext())
         return new string(input2);

      string possible_def1 = scanner.next();
      string discarded_def1 = scanner.getDiscarded();

      if (dictionary.count(possible_def1) == 0) // Not found in dictionary
      {
         input2.append(discarded_def1);
         input2.append(possible_def1);
      } else // definition found in dictionary
      {
         string found = dictionary.find(possible_def1)->second;

         input2.append(discarded_def1);
         input2.append(found);
      }

      if (!scanner.hasNext())
         return new string(input2);

      string to_downto = scanner.next();
      string discarded_td = scanner.getDiscarded();
      input2.append(discarded_td);
      input2.append(to_downto);

      if (!scanner.hasNext())
         return new string(input2);

      string possible_def2 = scanner.next();
      string discarded_def2 = scanner.getDiscarded();

      if (dictionary.count(possible_def2) == 0) // Not found in dictionary
      {
         input2.append(discarded_def2);
         input2.append(possible_def2);
      } else // definition found in dictionary
      {
         string found = dictionary.find(possible_def2)->second;

         input2.append(discarded_def2);
         input2.append(found);
      }

      if (!scanner.hasNext())
         return new string(input2);

      input2.append(scanner.rest());

      return new string(input2);
   }
};

}

#endif /* OPTFRAME_FOR_MODULE_HPP_ */
