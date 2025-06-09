

import std;
import optframe.core;
import optframe.heuristics;
import optframe.scannerpp;
import optframe.hyper;

using namespace optframe;

using MyESolution = std::pair<std::vector<int>, Evaluation<int>>;

int main() {
  A<int> a;
  a.f(10);

  sptr<Move<MyESolution>> m;
  sptr<NS<MyESolution>> ns;
  sptr<NSSeq<MyESolution>> nsseq;
  optframe::Timer t;
  sptr<Evaluator<std::vector<int>, Evaluation<int>, MyESolution>> ev;
  sptr<LocalSearch<MyESolution>> ls;
  sptr<BestImprovement<MyESolution>> bi;
  sptr<ILSL<MyESolution>> ils;
  sptr<BRKGA<MyESolution>> brkga;
  scannerpp::Scanner s{"10"};
  std::println("int -> {}", *s.nextInt());

  HeuristicFactory<MyESolution>* hf;
  std::print("now {}", t.now());
  return 0;
}
