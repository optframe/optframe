// SPDX-License-Identifier: MIT
// Copyright (C) 2025 - modlog library - https://github.com/igormcoelho/modlog

#ifndef MODLOG_MODLOG_HPP
#define MODLOG_MODLOG_HPP

//
#ifndef MODLOG_USE_CXX_MODULES
#ifndef _WIN32
#include <pthread.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

#include <chrono>
#include <ctime>
#include <filesystem>
#if __cplusplus >= 202002L && __has_include(<format>)
#include <format>
#define MODLOG_USE_STD_FORMAT 1
#endif
#include <functional>
#include <iostream>
#include <sstream>
#if __cplusplus >= 202002L && __has_include(<source_location>)
#include <source_location>
#define USE_STD_SRC_LOC 1
#endif
#include <string>
#include <thread>  // for std::terminate

#if __cplusplus >= 202002L && __has_include(<concepts>)
#include <concepts>
#define MODLOG_USE_STD_CONCEPTS 1
#endif

#ifdef MODLOG_STACKTRACE
#if __has_include(<stacktrace>)
#include <stacktrace>
#endif
#endif

#define MODLOG_MOD_EXPORT
#else
#define MODLOG_MOD_EXPORT export
#endif

namespace modlog {

#ifdef USE_STD_SRC_LOC
using my_source_location = std::source_location;
#define MY_SOURCE_LOCATION() std::source_location::current()
#else
struct my_source_location {
  std::string_view _file;
  int _line;
  int line() const { return _line; }
  std::string_view file_name() const { return _file; }
  constexpr my_source_location(std::string_view f, int l)
      : _file{f}, _line{l} {}
};
#define MY_SOURCE_LOCATION() \
  my_source_location { __FILE__, __LINE__ }
#endif

inline uintptr_t get_tid() {
#ifdef _WIN32
  return static_cast<uintptr_t>(::GetCurrentThreadId());
#else
  pthread_t tid = pthread_self();
// On ARM, pthread_t is 'long unsigned int', uintptr_t is 'unsigned int'
// On Mac, pthread_t is '_opaque_pthread_t *', uintptr_t is 'unsigned long'
// On linux, pthread_t is 'unsigned long', uintptr_t is 'unsigned long'
#if defined(__APPLE__) || defined(__FreeBSD__)
  return reinterpret_cast<uintptr_t>(
      tid);  // mac and freebsd (but Linux ok too...)
#elif defined(__arm__) || defined(__aarch64__)
  return static_cast<uintptr_t>(tid);  // ARM! (but Linux ok too...)
#else
  return static_cast<uintptr_t>(tid);  // default...
#endif

#endif
}

// =======================================
//     nullable ostream  ("/dev/null")
// =======================================

struct NullBuffer : std::streambuf {
  // accepts and ignores char 'c'
  int overflow(int c) override { return c; }
};

struct NullOStream : std::ostream {
  NullOStream() : std::ostream{&nb} {}

