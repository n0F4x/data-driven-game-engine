#include "contracts.hpp"

#include <string>

using namespace std::literals;

[[nodiscard]]
static auto make_precondition_violation_error_message(const std::source_location& location)
    -> std::string
{
    return "Precondition violated at "s + location.file_name() + '('
         + std::to_string(location.line()) + ':' + std::to_string(location.column())
         + ") `" + location.function_name() + '`';
}

[[nodiscard]]
static auto make_precondition_violation_error_message(
    const std::source_location& location,
    const std::string_view      message
) -> std::string
{
    return "Precondition violated at "s + location.file_name() + '('
         + std::to_string(location.line()) + ':' + std::to_string(location.column())
         + ") `" + location.function_name() + "`: " + message;
}

util::PreconditionViolation::PreconditionViolation(const std::source_location& location)
    : std::logic_error{ make_precondition_violation_error_message(location) }
{}

util::PreconditionViolation::PreconditionViolation(
    const std::source_location& location,
    const std::string_view      message
)
    : std::logic_error{ make_precondition_violation_error_message(location, message) }
{}
