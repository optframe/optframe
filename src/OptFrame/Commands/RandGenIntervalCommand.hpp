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

#ifndef RANDOM_NUMBER_INTERVAL_MODULE_HPP_
#define RANDOM_NUMBER_INTERVAL_MODULE_HPP_

#include "../Command.hpp"
#include "../RandGen.hpp"

#include "SystemSilentDefineCommand.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RandGenIntervalCommand : public Command<R, ADS, DS>
{
public:
   virtual ~RandGenIntervalCommand()
   {
   }

   string id()
   {
      return "randgen.interval";
   }

   string usage()
   {
      return "randgen.interval begin end [stored_number]\n Where: 'begin' and 'end' are positive integer values; 'stored_number' is the randomized number from [begin,end].";
   }

   bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);
      //cout << "random_number_interval command: '" << input << "'" << endl;

      if (!scanner.hasNext()) {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      if (!scanner.hasNext()) {
         cout << "missing 'begin' positive integer value!" << endl;
         cout << "Usage: " << usage() << endl;
         return false;
      }

      int begin = *scanner.nextInt();

      if (!scanner.hasNext()) {
         cout << "missing 'end' positive integer value!" << endl;
         cout << "Usage: " << usage() << endl;
         return false;
      }

      int end = *scanner.nextInt();

      RandGen& rg = factory.getRandGen();
      int value = rg.rand(end - begin + 1) + begin;

      //cout << "random_number_interval VALUE=" << value << endl;

      if (scanner.hasNext()) {
         string new_name = scanner.next();
         stringstream ss;
         ss << new_name << " " << value;
         return Command<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
      } else {
         cout << "random_number_interval command: random number is " << value << endl;
         return true;
      }
   }

   virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* RANDOM_NUMBER_INTERVAL_MODULE_HPP_ */
