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

#ifndef HELPMODULE_HPP_
#define HELPMODULE_HPP_

#include "../Command.hpp"

using namespace std;

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemHelpCommand : public Command<R, ADS, DS>
{
public:
   virtual ~SystemHelpCommand()
   {
   }

   string id()
   {
      return "system.help";
   }
   string usage()
   {
      return "system.help";
   }

   bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& all_functions, HeuristicFactory<R, ADS, DS>&, map<string, string>&, map<string, vector<string>>&, string)
   {
      cout << "Available modules are:" << endl;
      for (unsigned int i = 0; i < all_modules.size(); i++) {
         cout << all_modules[i]->id();
         if (all_modules[i]->handles.size() > 0)
            cout << " =>";
         for (unsigned h = 0; h < all_modules[i]->handles.size(); h++)
            cout << "\t" << all_modules[i]->handles[h] << " ";
         cout << endl;
      }

      cout << "Available functions are:" << endl;
      for (unsigned int i = 0; i < all_functions.size(); i++) {
         cout << all_functions[i]->id();
         if (all_functions[i]->handles.size() > 0)
            cout << " =>";
         for (unsigned h = 0; h < all_functions[i]->handles.size(); h++)
            cout << "\t" << all_functions[i]->handles[h];
         cout << endl;
      }

      cout << "Type 'system.usage module_name' to learn how to use the command." << endl;

      return true;
   }

   virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* HELPMODULE_HPP_ */
