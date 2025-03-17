#pragma once

#include <source_location>
#include <stdexcept>
#include <string_view>

namespace util {

class PreconditionViolation : public std::logic_error {
public:
    // TODO: use <stacktrace> library
    PreconditionViolation(
        const std::source_location& location,
        std::string_view            precondition
    );
};

}   // namespace util

#ifndef ENGINE_ENABLE_UNIT_TESTS
  #include <cassert>

  #define PRECOND(...) assert(__VA_ARGS__)
#else
  #define PRECOND(...)                                                        \
      if (!(__VA_ARGS__)) {                                                   \
          throw util::PreconditionViolation{ std::source_location::current(), \
                                             #__VA_ARGS__ };                  \
      }
#endif
