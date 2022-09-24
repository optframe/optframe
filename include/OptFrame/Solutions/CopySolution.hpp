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

#ifndef OPTFRAME_COPY_SOLUTION_HPP_
#define OPTFRAME_COPY_SOLUTION_HPP_

#include <cstdlib>
#include <iostream>

// for debug purposes
#include <assert.h>

// basic elements of an OptFrame Component
#include "../Component.hpp"

#include "../BaseConcepts.hpp"
//#include "../Util/printable.h"

// will require 'R', thus should require basic printability here

// copy-based solution (NO POINTER IS USED HERE)
// requires copy-constructor and move-constructor (for R and ADS)
// ADS requires empty constructor (but not R)

namespace optframe {

//template<XRepresentation R, class ADS = nullptr_t>
template<XRepresentation R, class ADS = _ADS>
class CopySolution : public Component
{
public:
   // allow direct access to internals (no issue)
   R r;     // representation
   ADS ads; // auxiliary data structure

   using typeR = R;
   using typeADS = ADS;

   // copy constructor for R (ADS requires empty constructor)
   explicit CopySolution(const R& _r)
     : r(_r)
     , ads(ADS())
   {
   }

   // copy constructor for R and ADS
   CopySolution(const R& _r, const ADS& _ads)
     : r(_r)
     , ads(_ads)
   {
   }

   // move constructor for R and ADS
   CopySolution(R&& _r, ADS&& _ads)
     : r(_r)
     , ads(_ads)
   {
   }

   // copy constructor
   CopySolution(const CopySolution<R, ADS>& s)
     : CopySolution(s.r, s.ads)
   {
#ifdef OPTFRAME_AC
      // copy listAC directly
      this->listAC = s.listAC;
#endif
   }

   // move constructor
   CopySolution(CopySolution<R, ADS>&& s)
     : CopySolution(s.r, s.ads)
   {
#ifdef OPTFRAME_AC
      // copy listAC directly
      this->listAC = s.listAC;
#endif
   }

   CopySolution<R, ADS>& operator=(const CopySolution<R, ADS>& s)
   {
      if (&s == this) // auto ref check
         return *this;

      r = s.r;
      ads = s.ads;

#ifdef OPTFRAME_AC
      // copy listAC directly
      this->listAC = s.listAC;
#endif

      return *this;
   }

   CopySolution<R, ADS>& operator=(CopySolution<R, ADS>&& s) noexcept
   {
      r = std::move(s.r);
      ads = std::move(s.ads);

#ifdef OPTFRAME_AC
      // copy listAC directly
      this->listAC = s.listAC;
#endif

      return *this;
   }

   // -----------------------------------
   // getters and setters for R and ADS
   // according to XBaseSolution concept
   // -----------------------------------

   R& getR()
   {
      return r;
   }

   const R& getR() const
   {
      return r;
   }

   ADS& getADS()
   {
      return ads;
   }

   const ADS& getADS() const
   {
      return ads;
   }

   ADS* getADSptr()
   {
      return &ads;
   }

   const ADS* getADSptr() const
   {
      return &ads;
   }

   // ==================================================

   // destructor for Solution (must free R and ADS objects)
   virtual ~CopySolution()
   {
   }

   // ==================
   // end canonical part
   // ==================

   // shouldn't clone here!

#ifdef OPTFRAME_AC
   std::shared_ptr<Component> sharedClone() const override
   {
      std::shared_ptr<Component> sptr{ &clone() };
      sptr->listAC = this->listAC;
      return sptr;
   }
#endif

   // TODO: remove!!!
   CopySolution<R, ADS>& clone() const
   {
      return *new CopySolution<R, ADS>(*this);
   }

   // =================
   // begin Object part
   // =================

   static string idComponent()
   {
      std::stringstream ss;
      ss << Component::idComponent() << ":CopySolution";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }

   virtual string toString() const
   {
      std::stringstream ss;
      ss << "CopySolution: " << r;
      //ss << "ADS: " << ads;DecoderRandomKeys
      return ss.str();
   }

   virtual bool toStream(std::ostream& os) const
   {
      /*
      if (&os == &optframe::cjson) {
         assert(false);
      }
      assert(false);
      return false;
      */
      // forward to operator<<
      os << (*this);
      return true;
   }

   friend std::ostream& operator<<(std::ostream& os, const CopySolution<R, ADS>& s)
   {
      //os << s.toString();
      if (&os == &optframe::cjson) {
         os << s.r; // invoke json print from 'r'
      } else
         os << s.toString();
      return os;
   }
};

template<XRepresentation R, class ADS = _ADS, XBaseSolution<R, ADS> S = CopySolution<R, ADS>>
struct _Testing
{
   S s;
};

}

#endif /* OPTFRAME_COPY_SOLUTION_HPP_ */
