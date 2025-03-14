
// C++
#include <algorithm>
#include <functional>
#include <iostream>
//
#include <OptFCore/FCore.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Heuristics/EA/RK/BRKGA.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT

// next definitions come here within namespace
// this also works when defining in global scope (same as 'class')
namespace TSP_brkga {

// define TSP solution type as 'vector<int>', using 'double' as evaluation type
using ESolutionTSP =
    std::pair<std::vector<int>,  // first part of search space element: solution
                                 // (representation)
              Evaluation<double>  // second part of search space element:
                                  // evaluation (objective value)
              >;

// TSP problem context and data reads
class ProblemContext {
 public:
  int n{0};             // number of clients
  Matrix<double> dist;  // distance matrix (Euclidean)
  // load data from Scanner
  void load(Scanner& scanner) {
    n = *scanner.nextInt();       // reads number of clients
    dist = Matrix<double>(n, n);  // initializes n x n matrix
    //
    vector<double> xvalues(n);
    vector<double> yvalues(n);
    //
    for (int i = 0; i < n; i++) {
      scanner.next();
      xvalues[i] = *scanner.nextDouble();  // reads x
      yvalues[i] = *scanner.nextDouble();  // reads y
    }
    // calculate distance values, for every client pair (i,j)
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        dist(i, j) = distance(xvalues.at(i), yvalues.at(i), xvalues.at(j),
                              yvalues.at(j));
  }
  // euclidean distance (double as return)
  double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
  }
};
// Create TSP Problem Context
// ProblemContext pTSP;

Evaluation<double> fevaluate(sref<ProblemContext> pTSP,
                             const std::vector<int>& s) {
  double f = 0;
  for (int i = 0; i < int(pTSP->n) - 1; i++) f += pTSP->dist(s[i], s[i + 1]);
  f += pTSP->dist(s[int(pTSP->n) - 1], s[0]);
  return Evaluation<double>{f};
}

// Evaluate
// FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE> ev{fevaluate};

// ===========================
// decoder function for BRKGA (and alike)

pair<Evaluation<double>, vector<int>> fDecode(
    sref<Evaluator<typename ESolutionTSP::first_type,
                   typename ESolutionTSP::second_type, ESolutionTSP>>
        eval,
    const vector<double>& rk) {
  std::vector<std::pair<double, int>> v(rk.size());
  int k = 0;
  for (unsigned i = 0; i < v.size(); i++) v[k] = pair<double, int>(rk[i], i);

  sort(v.begin(), v.end(),
       [](const pair<double, int>& i, const pair<double, int>& j) -> bool {
         return i.first < j.first;
       });

  // R = vector<int>
  vector<int> p(v.size());
  for (unsigned i = 0; i < v.size(); i++) p[i] = v[i].second;

  Evaluation<double> e = eval->evaluate(p);
  return make_pair(e, p);
}

// evaluator random keys (for TSP)
// FDecoderRK<std::pair<std::vector<int>, Evaluation<>>, double,
// MinOrMax::MINIMIZE> decoder{fDecode};

//
}  // namespace TSP_brkga

// import everything on main()
using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT
using namespace TSP_brkga;  // NOLINT

class MyRandomKeysInitEPop
    : public InitialEPopulation<
          std::pair<std::vector<double>, Evaluation<double>>> {
  using RSK = std::vector<double>;

 private:
  int sz;
  sref<RandGen> rg;

 public:
  explicit MyRandomKeysInitEPop(int size, sref<RandGen> _rg = new RandGen)
      : sz{size}, rg{_rg} {}

  // copy constructor
  // MyRandomKeysInitEPop(const MyRandomKeysInitEPop& self)
  //     : sz{self.sz}, rg{self.rg} {}

  ~MyRandomKeysInitEPop() override = default;

  VEPopulation<std::pair<RSK, Evaluation<double>>> generateEPopulation(
      unsigned populationSize, double timelimit) override {
    VEPopulation<std::pair<RSK, Evaluation<double>>> pop;

    for (unsigned i = 0; i < populationSize; i++) {
      vector<double> d(sz);
      for (int j = 0; j < sz; j++) d.at(j) = (rg->rand() % 100000) / 100000.0;
      std::pair<vector<double>, Evaluation<double>> p{d, Evaluation<>()};
      pop.push_back(p);
    }

    return pop;
  }
};  // MyRandomKeysInitPop

int main() {
  sref<RandGen> rg = new RandGen;  // avoids weird windows OS interactions

  // load data into problem context 'pTSP'
  Scanner scanner{"3\n1 10 10\n2 20 20\n3 30 30\n"};

  sref<ProblemContext> pTSP{new ProblemContext{}};
  pTSP->load(scanner);
  std::cout << pTSP->dist << std::endl;

  // Parameters BRKGA
  // (C1): Evaluator<S, XEv>& _evaluator, int key_size, unsigned numGen,
  // unsigned _popSize, double fracTOP, double fracBOT, double _probElitism) :

  sref<Evaluator<typename ESolutionTSP::first_type,
                 typename ESolutionTSP::second_type, ESolutionTSP>>
      eval{new FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE, ProblemContext>{
          pTSP, fevaluate}};

  sref<DecoderRandomKeys<ESolutionTSP, double>> _decoder{
      new FDecoderEvalRK<std::pair<std::vector<int>, Evaluation<>>, double>{
          eval, fDecode}};
  sref<InitialEPopulation<std::pair<vector<double>, ESolutionTSP::second_type>>>
      _initPop = new MyRandomKeysInitEPop(pTSP->n);  // passing key_size

  // eprk, pTSP.n, 1000, 30, 0.4, 0.3, 0.6
  BRKGA<ESolutionTSP, double> brkga(
      _decoder, MyRandomKeysInitEPop(pTSP->n, rg),  // key_size = pTSP.n
      30, 1000, 0.4, 0.3, 0.6, rg);

  auto status = brkga.search(10.0);  // 10.0 seconds max

  std::cout << "FINISHED" << std::endl;
  return 0;
}
