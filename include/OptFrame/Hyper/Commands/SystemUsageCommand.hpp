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

#ifndef USAGEMODULE_HPP_
#define USAGEMODULE_HPP_

#include "../Command.hpp"
#include "../HeuristicFactory.hpp"
#include "../PreprocessFunction.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemUsageCommand : public Command<R, ADS, DS>
{
public:
   virtual ~SystemUsageCommand()
   {
   }

   string id()
   {
      return "system.usage";
   }

   string usage()
   {
      return "system.usage module_or_function";
   }

   bool run(std::vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& all_functions, HeuristicFactory<R, ADS, DS>&, map<std::string, std::string>& dictionary, map<string, vector<string>>& ldictionary, string rest)
   {
      Scanner scanner(rest);

      string command = scanner.next();

      bool notfound = true;

      for (unsigned int i = 0; i < all_modules.size(); i++)
         if (all_modules[i]->canHandle(command, "")) {
            std::cout << "Usage: " << all_modules[i]->usage() << std::endl;
            notfound = false;
            break;
         }

      if (notfound)
         for (unsigned int i = 0; i < all_functions.size(); i++)
            if (command == all_functions[i]->id()) {
               std::cout << "Usage: " << all_functions[i]->usage() << std::endl;
               notfound = false;
               break;
            }

      if (notfound) {
         std::cout << "Command or function '" << command << "' not found!" << std::endl;
         return false;
      }

      return true;
   }

   virtual string* preprocess(std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<std::string, std::string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* USAGEMODULE_HPP_ */
