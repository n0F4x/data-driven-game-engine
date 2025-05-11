#pragma once

#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>

#ifndef ENGINE_ENABLE_UNIT_TESTS
  #include <cassert>
#endif

namespace util {

class PreconditionViolation : public std::logic_error {
public:
    // TODO: use <stacktrace> library
    constexpr explicit PreconditionViolation(std::source_location location);
    constexpr PreconditionViolation(
        std::source_location location,
        std::string_view     message
    );

private:
    [[nodiscard]]
    constexpr static auto make_precondition_violation_error_message(
        std::source_location location
    ) -> std::string;
};

}   // namespace util

constexpr util::PreconditionViolation::PreconditionViolation(
    const std::source_location location
)
    : std::logic_error{ make_precondition_violation_error_message(location) }
{}

constexpr util::PreconditionViolation::PreconditionViolation(
    const std::source_location location,
    const std::string_view     message
)
    : std::logic_error{ make_precondition_violation_error_message(location) + ": "
                        + message }
{}

constexpr auto util::PreconditionViolation::make_precondition_violation_error_message(
    const std::source_location location
) -> std::string
{
    using std::string_literals::operator""s;
    return "Precondition violated at "s + location.file_name() + '('
         + std::to_string(location.line()) + ':' + std::to_string(location.column())
         + ") `" + location.function_name() + '`';
}

namespace util::internal {

constexpr auto assert_precondition(
    const bool                                  condition,
    [[maybe_unused]] const std::source_location location
) -> void
{
    if (!condition) [[unlikely]] {
#ifndef ENGINE_ENABLE_UNIT_TESTS
        assert(false);
#else
        throw util::PreconditionViolation{ location };
#endif
    }
}

constexpr auto assert_precondition(
    const bool                                  condition,
    [[maybe_unused]] const std::source_location location,
    const char*                                 message
) -> void
{
    if (!condition) [[unlikely]] {
#ifndef ENGINE_ENABLE_UNIT_TESTS
        assert(false && message);
#else
        throw util::PreconditionViolation{ location, message };
#endif
    }
}

constexpr auto assert_precondition(
    const bool                                  condition,
    [[maybe_unused]] const std::source_location location,
    const std::string&                          message
) -> void
{
    if (!condition) [[unlikely]] {
#ifndef ENGINE_ENABLE_UNIT_TESTS
        assert(false && message.c_str());
#else
        throw util::PreconditionViolation{ location,
                                           static_cast<std::string_view>(message) };
#endif
    }
}

}   // namespace util::internal

#define PRECOND(condition, ...)                                               \
    util::internal::assert_precondition(                                      \
        condition, std::source_location::current() __VA_OPT__(, ) __VA_ARGS__ \
    )
