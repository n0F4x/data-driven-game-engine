module;

#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>

#include <fmt/color.h>

export module utility.contracts;

namespace util {

export constexpr auto assert_precondition(
    bool                 condition,
    const char*          condition_as_string,
    std::source_location location
) -> void;

export constexpr auto assert_precondition(
    bool                 condition,
    const char*          condition_as_string,
    std::source_location location,
    std::string_view     message
) -> void;

export class PreconditionViolation : public std::logic_error {
public:
    // TODO: use <stacktrace> library
    constexpr PreconditionViolation(
        std::source_location location,
        std::string_view     message
    );

    auto print() const -> void;

private:
    std::source_location m_location;
    std::string          m_message;

    [[nodiscard]]
    constexpr static auto headline() -> std::string_view;
    [[nodiscard]]
    constexpr static auto format_location(std::source_location location) -> std::string;

    [[nodiscard]]
    constexpr static auto
        format_what(std::source_location location, std::string_view message)
            -> std::string;
};

}   // namespace util

constexpr util::PreconditionViolation::PreconditionViolation(
    const std::source_location location,
    const std::string_view     message
)
    : std::logic_error{ format_what(location, message) },
      m_location{ location },
      m_message{ message }
{}

constexpr auto util::PreconditionViolation::headline() -> std::string_view
{
    constexpr static std::string_view result{ "Precondition violated!" };
    return result;
}

constexpr auto util::PreconditionViolation::format_location(
    const std::source_location location
) -> std::string
{
    // TODO: constexpr std::format
    return fmt::format(
        "{} ({}:{}) `{}`",
        location.file_name(),
        location.line(),
        location.column(),
        location.function_name()
    );
}

constexpr auto util::PreconditionViolation::format_what(
    const std::source_location location,
    const std::string_view     message
) -> std::string
{
    // TODO: constexpr std::format
    return fmt::format(
        "{}\n    source: {}\n    message: {}",
        headline(),
        format_location(location),
        message
    );
}

constexpr auto util::assert_precondition(
    const bool                                  condition,
    [[maybe_unused]] const char*                condition_as_string,
    [[maybe_unused]] const std::source_location location
) -> void
{
    if (!condition) [[unlikely]] {
        using std::string_literals::operator""s;
        util::PreconditionViolation precondition_violation{
            location,
            // TODO: constexpr std::format
            fmt::format("`{}`", condition_as_string)
        };
#ifdef ENGINE_ENABLE_UNIT_TESTS
        throw precondition_violation;
#elifndef NDEBUG
        precondition_violation.print();
        // TODO: stop debugger if present #p2546r5
        __builtin_debugtrap();
#endif
    }
}

constexpr auto util::assert_precondition(
    const bool                                  condition,
    [[maybe_unused]] const char*                condition_as_string,
    [[maybe_unused]] const std::source_location location,
    [[maybe_unused]] const std::string_view     message
) -> void
{
    if (!condition) [[unlikely]] {
        using std::string_literals::operator""s;
        util::PreconditionViolation precondition_violation{
            location,
            // TODO: constexpr std::format
            fmt::format("`{}`, \"{}\"", condition_as_string, message)
        };
#ifdef ENGINE_ENABLE_UNIT_TESTS
        throw precondition_violation;
#elifndef NDEBUG
        precondition_violation.print();
        // TODO: stop debugger if present #p2546r5
        __builtin_debugtrap();
#endif
    }
}

module :private;

auto util::PreconditionViolation::print() const -> void
{
    fmt::println("{}", fmt::styled(headline(), fmt::fg(fmt::color::orange_red)));
    fmt::println(
        "    {} {} ({}:{})",
        fmt::styled("source:", fmt::fg(fmt::color::spring_green)),
        m_location.file_name(),
        m_location.line(),
        m_location.column()
    );
    fmt::println(
        "    {} {}",
        fmt::styled("function:", fmt::fg(fmt::color::yellow_green)),
        m_location.function_name()
    );
    fmt::println(
        "    {} {}",
        fmt::styled("message:", fmt::fg(fmt::color::cornflower_blue)),
        fmt::styled(m_message, fmt::emphasis::bold)
    );
}
