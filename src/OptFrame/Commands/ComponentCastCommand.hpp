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

#ifndef OPTFRAME_COMPONENT_CAST_HPP_
#define OPTFRAME_COMPONENT_CAST_HPP_

#include "../Command.hpp"

#include "SystemSilentDefineCommand.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentCastCommand : public Command<R, ADS, DS>
{
public:
   virtual ~ComponentCastCommand()
   {
   }

   string id()
   {
      return "component.cast";
   }

   string usage()
   {
      string u = "component.cast   Component id   new_type  [variable]";
      return u;
   }

   bool run(vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      //cout << "component.cast: " << input << endl;

      Scanner scanner(input);

      if (!scanner.hasNext()) {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      string comp = scanner.next();

      if (!scanner.hasNext()) {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      int number = *scanner.nextInt();

      if (!scanner.hasNext()) {
         cout << "module " << id() << " error: missing type name!" << endl;
         cout << "Usage: " << usage() << endl;
         return false;
      }

      string type = scanner.next();

      string variable = "";
      if (scanner.hasNext())
         variable = scanner.next();

      //cout << "will look for action: '" << type << "'" << endl;

      for (unsigned a = 0; a < factory.actions.size(); a++)
         if (factory.actions[a]->handleComponent(type)) {
            return factory.actions[a]->doCast(comp, number, type, variable, factory, dictionary);
         }

      cout << "component.cast error: action with cast handle for type '" << type << "' not found!" << endl;
      return false;
   }

   virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* OPTFRAME_COMPONENT_CAST_HPP_ */
