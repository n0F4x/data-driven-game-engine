module;

#include <string>

export module ddge.modules.exec.ProviderFor;

import ddge.modules.exec.accessor_c;

import ddge.utility.meta.reflection.name_of;

using namespace std::string_literals;

namespace ddge::exec {

export template <accessor_c Accessor_T>
struct ProviderOf {
    static_assert(
        false,
        "Provider definition of accessor `"s + util::meta::name_of<Accessor_T>()
            + "` not found"
    );
};

export template <accessor_c Accessor_T>
using provider_of_t = typename ProviderOf<Accessor_T>::type;

}   // namespace ddge::exec
