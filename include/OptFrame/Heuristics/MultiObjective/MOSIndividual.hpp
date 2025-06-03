// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_MULTIOBJECTIVE_MOSINDIVIDUAL_HPP_
#define OPTFRAME_HEURISTICS_MULTIOBJECTIVE_MOSINDIVIDUAL_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C++
#include <algorithm>
#include <string>
#include <vector>
//
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Helper/Population.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Pareto/MultiEvaluation.hpp>
#include <OptFrame/Pareto/ParetoDominance.hpp>
#include <OptFrame/Search/MultiObjSearch.hpp>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

// MOSIndividual should be a XESolution = (S, XMEv)

// This is typically part of a Secondary Type (not primary)

// MultiObjSearch Individual
// template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS =
// OPTFRAME_DEFAULT_DS>

MOD_EXPORT template <XESolution XMES2>
class MOSIndividual {
  using S = typename XMES2::first_type;
  using XMEv = typename XMES2::second_type;

  // ======================================================
  // WARNING: I THINK we cannot put XESolution checks here,
  // until class is complete... terrible!
  // using XMSH2 = VEPopulation<MOSIndividual<XMES2>>;
  // XSearch<XMES2> XMSH2 = VEPopulation<MOSIndividual<XMES2>>;
  //
  template <class XES>
  using MyVEPopulation = std::vector<XES>;

  // static_assert(XSearch<XMSH2, XMES2>);

 public:
  using first_type = S;
  using second_type = XMEv;
  //
  S first;      // s
  XMEv second;  // mev  // temporarily kept without evaluation
  //

  double fitness;
  double diversity;
  int id;  // each element should have an unique identifier inside a population

  // individuals with same evaluation values (used for compression of
  // population) vector<MOSIndividual<XMES2>*> copies;
  //
  // WARNING: I THINK we cannot put XESolution checks here,
  // until class is complete... terrible!
  //
  // VEPopulation<MOSIndividual<XMES2>> copies;

  MyVEPopulation<MOSIndividual<XMES2>> copies;

  MOSIndividual(const S& _s, const XMEv& _mev) : first{_s}, second{_mev} {
    fitness = -1;
    diversity = -1;

    id = -1;
  }

  explicit MOSIndividual(const XMES2& se) : first{se.first}, second{se.second} {
    fitness = -1;
    diversity = -1;

    id = -1;
  }

  MOSIndividual(const MOSIndividual<XMES2>& ind)
      : first{ind.first}, second{ind.second} {
    fitness = ind.fitness;
    diversity = ind.diversity;

    id = ind.id;
  }

  MOSIndividual(MOSIndividual<XMES2>&& ind)
      : first{ind.first}, second{ind.second} {
    fitness = ind.fitness;
    diversity = ind.diversity;

    id = ind.id;
  }

  MOSIndividual<XMES2>& operator=(const MOSIndividual<XMES2>& other) {
    if (this == &other) return *this;

    this->first = other.first;
    this->second = other.second;
    this->fitness = other.fitness;
    this->diversity = other.diversity;
    this->id = other.id;
    return *this;
  }

  virtual ~MOSIndividual() {
    /*
    if (s)
      delete s;
    if (mev)
      delete mev;
      */
  }

  virtual bool betterThan(const MOSIndividual<XMES2>& ind) const {
    // assuming minimization of fitness and maximization of diversity
    if (fitness < ind.fitness) return true;
    if ((fitness == ind.fitness)) return diversity > ind.diversity;
    return false;
  }

  virtual bool betterFitness(const MOSIndividual<XMES2>& ind) const {
    // assuming minimization of fitness
    return fitness < ind.fitness;
  }

  virtual bool betterDiversity(const MOSIndividual<XMES2>& ind) const {
    // assuming maximization of diversity
    return diversity > ind.diversity;
  }

  // assuming minimization of fitness
  virtual bool minFitness() const { return true; }

  // assuming maximization of diversity
  virtual bool maxDiversity() const { return true; }

  friend std::ostream& operator<<(std::ostream& os,
                                  const MOSIndividual<XMES2>& ind) {
    os << ind.toString();
    return os;
  }

  virtual void print() const { std::cout << toString() << std::endl; }

  virtual std::string toString() const {
    std::stringstream ss;
    ss << "MOSIndividual #" << id << " fitness=" << fitness;
    ss << "\t diversity=" << diversity;
    ss << "\t[ ";
    for (unsigned e = 0; e < second.size(); e++) {
      ss << second.at(e).evaluation();
      ss << (e == second.size() - 1 ? " " : " ; ");
    }
    ss << " ]";
    return ss.str();
  }

  /*
  virtual MOSIndividual<XMES2>& clone() const {
    return *new MOSIndividual<XMES2>(*this);
  }
*/

  static void compress(std::vector<MOSIndividual<XMES2>*>& P) {
    for (int s = 0; s < ((int)P.size()) - 1; s++)
      for (int j = s + 1; j < ((int)P.size()); j++)
        if (P[s]->mev->sameValues(*P[j]->mev)) {
          P[s]->copies.push_back(P[j]);
          P.erase(P.begin() + j);
          j--;
        }
  }

  static void spreadToCopies(std::vector<MOSIndividual<XMES2>*>& P) {
    for (unsigned s = 0; s < P.size(); s++)
      for (int j = 0; j < P[s]->copies.size(); j++) {
        P[s]->copies[j]->fitness = P[s]->fitness;
        P[s]->copies[j]->distance = P[s]->distance;
      }
  }
};

/*
template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS =
OPTFRAME_DEFAULT_DS> class MOSPopulation
{
protected:
        vector<MOSIndividual<XMES2>*> P;

public:
        MOSPopulation()
        {
        }

        MOSPopulation(MOSIndividual<XMES2>* ind)
        {
                P.push_back(ind);
        }

        MOSPopulation(const vector<MOSIndividual<XMES2>*>& _P) :
                        P(_P)
        {
        }

        virtual ~MOSPopulation()
        {
        }

        virtual void setVector(std::vector<MOSIndividual<XMES2>*>& v)
        {
                P = v;
        }

        virtual vector<MOSIndividual<XMES2>*>& getVector()
        {
                return P;
        }

        virtual vector<MOSIndividual<XMES2>*> getVector() const
        {
                return P;
        }

        virtual MOSIndividual<XMES2>* at(unsigned id) const
        {
                return P[id];
        }

        virtual unsigned size() const
        {
                return P.size();
        }

        virtual void add(MOSIndividual<XMES2>* ind)
        {
                P.push_back(ind);
        }

        virtual void add(MOSPopulation<R, ADS, DS>& Pop)
        {
                P.insert(P.end(), Pop.P.begin(), Pop.P.end());
        }

        virtual void add(std::vector<MOSIndividual<XMES2>*>& v)
        {
                P.insert(P.end(), v.begin(), v.end());
        }

        virtual void clear()
        {
                P.clear();
        }

        virtual void free()
        {
                for(unsigned i = 0; i < P.size(); i++)
                        if(P[i])
                                delete P[i];
                P.clear();
        }
};
*/

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS

using TestIsMultiEv = MultiEvaluation<double>;
using TestIsSolutionMultiEv = std::vector<int>;

using TestXMES_MOS_Ind = std::pair<TestIsSolutionMultiEv, TestIsMultiEv>;

static_assert(XESolution<MOSIndividual<TestXMES_MOS_Ind>>);

#endif  // cpp_concepts

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_MULTIOBJECTIVE_MOSINDIVIDUAL_HPP_
