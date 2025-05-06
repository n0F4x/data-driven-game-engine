module;

#include <string>

export module core.scheduler.provider_index_for_argument;

import core.scheduler.concepts.provides_c;

import utility.meta.reflection.name_of;

namespace core::scheduler {

export template <typename Argument_T, typename... ArgumentProviders_T>
constexpr inline size_t provider_index_for_argument =
    []<size_t provider_index_T = 0>(this auto&& fn_self) {
        using namespace std::string_literals;
        static_assert(
            provider_index_T < sizeof...(ArgumentProviders_T),
            // TODO: use constexpr std::format
            "no provider found for argument `"s + util::meta::name_of<Argument_T>() + '`'
        );

        if constexpr (provides_c<ArgumentProviders_T...[provider_index_T], Argument_T>) {
            return provider_index_T;
        }
        else {
            return fn_self.template operator()<provider_index_T + 1>();
        }
    }();

}   // namespace core::scheduler
