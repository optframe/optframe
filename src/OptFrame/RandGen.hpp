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

#ifndef OPTFRAME_RANDGEN_HPP_
#define OPTFRAME_RANDGEN_HPP_

#include <cmath>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <vector>
//#include <tgmath.h>

#include "Action.hpp"
#include "Component.hpp"
#include "ComponentBuilder.h"

#include <random> //used for Binomial distribution requested by NGES
//#include <tr1/random>

namespace optframe {

// =================
// RandGen interface
// =================

// -------------
// uniform rands
// -------------
// rand() -> int32
// rand(uint32 limit) -> [0, limit-1]  (note that rand(0) is zero, by default, no exceptions thrown)
// rand(uint32 a, uint32 b) -> [a, b]
// rand01() -> double [0.0,1.0)
// randP(double p) -> bool (pass or not pass, probability p in [0,1])

// --------------
//    gaussian
// --------------
// randG() -> double (gaussian mean 0.0 stddev 1.0)
// randG(double mean, double stdev) -> double

// --------------
//   binomial
// --------------
// randB(double base, int tries) -> double (non-negative)
// randBN(double base, int tries) -> double (including negative values)

class RandGen : public Component
{
protected:
   // usual seeds has this standard (uint32)
   unsigned seed;

private:
   // using c++11 mersenne twister, by default
   std::mt19937 _gen;
   // not using c++ default
   //std::default_random_engine _gen;

public:
   // returns the random generator seed
   long getSeed() const
   {
      return seed;
   }

   // sets the random generator seed (uint32)
   RandGen& setSeed(unsigned _seed)
   {
      seed = _seed;
      initialize();
      // returning self-reference
      return *this;
   }

public:
   RandGen()
   {
      seed = generateRandomSeed();
      initialize();
   }

   RandGen(unsigned _seed)
     : seed(_seed)
   {
      initialize();
   }

   virtual ~RandGen()
   {
   }

public:
   // initialize random number generation
   virtual void initialize()
   {
      _gen = std::mt19937(this->seed);
   }

public:
   // random integer
   virtual int rand()
   {
      std::uniform_int_distribution<int> dis;
      return dis(_gen);
   }

   // random positive integer between 0 and n-1
   virtual unsigned rand(unsigned n)
   {
      // protect against underflow
      n = (n == 0) ? 0 : n - 1; // no exceptions thrown
      std::uniform_int_distribution<unsigned> dis(0, n);
      return dis(_gen);

      // TODO: https://ericlippert.com/2013/12/16/how-much-bias-is-introduced-by-the-remainder-technique/
      //return ::rand() % n; // old way, abandoned (too much bias!)
   }

   // randomized number between non-negatives [i, j]
   virtual unsigned rand(unsigned i, unsigned j)
   {
      std::uniform_int_distribution<unsigned> dis(i, j);
      return dis(_gen);
   }

   // random with probability 'p'
   virtual bool randP(double p)
   {
      std::uniform_real_distribution<double> distribution(0.0, 1.0);
      return rand01() < p;
   }

   // random uniform between [0,1)
   virtual double rand01()
   {
      std::uniform_real_distribution<double> dis(0.0, 1.0);
      return dis(_gen);
   }

   // rand binomial
   virtual int randB(double p, int tries)
   {
      std::binomial_distribution<int> dis(tries, p);
      return dis(_gen);
   }

   // rand binomial (positive or negative)
   virtual int randBN(double p, int tries)
   {
      int y = randB(p, tries);
      int sign = this->rand(2);
      if (sign == 1)
         y *= -1;
      return y;
   }

   virtual double randG(double mean, double stdev)
   {
      std::normal_distribution<double> dis(mean, stdev);
      return dis(_gen);
   }

   // random gaussian mean 0.0 stdev 1.0
   virtual double randG()
   {
      std::normal_distribution<double> dis(0.0, 1.0);
      return dis(_gen);
   }

   unsigned generateRandomSeed() const
   {
      // using random_device just for default seeding (if not provided otherwise)
      std::random_device rd;
      std::uniform_int_distribution<unsigned> dist;
      return dist(rd);
   }

   // deterministic shuffle using RandGen object
   template<class T>
   void shuffle(vector<T>& v)
   {
      if (v.size() > 0)
         for (unsigned int i = 0; i < v.size() - 1; i++) {
            int x = i + this->rand(v.size() - i - 1) + 1;
            T elem = v.at(i);
            v.at(i) = v.at(x);
            v.at(x) = elem;
         }
   }

   static string idComponent()
   {
      return "OptFrame:RandGen";
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (Component::compatible(s));
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class RandGenBuilder : public ComponentBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~RandGenBuilder()
   {
   }

   virtual Component*
   buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      if (!scanner.hasNext())
         return nullptr;

      long seed = scanner.nextLong();

      return new RandGen(seed);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair("long", "seed"));
      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == RandGen::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "RandGen";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};
}

#endif /* OPTFRAME_RANDGEN_HPP_ */
