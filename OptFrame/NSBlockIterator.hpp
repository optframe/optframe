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

#ifndef OPTFRAME_NSBLOCKITERATOR_HPP_
#define OPTFRAME_NSBLOCKITERATOR_HPP_

#include "Component.hpp"
#include "NSIterator.hpp"

using namespace std;

namespace optframe
{

// NSBlockIterator: iterates from blocks (parts) of the neighborhood structure
// these parts may share any characteristic that may help predicting the behavior of local optima

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class NSBlockIterator: public Component
{
public:
    virtual ~NSBlockIterator()
    {
    }

    virtual void first() = 0;
    virtual void next() = 0;
    virtual bool isDone() = 0;
    virtual NSIterator<R, ADS>& current() = 0;

    static string idComponent()
    {
        stringstream ss;
        ss << Component::idComponent() << ":NSBlockIterator";
        return ss.str();
    }

    virtual string id() const
    {
        return idComponent();
    }
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class DefaultNSBlockIterator: public NSBlockIterator<R, ADS>
{
protected:
    NSIterator<R, ADS>* it;

public:

    DefaultNSBlockIterator(NSIterator<R, ADS>& _it) :
            it(&_it)
    {
    }

    virtual ~DefaultNSBlockIterator()
    {
    }

    void first()
    {
    }

    void next()
    {
        it = nullptr;
    }

    bool isDone()
    {
        return it == nullptr;
    }

    virtual NSIterator<R, ADS>& current()
    {
        return *it;
    }

    static string idComponent()
    {
        stringstream ss;
        ss << Component::idComponent() << ":NSBlockIterator";
        return ss.str();
    }

    virtual string id() const
    {
        return idComponent();
    }
};

}

#endif //OPTFRAME_NSBLOCKITERATOR_HPP_
