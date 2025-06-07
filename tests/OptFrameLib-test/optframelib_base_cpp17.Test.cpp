
// OptFrame C library headers
#include <OptFrameLib/OptFrameLib.h>

#include <OptFrame/modlog/modlog.hpp>
#include <OptFrameLib/FCoreApi1Engine.hpp>

#include "OptFrameLib/LibCTypes.h"

int main() {
  FakeEnginePtr engine =
      optframe_api1d_create_engine((int)modlog::LogLevel::Disabled);

  std::stringstream ss;
  auto* eng = (FCoreApi1Engine*)engine;
  eng->logstream = &ss;
  eng->engineLogLevel = modlog::LogLevel::Debug;

  optframe_api0d_engine_welcome(engine);

  std::string welcome_message = ss.str();
  ss.clear();

  assert((welcome_message.find(
              "Welcome to OptFrame Functional Core (FCore) - version") !=
          std::string::npos));

  // testing welcome again on std::cout
  eng->logstream = &std::cout;
  optframe_api0d_engine_welcome(engine);
  // =========================

  bool b = optframe_api1d_destroy_engine(engine);

  assert(b == 1);

  return 0;
}