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

#ifndef LISTFROMPOPULATIONMODULE_HPP_
#define LISTFROMPOPULATIONMODULE_HPP_

#include "../Command.hpp"

#include "SystemDefineCommand.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentListFromPopulationCommand : public Command<R, ADS, DS>
{
public:
   virtual ~ComponentListFromPopulationCommand()
   {
   }

   string id()
   {
      return "component.list_from_population";
   }

   string usage()
   {
      return "component.list_from_population new_list_name loadpop id";
   }

   bool run(std::vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<std::string, std::string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scan(input);
      if (!scan.hasNext()) // no file
      {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      string listName = scan.next();

      if (!scan.hasNext()) // no file
      {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      string strloadpop = scan.next();

      if (strloadpop != "loadpop") {
         std::cout << "expected 'loadpop id', but found '" << strloadpop << "'" << std::endl;
         return false;
      }

      string id = scan.next();
      Scanner scan_pop(strloadpop + " " + id);
      Population<XES>* p = nullptr;
      factory.readComponent(p, scan_pop);

      stringstream listContent;

      listContent << "[ ";

      for (unsigned i = 0; i < p->size() - 1; i++) {
         int sid = factory.addComponent(p->at(i).clone());
         listContent << "loadsol " << sid << " , ";
      }

      if (p->size() > 0) {
         unsigned i = p->size() - 1;
         int sid = factory.addComponent(p->at(i).clone());
         listContent << "loadsol " << sid;
      }

      listContent << " ]";

      return Command<R, ADS, DS>::run_module("system.define", all_modules, allFunctions, factory, dictionary, ldictionary, listName + " " + listContent.str());
   }

   virtual string* preprocess(std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<std::string, std::string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* LISTFROMPOPULATIONMODULE_HPP_ */
