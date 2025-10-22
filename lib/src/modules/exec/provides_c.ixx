module;

#include <concepts>

export module ddge.modules.exec.provides_c;

namespace ddge::exec {

export template <typename Provider_T, typename T>
concept provides_c = requires(Provider_T& provider) {
    {
        provider.template provide<T>()
    } -> std::same_as<T>;
};

}   // namespace ddge::exec
