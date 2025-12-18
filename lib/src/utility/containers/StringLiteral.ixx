module;

#include <format>

export module ddge.utility.containers.StringLiteral;

namespace ddge::util {

export class StringLiteral {
public:
    consteval explicit(false) StringLiteral(const char* value) noexcept : m_value{ value }
    {}

    [[nodiscard]]
    explicit(false) constexpr operator const char*() const noexcept
    {
        return m_value;
    }

    [[nodiscard]]
    constexpr auto get() const noexcept -> const char*
    {
        return m_value;
    }

    [[nodiscard]]
    constexpr auto address() const noexcept -> const char* const*
    {
        return &m_value;
    }

private:
    const char* m_value;
};

export [[nodiscard]]
constexpr auto format_as(const StringLiteral string_literal) -> const char*
{
    return string_literal.get();
}

}   // namespace ddge::util

export template <>
struct std::formatter<ddge::util::StringLiteral> {
    constexpr static auto parse(const std::format_parse_context& parse_context)
        -> std::format_parse_context::iterator
    {
        return parse_context.begin();
    }

    constexpr static auto format(
        const ddge::util::StringLiteral& string_literal,
        std::format_context&             context
    ) -> std::format_context::iterator
    {
        return std::format_to(context.out(), "{}", string_literal.get());
    }
};
