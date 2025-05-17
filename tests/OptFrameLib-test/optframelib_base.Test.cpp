
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

  using unique_cstr = std::unique_ptr<char[]>;

  std::stringstream ss_log_level;
  ss_log_level << (int)modlog::LogLevel::Debug;
  bool b_out = optframe_api1d_engine_experimental_set_parameter(
      engine, "ENGINE_LOG_LEVEL", ss_log_level.str().c_str());
  "optframe_api1d_engine_experimental_set_parameter"_test = [b_out] {
    expect(b_out);
  };

  unique_cstr s_out{optframe_api1d_engine_experimental_get_parameter(
      engine, "ENGINE_LOG_LEVEL")};
  std::string_view view_sout{s_out.get()};
  "optframe_api1d_engine_experimental_get_parameter"_test = [view_sout] {
    expect(view_sout == "\"-1\"");
  };

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