module;

#include <compare>
#include <cstring>
#include <format>

export module ddge.utility.StringLiteral;

namespace ddge::util {

export class StringLiteral {
public:
    [[nodiscard]]
    static auto unsafe_create(const char* value) -> StringLiteral
    {
        StringLiteral result;
        result.m_value = value;
        return result;
    }

    consteval explicit(false) StringLiteral(const char* value) noexcept : m_value{ value }
    {
    }

    constexpr auto operator==(const StringLiteral& other) const noexcept -> bool
    {
        return std::strcmp(m_value, other.m_value) == 0;
    }

    constexpr auto operator<=>(const StringLiteral& other) const noexcept
        -> std::strong_ordering
    {
        const int result{ std::strcmp(m_value, other.m_value) };
        if (result == 0)
        {
            return std::strong_ordering::equal;
        }
        if (result < 0)
        {
            return std::strong_ordering::less;
        }
        return std::strong_ordering::greater;
    }

    [[nodiscard]]
    // NOLINTNEXTLINE(*-explicit-constructor, *-explicit-conversions)
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
    StringLiteral() = default;

    const char* m_value{};
};

[[nodiscard]]
constexpr auto format_as(const StringLiteral string_literal) -> const char*
{
    return string_literal.get();
}

}   // namespace ddge::util

template <>
struct std::formatter<ddge::util::StringLiteral> {
    constexpr static auto parse(const std::format_parse_context& parse_context)
        -> std::format_parse_context::iterator
    {
        return parse_context.begin();
    }

    static auto format(
        const ddge::util::StringLiteral& string_literal,
        std::format_context&             context
    ) -> std::format_context::iterator
    {
        return std::format_to(context.out(), "{}", string_literal.get());
    }
};
