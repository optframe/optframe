
#include <boost/ut.hpp>

// OptFrame C library headers
#include <OptFrameLib/OptFrameLib.h>

#include <OptFrame/modlog/modlog.hpp>
#include <OptFrameLib/FCoreApi1Engine.hpp>

#include "OptFrameLib/LibCTypes.h"

int main() {
  using namespace boost::ut;

  FakeEnginePtr engine =
      optframe_api1d_create_engine((int)modlog::LogLevel::Disabled);

  std::stringstream ss;
  auto* eng = (FCoreApi1Engine*)engine;
  eng->logstream = &ss;
  eng->engineLogLevel = modlog::LogLevel::Debug;

  optframe_api0d_engine_welcome(engine);

  std::string welcome_message = ss.str();
  ss.clear();

  "welcome"_test = [welcome_message] {
    expect((welcome_message.find(
                "Welcome to OptFrame Functional Core (FCore) - version") !=
            std::string::npos));
  };

  // testing welcome again on std::cout
  eng->logstream = &std::cout;
  optframe_api0d_engine_welcome(engine);

  "first_rand"_test = [eng] {
    eng->loader.factory.getRandGen()->setSeed(9999);
    expect(eng->loader.factory.getRandGen()->rand() % 100 == 5_i);
  };

  "optframe_api1d_engine_rand"_test = [engine] {
    optframe_api1d_engine_rand_set_seed(engine, 9999);
    expect(optframe_api1d_engine_rand(engine) % 100 == 5_i);
  };

  // manipulating seed from rand in engine
  eng->loader.factory.getRandGen()->setSeed(0);

  // =========================

  bool b = optframe_api1d_destroy_engine(engine);

  "optframe_api1d_destroy_engine"_test = [b] { expect(b == 1_i); };

  return 0;
}