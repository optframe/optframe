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

#ifndef CREATE_LIST_OF_COMPONENTS_MODULE_HPP_
#define CREATE_LIST_OF_COMPONENTS_MODULE_HPP_

#include "../Command.hpp"

#include "SystemSilentDefineCommand.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentCreateListCommand : public Command<R, ADS, DS>
{
public:
   virtual ~ComponentCreateListCommand()
   {
   }

   string id()
   {
      return "component.create_list";
   }

   string usage()
   {
      return "component.create_list list type list_name";
   }

   bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);

      vector<string>* plist = OptFrameList::readList(ldictionary, scanner);
      vector<string> list;
      if (plist) {
         list = vector<string>(*plist);
         delete plist;
      } else {
         cout << "module " << id() << " failed to read list!" << endl;
         return false;
      }

      string type = scanner.next();
      string name = scanner.next();

      vector<Component*> componentList;
      for (unsigned i = 0; i < list.size(); i++) {
         Scanner scan(list[i]);
         Component* comp = factory.getNextComponent(scan);
         string rest = Scanner::trim(scan.rest());
         if (rest != "") {
            cout << "command " << id() << " error: extra text after component name '" << rest << "'!" << endl;
            cout << "PROBABLY MISSING A COLON ',' IN THE LIST!" << endl;
            return false;
         }

         if (!comp) {
            cout << "create_list_of_components: error, component #" << i << " is nullptr! " << endl;
            return false;
         } else if (!comp->compatible(ComponentHelper::typeOfList(type))) {
            cout << "create_list_of_components: error, component #" << i << " ('" << comp->id() << "') in list incompatible with type '" << ComponentHelper::typeOfList(type) << "'" << endl;
            return false;
         } else
            componentList.push_back(comp);
      }

      int idx = factory.addComponentList(componentList, type);

      stringstream ss;

      ss << name << " " << ComponentHelper::typeOfList(type) << "[] " << idx;

      cout << "'" << ComponentHelper::typeOfList(type) << "[] " << idx << "' added." << endl;

      return Command<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
   }

   virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* CREATE_LIST_OF_COMPONENTS_MODULE_HPP_ */
