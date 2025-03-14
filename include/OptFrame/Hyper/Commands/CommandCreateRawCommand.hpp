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

#ifndef CREATE_RAW_MODULE_HPP_
#define CREATE_RAW_MODULE_HPP_

#include <string>

#include "../Command.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RawCommand : public Command<R, ADS, DS>
{
   string name;
   vector<string> parameters;
   vector<string> commands;

public:
   RawCommand(string _name, vector<string>& _parameters, vector<string>& _commands)
   {
      name = _name;
      parameters = _parameters;
      commands = _commands;
      //cout << "RAW MODULE(" << _name << "," << _parameters << "," << _commands << ")" << std::endl;
   }

private:
   Command<R, ADS, DS>* getCommand(std::vector<Command<R, ADS, DS>*>& modules, string module)
   {
      for (unsigned int i = 0; i < modules.size(); i++)
         if (modules[i]->canHandle(module, "")) // TODO: fix
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

      string* rest = m->preprocess(allFunctions, factory, dictionary, ldictionary, scanner.rest());

      if (!rest)
         return false;

      bool b = m->run(all_modules, allFunctions, factory, dictionary, ldictionary, *rest);

      delete rest;

      return b;
   }

public:
   virtual ~RawCommand()
   {
   }

   string id()
   {
      return name;
   }

   // default: hashing by id()
   virtual unsigned long hash()
   {
      unsigned long h = aux_hash(name);

      for (unsigned p = 0; p < parameters.size(); p++)
         h += aux_hash(parameters[p]);

      for (unsigned k = 0; k < commands.size(); k++)
         h += aux_hash(commands[k]);

      return h;
   }

   // auxiliar hash function
   unsigned long aux_hash(string s)
   {
      const char* str = s.c_str();

      unsigned long h = 5381;
      int c;

      while ((c = *str++))
         h = ((h << 5) + h) + c; /*option: hash * 33 + c */

      return h;
   }

   string usage()
   {
      string u = name;
      u.append(" ");
      for (unsigned int i = 0; i < parameters.size(); i++) {
         u.append(parameters[i]);
         u.append(" ");
      }

      return u;
   }

   bool run(std::vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<std::string, std::string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      string backup_input = input;

      Scanner scanner(input);

      //cout << "raw_module '" << id() << "' run: '" << input << "'" << std::endl;

      vector<string> values;
      values.push_back(backup_input); // "$_all_params"

      unsigned nsystem_params = values.size();

      for (unsigned int i = 0; i < (parameters.size() - nsystem_params); i++)
         if (!scanner.hasNext()) {
            std::cout << "Usage: " << usage() << std::endl;
            return false;
         } else
            values.push_back(scanner.next());

      for (unsigned v = 0; v < values.size(); v++) {
         //cout << "RAW MODULE " << id() << " DEFINING: '" << parameters[v] << "' as '" << values[v] << "'" << std::endl;
         if (!Command<R, ADS, DS>::defineText(parameters[v], values[v], dictionary)) {
            std::cout << "command.create_raw error: failed to define parameter '" << parameters[v] << "' to value '" << values[v] << "'" << std::endl;
            return false;
         }
      }

      //cout << "MODULE '" << id() << "' (CREATED) VALUES: '" << values << "'" << std::endl;

      for (unsigned int c = 0; c < commands.size(); c++) {
         string command = commands[c];
         //cout << "RAW MODULE " << id() << " COMMAND: '" << command << "'" << std::endl;

         for (unsigned int v = 0; v < values.size(); v++) {
            //cout << "RAW MODULE " << id() << " SOLVING VARIABLE '" << parameters[v] << "'" << std::endl;
            string* r = Command<R, ADS, DS>::solveVars(dictionary, ldictionary, command, parameters[v]);
            if (!r) {
               std::cout << "Raw module '" << id() << "' error: couldn't solve variable '" << parameters[v] << "'" << std::endl;
               return false;
            }
            command = *r;
            //cout << "RAW MODULE AFTER SOLVE: '" << command << "'" << std::endl << std::endl;
            delete r;
         }

         //cout << "RAW MODULE '" << id() << "' EXEC COMMAND: '" << command << "'" << std::endl;

         if (!exec_command(all_modules, allFunctions, factory, dictionary, ldictionary, command)) {
            std::cout << "raw_module '" << id() << "' error in command: " << command << std::endl;
            return false;
         }
      }

      return true;
   }

   virtual string* preprocess(std::vector<PreprocessFunction<R, ADS, DS>*>& fs, HeuristicFactory<R, ADS, DS>& hf, const map<std::string, std::string>& d, const map<string, vector<string>>& ld, string input)
   {
      // disable preprocess!! NEVER PREPROCESS HERE, IT'S THE ONLY DIFFERENCE FROM MODULE.CREATE!
      return new string(input);
   }
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class CommandCreateRawCommand : public Command<R, ADS, DS>
{
private:
   bool moduleExists(string moduleName, vector<Command<R, ADS, DS>*>& allCommands)
   {
      for (unsigned i = 0; i < allCommands.size(); i++)
         if (allCommands[i]->id() == moduleName)
            return true;
      return false;
   }

   Command<R, ADS, DS>* getCommand(std::vector<Command<R, ADS, DS>*>& modules, string module)
   {
      for (unsigned int i = 0; i < modules.size(); i++)
         if (module == modules[i]->id())
            return modules[i];
      return nullptr;
   }

public:
   virtual ~CommandCreateRawCommand()
   {
   }

   string id()
   {
      return "command.create_raw";
   }

   string usage()
   {
      return "command.create_raw name list_of_$parameters block_of_commands";
   }

   bool run(std::vector<Command<R, ADS, DS>*>& modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<std::string, std::string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);
      //cout << "create_module run: '" << input << "'" << std::endl;

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      string name = scanner.next();

      if (moduleExists(name, modules)) {
         std::cout << "command.create_raw command: couldn't create module '" << name << "' because it already exists!" << std::endl;
         return false;
      }

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      vector<string>* plist1 = OptFrameList::readList(ldictionary, scanner);
      vector<string> parameters;
      if (plist1) {
         parameters = vector<string>(*plist1);
         vector<string> system_params;
         string name_all_params = "_all_params_"; // + MODULE_NAME
         name_all_params.append(name);
         system_params.push_back(name_all_params);
         parameters.insert(parameters.begin(), system_params.begin(), system_params.end());
         delete plist1;
      } else
         return false;

      for (unsigned int i = 0; i < parameters.size(); i++)
         if (parameters[i][0] == '$') {
            std::cout << "command.create_raw error: operator $ in variable: '" << parameters[i] << "'" << std::endl;
            return false;
         }

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      vector<string> commands;

      vector<string>* plist = OptFrameList::readBlock(scanner);
      if (plist) {
         commands = vector<string>(*plist);
         delete plist;
      } else
         return false;

      Command<R, ADS, DS>* m = getCommand(modules, name);

      if (m != nullptr) {
         std::cout << "command.create_raw error: module with name '" << name << "' already exists!" << std::endl;
         return false;
      } else {
         modules.push_back(new RawCommand<R, ADS, DS>(name, parameters, commands));
         //cout << "raw_module '" << name << "' loaded." << std::endl;
         return true;
      }
   }

   virtual string* preprocess(std::vector<PreprocessFunction<R, ADS, DS>*>& fs, HeuristicFactory<R, ADS, DS>& hf, const map<std::string, std::string>& d, const map<string, vector<string>>& ld, string input)
   {
      int end = -1;
      string body = "";
      for (int i = 0; i < ((int)input.length()); i++)
         if (input[i] == '{') {
            end = i;
            break;
         } else
            body += input[i];

      if (end < 0)
         return nullptr; // no brackets

      string* pbody = Command<R, ADS, DS>::defaultPreprocess(fs, hf, d, ld, body);

      if (!pbody)
         return nullptr;

      string ninput = *pbody;

      delete pbody;

      for (int i = end; i < ((int)input.length()); i++)
         ninput += input[i];

      return new string(ninput);
   }
};

}

#endif /* CREATE_RAW_MODULE_HPP_ */
