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

#ifndef FOREACHMODULE_HPP_
#define FOREACHMODULE_HPP_

#include <string>

#include "../Command.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ForEachCommand : public Command<R, ADS, DS>
{
public:
   virtual ~ForEachCommand()
   {
   }

   string id()
   {
      return "for_each";
   }

   string usage()
   {
      return "for_each variable list_of_values block_of_commands";
   }

   bool run(vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);
      //cout << "for_each run: '" << input << "'" << endl;

      if (!scanner.hasNext()) {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      string var = scanner.next();

      if (var[0] == '$') {
         cout << "for_each error: operator $ in variable '" << var << "'" << endl;
         return false;
      }

      bool initialStatusVar = Command<R, ADS, DS>::isInDictionary(var, dictionary, ldictionary);

      vector<string>* pvalues = OptFrameList::readList(ldictionary, scanner);
      vector<string> values;
      if (pvalues) {
         values = vector<string>(*pvalues);
         delete pvalues;
      } else {
         cout << "for_each error: couldn't read input list!" << endl;
         return false;
      }

      vector<string>* pcommands = OptFrameList::readBlock(scanner);
      vector<string> commands;
      if (pcommands) {
         commands = vector<string>(*pcommands);
         delete pcommands;
      } else {
         cout << "for_each error: couldn't read block of commands!" << endl;
         return false;
      }

      if (!Command<R, ADS, DS>::testUnused(id(), scanner))
         return false;

      for (unsigned int v = 0; v < values.size(); v++) {
         Command<R, ADS, DS>::undefine(var, dictionary, ldictionary);

         if (!Command<R, ADS, DS>::define(var, values[v], dictionary, ldictionary)) {
            cout << "for_each error: failed to define variable '" << var << "' to value '" << values[v] << "'" << endl;
            return false;
         }

         if (!Command<R, ADS, DS>::run_module("system.run", allCommands, allFunctions, factory, dictionary, ldictionary, OptFrameList::blockToString(commands))) {
            cout << "for_each command: error in command!" << endl;
            return false;
         }
      }

      if (!initialStatusVar)
         Command<R, ADS, DS>::undefine(var, dictionary, ldictionary);

      return true;
   }

   // should preprocess only until list of commands
   virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
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

      string possibleDefinition = scanner.next();
      string discarded2 = scanner.getDiscarded();

      if (dictionary.count(possibleDefinition) == 0) // Not found in dictionary
      {
         input2.append(discarded2);
         input2.append(possibleDefinition); // EXPLICIT LIST
      } else                                // definition found in dictionary
      {
         string found = dictionary.find(possibleDefinition)->second;

         input2.append(discarded2);
         input2.append(found);
      }

      input2.append(scanner.rest());

      return new string(input2);
   }
};

}

#endif /* FOREACHMODULE_HPP_ */