 private:
  NullBuffer nb;
};

// =======================================
//      log levels and default config
// =======================================

/*
// broken on windows!
MODLOG_MOD_EXPORT enum LogLevel {
  SILENT = -1,
  INFO = 0,
  WARNING = 1,
  ERROR = 2,
  FATAL = 3
};
*/

#if __cplusplus >= 202002L
MODLOG_MOD_EXPORT enum class LogLevel : int {
  Silent = -2,
  Debug = -1,
  Info = 0,
  Warning = 1,
  Error = 2,
  Fatal = 3,
  Disabled = 4
};
#else
MODLOG_MOD_EXPORT enum LogLevel : int {
  Silent = -2,
  Debug = -1,
  Info = 0,
  Warning = 1,
  Error = 2,
  Fatal = 3,
  Disabled = 4
};
#endif

// Recommendation: do not use UPPER_CASE, use CamelCase
// Example:    Log(Info) << "my text!";
// Instead of: Log(INFO) << "my text!";
// On windows, upper case will not work, only in Linux/MacOS
// But... leaving UPPER CASE here for COMPATIBILITY purposes!
#ifndef _WIN32
MODLOG_MOD_EXPORT constexpr LogLevel SILENT = LogLevel::Silent;
MODLOG_MOD_EXPORT constexpr LogLevel DEBUG = LogLevel::Debug;
MODLOG_MOD_EXPORT constexpr LogLevel INFO = LogLevel::Info;
MODLOG_MOD_EXPORT constexpr LogLevel WARNING = LogLevel::Warning;
// ERROR is problematic on Windows
MODLOG_MOD_EXPORT constexpr LogLevel ERROR = LogLevel::Error;
MODLOG_MOD_EXPORT constexpr LogLevel FATAL = LogLevel::Fatal;
MODLOG_MOD_EXPORT constexpr LogLevel DISABLED = LogLevel::Disabled;
#else
// Windows do not have upper case macros, so must use CamelCase ones
MODLOG_MOD_EXPORT using modlog::LogLevel::Silent;
MODLOG_MOD_EXPORT using modlog::LogLevel::Debug;
MODLOG_MOD_EXPORT using modlog::LogLevel::Info;
MODLOG_MOD_EXPORT using modlog::LogLevel::Warning;
MODLOG_MOD_EXPORT using modlog::LogLevel::Error;
MODLOG_MOD_EXPORT using modlog::LogLevel::Fatal;
MODLOG_MOD_EXPORT using modlog::LogLevel::Disabled;
#endif

// =======================================
//             handling Fatal
// =======================================
struct FatalStream : private std::streambuf, public std::ostream {
 private:
  std::stringstream buffer;

 public:
  explicit FatalStream() : std::ostream{this} {}

 private:
  int overflow(int c) override {
    buffer.put((char)c);
    if (c == '\n') kill();
    return c;
  }

