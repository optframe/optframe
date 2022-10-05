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

#ifndef DICTIONARYMODULE_HPP_
#define DICTIONARYMODULE_HPP_

#include "../Command.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemDictionaryCommand : public Command<R, ADS, DS>
{
public:
   virtual ~SystemDictionaryCommand()
   {
   }

   string id()
   {
      return "system.dictionary";
   }
   string usage()
   {
      return "system.dictionary";
   }

   bool run(vector<Command<R, ADS, DS>*>&, vector<PreprocessFunction<R, ADS, DS>*>&, HeuristicFactory<R, ADS, DS>&, map<string, string>& dictionary, map<string, vector<string>>& ldictionary, string)
   {
      cout << "Text dictionary: " << endl;

      map<string, string>::iterator iter;
      int size1 = 0;
      for (iter = dictionary.begin(); iter != dictionary.end(); iter++) {
         string d = iter->first;
         string v = iter->second;
         cout << "'" << d << "' => '" << v << "'" << endl;
         size1++;
      }
      cout << size1 << " entries." << endl;

      // --------------------------------

      cout << "List dictionary: " << endl;

      map<string, vector<string>>::iterator iter2;
      int size2 = 0;
      for (iter2 = ldictionary.begin(); iter2 != ldictionary.end(); iter2++) {
         string d = iter2->first;
         vector<string>& v = iter2->second;
         cout << "'" << d << "' => '" << v << "'" << endl;
         size2++;
      }
      cout << size2 << " entries." << endl;

      return true;
   }

   virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* DICTIONARYMODULE_HPP_ */
