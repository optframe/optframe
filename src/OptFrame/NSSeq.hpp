// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_NSSEQ_HPP_
#define OPTFRAME_NSSEQ_HPP_

#include "NS.hpp"
#include "NSIterator.hpp"
#include "NSBlockIterator.hpp"

#include <type_traits> // static assert is_same

using namespace std;

namespace optframe
{

template<XESolution XES, XEvaluation XEv = typename XES::second_type, XESolution XSH = XES>
class NSSeq: public NS<XES, XEv, XSH>
{
public:
/*
   NSSeq()
   {
      static_assert(std::is_same<XEv, decltype(declval<XES>.second)>, "XEv type test over XES");
   }
*/
    virtual ~NSSeq()
    {
    }

    virtual uptr<Move<XES, XEv, XSH>> randomMove(const XES&) = 0;

    virtual uptr<NSIterator<XES, XEv, XSH>> getIterator(const XES&) = 0;

    // experimental! Should create novel NSSeqBlock perhaps
    virtual uptr<NSBlockIterator<XES, XEv>> getBlockIterator(const XES& s)
    {
        uptr<NSIterator<XES, XEv>> it = this->getIterator(s);
        return uptr<NSBlockIterator<XES, XEv>>(new DefaultNSBlockIterator<XES, XEv>(*it));
    }

    // ============= For 'Local Optimum'-based methods =============

    // GET LOCAL OPTIMUM INFORMATION FROM SOLUTION (ADS PREFERED?)
    virtual LOS getLOS(const XES& s)
    {
        return los_unknown;
    }

    // INSERT LOCAL OPTIMUM INFORMATION IN SOLUTION (IN ADS? USER DECIDES.)
    // MAYBE IT's BETTER TO USE ONLY IN ITERATORS! TODO: THINK ABOUT IT...
    //virtual void setLOS(LOS status, S& s)
    //{
    //}

public:
    static string idComponent()
    {
        stringstream ss;
        ss << NS<XES, XEv>::idComponent() << ":NSSeq";
        return ss.str();
    }

    virtual string id() const
    {
        return idComponent();
    }

    virtual bool compatible(string s)
    {
        return (s == idComponent()) || (NS<XES, XEv>::compatible(s));
    }
};

// General test for NSSeq
//using nsseq_test_base = NSSeq<pair<IsSolution<vector<int>, short>, IsEvaluation<double>>, IsEvaluation<double>>;
using nsseq_test_base = NSSeq<pair<IsSolution<vector<int>, short> , Evaluation<> >>;


} // using namespace optframe

#endif /*OPTFRAME_NSSEQ_HPP_*/
