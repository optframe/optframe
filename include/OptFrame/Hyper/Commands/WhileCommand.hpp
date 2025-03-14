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

#ifndef OPTFRAME_WHILE_MODULE_HPP_
#define OPTFRAME_WHILE_MODULE_HPP_

#include <string>

#include "../Command.hpp"

#include "SystemRunCommand.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class WhileCommand : public Command<R, ADS, DS>
{
public:
   virtual ~WhileCommand()
   {
   }

   string id()
   {
      return "while";
   }

   string usage()
   {
      return "while boolean list_of_commands";
   }

   string formatBool(bool b)
   {
      if (b)
         return "true";
      else
         return "false";
   }

   bool parseBool(string b)
   {
      return b == "true";
   }

   bool run(std::vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<std::string, std::string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      //cout << "INPUT: '" << input << "'" << std::endl;

      stringstream boolean_expr;
      unsigned j = 0;
      for (unsigned i = 0; i < input.size(); i++)
         if (input.at(i) != '{') {
            boolean_expr << input.at(i);
            j++;
         } else
            break;

      //cout << "'" << input.at(j) << "'" << std::endl; // should be a '['

      //cout << "BOOLEAN EXPRESSION: '" << Scanner::trim(boolean_expr.str()) << "'" << std::endl;

      stringstream sscommands;
      for (unsigned k = j; k < input.size(); k++)
         sscommands << input.at(k);

      //cout << "COMMANDS: '" << sscommands.str() << "'" << std::endl;

      Scanner scanner(sscommands.str());

      vector<string> commands;
      vector<string>* p_commands = OptFrameList::readBlock(scanner);
      if (p_commands) {
         commands = vector<string>(*p_commands);
         delete p_commands;
      } else
         return false;

      // check if all the text was used!
      if (!Command<R, ADS, DS>::testUnused(id(), scanner))
         return false;

      string* scond1 = Command<R, ADS, DS>::defaultPreprocess(allFunctions, factory, dictionary, ldictionary, boolean_expr.str());

      if (!scond1)
         return nullptr;

      string scondition = *scond1;

      delete scond1;

      while (parseBool(scondition)) {
         if (!Command<R, ADS, DS>::run_module("system.run", allCommands, allFunctions, factory, dictionary, ldictionary, OptFrameList::blockToString(commands))) {
            std::cout << "while command: error in command!" << std::endl;
            return false;
         }

         string* scond = Command<R, ADS, DS>::defaultPreprocess(allFunctions, factory, dictionary, ldictionary, boolean_expr.str());

         if (!scond)
            return false;

         scondition = *scond;
         delete scond;
      }

      return true;
   }

   // should preprocess only until list of commands
   virtual string* preprocess(std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<std::string, std::string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      // disable preprocess!!
      return new string(input);
   }
};

}

#endif /* OPTFRAME_WHILE_MODULE_HPP_ */
