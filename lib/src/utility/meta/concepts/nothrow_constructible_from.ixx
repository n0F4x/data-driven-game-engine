module;

#include <concepts>

export module utility.meta.concepts.nothrow_constructible_from;

namespace util::meta {

export template <typename T, typename... Args>
concept nothrow_constructible_from_c = std::destructible<T>
                                    && std::is_nothrow_constructible_v<T, Args...>;

}   // namespace util::meta
