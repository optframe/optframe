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

using namespace std;

namespace optframe
{

template<XSolution S, XEvaluation XEv = Evaluation<>, XSearch<S, XEv> XSH = std::pair<S, XEv>>
class NSSeq: public NS<S, XEv, XSH>
{
public:

    virtual ~NSSeq()
    {
    }

    virtual uptr<Move<S, XEv, XSH>> randomMove(const S&) = 0;

    virtual uptr<NSIterator<S, XEv, XSH>> getIterator(const S&) = 0;

    virtual NSBlockIterator<S, XEv>* getBlockIterator(const S& s)
    {
        uptr<NSIterator<S, XEv>> it = this->getIterator(s);
        return new DefaultNSBlockIterator<S, XEv>(*it);
    }

    // ============= For 'Local Optimum'-based methods =============

    // GET LOCAL OPTIMUM INFORMATION FROM SOLUTION (ADS PREFERED?)
    virtual LOS getLOS(const S& s)
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
        ss << NS<S, XEv>::idComponent() << ":NSSeq";
        return ss.str();
    }

    virtual string id() const
    {
        return idComponent();
    }

    virtual bool compatible(string s)
    {
        return (s == idComponent()) || (NS<S, XEv>::compatible(s));
    }
};

}

#endif /*OPTFRAME_NSSEQ_HPP_*/
