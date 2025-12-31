module;

#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>

#include <entt/core/type_info.hpp>

export module ddge.utility.meta.reflection.name_of;

namespace ddge::util::meta {

export template <typename T>
consteval auto name_of() noexcept -> std::string_view;

export template <auto T>
    requires(std::is_member_object_pointer_v<decltype(T)>)
consteval auto name_of() noexcept -> std::string_view;

}   // namespace ddge::util::meta

namespace ddge::util::meta {

// TODO: use reflection

template <typename T>
consteval auto name_of() noexcept -> std::string_view
{
    return entt::type_name<T>::value();
}

template <auto T>
[[nodiscard]]
consteval auto raw_pretty_function_name() noexcept -> std::string_view
{
#if defined __clang__ || defined __GNUC__
    return __PRETTY_FUNCTION__;
#elif defined _MSC_VER
    return __FUNCSIG__;
#else
    static_assert(false, "Compiler has no pretty function");
#endif
}

[[nodiscard]]
consteval auto pretty_member_name_trailing_character() noexcept -> char
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

template <auto member_variable_T>
    requires(std::is_member_object_pointer_v<decltype(member_variable_T)>)
consteval auto name_of() noexcept -> std::string_view
{
    constexpr std::string_view pretty{ raw_pretty_function_name<member_variable_T>() };

    constexpr std::string_view::size_type trailing_offset =
        pretty.rfind(pretty_member_name_trailing_character());
    static_assert(trailing_offset != std::string_view::npos);

    constexpr std::string_view::size_type offset = pretty.rfind("::", trailing_offset);
    static_assert(offset != std::string_view::npos);

    return pretty.substr(offset + 2, trailing_offset - (offset + 2));
}

}   // namespace ddge::util::meta
