module;

#include <concepts>
#include <utility>

export module ddge.utility.meta.concepts.hashable;

namespace ddge::util::meta {

export template <typename T>
concept hashable_c = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

}   // namespace ddge::util::meta
