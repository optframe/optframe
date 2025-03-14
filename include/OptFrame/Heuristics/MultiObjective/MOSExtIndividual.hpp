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

#ifndef OPTFRAME_MOS_EXTENDED_INDIVIDUAL_HPP_
#define OPTFRAME_MOS_EXTENDED_INDIVIDUAL_HPP_

#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NSSeq.hpp>
#include <algorithm>

#include "../../Evaluation.hpp"
#include "../../Helper/Solution.hpp"
#include "../../ParetoDominance.hpp"
#include "../../Population.hpp"
#include "../../Search/MultiObjSearch.hpp"
#include "MOSIndividual.hpp"

namespace optframe {

// MultiObjSearch Extended Individual
template <class R, class X, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class MOSExtIndividual : public MOSIndividual<X, ADS, DS> {
 public:
  MOSIndividual<R>& parent;

  MOSExtIndividual(Solution<X, ADS>* s, MultiEvaluation<DS>* mev,
                   MOSIndividual<R>* _parent)
      : MOSIndividual<X, ADS, DS>(s, mev), parent(*_parent) {}

  MOSExtIndividual(Solution<R, ADS>& s, MultiEvaluation<DS>& mev,
                   MOSIndividual<R>* _parent)
      : MOSIndividual<X, ADS, DS>(s, mev), parent(*_parent) {}

  MOSExtIndividual(const MOSExtIndividual<R, X, ADS, DS>& ind)
      : MOSIndividual<X, ADS, DS>(&ind.s->clone(), &ind.mev->clone()),
        parent(ind.parent) {
    this->fitness = ind.fitness;
    this->diversity = ind.diversity;

    this->id = ind.id;
  }

  virtual ~MOSExtIndividual() {}

  void print() const override {
    std::cout << "MOSExtIndividual: parent=" << &parent
              << " fitness=" << this->fitness
              << "\t diversity=" << this->diversity;
    std::cout << "\t[ ";
    for (unsigned e = 0; e < this->mev->size(); e++)
      std::cout << this->mev->at(e).evaluation()
                << (e == this->mev->size() - 1 ? " " : " ; ");
    std::cout << " ]";
    std::cout << std::endl;
  }

  virtual MOSIndividual<X, ADS, DS>& clone() const {
    return *new MOSExtIndividual<R, X, ADS, DS>(*this);
  }
};

/*
template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS =
OPTFRAME_DEFAULT_DS> class MOSExtPopulation: public MOSPopulation<R>
{
public:

        vector<MOSExtIndividual<R, X>*> PX;

        MOSExtPopulation() :
                        MOSPopulation<R>()
        {
        }

        MOSExtPopulation(const vector<MOSIndividual<R>*>& _PS, const
vector<MOSExtIndividual<R, ADS, DS>*>& _PX) : MOSPopulation<R>(_PS), PX(_PX)
        {
        }

        virtual ~MOSExtPopulation()
        {
        }

        inline void setVector(std::vector<MOSIndividual<R>*>& v)
        {
                this->P = v;
        }

        inline void setVector(std::vector<MOSExtIndividual<R, X>*>& v)
        {
                PX = v;
        }

        // reconfigure GET methods for 'PX'
        inline vector<MOSIndividual<R>*>& getVector()
        {
                return getXVector();
        }

        inline vector<MOSIndividual<R>*>& getSVector()
        {
                return this->P;
        }

        inline vector<MOSExtIndividual<R, X>*>& getXVector()
        {
                return PX;
        }

        // reconfigure GET methods for 'PX'
        inline vector<MOSIndividual<R>*> getVector() const
        {
                return getXVector();
        }

        inline vector<MOSIndividual<R>*> getSVector() const
        {
                return this->P;
        }

        inline vector<MOSExtIndividual<R, X, ADS, DS>*> getXVector() const
        {
                return PX;
        }

        // reconfigure GET methods for 'PX'
        inline MOSIndividual<R>* at(unsigned id) const
        {
                return PX[id];
        }

        inline MOSIndividual<R>* atS(unsigned id) const
        {
                return this->P[id];
        }

        inline MOSExtIndividual<R, X, ADS, DS>* atX(unsigned id) const
        {
                return PX[id];
        }

        // reconfigure GET methods for 'PX'
        inline unsigned size() const
        {
                return PX.size();
        }

        inline unsigned sizeS() const
        {
                return this->P.size();
        }

        inline unsigned sizeX() const
        {
                return PX.size();
        }

        inline void add(MOSIndividual<R>* ind)
        {
                this->P.push_back(ind);
        }

        inline void add(MOSExtIndividual<R, X, ADS, DS>* ind)
        {
                PX.push_back(ind);
        }

        virtual void add(MOSPopulation<R>& Pop)
        {
                this->P.insert(this->P.end(), Pop.P.begin(), Pop.P.end());
        }

        virtual void add(MOSExtPopulation<R, X, ADS, DS>& Pop)
        {
                this->P.insert(this->P.end(), Pop.P.begin(), Pop.P.end());
                PX.insert(PX.end(), Pop.PX.begin(), Pop.PX.end());
        }

        inline void add(std::vector<MOSIndividual<R>*>& v)
        {
                this->P.insert(this->P.end(), v.begin(), v.end());
        }

        inline void add(std::vector<MOSExtIndividual<R, X, ADS, DS>*>& v)
        {
                PX.insert(PX.end(), v.begin(), v.end());
        }

        // reconfigure for both
        inline void clear()
        {
                PX.clear();
                this->P.clear();
        }

        // reconfigure for both
        virtual void free()
        {
                for(unsigned i = 0; i < this->P.size(); i++)
                        if(this->P[i])
                                delete this->P[i];
                this->P.clear();

                for(unsigned i = 0; i < PX.size(); i++)
                        if(PX[i])
                                delete PX[i];
                PX.clear();
        }
};
*/

}  // namespace optframe

#endif /*OPTFRAME_MOS_EXTENDED_INDIVIDUAL_HPP_*/
