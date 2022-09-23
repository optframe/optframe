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

#ifndef EMPIRICALMODULE_HPP_
#define EMPIRICALMODULE_HPP_

#include "../Command.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class EmpiricalCommand : public Command<R, ADS, DS>
{
public:
   virtual ~EmpiricalCommand()
   {
   }

   string id()
   {
      return "empirical";
   }
   string usage()
   {
      string u = "empirical N T TF BF EVAL METHOD OUTPUTFILE\n WHERE:\n";
      u += "N is the number of tests to be executed;\n";
      u += "T is the timelimit, in seconds, for each test; (0 for no timelimit)\n";
      u += "TF is the target evaluation function value;\n";
      u += "BF is the best known evaluation function value;\n";
      u += "METHOD is the method to be tested with its own parameters;\n";
      u += "OUTPUTFILE is the output file.\n";

      return u;
   }

   bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string>>& ldictionary, string input)
   {
      Scanner scanner(input);

      if (!scanner.hasNext()) {
         cout << "Usage: " << usage() << endl;
         return true;
      }

      int n = *scanner.nextInt();
      int t = *scanner.nextDouble();
      double tf = *scanner.nextDouble();
      double bf = *scanner.nextDouble();

      pair<SingleObjSearch<R, ADS, DS>*, string> method = factory.createSingleObjSearch(scanner.rest());

      SingleObjSearch<R, ADS, DS>* h = method.first;

      string filename = method.second;

      if (t == 0) // if no timelimit
         t = 1000000000;

      long timelimit = t;

      if (bf == 0) // if best is zero
         bf = 0.00001;

      FILE* file = fopen(filename.c_str(), "a");
      if (!file) {
         cout << "Error creating file '" << filename << "'" << endl;
         return true;
      }

      double t_now = 0;
      double fo_now = 0;

      vector<double> time_spent;

      for (int i = 0; i < n; i++) {
         long seed = time(nullptr) + i;
         fprintf(file, "%ld\t", seed);

         cout << "Test " << i << " {seed=" << seed << "}... Running";
         Timer t(false);

         pair<Solution<R, ADS>&, Evaluation<DS>&>* result = h->search(timelimit, tf);

         if (!result) {
            cout << "EMPIRICAL ERROR!" << endl;
            return false;
         }

         Solution<R, ADS>* s2 = &result->first;

         t_now = t.now();
         fo_now = result->second.evaluation();
         fprintf(file, "%f\t%.3f\t", fo_now, t_now);
         time_spent.push_back(t_now);

         cout << "... Finished! (" << t.now() << "secs.)" << endl;

         delete s2;

         fprintf(file, "\n");
      }

      sort(time_spent.begin(), time_spent.end());

      fprintf(file, "EMPIRICAL PROBABILITY DISTRIBUTION:\ni\ttime(i)\tp(i)=(i-0.5)/N\n0\t0\t0\n");
      for (int i = 1; i <= n; i++)
         fprintf(file, "%i\t%.3f\t%.3f\n", i, time_spent[i - 1], (i - 0.5) / n);

      fprintf(file, "PARAMETERS:%s\n", input.c_str());

      fclose(file);
      return true;
   }

   virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string>>& ldictionary, string input)
   {
      return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
   }
};

}

#endif /* EMPIRICALMODULE_HPP_ */
