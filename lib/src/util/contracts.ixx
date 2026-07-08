module;

#include <concepts>
#include <format>
#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>

export module ddge.util.contracts;

namespace ddge::util {

export class PreconditionViolation : public std::logic_error {
public:
    constexpr PreconditionViolation(
        const std::source_location& location,
        const std::string_view      condition_as_string,
        const std::string_view      message
    )
        : std::logic_error{
              std::format("{}\n{}\n{}", headline(), condition_as_string, message)
          },
          m_location{ location },
          m_condition_as_string{ condition_as_string },
          m_message{ message }
    {}

    auto print() const -> void;

private:
    std::source_location m_location;
    std::string          m_condition_as_string;
    std::string          m_message;

    [[nodiscard]]
    constexpr static auto headline() -> std::string_view
    {
        return "Precondition violated!";
    }
};

export template <typename Exception_T = PreconditionViolation>
    requires std::derived_from<Exception_T, PreconditionViolation>
constexpr auto propagate_precondition_violation(
    const std::string_view      condition_as_string,
    const std::source_location& location,
    const std::string_view      message = ""
) -> void
{
    const Exception_T precondition_violation{
        location,
        condition_as_string,
        message,
    };

#ifdef ENGINE_ENABLE_UNIT_TESTS
    throw auto{ precondition_violation };
#endif

    precondition_violation.print();

// TODO: std::breakpoint #p2546r5
#ifdef __clang__
    __builtin_debugtrap();
#elifdef _MSC_VER
    __debugbreak();
#else
    static_assert(false, "Compiler not supported");
#endif
}

}   // namespace ddge::util
