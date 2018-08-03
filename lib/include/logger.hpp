#pragma once

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <string>

enum class LogLevel {trace = spdlog::level::level_enum::trace,
                     debug = spdlog::level::level_enum::debug,
                     info = spdlog::level::level_enum::info,
                     warn = spdlog::level::level_enum::warn,
                     error = spdlog::level::level_enum::err,
                     critical = spdlog::level::level_enum::critical,
                     off = spdlog::level::level_enum::off
};

namespace fetch {
  namespace oef {
    class Logger {
      static constexpr const char * const logger_name = "fetch::oef::logger";
    private:
      std::string _section{""};
      std::shared_ptr<spdlog::logger> _logger{nullptr};
    public:
      Logger(const std::string &section);
      
      std::string section() const noexcept { return _section; }
      
      template <typename Arg1, typename... Args>
      void log(const LogLevel level, const char *fmt, const Arg1 &arg1, const Args &... args) {
        auto new_fmt = "[{}] " + std::string(fmt);
        _logger->log(static_cast<spdlog::level::level_enum>(level), new_fmt.c_str(), section(),
                     arg1, args...);
      }

      template <typename Arg1, typename... Args>
      void trace(const char *fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::trace, fmt, arg1, args...);
      }

      template <typename Arg1>
      void trace(const Arg1 &arg1) {
        trace("{}", arg1);
      }

      template <typename Arg1, typename... Args>
      void debug(const char *fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::debug, fmt, arg1, args...);
      }

      template <typename Arg1>
      void debug(const Arg1 &arg1) {
        debug("{}", arg1);
      }

      template <typename Arg1, typename... Args>
      void info(const char *fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::info, fmt, arg1, args...);
      }

      template <typename Arg1>
      void info(const Arg1 &arg1) {
        info("{}", arg1);
      }

      template <typename Arg1, typename... Args>
      void warn(const char *fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::warn, fmt, arg1, args...);
      }

      template <typename Arg1>
      void warn(const Arg1 &arg1) {
        warn("{}", arg1);
      }

      template <typename Arg1, typename... Args>
      void error(const char *fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::error, fmt, arg1, args...);
      }

      template <typename Arg1>
      void error(const Arg1 &arg1) {
        error("{}", arg1);
      }

      template <typename Arg1, typename... Args>
      void critical(const char *fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::critical, fmt, arg1, args...);
      }

      template <typename Arg1>
      void critical(const Arg1 &arg1) {
        critical("{}", arg1);
      }

      static void level(const LogLevel level) noexcept {
        spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
      }

    };
  }
}
