#pragma once

#include <source_location>
#include <stdexcept>
#include <string_view>

namespace util {

class PreconditionViolation : public std::logic_error {
public:
    // TODO: use <stacktrace> library
    explicit PreconditionViolation(const std::source_location& location);
    PreconditionViolation(const std::source_location& location, std::string_view message);
};

}   // namespace util

#ifndef ENGINE_ENABLE_UNIT_TESTS
  #include <cassert>

  #define PRECOND(condition, ...) assert(condition __VA_OPT__(&&) __VA_ARGS__)
#else
  #define PRECOND(condition, ...)                                              \
      if (!(condition)) {                                                      \
          throw util::PreconditionViolation{ std::source_location::current()   \
                                                 __VA_OPT__(, ) __VA_ARGS__ }; \
      }
#endif
