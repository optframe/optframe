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

#ifndef OPTFRAME_COMPONENT_LIST_MODULE_HPP_
#define OPTFRAME_COMPONENT_LIST_MODULE_HPP_

#include "../Command.hpp"

#include "SystemDefineCommand.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentListCommand : public Command<R, ADS, DS>
{
public:
   virtual ~ComponentListCommand()
   {
   }

   string id()
   {
      return "component.list";
   }

   string usage()
   {
      return "component.list pattern [store_list]\nWhere: store_list is an optional variable to store the components.";
   }

   bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);

      string pattern = "OptFrame:";

      if (scanner.hasNext()) {
         pattern = scanner.next();
      }

      vector<string> components = factory.listComponents(pattern);
      vector<string> componentLists = factory.listComponentLists(pattern);

      components.insert(components.end(), componentLists.begin(), componentLists.end());

      //cout << "component.list command: NUM. COMPONENTS IS " << components.size() << endl;

      if (!scanner.hasNext()) {
         for (int i = 0; i < (int)components.size(); i++)
            cout << components[i] << endl;

         return true;
      } else {
         string new_name = scanner.next();

         //cout << "component.list command: NEW_NAME = '" << new_name << "'" << endl;

         stringstream ss;

         //cout << "components.list command: COMPONENTS '" << components << "'" << endl;

         ss << new_name << " " << OptFrameList::listToString(components);

         //cout << "component.list command: CREATING LIST OF COMPONENTS '" << ss.str() << "'" << endl;

         return Command<R, ADS, DS>::run_module("list.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
      }
   }

   // disable preprocess to don't destroy type!
   virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      // disable preprocess!!
      return new string(input);
   }
};

}

#endif /* OPTFRAME_COMPONENT_LIST_MODULE_HPP_ */
