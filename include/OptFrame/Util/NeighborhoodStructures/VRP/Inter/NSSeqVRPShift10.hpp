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

#ifndef NSSeqVRPShift10_HPP_
#define NSSeqVRPShift10_HPP_

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

// template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
// class ADS, XBaseSolution<vector<vector<T> >,ADS> S, class MOVE =
// MoveTSP2Opt<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR
// = NSIteratorTSP2Opt<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>
template <class T, class ADS = OPTFRAME_DEFAULT_ADS,
          XBaseSolution<vector<vector<T>>, ADS> S =
              CopySolution<vector<vector<T>>, ADS>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class MoveVRPShift10 : public Move<XES, XEv>  // Move<vector<vector<T> > , ADS>
{
  using Routes = vector<vector<T>>;

 protected:
  int r1, r2;  // routes id's
  int cli;     // cli shifted in r1
  int pos;     // insertion position in r2

  OPTFRAME_DEFAULT_PROBLEM* problem;

 public:
  MoveVRPShift10(int _r1, int _r2, int _cli, int _pos,
                 OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr)
      : r1(_r1), r2(_r2), cli(_cli), pos(_pos), problem(_problem) {}

  virtual ~MoveVRPShift10() {}

  int get_r1() { return r1; }

  int get_r2() { return r2; }

  int get_cli() { return cli; }

  int get_pos() { return pos; }

  virtual bool canBeApplied(const XES& se) override {
    const Routes& rep = se.first.getR();
    bool numRoutes = rep.size() >= 2;
    return ((r1 >= 0) && (r2 >= 0) && (cli >= 0) && (pos >= 0) && numRoutes);
  }

  virtual void updateNeighStatus(ADS& ads) {}

  virtual uptr<Move<XES>> apply(XES& se) override {
    Routes& rep = se.first.getR();
    // pegando o cliente
    int c = rep.at(r1).at(cli);

    // removendo os clientes de cada rota
    rep.at(r1).erase(rep.at(r1).begin() + cli);

    // fazendo a inserção
    rep.at(r2).insert(rep.at(r2).begin() + pos, c);
    return uptr<Move<XES>>(new MoveVRPShift10(r2, r1, pos, cli));
  }

  bool operator==(const Move<XES>& _m) const override {
    const MoveVRPShift10& m = (const MoveVRPShift10&)_m;
    return ((r1 == m.r1) && (r2 == m.r2) && (cli == m.cli) && (pos == m.pos));
  }

  void print() const override {
    cout << "MoveVRPShift10( ";
    cout << r1 << " , ";
    cout << r2 << " , ";
    cout << cli << " , ";
    cout << pos << " )";
  }
};

// template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE =
// MoveVRPShift10<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM>
template <class T, class ADS, XBaseSolution<vector<vector<T>>, ADS> S,
          class MOVE = MoveVRPShift10<T, ADS, S>,
          class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>>
class NSIteratorVRPShift10
    : public NSIterator<XES, XEv>  // NSIterator<vector<vector<T> > , ADS>
{
  typedef vector<vector<T>> Routes;

 protected:
  uptr<Move<XES>> m;
  vector<uptr<Move<XES>>> moves;
  int index;  // index of moves
  const Routes& r;

  P* p;  // has to be the last

 public:
  NSIteratorVRPShift10(const Routes& _r, const ADS& _ads, P* _p = nullptr)
      : r(_r), p(_p) {
    m = nullptr;
    index = 0;
  }

  virtual ~NSIteratorVRPShift10() {
    /*if (moves.size() > 0) //todo see this with Igor
               {
               for (unsigned int i = 0; i < moves.size(); i++)
               delete moves[i];
               moves.clear();
               }*/
  }

  virtual void first() override {
    for (int r1 = 0; r1 < r.size(); r1++) {
      for (int r2 = 0; r2 < r.size(); r2++) {
        if (r1 != r2) {
          for (int cli = 0; cli < r.at(r1).size(); cli++) {
            for (int pos = 0; pos <= r.at(r2).size(); pos++) {
              moves.push_back(uptr<Move<XES>>(new MOVE(r1, r2, cli, pos, p)));
            }
          }
        }
      }
    }

    if (moves.size() > 0) {
      m = std::move(
          moves[index]);  // stealing from vector... verify if this is correct!
                          // otherwise, must need clone() on Move
    } else
      m = nullptr;
  }

  virtual void next() override {
    index++;
    if (index < moves.size()) {
      m = std::move(
          moves[index]);  // stealing from vector... verify if this is correct!
                          // otherwise, must need clone() on Move
    } else
      m = nullptr;
  }

  virtual bool isDone() override { return m == nullptr; }

  virtual uptr<Move<XES>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "NSSeqVRPShift10. Aborting." << endl;
      exit(1);
    }

    uptr<Move<XES>> m2 = std::move(m);
    return m2;
  }
};

// template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE =
// MoveVRPShift10<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR
// = NSIteratorVRPShift10<T, ADS, MOVE, P> >
template <class T, class ADS, XBaseSolution<vector<vector<T>>, ADS> S,
          class MOVE = MoveVRPShift10<T, ADS, S>,
          class P = OPTFRAME_DEFAULT_PROBLEM,
          class NSITERATOR = NSIteratorVRPShift10<T, ADS, S, MOVE, P>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>,
          XSearch<XES> XSH = std::pair<S, XEv>>
class NSSeqVRPShift10 : public NSSeq<XES, XEv, XSH> {
  typedef vector<vector<T>> Routes;

 private:
  P* p;  // has to be the last

 public:
  NSSeqVRPShift10(P* _p = nullptr) : p(_p) {}

  virtual ~NSSeqVRPShift10() {}

  virtual uptr<Move<XES>> randomMove(const XES& se) override {
    const Routes& rep = se.first.getR();
    if (rep.size() < 2) return uptr<Move<XES>>(new MOVE(-1, -1, -1, -1, p));

    int r1 = rand() % rep.size();
    if (rep.at(r1).size() == 0) {
      return uptr<Move<XES>>(new MOVE(-1, -1, -1, -1, p));
    }

    int r2;
    do {
      r2 = rand() % rep.size();
    } while (r1 == r2);

    int cli = rand() % rep.at(r1).size();

    int pos = rand() % (rep.at(r2).size() + 1);
    return uptr<Move<XES>>(
        new MOVE(r1, r2, cli, pos, p));  // return a random move
  }

  /*
       virtual uptr<Move<XES>> validRandomMove(const XES& s) override
       {
     const Routes& rep = s.getR();
               int maxValidMove = 50;
               for (int iter = 0; iter < maxValidMove; iter++)
               {
                       uptr<Move<XES>> moveValid = this->randomMove(s);
                       if (moveValid->canBeApplied(s))
                               return moveValid;
                       //else
                       //	delete moveValid;
               }

               return nullptr;
       }
*/

  virtual uptr<NSIterator<XES>> getIterator(const XES& se) override {
    XSolution AUTO_CONCEPTS& s = se.first;
    return uptr<NSIterator<XES>>(new NSITERATOR(s.getR(), s.getADS(), p));
  }

  std::string toString() const override {
    stringstream ss;
    ss << "NSSeqVRPShift10 with move: " << MOVE::idComponent();
    return ss.str();
  }

  void print() const override {
    cout << "NSSeqVRPShift10 with move: " << MOVE::idComponent();
  }
};

#endif /*NSSeqVRPShift10_HPP_*/
