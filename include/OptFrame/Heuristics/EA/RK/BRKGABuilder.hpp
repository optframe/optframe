
// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#pragma once

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

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

// NOTE: X2ES_Factory only used on HeuristicFactory!!
// Not possible to configure BRKGA Population here and keep compatibility with
// others...
// TODO: could we somehow Type-Hide the X2ES or X2ES_Factory parameters? Does it
// make sense?
// TODO: only time can tell...
//
// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES>
#else
MOD_EXPORT template <typename XES>
#endif
class BRKGABuilder : public GlobalSearchBuilder<XES> {
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

  virtual GlobalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                                   std::string family = "") override {
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

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(DecoderRandomKeys<XES, KeyType>::idComponent(), "decoder"));
    params.push_back(std::make_pair(InitialEPopulation<XES2>::idComponent(),
                                    "initial epopulation"));
    params.push_back(std::make_pair("OptFrame:unsigned", "popSize"));
    params.push_back(std::make_pair("OptFrame:unsigned", "numGen"));
    params.push_back(std::make_pair("OptFrame:double", "fracTOP"));
    params.push_back(std::make_pair("OptFrame:double", "fracBOT"));
    params.push_back(std::make_pair("OptFrame:double", "probElitism"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BRKGA<XES, double>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << GlobalSearchBuilder<XES>::idComponent() << FamilyEA::family() << ":"
       << FamilyRK::family() << "BRKGA";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe