#include "contracts.hpp"

#include <string>

using namespace std::literals;

[[nodiscard]]
static auto make_precondition_violation_error_message(
    const std::source_location& location,
    const std::string_view      precondition
) -> std::string
{
    return "Precondition violated at "s + location.file_name() + '('
         + std::to_string(location.line()) + ':' + std::to_string(location.column())
         + ") `" + location.function_name() + "`: " + precondition;
}

util::PreconditionViolation::PreconditionViolation(
    const std::source_location& location,
    const std::string_view      precondition
)
    : std::logic_error{ make_precondition_violation_error_message(location, precondition) }
{}
