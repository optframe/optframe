// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OPTFRAME_HEURISTICS_EA_RK_BRKGA_HPP_  // NOLINT
#define OPTFRAME_HEURISTICS_EA_RK_BRKGA_HPP_  // NOLINT

// C++
#include <algorithm>
#include <string>
//
#include <OptFrame/GlobalSearch.hpp>
#include <OptFrame/Helper/EPopulation.hpp>
#include <OptFrame/Heuristics/EA/RK/RK.hpp>
#include <OptFrame/Heuristics/EA/RK/RKGA.hpp>
#include <OptFrame/InitialPopulation.hpp>
#include <OptFrame/SingleObjSearch.hpp>

// BRKGA - Biased-Random Key Genetic Algorithms

namespace optframe {

// It seems that R is connected to S here... S<R> is rule perhaps?
// maybe better to not provide XRS directly here (used to be = = RSolution<R>)
// RKeys is the representation of random keys
//
// template<XRepresentation R, XRSolution<R> XRS, XEvaluation XEv,
// XRepresentation RKeys = optframe::random_keys>
//
// Assuming XSolution allows "raw representation" such as vector<int>...
// Will require XSolution instead of XRepresentation, since legacy Evaluation
// already deals with that User may also want to "decode" the ADS from keys, if
// necessary...
//
template <XESolution XES, optframe::comparability KeyType = double,
          XESolution XES2 =
              std::pair<std::vector<KeyType>,
                        typename XES::second_type>>  //,
                                                     // XSearch<XES2> XSH2 =
                                                     // VEPopulation<XES2>>
class BRKGA : public RKGA<XES, KeyType, XES2> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  // using RSK = RSolution<RKeys>;
  using RSK = typename XES2::first_type;

 protected:
  double probElitism;

 public:
  // unified constructors (explicit initPop)
  // to pass 'key_size' just use parameter "_initPop =
  // RandomKeysInitPop(key_size)"
  BRKGA(sref<DecoderRandomKeys<XES, KeyType>> _decoder,
        sref<InitialEPopulation<XES2>> _initPop, unsigned _popSize,
        unsigned numGen, double fracTOP, double fracBOT, double _probElitism,
        sref<RandGen> _rg = new RandGen)
      : RKGA<XES, KeyType>(_decoder, _initPop, _popSize, numGen, fracTOP,
                           fracBOT, _rg),
        probElitism(_probElitism) {
    assert(probElitism > 0.5);
    assert(probElitism <= 1.0);
    // TODO: keep this?
    assert(!_initPop->canEvaluate());
  }

  virtual ~BRKGA() {}

  // returns an owning reference (take good care of it, and delete!)
  virtual RSK cross(const VEPopulation<XES2>& pop) override {
    if (Component::debug)
      (*Component::logdata)
          << "BRKGA cross(|pop|=" << pop.size() << ")" << std::endl;
    // assert(this->key_size > 0); // In case of using InitPop, maybe must
    // receive a Selection or Crossover object...

    const RSK& p1 = pop.at(this->rg->rand() % this->eliteSize).first;
    const RSK& p2 = pop.at(this->eliteSize +
                           this->rg->rand() % (pop.size() - this->eliteSize))
                        .first;

    // random_keys* v = new random_keys(this->key_size, 0.0);
    // for (int i = 0; i < this->key_size; i++)
    VEPopulation<XES2> p_single =
        this->initPop->generateEPopulation(1, 0.0);  // implicit 'key_size'
    // TODO: should cache 'key_size' to prevent unused rands on generation
    random_keys v(p_single.at(0).first);  // copy or 'move' ?
    std::fill(v.begin(), v.end(), 0);
    for (unsigned i = 0; i < v.size(); i++)
      if ((this->rg->rand() % 1000000) / 1000000.0 < probElitism)
        // v->at(i) = p1.getR()[i];
        v.at(i) = p1[i];
      else
        // v->at(i) = p2.getR()[i];
        v.at(i) = p2[i];
    // return *new RSK(v);
    return v;  // TODO: pass by std::move() or unique_ptr
  }

  virtual bool setVerboseR() override {
    this->setVerbose();
    return RKGA<XES, KeyType>::setVerboseR();
  }
};  // class BRKGA

// NOTE: X2ES_Factory only used on HeuristicFactory!!
// Not possible to configure BRKGA Population here and keep compatibility with
// others...
// TODO: could we somehow Type-Hide the X2ES or X2ES_Factory parameters? Does it
// make sense?
// TODO: only time can tell...
//
// template<XESolution XES, XESolution XES2_Factory = XES,
// X2ESolution<XES2_Factory> X2ES_Factory = MultiESolution<XES2_Factory>>
template <XESolution XES, XESolution XES2_Factory,
          XSearch<XES2_Factory> X2ES_Factory>
class BRKGABuilder
    : public GlobalSearchBuilder<XES, XES, XES2_Factory, X2ES_Factory> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  // using S = typename XES::first_type;
  // using XEv = typename XES::second_type;
  using KeyType = double;
  // XESolution
  using XES2 = std::pair<std::vector<KeyType>, XEv>;
  // X2ESolution<XES2>
  using X2ES = VEPopulation<XES2>;

  virtual ~BRKGABuilder() {}

  virtual GlobalSearch<XES>* build(
      Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES_Factory>& hf,
      string family = "") override {
    sptr<DecoderRandomKeys<XES, KeyType>> decoder;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(decoder, id_0, sid_0);

    sptr<InitialEPopulation<XES2>> initPop;
    std::string sid_1 = scanner.next();
    int id_1 = *scanner.nextInt();
    hf.assign(initPop, id_1, sid_1);

    unsigned _popSize = *scanner.nextInt();
    unsigned numGen = *scanner.nextInt();
    double fracTOP = *scanner.nextDouble();
    double fracBOT = *scanner.nextDouble();
    double _probElitism = *scanner.nextDouble();

    // GlobalSearch<XES>* sos = new BRKGA<XES, KeyType, XES2, X2ES>(
    GlobalSearch<XES>* sos = new BRKGA<XES, KeyType, XES2>(
        decoder, initPop, _popSize, numGen, fracTOP, fracBOT, _probElitism,
        hf.getRandGen());

    return sos;
  }

  virtual vector<pair<string, string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(DecoderRandomKeys<XES, KeyType>::idComponent(), "decoder"));
    params.push_back(make_pair(InitialEPopulation<XES2>::idComponent(),
                               "initial epopulation"));
    params.push_back(make_pair("OptFrame:unsigned", "popSize"));
    params.push_back(make_pair("OptFrame:unsigned", "numGen"));
    params.push_back(make_pair("OptFrame:double", "fracTOP"));
    params.push_back(make_pair("OptFrame:double", "fracBOT"));
    params.push_back(make_pair("OptFrame:double", "probElitism"));

    return params;
  }

  virtual bool canBuild(string component) override {
    return component == BRKGA<XES, double>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << GlobalSearchBuilder<XES, XES, XES2_Factory,
                              X2ES_Factory>::idComponent()
       << EA::family() << ":" << RK::family() << "BRKGA";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_RK_BRKGA_HPP_ // NOLINT
