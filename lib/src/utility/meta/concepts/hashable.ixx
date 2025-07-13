module;

#include <concepts>
#include <utility>

export module utility.meta.concepts.hashable;

namespace util::meta {

export template <typename T>
concept hashable_c = requires(T a) {
    {
        std::hash<T>{}(a)
    } -> std::convertible_to<size_t>;
};

}   // namespace util::meta
