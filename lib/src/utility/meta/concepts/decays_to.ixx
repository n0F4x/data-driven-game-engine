module;

#include <concepts>

export module utility.meta.concepts.decays_to;

namespace util::meta {

export template <typename T, typename U>
concept decays_to_c = std::same_as<std::decay_t<T>, U>;

}   // namespace util::meta
