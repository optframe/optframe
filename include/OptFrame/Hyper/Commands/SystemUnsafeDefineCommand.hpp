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

#ifndef UNSAFE_DEFINE_MODULE_HPP_
#define UNSAFE_DEFINE_MODULE_HPP_

#include "../Command.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemUnsafeDefineCommand : public Command<R, ADS, DS>
{
public:
   virtual ~SystemUnsafeDefineCommand()
   {
   }

   string id()
   {
      return "system.unsafe_define";
   }
   string usage()
   {
      return "system.unsafe_define new_name text";
   }

   bool run(std::vector<Command<R, ADS, DS>*>&, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>&, map<std::string, std::string>& dictionary, map<string, vector<string>>& ldictionary, string rest)
   {
      Scanner scanner(rest);

      if (!scanner.hasNext())
         return false;

      string new_name = scanner.next();

      if (new_name != "") {
         string second_word = scanner.rest();
         return Command<R, ADS, DS>::defineText(new_name, second_word, dictionary);
      } else {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }
   }

   // FAITH ON USER!! NO PREPROCESSING :D
   virtual string* preprocess(std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<std::string, std::string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return new string(input);
   }
};

}

#endif /* UNSAFE_DEFINE_MODULE_HPP_ */
