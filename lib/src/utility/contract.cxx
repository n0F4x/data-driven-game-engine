module;

#include <fmt/color.h>

module ddge.utility.contracts;

namespace ddge::util {

auto PreconditionViolation::print() const -> void
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
        "    {} {}, {}",
        fmt::styled("message:", fmt::fg(fmt::color::cornflower_blue)),
        fmt::styled(m_condition_as_string, fmt::emphasis::bold),
        fmt::styled(m_message, fmt::emphasis::bold)
    );
}

}   // namespace ddge::util
