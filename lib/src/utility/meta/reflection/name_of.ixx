module;

#include <source_location>
#include <string_view>
#include <type_traits>

export module ddge.utility.meta.reflection.name_of;

namespace ddge::util::meta {

// TODO: use reflection instead

export template <typename T>
consteval auto name_of() noexcept -> std::string_view;

export template <auto T>
    requires(std::is_member_object_pointer_v<decltype(T)>)
consteval auto name_of() noexcept -> std::string_view;

}   // namespace ddge::util::meta

namespace ddge::util::meta {

template <typename T>
[[nodiscard]]
consteval auto raw_pretty_function_name() noexcept -> std::string_view
{
    /* Clang:
     * std::string_view ddge::util::meta::raw_pretty_function_name() [T = ...]
     *
     * GCC:
     * consteval std::string_view
     * ddge::util::meta::raw_pretty_function_name()
     * [with auto T = ...; std::string_view = std::basic_string_view<char>]
     *
     * MSVC:
     * class std::basic_string_view<char,struct std::char_traits<char> >
     * __cdecl ddge::util::meta::raw_pretty_function_name<T...>(void) noexcept
     */
    return std::source_location::current().function_name();
}

template <auto T>
[[nodiscard]]
consteval auto raw_pretty_function_name() noexcept -> std::string_view
{
    return std::source_location::current().function_name();
}

[[nodiscard]]
consteval auto pretty_name_trailing_character() noexcept -> char
{
#if defined __clang__
    return ']';
#elif defined __GNUC__
    return ';'
#elif defined _MSC_VER
    return '>';
#else
    static_assert(false, "Compiler is not supported");
#endif
}

template <typename T>
consteval auto name_of() noexcept -> std::string_view
{
    constexpr std::string_view pretty{ raw_pretty_function_name<T>() };
    constexpr std::string_view leading_characters{
#ifdef __clang__
        "= "
#elifdef __GNUC__
        "= "
#elifdef _MSC_VER
        "ddge::util::meta::raw_pretty_function_name<"
#else
        [] [[noreturn]] static -> std::string_view {
            static_assert(false, "Compiler is not supported");
        }()
#endif
    };

    constexpr std::string_view::size_type trailing_offset =
        pretty.rfind(pretty_name_trailing_character());
    static_assert(trailing_offset != std::string_view::npos);

    constexpr std::string_view::size_type offset =
        pretty.rfind(leading_characters, trailing_offset);
    static_assert(offset != std::string_view::npos);

    return pretty.substr(
        offset + leading_characters.length(),
        trailing_offset - (offset + leading_characters.length())
    );
}

template <auto member_variable_T>
    requires(std::is_member_object_pointer_v<decltype(member_variable_T)>)
consteval auto name_of() noexcept -> std::string_view
{
    constexpr std::string_view pretty{ raw_pretty_function_name<member_variable_T>() };
    constexpr std::string_view leading_characters{ "::" };

    constexpr std::string_view::size_type trailing_offset =
        pretty.rfind(pretty_name_trailing_character());
    static_assert(trailing_offset != std::string_view::npos);

    constexpr std::string_view::size_type offset =
        pretty.rfind(leading_characters, trailing_offset);
    static_assert(offset != std::string_view::npos);

    return pretty.substr(
        offset + leading_characters.length(),
        trailing_offset - (offset + leading_characters.length())
    );
}

}   // namespace ddge::util::meta
