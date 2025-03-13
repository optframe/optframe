

import std;
import optframe.core;

using MyESolution = std::pair<std::vector<int>, optframe::Evaluation<int>>;

int main() {
  A<int> a;
  a.f(10);

  sptr<optframe::Move<MyESolution>> m;
  sptr<optframe::NS<MyESolution>> ns;
  sptr<optframe::NSSeq<MyESolution>> nsseq;
  optframe::Timer t;
  sptr<optframe::Evaluator<std::vector<int>, optframe::Evaluation<int>,
                           MyESolution>>
      ev;
  std::print("now {}", t.now());
  return 0;
}