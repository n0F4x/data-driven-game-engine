module;

#include <string>

export module core.scheduler.provider_index_for_accessor;

import core.scheduler.concepts.provides_c;

import utility.meta.reflection.name_of;

namespace core::scheduler {

export template <typename Accessor_T, typename... Providers_T>
inline constexpr std::size_t provider_index_for_accessor =
    []<std::size_t provider_index_T = 0>(this auto&& fn_self) {
        using namespace std::string_literals;
        static_assert(
            provider_index_T < sizeof...(Providers_T),
            // TODO: use constexpr std::format
            "no provider found for accessor `"s + util::meta::name_of<Accessor_T>() + '`'
        );

        if constexpr (provides_c<Providers_T...[provider_index_T], Accessor_T>) {
            return provider_index_T;
        }
        else {
            return fn_self.template operator()<provider_index_T + 1>();
        }
    }();

}   // namespace core::scheduler
