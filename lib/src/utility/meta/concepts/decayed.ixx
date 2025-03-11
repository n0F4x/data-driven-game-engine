module;

#include <concepts>

export module utility.meta.concepts.decayed;

namespace util::meta {

export template <typename T>
concept decayed_c = std::same_as<std::decay_t<T>, T>;

}   // namespace util::meta
