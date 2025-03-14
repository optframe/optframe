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

#ifndef LIST_BUILDER_OF_COMPONENT_MODULE_HPP_
#define LIST_BUILDER_OF_COMPONENT_MODULE_HPP_

#include "../Command.hpp"

#include "SystemSilentDefineCommand.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentBuilderOfComponentCommand : public Command<R, ADS, DS>
{
public:
   virtual ~ComponentBuilderOfComponentCommand()
   {
   }

   string id()
   {
      return "component.builder_of_component";
   }

   string usage()
   {
      return "component.builder_of_component component_name [store_list]\nWhere: store_list is an optional variable to store the builders.";
   }

   bool run(std::vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<std::string, std::string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);

      string component_name = scanner.next();

      string pattern = "OptFrame:ComponentBuilder:";
      // SKIP "OptFrame:" and replace by "OptFrame:ComponentBuilder:"
      for (unsigned i = Component::idComponent().length(); i < component_name.size(); i++)
         pattern += component_name.at(i);

      std::vector<std::pair<string, std::vector<std::pair<std::string, std::string>>>> builders = factory.listBuilders(pattern);

      if (!scanner.hasNext()) {
         for (int i = 0; i < (int)builders.size(); i++) {
            std::cout << builders[i].first << "\t";
            for (unsigned j = 0; j < builders[i].second.size(); j++)
               std::cout << builders[i].second[j].first << "=>'" << builders[i].second[j].second << "' ";
            std::cout << std::endl;
         }

         return true;
      } else {
         string new_name = scanner.next();
         std::stringstream ss;

         ss << new_name << " [";
         for (unsigned i = 0; i < builders.size(); i++) {
            ss << " [ " << builders[i].first << " , [ ";
            for (unsigned j = 0; j < builders[i].second.size(); j++) {
               ss << "[ " << builders[i].second[j].first << " , " << builders[i].second[j].second << "] ";
               if (j != builders[i].second.size() - 1)
                  ss << ",";
            }
            ss << " ] ] ";

            if (i != builders.size() - 1)
               ss << ",";
         }
         ss << " ]";

         return Command<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
      }
   }

   virtual string* preprocess(std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<std::string, std::string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* LIST_BUILDER_OF_COMPONENT_MODULE_HPP_ */
