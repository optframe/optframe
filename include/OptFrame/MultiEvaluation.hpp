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

#ifndef OPTFRAME_MULTI_EVALUATION_HPP_
#define OPTFRAME_MULTI_EVALUATION_HPP_

#include "Evaluation.hpp"

using namespace std;

namespace optframe {

// TODO: pass ObjType here
// TODO: convert MultiEvaluation<> to XEvaluation model too
template<optframe::objval ObjType = evtype>
class MultiEvaluation : public Component
{
protected:
   vector<Evaluation<>> vev; // TODO: pass ObjType here

public:
   bool outdated{ true };

public:
   MultiEvaluation()
   {
      static_assert(XEvaluation<MultiEvaluation<ObjType>>);
   }

   MultiEvaluation(Evaluation<> ev)
   {
      vev.push_back(ev);
      static_assert(XEvaluation<MultiEvaluation<ObjType>>);
   }

   //	MultiEvaluation(const Evaluation<> ev)
   //	{
   //		vev.push_back(&ev.clone());
   //	}

   MultiEvaluation(const vector<double>& vd)
   {
      for (unsigned i = 0; i < vd.size(); i++)
         vev.push_back(Evaluation(vd[i]));
   }

   //	MultiEvaluation(const vector<Evaluation<>*>& _vev)
   //	{
   //		for (unsigned i = 0; i < _vev.size(); i++)
   //			vev.push_back(&_vev[i]->clone());
   //	}

   //	MultiEvaluation(MultiEvaluation<>&& mev)
   //	{
   //		cout<<"finally here..."<<endl;
   //		for (unsigned i = 0; i < mev.vev.size(); i++)
   //			vev.push_back(std::move(mev.vev[i]));
   //	}

   MultiEvaluation(const MultiEvaluation<>& mev)
   {
      for (unsigned i = 0; i < mev.vev.size(); i++)
         vev.push_back(mev.vev[i]);
   }

   MultiEvaluation(MultiEvaluation<>&& mev)
     : vev(std::move(mev.vev))
   {
   }

   virtual ~MultiEvaluation()
   {
      this->clear();
   }

   void print()
   {
      for (unsigned i = 0; i < vev.size(); i++)
         vev[i].print();
   }

   //	void addEvaluation(const Evaluation<> ev)
   //	{
   //		vev.push_back(ev.clone());
   //	}

   void addEvaluation(Evaluation<>& ev)
   {
      vev.push_back(ev);
   }

   void addEvaluation(Evaluation<>&& ev)
   {
      vev.push_back(std::move(ev));
   }

   unsigned size() const
   {
      return vev.size();
   }

   void erase(unsigned index)
   {
      vev.erase(vev.begin() + index);
   }

   Evaluation<>& at(unsigned index)
   {
      return vev[index];
   }

   const Evaluation<>& at(unsigned index) const
   {
      return vev[index];
   }

   evtype atObjVal(unsigned index)
   {
      return vev[index].evaluation();
   }

   // TODO: test!!
   MultiEvaluation<> diff(const MultiEvaluation<>& other)
   {
      MultiEvaluation<> r = *this;
      for (unsigned i = 0; i < r.size(); i++)
         r.at(i) = r.at(i).diff(other.at(i));
      return r;
   }

   Evaluation<>& operator[](unsigned index)
   {
      return vev[index];
   }

   const Evaluation<>& operator[](unsigned index) const
   {
      return vev[index];
   }

   void setOutdated(unsigned index, bool status)
   {
      vev[index].outdated = status;
   }

   //	const vector<Evaluation<>*>& getVector() const
   //	{
   //		return vev;
   //	}

   //	vector<Evaluation<>*> getCloneVector() const
   //	{
   //		vector<Evaluation<>*> v_e;
   //		for (unsigned i = 0; i < vev.size(); i++)
   //			v_e.push_back(&vev[i]->clone());
   //		return v_e;
   //	}

   //	bool sameValues(const MultiEvaluation<>& mev)
   //	{
   //		if (vev.size() != mev.vev.size())
   //			return false;
   //
   //		for (unsigned i = 0; i < vev.size(); i++)
   //			if (vev[i]->evaluation() != mev.vev[i]->evaluation())
   //				return false;
   //		return true;
   //	}

   virtual MultiEvaluation<>& operator=(const MultiEvaluation<>& mev)
   {
      if (&mev == this) // auto ref check
         return *this;

      this->vev.clear();
      for (unsigned i = 0; i < mev.vev.size(); i++)
         this->vev.push_back(mev.vev[i]);

      return *this;
   }

   virtual MultiEvaluation<>& clone() const
   {
      return *new MultiEvaluation(*this);
   }

   //	void clearNoKill()
   //	{
   //		this->vev.clear();
   //	}

   void clear()
   {
      this->vev.clear();
   }

   // update Evaluation with costs
   virtual void update(MultiEvaluation<ObjType>& mevTarget) const
   {
      // this task was performed before by MoveCost... now unifying in Evaluation
      // how to do this?
      assert(false);
   }

   virtual void print() const override
   {
      cout << toString() << endl;
   }

   virtual string toString() const override
   {
      stringstream ss;
      ss << "MultiEvaluation<> (" << vev.size() << "):";
      for (unsigned i = 0; i < vev.size(); i++)
         ss << vev[i].toString() << endl;
      return ss.str();
   }

   friend std::ostream& operator<<(std::ostream& os, const MultiEvaluation& me)
   {
      os << me.toString();
      return os;
   }
};

// Compilation test for concepts
// MultiEvaluation is considered an XEvaluation type (compatible with past implementations)
// TODO: remove this
static_assert(XEvaluation<MultiEvaluation<>>);

#ifndef NDEBUG
struct optframe_debug_test_multievaluation
{
   MultiEvaluation<> testEvaluation;
};
#endif

} // namespace optframe

#endif /*OPTFRAME_MULTI_EVALUATION_HPP_*/
