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

#ifndef OPTFRAME_LIST_ADD_MODULE_HPP_
#define OPTFRAME_LIST_ADD_MODULE_HPP_

#include <string>

#include "../Command.hpp"

#include "ListSilentDefineCommand.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ListAddCommand : public Command<R, ADS, DS>
{
public:
   virtual ~ListAddCommand()
   {
   }

   string id()
   {
      return "list.add";
   }

   string usage()
   {
      return "list.add list index new_list_name value";
   }

   bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      //cout << "list_add command: '" << input << "'" << endl;

      Scanner scanner(input);

      if (!scanner.hasNext()) {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      vector<string> list;
      vector<string>* p_list = OptFrameList::readList(ldictionary, scanner);
      if (p_list) {
         list = vector<string>(*p_list);
         delete p_list;
      } else
         return false;

      if (!scanner.hasNext()) {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      int index = *scanner.nextInt();
      index--; // index is [1..size()], but in c++ is [0..size()-1]

      if (!scanner.hasNext()) {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      string new_name = Scanner::trim(scanner.next());

      if ((new_name != "") && (new_name.at(0) == '[')) {
         cout << "list_add command: invalid list new name '" << new_name << "'" << endl;
         return false;
      }

      if ((index < 0) || (index > ((int)list.size()))) // index can be equal to size (adding)
      {
         cout << "list_add command: invalid index '" << index << "'!" << endl;
         return false;
      }

      if (!scanner.hasNext()) {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      string value = Scanner::trim(scanner.rest());
      vector<string> vv;
      vv.push_back(value);

      list.insert(list.begin() + index, vv.begin(), vv.end());

      stringstream ss;
      ss << new_name << " " << OptFrameList::listToString(list);

      //TODO: acessar dicionario de listas diretamente!! Bem mais eficiente!

      return Command<R, ADS, DS>::run_module("list.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
   }

   virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* OPTFRAME_LIST_ADD_MODULE_HPP_ */
