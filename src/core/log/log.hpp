#pragma once

#include <spdlog/spdlog.h>

namespace core::log::internal {

[[nodiscard]]
auto logger() -> spdlog::logger&;

}   // namespace core::log::internal

#define ENGINE_LOG_LEVEL_OFF      0
#define ENGINE_LOG_LEVEL_CRITICAL 1
#define ENGINE_LOG_LEVEL_ERROR    2
#define ENGINE_LOG_LEVEL_WARNING  3
#define ENGINE_LOG_LEVEL_INFO     4
#define ENGINE_LOG_LEVEL_DEBUG    5
#define ENGINE_LOG_LEVEL_TRACE    6

#if ENGINE_LOG_LEVEL >= ENGINE_LOG_LEVEL_CRITICAL
  #define ENGINE_LOG_CRITICAL(...)                                           \
      SPDLOG_LOGGER_CALL(                                                    \
          &core::log::internal::logger, spdlog::level::critical, __VA_ARGS__ \
      )
#else
  #define ENGINE_LOG_CRITICAL(...) (void)0
#endif

#if ENGINE_LOG_LEVEL >= ENGINE_LOG_LEVEL_ERROR
  #define ENGINE_LOG_ERROR(...) \
      SPDLOG_LOGGER_CALL(&core::log::internal::logger, spdlog::level::error, __VA_ARGS__)
#else
  #define ENGINE_LOG_ERROR(...) (void)0
#endif

#if ENGINE_LOG_LEVEL >= ENGINE_LOG_LEVEL_WARNING
  #define ENGINE_LOG_WARNING(...) \
      SPDLOG_LOGGER_CALL(&core::log::internal::logger, spdlog::level::warn, __VA_ARGS__)
#else
  #define ENGINE_LOG_WARNING(...) (void)0
#endif

#if ENGINE_LOG_LEVEL >= ENGINE_LOG_LEVEL_INFO
  #define ENGINE_LOG_INFO(...) \
      SPDLOG_LOGGER_CALL(&core::log::internal::logger, spdlog::level::info, __VA_ARGS__)
#else
  #define ENGINE_LOG_INFO(...) (void)0
#endif

#if ENGINE_LOG_LEVEL >= ENGINE_LOG_LEVEL_DEBUG
  #define ENGINE_LOG_DEBUG(...) \
      SPDLOG_LOGGER_CALL(&core::log::internal::logger, spdlog::level::debug, __VA_ARGS__)
#else
  #define ENGINE_LOG_DEBUG(...) (void)0
#endif

#if ENGINE_LOG_LEVEL >= ENGINE_LOG_LEVEL_TRACE
  #define ENGINE_LOG_TRACE(...) \
      SPDLOG_LOGGER_CALL(&core::log::internal::logger, spdlog::level::trace, __VA_ARGS__)
#else
  #define ENGINE_LOG_TRACE(...) (void)0
#endif
