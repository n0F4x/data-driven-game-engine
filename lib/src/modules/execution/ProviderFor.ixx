module;

#include <string>

export module ddge.modules.execution.ProviderFor;

import ddge.modules.execution.accessor_c;

import ddge.utility.meta.reflection.name_of;

using namespace std::string_literals;

namespace ddge::exec {
export template <accessor_c Accessor_T>
struct ProviderFor {
    static_assert(
        false,
        "No provider for accessor `"s + util::meta::name_of<Accessor_T>() + '`'
    );
};

export template <accessor_c Accessor_T>
using provider_for_t = typename ProviderFor<Accessor_T>::type;

}   // namespace ddge::exec
