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

#ifndef OPTFRAME_MOVENSSEQUNION_HPP_
#define OPTFRAME_MOVENSSEQUNION_HPP_

// Framework includes
#include "../../../Move.hpp"

using namespace std;

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>>
class MoveNSSeqUnion : public Move<XES, XEv> {
 protected:
  int id;
  uptr<Move<XES, XEv>> m{nullptr};

 public:
  /*
       MoveNSSeqUnion(int _id, uptr<Move<XES, XEv>>&& _m) :
                       id(_id), m{std::move(_m)}
       {
       }
*/

  /*
  // STRANGE ERROR!! CANNOT RECEIVE uptr...
       MoveNSSeqUnion(int _id, uptr<Move<XES, XEv>> _m) :
                       //id(_id), m(std::move(_m))
        id(_id), m(_m)
       {
       }
*/

  // this pointer will be owned by ME... never delete it!
  MoveNSSeqUnion(int _id, Move<XES, XEv>* _m)
      :  // id(_id), m(std::move(_m))
        id(_id),
        m(_m) {}

  int get_id() { return id; }

  uptr<Move<XES, XEv>>& get_m() { return m; }

  virtual ~MoveNSSeqUnion() {
    // delete &m;
  }

  bool canBeApplied(const XES& s) override { return m->canBeApplied(s); }

  uptr<Move<XES, XEv>> apply(XES& se) override {
    return uptr<Move<XES, XEv>>(
        new MoveNSSeqUnion<S, XEv, XES>(id, m->apply(se).release()));
  }

  uptr<Move<XES, XEv>> applyUpdate(Evaluation<>& e, S& s) {
    XES se(s, e);
    return uptr<Move<XES, XEv>>(
        new MoveNSSeqUnion<S, XEv, XES>(id, m->apply(se).release()));
  }

  bool operator==(const Move<XES, XEv>& _m) const override {
    const MoveNSSeqUnion<S, XEv, XES>& m1 =
        (const MoveNSSeqUnion<S, XEv, XES>&)_m;
    if (id == m1.id)
      return *m == *(m1.m);
    else
      return false;
  }

  void print() const override {
    cout << "MoveNSSeqUnion: id=" << id << "; move = ";
    m->print();
  }
};

#endif /*OPTFRAME_MOVENSSEQUNION_HPP_*/
