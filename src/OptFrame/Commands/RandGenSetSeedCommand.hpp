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

#ifndef RANDGENMODULE_HPP_
#define RANDGENMODULE_HPP_

#include "../Command.hpp"
#include "../RandGen.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RandGenSetSeedCommand : public Command<R, ADS, DS>
{
public:
   virtual ~RandGenSetSeedCommand()
   {
   }

   string id()
   {
      return "randgen.set_seed";
   }

   string usage()
   {
      return "randgen.set_seed seed\n Where: 'seed' is a positive integer value for the system random number generator.";
   }

   bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);

      if (!scanner.hasNext()) {
         cout << "missing 'seed' positive integer value!" << endl;
         cout << "Usage: " << usage() << endl;
         return false;
      }

      unsigned long seed = *scanner.nextInt();

      cout << "randgen.set_seed command: setting system random number generator seed to: " << seed << endl;

      RandGen& rg = factory.getRandGen();
      rg.setSeed(seed);
      rg.initialize();

      return true;
   }

   virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* RANDGENMODULE_HPP_ */
