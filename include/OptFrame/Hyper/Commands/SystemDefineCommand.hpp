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

#ifndef OPTFRAME_SYSTEM_DEFINE_MODULE_HPP_
#define OPTFRAME_SYSTEM_DEFINE_MODULE_HPP_

#include "../Command.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemDefineCommand : public Command<R, ADS, DS>
{
public:
   virtual ~SystemDefineCommand()
   {
   }

   string id()
   {
      return "system.define";
   }
   string usage()
   {
      return "system.define new_name text_to_be_substituted_from_the_new_name";
   }

   bool run(std::vector<Command<R, ADS, DS>*>&, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>&, map<std::string, std::string>& dictionary, map<string, vector<string>>&, string rest)
   {
      Scanner scanner(rest);

      if (!scanner.hasNext())
         return false;

      string new_name = scanner.next();

      if (new_name != "") {
         string second_word = scanner.rest();
         if (Command<R, ADS, DS>::defineText(new_name, second_word, dictionary)) {
            std::cout << "Word '" << new_name << "' now means: '" << dictionary[new_name] << "'" << std::endl;
            return true;
         } else
            return false;
      } else {
         std::cout << "Usage: " << usage() << std::endl;
         return false;
      }
   }

   virtual string* preprocess(std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<std::string, std::string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);

      // First, remove the comments '%'

      string input2 = "";

      while (scanner.hasNextChar()) {
         char c = *scanner.nextChar();
         if (c != '%')
            input2 += c;
         else
            break;
      }

      scanner = Scanner(input2);

      // Second, use the dictionary, but before...
      // WAIT! Save the definition name!

      if (!scanner.hasNext())
         return new string(input2);

      string name = scanner.next();
      string discarded = scanner.getDiscarded();

      // now proceed as usual

      string* input3 = Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, scanner.rest());

      if (!input3)
         return nullptr;

      string input4;
      input4.append(discarded);
      input4.append(name);
      input4.append(" ");
      input4.append(*input3);

      delete input3;

      string input5 = Scanner::trim(input4);

      return new string(input5);
   }
};

}

#endif /* OPTFRAME_SYSTEM_DEFINE_MODULE_HPP_ */
