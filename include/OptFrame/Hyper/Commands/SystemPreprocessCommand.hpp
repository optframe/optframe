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

#ifndef OPTFRAME_SYSTEM_PREPROCESS_MODULE_HPP_
#define OPTFRAME_SYSTEM_PREPROCESS_MODULE_HPP_

#include "../Command.hpp"

#include "SystemUnsafeDefineCommand.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemPreprocessCommand : public Command<R, ADS, DS>
{
public:
   virtual ~SystemPreprocessCommand()
   {
   }

   string id()
   {
      return "system.preprocess";
   }

   string usage()
   {
      return "system.preprocess return_value module_name input";
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

   bool run(std::vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<std::string, std::string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      string name = scanner.next();

      if (!scanner.hasNext()) {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }

      string module = scanner.next();

      string inp = scanner.rest();

      Command<R, ADS, DS>* m = getCommand(all_modules, module, "");

      if (!m) {
         std::cout << "preprocess command: nullptr module!" << std::endl;
         return false;
      }

      string* final = m->preprocess(allFunctions, factory, dictionary, ldictionary, inp);

      if (!final)
         return false;

      std::stringstream ss;
      ss << name << " " << (*final);

      delete final;

      return Command<R, ADS, DS>::run_module("system.unsafe_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
   }

   // runs raw module without preprocessing
   virtual string* preprocess(std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<std::string, std::string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return new string(input); // disable pre-processing
   }
};

}

#endif /* OPTFRAME_SYSTEM_PREPROCESS_MODULE_HPP_ */