  void kill() {
    std::cerr << buffer.str() << std::endl;
#if MODLOG_STACKTRACE && defined(__cpp_lib_stacktrace)
    std::cerr << std::stacktrace::current() << std::endl;
#else
    std::cerr << "WARNING: stacktrace unavailable, must #include <stacktrace> "
                 "and -lstdc++exp"
              << std::endl;
#endif
    std::terminate();
  }
};

MODLOG_MOD_EXPORT inline FatalStream fatal;

// =======================================
//         helper prefix function
// =======================================

MODLOG_MOD_EXPORT inline std::ostream& default_prefix_data(
    std::ostream& os, LogLevel l, tm now_tm, std::chrono::microseconds us,
    uintptr_t tid, std::string_view short_file, int line, bool debug) {
  // TODO: check if locking is required for multi-threaded setups...
  char level = '?';
  if (l == LogLevel::Debug)
    level = 'D';
  else if (l == LogLevel::Info)
    level = 'I';
  else if (l == LogLevel::Warning)
    level = 'W';
  else if (l == LogLevel::Error)
    level = 'E';
  else if (l == LogLevel::Fatal)
    level = 'F';

  // add line break before, since we cannot control what's done after...
  os << std::endl;
// #if defined(__cpp_lib_format)
#ifdef MODLOG_USE_STD_FORMAT
  os << std::format("{}{:04}{:02}{:02} {:02}:{:02}:{:02}.{:06} {:}", level,
                    now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday,
                    now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec, us.count(),
                    tid);
#else
  os << level << std::setw(4) << std::setfill('0') << (now_tm.tm_year + 1900)
     << std::setw(2) << std::setfill('0') << (now_tm.tm_mon + 1) << std::setw(2)
     << std::setfill('0') << now_tm.tm_mday << ' ' << std::setw(2)
     << std::setfill('0') << now_tm.tm_hour << ':' << std::setw(2)
     << std::setfill('0') << now_tm.tm_min << ':' << std::setw(2)
     << std::setfill('0') << now_tm.tm_sec << '.' << std::setw(6)
     << std::setfill('0') << us.count() << ' ' << tid;
#endif

// #if defined(__cpp_lib_format)
#ifdef MODLOG_USE_STD_FORMAT

  if (!short_file.empty())
    os << std::format(" {}:{}] ", short_file, line);
  else
    os << std::format("] ");
#else
  if (!short_file.empty()) {
    os << " " << short_file << ":" << line << "] ";
  } else {
    os << "] ";
  }
#endif

  return (level == 'F') ? fatal : os;
}

MODLOG_MOD_EXPORT inline std::ostream& json_prefix(
    std::ostream& os, LogLevel l, tm now_tm, std::chrono::microseconds us,
    uintptr_t tid, std::string_view short_file, int line, bool debug) {
  // TODO: check if locking is required for multi-threaded setups...
  std::string slevel;
  if (l == LogLevel::Debug)
    slevel = "debug";
  else if (l == LogLevel::Info)
    slevel = "info";
  else if (l == LogLevel::Warning)
    slevel = "warn";
  else if (l == LogLevel::Error)
    slevel = "error";
  else if (l == LogLevel::Fatal)
    slevel = "fatal";
  if (debug) slevel = "debug";

// #if defined(__cpp_lib_format)
#ifdef MODLOG_USE_STD_FORMAT
  std::string stime =
      std::format("{:04}{:02}{:02} {:02}:{:02}:{:02}.{:06}",
                  now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday,
                  now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec, us.count());
#else
  std::stringstream ss;
  ss << std::setw(4) << std::setfill('0') << (now_tm.tm_year + 1900)
     << std::setw(2) << std::setfill('0') << (now_tm.tm_mon + 1) << std::setw(2)
     << std::setfill('0') << now_tm.tm_mday << ' ' << std::setw(2)
     << std::setfill('0') << now_tm.tm_hour << ':' << std::setw(2)
     << std::setfill('0') << now_tm.tm_min << ':' << std::setw(2)
     << std::setfill('0') << now_tm.tm_sec << '.' << std::setw(6)
     << std::setfill('0') << us.count();
  std::string stime = ss.str();
#endif
  os << "{\"level\":\"" << slevel << "\", \"timestamp\":\"" << stime << "\", ";

  if (!short_file.empty())
    os << "\"caller\":\"" << short_file << ":" << line << "\", ";

  os << "\"tid\":" << tid << ", ";

  os << "\"msg\":\"";
  return (l == LogLevel::Fatal) ? fatal : os;
}

MODLOG_MOD_EXPORT class LogConfig {
 public:
  std::ostream* os{&std::cerr};
  // OBS: could host a unique_ptr here, if necessary for thirdparty streams
  // OBS 2: not necessary for the moment... if you need it, just let us know!
  LogLevel minlog{LogLevel::Info};
  int vlevel{0};
  bool prefix{true};
  NullOStream no;
  using FuncLogPrefix = std::function<std::ostream&(
      std::ostream&, LogLevel, std::tm, std::chrono::microseconds,
      std::uintptr_t, std::string_view, int, bool)>;
  FuncLogPrefix fprefixdata{default_prefix_data};

  std::string getFilename(std::string_view vpath) {
    std::string path{vpath};
    auto pos = path.find_last_of("/\\");
    if (pos != std::string::npos)
      return path.substr(pos + 1);
    else
      return path;
#if 0
    return std::filesystem::path(vpath).filename().string();
#endif
  }

  std::ostream& fprefix(std::ostream* os, LogLevel l, std::string_view path,
                        int line, bool debug) {
    // add line break before, since we cannot control what's done after...
    // *os << std::endl;
    using namespace std::chrono;  // NOLINT

    auto now = system_clock::now();
    auto now_time_t = system_clock::to_time_t(now);
    auto now_tm = *std::localtime(&now_time_t);
    auto us = duration_cast<microseconds>(now.time_since_epoch()) % 1'000'000;
    auto tid = get_tid();
    std::string short_file = "";
    if (!path.empty()) short_file = getFilename(path);

    // =====================================
    // use personalized prefix data function
    // =====================================

    // TODO: does fprefixdata needs to "return" ostream?
    // will it handle fatal too? for now, leaving both options...
    auto& os2 =
        this->fprefixdata(*os, l, now_tm, us, tid, short_file, line, debug);
    // TODO: dump 'os2' into Fatal or not?
    // TODO 2: decide later... better to keep automatic Fatal for now!
    return (l == LogLevel::Fatal) ? fatal : os2;
  }
};

MODLOG_MOD_EXPORT inline LogConfig modlog_default;

// #ifdef __cpp_concepts
#ifdef MODLOG_USE_STD_CONCEPTS
template <typename Self>
concept Loggable = requires(Self obj) {
  { obj.stream() } -> std::same_as<std::ostream&>;
  { obj.log() } -> std::same_as<LogLevel>;
  { obj.prefix() } -> std::same_as<bool>;
} || requires(Self obj) {
  { obj.log() } -> std::same_as<LogConfig>;
};
#else
#define Loggable typename
#endif

// ==============================
// logs with global configuration
// ==============================

MODLOG_MOD_EXPORT inline std::ostream& Log(
    LogLevel sev = LogLevel::Info,
    // const std::source_location location = std::source_location::current()) {
    const my_source_location location = MY_SOURCE_LOCATION()) {
  if (modlog_default.minlog == LogLevel::Disabled) return modlog_default.no;
#ifdef NDEBUG
  if (sev < LogLevel::Info) return modlog_default.no;
#endif
  return (sev < modlog_default.minlog)
             ? modlog_default.no
             : (modlog_default.prefix
                    ? modlog_default.fprefix(modlog_default.os, sev,
                                             location.file_name(),
                                             location.line(), false)
                    : *modlog_default.os);
}

// ===============================
// vlogs with global configuration
// ===============================

MODLOG_MOD_EXPORT inline std::ostream& VLog(
    int vlevel,
    // const std::source_location location = std::source_location::current()) {
    const my_source_location location = MY_SOURCE_LOCATION()) {
  if (modlog_default.minlog == LogLevel::Disabled) return modlog_default.no;
#ifdef NDEBUG
  if (vlevel > 0) return modlog_default.no;
#endif
  return (LogLevel::Info < modlog_default.minlog) ||
                 (vlevel > modlog_default.vlevel)
             ? modlog_default.no
             : (modlog_default.prefix
                    ? modlog_default.fprefix(modlog_default.os, LogLevel::Info,
                                             location.file_name(),
                                             location.line(), true)
                    : *modlog_default.os);
}

// =======================================
// logs with object-specific configuration
// =======================================

/*
// No "Default" logging for objects
MODLOG_MOD_EXPORT template <Loggable LogObj>
inline std::ostream& Log(
    LogObj* lo,
    // const std::source_location location = std::source_location::current()) {
    const my_source_location location = MY_SOURCE_LOCATION()) {
  return (LogLevel::Info < lo->log().minlog)
             ? modlog_default.no
             : (lo->log().prefix
                    ? modlog_default.fprefix(lo->log().os, LogLevel::Info,
                                             location.file_name(),
                                             location.line(), false)
                    : *lo->log().os);
}
*/

MODLOG_MOD_EXPORT template <Loggable LogObj>
inline std::ostream& Log(
    LogLevel sev, LogObj* lo,
    // const std::source_location location = std::source_location::current()) {
    const my_source_location location = MY_SOURCE_LOCATION()) {
  if (lo->log().minlog == LogLevel::Disabled) return modlog_default.no;
#ifdef NDEBUG
  if (sev < LogLevel::Info) return modlog_default.no;
#endif
  return (sev < lo->log().minlog)
             ? modlog_default.no
             : (lo->log().prefix
                    ? lo->log().fprefix(lo->log().os, sev, location.file_name(),
                                        location.line(), false)
                    : *lo->log().os);
}

// ================================
//    support for file logging
// ================================

inline void StartLogs(std::string_view app_name) {
  // TODO: create temporary log files
  Log(LogLevel::Warning)
      << "WARNING: modlog does not currently support file logging!";
}

inline void StopLogs() {
  // flush last line!
  *modlog_default.os << std::endl;
}

// ================================
//     semantic stream - utils
// ================================

struct SemStream : private std::streambuf, public std::ostream {
 private:
  std::stringstream oss;

 public:
  std::ostream* os;

  explicit SemStream(std::ostream& _os) : std::ostream{this}, os{&_os} {}

  SemStream() : std::ostream{this}, os{&oss} {}

 private:
  int overflow(int c) override {
    (*os).put(c);
    return 0;
  }

 public:
  void setStream(std::ostream& _os) { os = &_os; }

  std::string dump() {
    std::string dump{oss.str()};
    oss.str(std::string{});
    os->clear();
    return dump;
  }
};

}  // namespace modlog

#endif
