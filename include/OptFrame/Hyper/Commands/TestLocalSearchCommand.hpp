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

#ifndef TEST_LOCAL_SEARCH_MODULE_HPP_
#define TEST_LOCAL_SEARCH_MODULE_HPP_

#include <math.h>

#include "../Command.hpp"
#include "../Constructive.h"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class TestLocalSearchCommand : public Command<R, ADS, DS> {
 public:
  virtual ~TestLocalSearchCommand() {}

  string id() { return "testls"; }

  string usage() {
    string u =
        "test N T TF BF OptFrame:Evaluator id (OptFrame:Constructive id | "
        "OptFrame:Solution id) OptFrame:LocalSearch id OUTPUTFILE "
        "[solution_name]\n WHERE:\n";
    u += "N is the number of tests to be executed;\n";
    u +=
        "T is the timelimit, in seconds, for each test; (0 for no timelimit)\n";
    u += "TF is the target evaluation function value;\n";
    u += "BF is the best known evaluation function value;\n";
    u += "OUTPUTFILE is the output file;\n";
    u += "[solution_name] is a name given to the best found solution "
         "(optional).";

    return u;
  }

  bool run(std::vector<Command<R, ADS, DS>*>& all_modules,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>& factory,
           std::map<std::string, std::string>& dictionary,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string input) {
    Scanner scanner(input);

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }
    int n;
    string sn = scanner.next();
    try {
      n = Scanner::parseInt(sn);
    } catch (ConversionError& e) {
      std::cout << "testls error: reading 'N'" << std::endl;
      return false;
    }

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }
    int t;
    string st = scanner.next();
    try {
      t = Scanner::parseInt(st);
    } catch (ConversionError& e) {
      std::cout << "testls error: reading 't'" << std::endl;
      return false;
    }

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }
    double tf;
    string stf = scanner.next();
    try {
      tf = Scanner::parseDouble(stf);
    } catch (ConversionError& e) {
      std::cout << "testls error: reading 'tf'" << std::endl;
      return false;
    }

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }
    double bf;
    string sbf = scanner.next();
    try {
      bf = Scanner::parseDouble(sbf);
    } catch (ConversionError& e) {
      std::cout << "testls error: reading 'bf'" << std::endl;
      return false;
    }

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }
    Evaluator<R, ADS, DS>* eval;
    factory.assign(eval, *scanner.nextInt(), scanner.next());

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }
    string sc = scanner.next();

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }
    int id = *scanner.nextInt();

    Solution<R, ADS>* s1 = nullptr;
    Constructive<S>* constructive = nullptr;

    if (sc == Solution<R, ADS>::idComponent())
      factory.assign(s1, id, sc);
    else if (sc == Constructive<S>::idComponent())
      factory.assign(constructive, id, sc);
    else {
      std::cout << "testls command: ERROR IN TEST LOCAL SEARCH MODULE! NO SUCH "
                   "CONSTRUCTIVE! ('"
                << sc << " " << id << "')" << std::endl;
      return false;
    }

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }
    pair<LocalSearch<R, ADS, DS>*, string> method =
        factory.createLocalSearch(scanner.rest());

    LocalSearch<R, ADS, DS>* h = method.first;

    string rest = method.second;

    Scanner scan_rest(rest);
    // TODO: check \" in filename to allow spaces in the name
    string filename = scan_rest.next();

    if (t == 0)  // if no timelimit
      t = 1000000000;

    long timelimit = t;

    if (bf == 0)  // if best is zero
      bf = 0.00001;

    FILE* file = fopen(filename.c_str(), "a");
    if (!file) {
      std::cout << "Error creating file '" << filename << "'" << std::endl;
      return false;
    }

    fprintf(file, "PARAMETERS:%s\n", input.c_str());

    // bool minimization = eval->betterThan(1, 2); // TODO
    Solution<R, ADS>* s_star = nullptr;

    double s_fo_ini = 0;
    double s_t_ini = 0;
    double s_fo_end = 0;
    double s_t_end = 0;
    double min_fo = 1000000000;
    double min_t = 1000000000;
    double max_fo = -1000000000;
    double max_t = -1000000000;

    double t_now = 0;
    double fo_now = 0;
    double variance = 0;

    vector<long double> s_fo_tests(n);

    for (int i = 0; i < n; i++) {
      std::cout << "Test " << i << "... Running";
      Timer t(false);

      Solution<R, ADS>& s =
          ((s1 != nullptr) ? s1->clone() : constructive->generateSolution());

      t_now = t.now();
      Evaluation<DS>& e = eval->evaluate(s);
      fo_now = e.evaluation();
      delete &e;
      fprintf(file, "%.3f\t%.3f\t", fo_now, t_now);
      s_fo_ini += fo_now;
      s_t_ini += t_now;

      Solution<R, ADS>* s2 = &h->search(s, timelimit, tf);
      delete &s;
      t_now = t.now();
      Evaluation<DS>& e2 = eval->evaluate(*s2);
      fo_now = e2.evaluation();
      delete &e2;
      s_fo_tests.at(i) = fo_now;
      fprintf(file, "%.3f\t%.3f\t", fo_now, t_now);
      s_fo_end += fo_now;
      s_t_end += t_now;

      if (fo_now < min_fo) min_fo = fo_now;
      if (t_now < min_t) min_t = t_now;
      if (fo_now > max_fo) max_fo = fo_now;
      if (t_now > max_t) max_t = t_now;

      std::cout << "... Finished! (" << t.now() << "secs.)" << std::endl;

      if (!s_star)
        s_star = &s2->clone();
      else if (eval->betterThan(*s2, *s_star)) {
        delete s_star;
        s_star = &s2->clone();
      }

      delete s2;

      fprintf(file, "\n");
    }

    s_fo_ini /= n;
    s_t_ini /= n;
    s_fo_end /= n;
    s_t_end /= n;

    // calculating variance
    for (int i = 0; i < n; i++) {
      variance += pow((s_fo_end - s_fo_tests[i]), 2);
    }
    variance /= (n * 1.0);

    fprintf(file, "AVERAGE:\t%f\t%f\t%f\t%f\n", s_fo_ini, s_t_ini, s_fo_end,
            s_t_end);
    fprintf(file, "MIN:\t-\t-\t%f\t%f\n", min_fo, min_t);
    fprintf(file, "MAX:\t-\t-\t%f\t%f\n", max_fo, max_t);
    if (min_fo == 0) min_fo = 0.00001;
    fprintf(file, "VARIABILITY:\t-\t-\t%f\t-\n", (s_fo_end - min_fo) / min_fo);
    fprintf(file, "VARIANCE: \t-\t-\t%f\t-\n", variance);
    fprintf(file, "STANDARD DEVIATION : \t-\t-\t%f\t-\n", sqrt(variance));
    fprintf(file, "GAP_FROM_BEST:\t%f\t-\t%f\t-\n", (s_fo_ini - bf) / bf,
            (s_fo_end - bf) / bf);
    fprintf(file, "GAP_FROM_AVG:\t%f\t-\t%f\t-\n", (s_fo_ini - bf) / s_fo_ini,
            (s_fo_end - bf) / s_fo_end);
    fprintf(file, "IMPROVEMENT:\t-\t-\t%f\t-\n", (bf - min_fo) / bf);
    fprintf(file, "BEST(LIT):\t%f\n", bf);

    fclose(file);

    int new_id = factory.addComponent(*s_star);

    stringstream str;
    str << Solution<R, ADS>::idComponent() << " " << new_id;
    string s_new_id = str.str();

    // cout << "'" << s_new_id << "' added." << std::endl;

    if (scan_rest.hasNext()) {
      string new_name = scan_rest.next();
      return Command<R, ADS, DS>::run_module(
          "system.silent_define", all_modules, allFunctions, factory,
          dictionary, ldictionary, new_name + " " + s_new_id);
    }

    return true;
  }

  virtual string* preprocess(
      std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
      HeuristicFactory<R, ADS, DS>& hf,
      const std::map<std::string, std::string>& dictionary,
      const std::map<std::string, std::vector<std::string>>& ldictionary,
      string input) {
    return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary,
                                                  ldictionary, input);
  }
};

}  // namespace optframe

#endif /* TEST_LOCAL_SEARCH_MODULE_HPP_ */
