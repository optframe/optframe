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

#ifndef CREATE_NUMERIC_LIST_MODULE_HPP_
#define CREATE_NUMERIC_LIST_MODULE_HPP_

#include "../Command.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class CreateNumericListCommand : public Command<R, ADS, DS>
{
public:
   virtual ~CreateNumericListCommand()
   {
   }

   string id()
   {
      return "create_numeric_list";
   }
   string usage()
   {
      return "create_numeric_list begin end list_name";
   }

   bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);

      int begin = *scanner.nextInt();
      int end = *scanner.nextInt();
      string lname = scanner.next();

      stringstream ss;

      ss << lname << " [";
      for (int i = begin; i < end; i++)
         ss << i << ",";
      if ((end - begin) >= 0)
         ss << end;
      ss << " ]";

      return Command<R, ADS, DS>::run_module("list.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
   }

   virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* CREATE_NUMERIC_LIST_MODULE_HPP_ */
