#pragma once

#include <iostream>
//
#include <OptFrame/Hyper/CheckCommand.hpp>
#include <OptFrame/Hyper/Loader.hpp>
//
#include <OptFrameLib/FCoreLibSolution.hpp>

class FCoreApi1Engine {
 public:
  FCoreApi1Engine(std::ostream* _logstream = &std::cout)
      : logstream{_logstream} {
    // parameter: NS_VALID_RANDOM_MOVE_MAX_TRIES (just try a SINGLE move)
    this->experimentalParams["NS_VALID_RANDOM_MOVE_MAX_TRIES"] = "1";
    // parameter: EVTYPE_NUM_ZERO_PRECISION
    std::stringstream ss_zero;
    ss_zero << std::fixed << std::setprecision(8)
            << optframe::num_zero_precision<double>();
    this->experimentalParams["EVTYPE_NUM_ZERO_PRECISION"] = ss_zero.str();
    // modlog system is:
    // -2 silent, -1 debug, 0 info, 1 warn, 2 err, 3 fatal, 4 disabled
    this->experimentalParams["ENGINE_LOG_LEVEL"] = "1";     // warn
    this->experimentalParams["COMPONENT_LOG_LEVEL"] = "0";  // info
    updateParameters();  // refresh engine parameters
  }

  void updateParameters() {
    using modlog::LogLevel::Debug;
    this->engineLogLevel = (modlog::LogLevel)std::stoi(
        this->experimentalParams["ENGINE_LOG_LEVEL"]);
    this->componentLogLevel = (modlog::LogLevel)std::stoi(
        this->experimentalParams["COMPONENT_LOG_LEVEL"]);
    //
    if (this->engineLogLevel == Debug)
      Log(Debug, this) << "set Engine LogLevel to Debug" << std::endl;
    check.setMessageLevel(engineLogLevel);
    loader.factory.setLogLevel(engineLogLevel);
  }

  // object-specific configuration for modlog log system
  using FuncLogPrefix = typename modlog::LogConfig::FuncLogPrefix;
  FuncLogPrefix fprefixlogs{[](std::ostream& os, modlog::LogLevel l,
                               std::tm local_tm, std::chrono::microseconds us,
                               std::uintptr_t tid, std::string_view short_file,
                               int line, bool debug) -> std::ostream& {
    std::string slevel;
    if (l == modlog::LogLevel::Info)
      slevel = "info";
    else if (l == modlog::LogLevel::Warning)
      slevel = "warn";
    else if (l == modlog::LogLevel::Error)
      slevel = "error";
    else if (l == modlog::LogLevel::Debug)
      debug = true;
    if (debug) slevel = "debug";
    os << "level=" << slevel;
    if (debug) os << " caller=" << short_file << ":" << line;
    os << " msg=";
    return os;
  }};

  std::ostream* logstream{&std::cout};

  modlog::LogConfig log() const {
    return {.os = logstream,
            .minlog = this->engineLogLevel,
            .fprefixdata{fprefixlogs}};
  }

  std::map<std::string, std::string> experimentalParams;

  // LOG SYSTEM:
  // use 'engineLogLevel' for system logs, but 'componentLogLevel' for specific
  // components.
  modlog::LogLevel engineLogLevel;
  modlog::LogLevel componentLogLevel;

  optframe::Loader<FCoreLibESolution> loader;
  optframe::CheckCommand<FCoreLibESolution> check;  // no verbose here...
};
